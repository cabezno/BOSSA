/*
 * Copyright (c) 2013-2026 Bossa Project, LLC
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "settings.h"

#include "Logger.h"
#include "qmltypes/qmlapplication.h"

#include <algorithm>

#include <QApplication>
#include <QAudioDevice>
#include <QColor>
#include <QColorDialog>
#include <QDir>
#include <QFile>
#include <QLocale>
#include <QMediaDevices>
#include <QStandardPaths>
#include <qdesktopservices.h>

static const QString APP_DATA_DIR_KEY("appdatadir");
static const QString SHOTCUT_INI_FILENAME("/bossa.ini");
static const QString RECENT_INI_FILENAME("recent.ini");
static QScopedPointer<BossaSettings> instance;
static QString appDataForSession;
static const int kMaximumTrackHeight = 125;
static const QString kRecentKey("recent");
static const QString kProjectsKey("projects");

namespace {
struct ModeMap
{
    BossaSettings::ProcessingMode id;
    const char *name;
};
static constexpr ModeMap kModeMap[] = {
    {BossaSettings::Native8Cpu, "Native8Cpu"},
    {BossaSettings::Linear8Cpu, "Linear8Cpu"},
    {BossaSettings::Native10Cpu, "Native10Cpu"},
    {BossaSettings::Linear10Cpu, "Linear10Cpu"},
    {BossaSettings::Linear10GpuCpu, "Linear10GpuCpu"},
};
} // anonymous namespace

BossaSettings &BossaSettings::singleton()
{
    if (!instance) {
        if (appDataForSession.isEmpty()) {
            instance.reset(new BossaSettings);
            if (instance->settings.value(APP_DATA_DIR_KEY).isValid()
                && QFile::exists(instance->settings.value(APP_DATA_DIR_KEY).toString()
                                 + SHOTCUT_INI_FILENAME))
                instance.reset(
                    new BossaSettings(instance->settings.value(APP_DATA_DIR_KEY).toString()));
        } else {
            instance.reset(new BossaSettings(appDataForSession));
        }
    }
    return *instance;
}

BossaSettings::BossaSettings()
    : QObject()
    , m_recent(QDir(appDataLocation()).filePath(RECENT_INI_FILENAME), QSettings::IniFormat)
{
    migrateLayout();
    migrateRecent();
}

BossaSettings::BossaSettings(const QString &appDataLocation)
    : QObject()
    , settings(appDataLocation + SHOTCUT_INI_FILENAME, QSettings::IniFormat)
    , m_appDataLocation(appDataLocation)
    , m_recent(QDir(appDataLocation).filePath(RECENT_INI_FILENAME), QSettings::IniFormat)
{
    migrateLayout();
    migrateRecent();
}

void BossaSettings::migrateRecent()
{
    // Migrate recent to separate INI file
    auto oldRecents = settings.value(kRecentKey).toStringList();
    if (recent().isEmpty() && !oldRecents.isEmpty()) {
        auto newRecents = recent();
        for (const auto &a : oldRecents) {
            if (a.size() < BossaSettings::MaxPath && !newRecents.contains(a)) {
                while (newRecents.size() > 100) {
                    newRecents.removeFirst();
                }
                newRecents.append(a);
            }
        }
        setRecent(newRecents);
        m_recent.sync();
        //        settings.remove("recent");
        settings.sync();
    }
}

void BossaSettings::migrateLayout()
{
    // Migrate old startup layout to a custom layout and start fresh
    if (!settings.contains("geometry2")) {
        auto geometry = settings.value("geometry").toByteArray();
        auto windowState = settings.value("windowState").toByteArray();
        setLayout(tr("Old (before v23) Layout"), geometry, windowState);
        setLayoutMode(2);
        settings.sync();
    }
}

void BossaSettings::log()
{
    LOG_INFO() << "language" << language();
    LOG_INFO() << "deinterlacer" << playerDeinterlacer();
    LOG_INFO() << "external monitor" << playerExternal();
    LOG_INFO() << "GPU processing" << playerGPU();
    LOG_INFO() << "interpolation" << playerInterpolation();
    LOG_INFO() << "video mode" << playerProfile();
    LOG_INFO() << "realtime" << playerRealtime();
    LOG_INFO() << "audio channels" << playerAudioChannels();
#if defined(Q_OS_WIN) || defined(Q_OS_LINUX)
    if (::qEnvironmentVariableIsSet("SDL_AUDIODRIVER")) {
        LOG_INFO() << "audio driver" << ::qgetenv("SDL_AUDIODRIVER");
    } else {
        LOG_INFO() << "audio driver" << playerAudioDriver();
    }
#endif
}

QString BossaSettings::language() const
{
    QString language = settings.value("language", QLocale().name()).toString();
    if (language == "en")
        language = "en_US";
    return language;
}

void BossaSettings::setLanguage(const QString &s)
{
    settings.setValue("language", s);
}

double BossaSettings::imageDuration() const
{
    return settings.value("imageDuration", 4.0).toDouble();
}

void BossaSettings::setImageDuration(double d)
{
    settings.setValue("imageDuration", d);
}

QString BossaSettings::openPath() const
{
    return settings
        .value("openPath", QStandardPaths::standardLocations(QStandardPaths::MoviesLocation))
        .toString();
}

void BossaSettings::setOpenPath(const QString &s)
{
    settings.setValue("openPath", s);
    emit savePathChanged();
}

QString BossaSettings::savePath() const
{
    return settings
        .value("savePath", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation))
        .toString();
}

void BossaSettings::setSavePath(const QString &s)
{
    settings.setValue("savePath", s);
    emit savePathChanged();
}

QStringList BossaSettings::recent() const
{
    return m_recent.value(kRecentKey).toStringList();
}

void BossaSettings::setRecent(const QStringList &ls)
{
    if (ls.isEmpty())
        m_recent.remove(kRecentKey);
    else if (!clearRecent())
        m_recent.setValue(kRecentKey, ls);
}

QStringList BossaSettings::projects()
{
    auto ls = m_recent.value(kProjectsKey).toStringList();
    if (ls.isEmpty()) {
        for (auto &r : recent()) {
            if (r.endsWith(".mlt"))
                ls << r;
        }
        // Prevent entering this block repeatedly
        if (ls.isEmpty())
            ls << QString();
        setProjects(ls);
    }
    return ls;
}

void BossaSettings::setProjects(const QStringList &ls)
{
    if (ls.isEmpty())
        m_recent.remove(kProjectsKey);
    else if (!clearRecent())
        m_recent.setValue(kProjectsKey, ls);
}

QString BossaSettings::theme() const
{
    return settings.value("theme", "dark").toString();
}

void BossaSettings::setTheme(const QString &s)
{
    settings.setValue("theme", s);
}

QThread::Priority BossaSettings::jobPriority() const
{
    const auto priority = settings.value("jobPriority", "low").toString();
    if (priority == "low") {
        return QThread::LowPriority;
    }
    return QThread::NormalPriority;
}

void BossaSettings::setJobPriority(const QString &s)
{
    settings.setValue("jobPriority", s);
}

bool BossaSettings::showTitleBars() const
{
    return settings.value("titleBars", true).toBool();
}

void BossaSettings::setShowTitleBars(bool b)
{
    settings.setValue("titleBars", b);
}

bool BossaSettings::showToolBar() const
{
    return settings.value("toolBar", true).toBool();
}

void BossaSettings::setShowToolBar(bool b)
{
    settings.setValue("toolBar", b);
}

bool BossaSettings::textUnderIcons() const
{
    return settings.value("textUnderIcons", true).toBool();
}

void BossaSettings::setTextUnderIcons(bool b)
{
    settings.setValue("textUnderIcons", b);
}

bool BossaSettings::smallIcons() const
{
    return settings.value("smallIcons", false).toBool();
}

void BossaSettings::setSmallIcons(bool b)
{
    settings.setValue("smallIcons", b);
    emit smallIconsChanged();
}

QByteArray BossaSettings::windowGeometry() const
{
    return settings.value("geometry2").toByteArray();
}

void BossaSettings::setWindowGeometry(const QByteArray &a)
{
    settings.setValue("geometry2", a);
}

QByteArray BossaSettings::windowGeometryDefault() const
{
    return settings.value("geometryDefault").toByteArray();
}

void BossaSettings::setWindowGeometryDefault(const QByteArray &a)
{
    settings.setValue("geometryDefault", a);
}

QByteArray BossaSettings::windowState() const
{
    return settings.value("windowState2").toByteArray();
}

void BossaSettings::setWindowState(const QByteArray &a)
{
    settings.setValue("windowState2", a);
}

QByteArray BossaSettings::windowStateDefault() const
{
    return settings.value("windowStateDefault").toByteArray();
}

void BossaSettings::setWindowStateDefault(const QByteArray &a)
{
    settings.setValue("windowStateDefault", a);
}

QString BossaSettings::viewMode() const
{
    return settings.value("playlist/viewMode").toString();
}

void BossaSettings::setViewMode(const QString &viewMode)
{
    settings.setValue("playlist/viewMode", viewMode);
    emit viewModeChanged();
}

QString BossaSettings::filesViewMode() const
{
    return settings.value("files/viewMode", QLatin1String("tiled")).toString();
}

void BossaSettings::setFilesViewMode(const QString &viewMode)
{
    settings.setValue("files/viewMode", viewMode);
    emit filesViewModeChanged();
}

QStringList BossaSettings::filesLocations() const
{
    QStringList result;
    for (const auto &s : settings.value("files/locations").toStringList()) {
        if (!s.startsWith("__"))
            result << s;
    }
    return result;
}

QString BossaSettings::filesLocationPath(const QString &name) const
{
    QString key = QStringLiteral("files/location/%1").arg(name);
    return settings.value(key).toString();
}

bool BossaSettings::setFilesLocation(const QString &name, const QString &path)
{
    bool isNew = false;
    QStringList locations = filesLocations();
    if (!locations.contains(name)) {
        isNew = true;
        locations.append(name);
        settings.setValue("files/locations", locations);
    }
    settings.setValue("files/location/" + name, path);
    return isNew;
}

bool BossaSettings::removeFilesLocation(const QString &name)
{
    QStringList list = filesLocations();
    int index = list.indexOf(name);
    if (index > -1) {
        list.removeAt(index);
        if (list.isEmpty())
            settings.remove("files/locations");
        else
            settings.setValue("files/locations", list);
        settings.remove("files/location/" + name);
        return true;
    }
    return false;
}

QStringList BossaSettings::filesOpenOther(const QString &type) const
{
    return settings.value("files/openOther/" + type).toStringList();
}

void BossaSettings::setFilesOpenOther(const QString &type, const QString &filePath)
{
    QStringList filePaths = filesOpenOther(type);
    filePaths.removeAll(filePath);
    filePaths.append(filePath);
    settings.setValue("files/openOther/" + type, filePaths);
}

bool BossaSettings::removeFilesOpenOther(const QString &type, const QString &filePath)
{
    QStringList list = filesOpenOther(type);
    int index = list.indexOf(filePath);
    if (index > -1) {
        list.removeAt(index);
        if (list.isEmpty())
            settings.remove("files/openOther/" + type);
        else
            settings.setValue("files/openOther/" + type, list);
        return true;
    }
    return false;
}

QString BossaSettings::filesCurrentDir() const
{
    const auto ls = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    auto path = settings.value("files/currentDir", ls.first()).toString();
    if (!QFile::exists(path)) {
        LOG_DEBUG() << "dir does not exist:" << QDir::toNativeSeparators(path);
        path = ls.first();
    }
    return path;
}

void BossaSettings::setFilesCurrentDir(const QString &s)
{
    settings.setValue("files/currentDir", s);
}

bool BossaSettings::filesFoldersOpen() const
{
    return settings.value("files/foldersOpen", true).toBool();
}

void BossaSettings::setFilesFoldersOpen(bool b)
{
    settings.setValue("files/foldersOpen", b);
}

QString BossaSettings::exportFrameSuffix() const
{
    return settings.value("exportFrameSuffix", ".png").toString();
}

void BossaSettings::setExportFrameSuffix(const QString &exportFrameSuffix)
{
    settings.setValue("exportFrameSuffix", exportFrameSuffix);
}

QString BossaSettings::encodePath() const
{
    return settings
        .value("encode/path", QStandardPaths::standardLocations(QStandardPaths::MoviesLocation))
        .toString();
}

void BossaSettings::setEncodePath(const QString &s)
{
    settings.setValue("encode/path", s);
}

bool BossaSettings::encodeFreeSpaceCheck() const
{
    return settings.value("encode/freeSpaceCheck", true).toBool();
}

void BossaSettings::setEncodeFreeSpaceCheck(bool b)
{
    settings.setValue("encode/freeSpaceCheck", b);
}

bool BossaSettings::encodeUseHardware() const
{
    return settings.value("encode/useHardware").toBool();
}

void BossaSettings::setEncodeUseHardware(bool b)
{
    settings.setValue("encode/useHardware", b);
}

QStringList BossaSettings::encodeHardware() const
{
    return settings.value("encode/hardware").toStringList();
}

void BossaSettings::setEncodeHardware(const QStringList &ls)
{
    if (ls.isEmpty())
        settings.remove("encode/hardware");
    else
        settings.setValue("encode/hardware", ls);
}

bool BossaSettings::encodeHardwareDecoder() const
{
    return settings.value("encode/hardwareDecoder", false).toBool();
}

void BossaSettings::setEncodeHardwareDecoder(bool b)
{
    settings.setValue("encode/hardwareDecoder", b);
}

bool BossaSettings::encodeAdvanced() const
{
    return settings.value("encode/advanced", false).toBool();
}

void BossaSettings::setEncodeAdvanced(bool b)
{
    settings.setValue("encode/advanced", b);
}

bool BossaSettings::convertAdvanced() const
{
    return settings.value("convertAdvanced", false).toBool();
}

void BossaSettings::setConvertAdvanced(bool b)
{
    settings.setValue("convertAdvanced", b);
}

BossaSettings::ProcessingMode BossaSettings::processingMode()
{
    if (settings.contains("processingMode")) {
        auto result = (BossaSettings::ProcessingMode) settings.value("processingMode").toInt();
        if (result == Linear8Cpu) {
            // No longer supported but kept to prevent unexpected processing behavior going from
            // beta to release
            result = Native8Cpu;
        }
        return result;
    } else if (settings.contains("player/gpu2")) {
        // Legacy GPU Mode
        if (settings.value("player/gpu2").toBool()) {
            return BossaSettings::Linear10GpuCpu;
        }
    }
    return BossaSettings::Native8Cpu;
}

void BossaSettings::setProcessingMode(ProcessingMode mode)
{
    settings.setValue("processingMode", mode);
    emit playerGpuChanged();
}

QString BossaSettings::processingModeStr(BossaSettings::ProcessingMode mode)
{
    for (const auto &m : kModeMap) {
        if (m.id == mode)
            return QString::fromLatin1(m.name);
    }
    LOG_ERROR() << "Unknown processing mode" << mode;
    return QStringLiteral("Native8Cpu");
}

BossaSettings::ProcessingMode BossaSettings::processingModeId(const QString &mode)
{
    for (const auto &m : kModeMap) {
        if (mode == QLatin1String(m.name))
            return m.id;
    }
    LOG_ERROR() << "Unknown processing mode" << mode;
    return Native8Cpu;
}

bool BossaSettings::showConvertClipDialog() const
{
    return settings.value("showConvertClipDialog", true).toBool();
}

void BossaSettings::setShowConvertClipDialog(bool b)
{
    settings.setValue("showConvertClipDialog", b);
}

bool BossaSettings::encodeParallelProcessing() const
{
    return settings.value("encode/parallelProcessing", false).toBool();
}

void BossaSettings::setEncodeParallelProcessing(bool b)
{
    settings.setValue("encode/parallelProcessing", b);
}

int BossaSettings::playerAudioChannels() const
{
    return settings.value("player/audioChannels", 2).toInt();
}

void BossaSettings::setPlayerAudioChannels(int i)
{
    settings.setValue("player/audioChannels", i);
    emit playerAudioChannelsChanged(i);
}

QString BossaSettings::playerDeinterlacer() const
{
    QString result = settings.value("player/deinterlacer", "onefield").toString();
    //XXX workaround yadif crashing with mlt_transition
    if (result == "yadif" || result == "yadif-nospatial")
        result = "onefield";
    return result;
}

void BossaSettings::setPlayerDeinterlacer(const QString &s)
{
    settings.setValue("player/deinterlacer", s);
}

QString BossaSettings::playerExternal() const
{
    auto result = settings.value("player/external", "").toString();
    // "sdi" is no longer supported DVEO VidPort
    return result == "sdi" ? "" : result;
}

void BossaSettings::setPlayerExternal(const QString &s)
{
    settings.setValue("player/external", s);
}

bool BossaSettings::playerJACK() const
{
    return settings.value("player/jack", false).toBool();
}

QString BossaSettings::playerInterpolation() const
{
    return settings.value("player/interpolation", "bilinear").toString();
}

void BossaSettings::setPlayerInterpolation(const QString &s)
{
    settings.setValue("player/interpolation", s);
}

bool BossaSettings::playerGPU() const
{
    // This is the legacy function for the old GPU mode.
    if (settings.contains("processingMode")) {
        ProcessingMode mode = (ProcessingMode) settings.value("processingMode").toInt();
        return mode == Linear10GpuCpu;
    } else if (settings.contains("player/gpu2")) {
        // Legacy GPU Mode
        return settings.value("player/gpu2").toBool();
    }
    return false;
}

bool BossaSettings::playerWarnGPU() const
{
    return false; //settings.value("player/warnGPU", false).toBool();
}

void BossaSettings::setPlayerJACK(bool b)
{
    settings.setValue("player/jack", b);
}

int BossaSettings::playerDecklinkGamma() const
{
    return settings.value("player/decklinkGamma", 0).toInt();
}

void BossaSettings::setPlayerDecklinkGamma(int i)
{
    settings.setValue("player/decklinkGamma", i);
}

int BossaSettings::playerKeyerMode() const
{
    return settings.value("player/keyer", 0).toInt();
}

void BossaSettings::setPlayerKeyerMode(int i)
{
    settings.setValue("player/keyer", i);
}

bool BossaSettings::playerMuted() const
{
    return settings.value("player/muted", false).toBool();
}

void BossaSettings::setPlayerMuted(bool b)
{
    settings.setValue("player/muted", b);
}

QString BossaSettings::playerProfile() const
{
    return settings.value("player/profile", "").toString();
}

void BossaSettings::setPlayerProfile(const QString &s)
{
    settings.setValue("player/profile", s);
}

bool BossaSettings::playerProgressive() const
{
    return settings.value("player/progressive", true).toBool();
}

void BossaSettings::setPlayerProgressive(bool b)
{
    settings.setValue("player/progressive", b);
}

bool BossaSettings::playerRealtime() const
{
    return settings.value("player/realtime", true).toBool();
}

void BossaSettings::setPlayerRealtime(bool b)
{
    settings.setValue("player/realtime", b);
}

bool BossaSettings::playerScrubAudio() const
{
    return settings.value("player/scrubAudio", true).toBool();
}

void BossaSettings::setPlayerScrubAudio(bool b)
{
    settings.setValue("player/scrubAudio", b);
}

int BossaSettings::playerVolume() const
{
    return settings.value("player/volume", 88).toInt();
}

void BossaSettings::setPlayerVolume(int i)
{
    settings.setValue("player/volume", i);
}

float BossaSettings::playerZoom() const
{
    return settings.value("player/zoom", 0.0f).toFloat();
}

void BossaSettings::setPlayerZoom(float f)
{
    settings.setValue("player/zoom", f);
}

int BossaSettings::playerPreviewScale() const
{
    return settings.value("player/previewScale", 0).toInt();
}

void BossaSettings::setPlayerPreviewScale(int i)
{
    settings.setValue("player/previewScale", i);
}

bool BossaSettings::playerPreviewHardwareDecoder() const
{
    return settings.value("player/previewHardwareDecoder", true).toBool();
}

bool BossaSettings::playerPreviewHardwareDecoderIsSet() const
{
    return settings.contains("player/previewHardwareDecoder");
}

void BossaSettings::setPlayerPreviewHardwareDecoder(bool b)
{
    settings.setValue("player/previewHardwareDecoder", b);
}

int BossaSettings::playerVideoDelayMs() const
{
    return settings.value("player/videoDelayMs", 0).toInt();
}

void BossaSettings::setPlayerVideoDelayMs(int i)
{
    settings.setValue("player/videoDelayMs", i);
}

double BossaSettings::playerJumpSeconds() const
{
    return settings.value("player/jumpSeconds", 60.0).toDouble();
}

void BossaSettings::setPlayerJumpSeconds(double i)
{
    settings.setValue("player/jumpSeconds", i);
}

QString BossaSettings::playerAudioDriver() const
{
#if defined(Q_OS_WIN)
    auto s = playerAudioChannels() > 2 ? "directsound" : "winmm";
#else
    auto s = "pulseaudio";
#endif
    if (::qEnvironmentVariableIsSet("SDL_AUDIODRIVER")) {
        return ::qgetenv("SDL_AUDIODRIVER");
    } else {
        return settings.value("player/audioDriver", s).toString();
    }
}

void BossaSettings::setPlayerAudioDriver(const QString &s)
{
    settings.setValue("player/audioDriver", s);
}

bool BossaSettings::playerPauseAfterSeek() const
{
    return settings.value("player/pauseAfterSeek", true).toBool();
}

void BossaSettings::setPlayerPauseAfterSeek(bool b)
{
    settings.setValue("player/pauseAfterSeek", b);
}

QString BossaSettings::playlistThumbnails() const
{
    return settings.value("playlist/thumbnails", "small").toString();
}

void BossaSettings::setPlaylistThumbnails(const QString &s)
{
    settings.setValue("playlist/thumbnails", s);
    emit playlistThumbnailsChanged();
}

bool BossaSettings::playlistAutoplay() const
{
    return settings.value("playlist/autoplay", true).toBool();
}

void BossaSettings::setPlaylistAutoplay(bool b)
{
    settings.setValue("playlist/autoplay", b);
}

bool BossaSettings::playlistShowColumn(const QString &column)
{
    return settings.value("playlist/columns/" + column, true).toBool();
}

void BossaSettings::setPlaylistShowColumn(const QString &column, bool b)
{
    settings.setValue("playlist/columns/" + column, b);
}

bool BossaSettings::timelineDragScrub() const
{
    return settings.value("timeline/dragScrub", false).toBool();
}

void BossaSettings::setTimelineDragScrub(bool b)
{
    settings.setValue("timeline/dragScrub", b);
    emit timelineDragScrubChanged();
}

bool BossaSettings::timelineShowWaveforms() const
{
    return settings.value("timeline/waveforms", true).toBool();
}

void BossaSettings::setTimelineShowWaveforms(bool b)
{
    settings.setValue("timeline/waveforms", b);
    emit timelineShowWaveformsChanged();
}

bool BossaSettings::timelineShowThumbnails() const
{
    return settings.value("timeline/thumbnails", true).toBool();
}

void BossaSettings::setTimelineShowThumbnails(bool b)
{
    settings.setValue("timeline/thumbnails", b);
    emit timelineShowThumbnailsChanged();
}

bool BossaSettings::timelineRipple() const
{
    return settings.value("timeline/ripple", false).toBool();
}

void BossaSettings::setTimelineRipple(bool b)
{
    settings.setValue("timeline/ripple", b);
    emit timelineRippleChanged();
}

bool BossaSettings::timelineRippleAllTracks() const
{
    return settings.value("timeline/rippleAllTracks", false).toBool();
}

void BossaSettings::setTimelineRippleAllTracks(bool b)
{
    settings.setValue("timeline/rippleAllTracks", b);
    emit timelineRippleAllTracksChanged();
}

bool BossaSettings::timelineRippleMarkers() const
{
    return settings.value("timeline/rippleMarkers", false).toBool();
}

void BossaSettings::setTimelineRippleMarkers(bool b)
{
    settings.setValue("timeline/rippleMarkers", b);
    emit timelineRippleMarkersChanged();
}

bool BossaSettings::timelineSnap() const
{
    return settings.value("timeline/snap", true).toBool();
}

void BossaSettings::setTimelineSnap(bool b)
{
    settings.setValue("timeline/snap", b);
    emit timelineSnapChanged();
}

int BossaSettings::timelineTrackHeight() const
{
    return qMin(settings.value("timeline/trackHeight", 50).toInt(), kMaximumTrackHeight);
}

void BossaSettings::setTimelineTrackHeight(int n)
{
    settings.setValue("timeline/trackHeight", qMin(n, kMaximumTrackHeight));
}

bool BossaSettings::timelineScrollZoom() const
{
    return settings.value("timeline/scrollZoom", true).toBool();
}

void BossaSettings::setTimelineScrollZoom(bool b)
{
    settings.setValue("timeline/scrollZoom", b);
    emit timelineScrollZoomChanged();
}

bool BossaSettings::timelineFramebufferWaveform() const
{
    return settings.value("timeline/framebufferWaveform", true).toBool();
}

void BossaSettings::setTimelineFramebufferWaveform(bool b)
{
    settings.setValue("timeline/framebufferWaveform", b);
    emit timelineFramebufferWaveformChanged();
}

int BossaSettings::audioReferenceTrack() const
{
    return settings.value("timeline/audioReferenceTrack", 0).toInt();
}
void BossaSettings::setAudioReferenceTrack(int track)
{
    settings.setValue("timeline/audioReferenceTrack", track);
}

double BossaSettings::audioReferenceSpeedRange() const
{
    return settings.value("timeline/audioReferenceSpeedRange", 0).toDouble();
}
void BossaSettings::setAudioReferenceSpeedRange(double range)
{
    settings.setValue("timeline/audioReferenceSpeedRange", range);
}

bool BossaSettings::timelinePreviewTransition() const
{
    return settings.value("timeline/previewTransition", true).toBool();
}

void BossaSettings::setTimelinePreviewTransition(bool b)
{
    settings.setValue("timeline/previewTransition", b);
}

void BossaSettings::setTimelineScrolling(BossaSettings::TimelineScrolling value)
{
    settings.remove("timeline/centerPlayhead");
    settings.setValue("timeline/scrolling", value);
    emit timelineScrollingChanged();
}

BossaSettings::TimelineScrolling BossaSettings::timelineScrolling() const
{
    if (settings.contains("timeline/centerPlayhead")
        && settings.value("timeline/centerPlayhead").toBool())
        return BossaSettings::TimelineScrolling::CenterPlayhead;
    else
        return BossaSettings::TimelineScrolling(
            settings.value("timeline/scrolling", PageScrolling).toInt());
}

bool BossaSettings::timelineAutoAddTracks() const
{
    return settings.value("timeline/autoAddTracks", false).toBool();
}

void BossaSettings::setTimelineAutoAddTracks(bool b)
{
    if (b != timelineAutoAddTracks()) {
        settings.setValue("timeline/autoAddTracks", b);
        emit timelineAutoAddTracksChanged();
    }
}

bool BossaSettings::timelineRectangleSelect() const
{
    return settings.value("timeline/rectangleSelect", true).toBool();
}

void BossaSettings::setTimelineRectangleSelect(bool b)
{
    settings.setValue("timeline/rectangleSelect", b);
    emit timelineRectangleSelectChanged();
}

bool BossaSettings::timelineAdjustGain() const
{
    return settings.value("timeline/adjustGain", false).toBool();
}

void BossaSettings::setTimelineAdjustGain(bool b)
{
    settings.setValue("timeline/adjustGain", b);
    emit timelineAdjustGainChanged();
}

QString BossaSettings::filterFavorite(const QString &filterName)
{
    return settings.value("filter/favorite/" + filterName, "").toString();
}

void BossaSettings::setFilterFavorite(const QString &filterName, const QString &value)
{
    settings.setValue("filter/favorite/" + filterName, value);
}

QStringList BossaSettings::addOnFilterServices() const
{
    return settings.value("filter/addOnServices").toStringList();
}

void BossaSettings::setAddOnFilterServices(const QStringList &services)
{
    settings.setValue("filter/addOnServices", services);
}

double BossaSettings::audioInDuration() const
{
    return settings.value("filter/audioInDuration", 1.0).toDouble();
}

void BossaSettings::setAudioInDuration(double d)
{
    settings.setValue("filter/audioInDuration", d);
    emit audioInDurationChanged();
}

double BossaSettings::audioOutDuration() const
{
    return settings.value("filter/audioOutDuration", 1.0).toDouble();
}

void BossaSettings::setAudioOutDuration(double d)
{
    settings.setValue("filter/audioOutDuration", d);
    emit audioOutDurationChanged();
}

double BossaSettings::videoInDuration() const
{
    return settings.value("filter/videoInDuration", 1.0).toDouble();
}

void BossaSettings::setVideoInDuration(double d)
{
    settings.setValue("filter/videoInDuration", d);
    emit videoInDurationChanged();
}

double BossaSettings::videoOutDuration() const
{
    return settings.value("filter/videoOutDuration", 1.0).toDouble();
}

void BossaSettings::setVideoOutDuration(double d)
{
    settings.setValue("filter/videoOutDuration", d);
    emit videoOutDurationChanged();
}

int BossaSettings::audioInCurve() const
{
    return settings.value("filter/audioInCurve", mlt_keyframe_linear).toInt();
}

void BossaSettings::setAudioInCurve(int c)
{
    settings.setValue("filter/audioInCurve", c);
    emit audioInCurveChanged();
}

int BossaSettings::audioOutCurve() const
{
    return settings.value("filter/audioOutCurve", mlt_keyframe_linear).toInt();
}

void BossaSettings::setAudioOutCurve(int c)
{
    settings.setValue("filter/audioOutCurve", c);
    emit audioOutCurveChanged();
}

bool BossaSettings::askOutputFilter() const
{
    return settings.value("filter/askOutput", true).toBool();
}

void BossaSettings::setAskOutputFilter(bool b)
{
    settings.setValue("filter/askOutput", b);
    emit askOutputFilterChanged();
}

bool BossaSettings::loudnessScopeShowMeter(const QString &meter) const
{
    return settings.value("scope/loudness/" + meter, true).toBool();
}

void BossaSettings::setLoudnessScopeShowMeter(const QString &meter, bool b)
{
    settings.setValue("scope/loudness/" + meter, b);
}

void BossaSettings::setMarkerColor(const QColor &color)
{
    settings.setValue("markers/color", color.name());
}

QColor BossaSettings::markerColor() const
{
    return QColor(settings.value("markers/color", "green").toString());
}

void BossaSettings::setMarkersShowColumn(const QString &column, bool b)
{
    settings.setValue("markers/columns/" + column, b);
}

bool BossaSettings::markersShowColumn(const QString &column) const
{
    return settings.value("markers/columns/" + column, true).toBool();
}

void BossaSettings::setMarkerSort(int column, Qt::SortOrder order)
{
    settings.setValue("markers/sortColumn", column);
    settings.setValue("markers/sortOrder", order);
}

int BossaSettings::getMarkerSortColumn()
{
    return settings.value("markers/sortColumn", -1).toInt();
}

Qt::SortOrder BossaSettings::getMarkerSortOrder()
{
    return (Qt::SortOrder) settings.value("markers/sortOrder", Qt::AscendingOrder).toInt();
}

int BossaSettings::drawMethod() const
{
#ifdef Q_OS_WIN
    return settings.value("opengl", Qt::AA_UseOpenGLES).toInt();
#else
    return settings.value("opengl", Qt::AA_UseDesktopOpenGL).toInt();
#endif
}

void BossaSettings::setDrawMethod(int i)
{
    settings.setValue("opengl", i);
}

bool BossaSettings::safeMode() const
{
    return settings.value("safeMode", false).toBool();
}

void BossaSettings::setSafeMode(bool value)
{
    settings.setValue("safeMode", value);
}

bool BossaSettings::noUpgrade() const
{
    return settings.value("noupgrade", false).toBool();
}

void BossaSettings::setNoUpgrade(bool value)
{
    settings.setValue("noupgrade", value);
}

bool BossaSettings::checkUpgradeAutomatic()
{
    return settings.value("checkUpgradeAutomatic", false).toBool();
}

void BossaSettings::setCheckUpgradeAutomatic(bool b)
{
    settings.setValue("checkUpgradeAutomatic", b);
}

bool BossaSettings::askUpgradeAutomatic()
{
    return settings.value("askUpgradeAutmatic", true).toBool();
}

void BossaSettings::setAskUpgradeAutomatic(bool b)
{
    settings.setValue("askUpgradeAutmatic", b);
}

bool BossaSettings::askChangeVideoMode()
{
    return settings.value("askChangeVideoMode", true).toBool();
}

void BossaSettings::setAskChangeVideoMode(bool b)
{
    settings.setValue("askChangeVideoMode", b);
}

void BossaSettings::sync()
{
    settings.sync();
}

QString BossaSettings::appDataLocation() const
{
    if (!m_appDataLocation.isEmpty())
        return m_appDataLocation;
    else
        return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
}

void BossaSettings::setAppDataForSession(const QString &location)
{
    // This is intended to be called when using a command line option
    // to set the AppData location.
    appDataForSession = location;
    if (instance)
        instance.reset(new BossaSettings(location));
}

void BossaSettings::setAppDataLocally(const QString &location)
{
    // This is intended to be called when using a GUI action to set the
    // the new AppData location.

    // Copy the existing settings if they exist.
    if (!QFile::exists(location + SHOTCUT_INI_FILENAME)) {
        QSettings newSettings(location + SHOTCUT_INI_FILENAME, QSettings::IniFormat);
        foreach (const QString &key, settings.allKeys())
            newSettings.setValue(key, settings.value(key));
        newSettings.sync();
    }

    // Set the new location.
    QSettings localSettings;
    localSettings.setValue(APP_DATA_DIR_KEY, location);
    localSettings.sync();
}

QStringList BossaSettings::layouts() const
{
    QStringList result;
    for (const auto &s : settings.value("layout/layouts").toStringList()) {
        if (!s.startsWith("__"))
            result << s;
    }
    return result;
}

bool BossaSettings::setLayout(const QString &name,
                                const QByteArray &geometry,
                                const QByteArray &state)
{
    bool isNew = false;
    QStringList layouts = this->layouts();
    if (layouts.indexOf(name) == -1) {
        isNew = true;
        layouts.append(name);
        settings.setValue("layout/layouts", layouts);
    }
    settings.setValue(QStringLiteral("layout/%1_%2").arg(name, "geometry"), geometry);
    settings.setValue(QStringLiteral("layout/%1_%2").arg(name, "state"), state);
    return isNew;
}

QByteArray BossaSettings::layoutGeometry(const QString &name)
{
    QString key = QStringLiteral("layout/%1_geometry").arg(name);
    return settings.value(key).toByteArray();
}

QByteArray BossaSettings::layoutState(const QString &name)
{
    QString key = QStringLiteral("layout/%1_state").arg(name);
    return settings.value(key).toByteArray();
}

bool BossaSettings::removeLayout(const QString &name)
{
    QStringList list = layouts();
    int index = list.indexOf(name);
    if (index > -1) {
        list.removeAt(index);
        if (list.isEmpty())
            settings.remove("layout/layouts");
        else
            settings.setValue("layout/layouts", list);
        settings.remove(QStringLiteral("layout/%1_%2").arg(name, "geometry"));
        settings.remove(QStringLiteral("layout/%1_%2").arg(name, "state"));
        return true;
    }
    return false;
}

int BossaSettings::layoutMode() const
{
    return settings.value("layout/mode", -1).toInt();
}

void BossaSettings::setLayoutMode(int mode)
{
    settings.setValue("layout/mode", mode);
}

bool BossaSettings::clearRecent() const
{
    return settings.value("clearRecent", false).toBool();
}

void BossaSettings::setClearRecent(bool b)
{
    settings.setValue("clearRecent", b);
}

QString BossaSettings::projectsFolder() const
{
    return settings
        .value("projectsFolder", QStandardPaths::standardLocations(QStandardPaths::MoviesLocation))
        .toString();
}

void BossaSettings::setProjectsFolder(const QString &path)
{
    settings.setValue("projectsFolder", path);
}

QString BossaSettings::audioInput() const
{
    QString defaultValue = "default";
#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
    for (const auto &deviceInfo : QMediaDevices::audioInputs()) {
        defaultValue = deviceInfo.description();
    }
#endif
    return settings.value("audioInput", defaultValue).toString();
}

void BossaSettings::setAudioInput(const QString &name)
{
    settings.setValue("audioInput", name);
}

QString BossaSettings::videoInput() const
{
    return settings.value("videoInput").toString();
}

void BossaSettings::setVideoInput(const QString &name)
{
    settings.setValue("videoInput", name);
}

QString BossaSettings::glaxnimatePath() const
{
    QDir dir(qApp->applicationDirPath());
    return settings.value("glaxnimatePath", dir.absoluteFilePath("glaxnimate")).toString();
}

void BossaSettings::setGlaxnimatePath(const QString &path)
{
    settings.setValue("glaxnimatePath", path);
}

void BossaSettings::resetGlaxnimatePath()
{
    settings.remove("glaxnimatePath");
}

bool BossaSettings::exportRangeMarkers() const
{
    return settings.value("exportRangeMarkers", true).toBool();
}

void BossaSettings::setExportRangeMarkers(bool b)
{
    settings.setValue("exportRangeMarkers", b);
}

bool BossaSettings::proxyEnabled() const
{
    return settings.value("proxy/enabled", false).toBool();
}

void BossaSettings::setProxyEnabled(bool b)
{
    settings.setValue("proxy/enabled", b);
}

QString BossaSettings::proxyFolder() const
{
    QDir dir(appDataLocation());
    const char *subfolder = "proxies";
    if (!dir.cd(subfolder)) {
        if (dir.mkdir(subfolder))
            dir.cd(subfolder);
    }
    return settings.value("proxy/folder", dir.path()).toString();
}

void BossaSettings::setProxyFolder(const QString &path)
{
    settings.setValue("proxy/folder", path);
}

bool BossaSettings::proxyUseProjectFolder() const
{
    return settings.value("proxy/useProjectFolder", true).toBool();
}

void BossaSettings::setProxyUseProjectFolder(bool b)
{
    settings.setValue("proxy/useProjectFolder", b);
}

bool BossaSettings::proxyUseHardware() const
{
    return settings.value("proxy/useHardware", false).toBool();
}

void BossaSettings::setProxyUseHardware(bool b)
{
    settings.setValue("proxy/useHardware", b);
}

void BossaSettings::clearShortcuts(const QString &name)
{
    QString key = "shortcuts/" + name;
    settings.remove(key);
}

void BossaSettings::setShortcuts(const QString &name, const QList<QKeySequence> &shortcuts)
{
    QString key = "shortcuts/" + name;
    QString shortcutSetting;
    if (shortcuts.size() > 0)
        shortcutSetting += shortcuts[0].toString();
    shortcutSetting += "||";
    if (shortcuts.size() > 1)
        shortcutSetting += shortcuts[1].toString();
    settings.setValue(key, shortcutSetting);
}

QList<QKeySequence> BossaSettings::shortcuts(const QString &name)
{
    QString key = "shortcuts/" + name;
    QList<QKeySequence> shortcuts;
    QString shortcutSetting = settings.value(key, "").toString();
    if (!shortcutSetting.isEmpty()) {
        for (const QString &s : shortcutSetting.split("||"))
            shortcuts << QKeySequence::fromString(s);
    }
    return shortcuts;
}

double BossaSettings::slideshowImageDuration(double defaultSeconds) const
{
    return settings.value("slideshow/clipDuration", defaultSeconds).toDouble();
}

void BossaSettings::setSlideshowImageDuration(double seconds)
{
    settings.setValue("slideshow/clipDuration", seconds);
}

double BossaSettings::slideshowAudioVideoDuration(double defaultSeconds) const
{
    return settings.value("slideshow/audioVideoDuration", defaultSeconds).toDouble();
}

void BossaSettings::setSlideshowAudioVideoDuration(double seconds)
{
    settings.setValue("slideshow/audioVideoDuration", seconds);
}

int BossaSettings::slideshowAspectConversion(int defaultAspectConversion) const
{
    return settings.value("slideshow/aspectConversion", defaultAspectConversion).toInt();
}

void BossaSettings::setSlideshowAspectConversion(int aspectConversion)
{
    settings.setValue("slideshow/aspectConversion", aspectConversion);
}

int BossaSettings::slideshowZoomPercent(int defaultZoomPercent) const
{
    return settings.value("slideshow/zoomPercent", defaultZoomPercent).toInt();
}

void BossaSettings::setSlideshowZoomPercent(int zoomPercent)
{
    settings.setValue("slideshow/zoomPercent", zoomPercent);
}

double BossaSettings::slideshowTransitionDuration(double defaultTransitionDuration) const
{
    return settings.value("slideshow/transitionDuration", defaultTransitionDuration).toDouble();
}

void BossaSettings::setSlideshowTransitionDuration(double transitionDuration)
{
    settings.setValue("slideshow/transitionDuration", transitionDuration);
}

int BossaSettings::slideshowTransitionStyle(int defaultTransitionStyle) const
{
    return settings.value("slideshow/transitionStyle", defaultTransitionStyle).toInt();
}

void BossaSettings::setSlideshowTransitionStyle(int transitionStyle)
{
    settings.setValue("slideshow/transitionStyle", transitionStyle);
}

int BossaSettings::slideshowTransitionSoftness(int defaultTransitionStyle) const
{
    return settings.value("slideshow/transitionSoftness", defaultTransitionStyle).toInt();
}

void BossaSettings::setSlideshowTransitionSoftness(int transitionSoftness)
{
    settings.setValue("slideshow/transitionSoftness", transitionSoftness);
}

bool BossaSettings::keyframesDragScrub() const
{
    return settings.value("keyframes/dragScrub", false).toBool();
}

void BossaSettings::setKeyframesDragScrub(bool b)
{
    settings.setValue("keyframes/dragScrub", b);
    emit keyframesDragScrubChanged();
}

void BossaSettings::setSubtitlesShowColumn(const QString &column, bool b)
{
    settings.setValue("subtitles/columns/" + column, b);
}

bool BossaSettings::subtitlesShowColumn(const QString &column) const
{
    return settings.value("subtitles/columns/" + column, true).toBool();
}

void BossaSettings::setSubtitlesTrackTimeline(bool b)
{
    settings.setValue("subtitles/trackTimeline", b);
}

bool BossaSettings::subtitlesTrackTimeline() const
{
    return settings.value("subtitles/trackTimeline", true).toBool();
}

void BossaSettings::setSubtitlesShowPrevNext(bool b)
{
    settings.setValue("subtitles/showPrevNext", b);
}

bool BossaSettings::subtitlesShowPrevNext() const
{
    return settings.value("subtitles/showPrevNext", true).toBool();
}

QString BossaSettings::speechLanguage() const
{
    return settings.value("speech/language", QStringLiteral("a")).toString();
}

void BossaSettings::setSpeechLanguage(const QString &code)
{
    settings.setValue("speech/language", code);
}

QString BossaSettings::speechVoice() const
{
    return settings.value("speech/voice", QString()).toString();
}

void BossaSettings::setSpeechVoice(const QString &voiceId)
{
    settings.setValue("speech/voice", voiceId);
}

double BossaSettings::speechSpeed() const
{
    return settings.value("speech/speed", 1.0).toDouble();
}

void BossaSettings::setSpeechSpeed(double speed)
{
    settings.setValue("speech/speed", speed);
}

void BossaSettings::saveCustomColors()
{
    // QColorDialog supports up to 48 custom colors (16 in older versions)
    QStringList colorList;
    for (int i = 0; i < QColorDialog::customCount(); ++i) {
        QColor color = QColorDialog::customColor(i);
        if (color.isValid()) {
            colorList.append(color.name(QColor::HexArgb));
        } else {
            colorList.append(QString());
        }
    }
    settings.setValue("colorDialog/customColors", colorList);
}

void BossaSettings::restoreCustomColors()
{
    QStringList colorList = settings.value("colorDialog/customColors").toStringList();
    for (int i = 0; i < colorList.size() && i < QColorDialog::customCount(); ++i) {
        const QString &colorName = colorList.at(i);
        if (!colorName.isEmpty()) {
            QColor color(colorName);
            if (color.isValid()) {
                // Use rgba() to preserve alpha channel
                QColorDialog::setCustomColor(i, color.rgba());
            }
        }
    }
}

void BossaSettings::setWhisperExe(const QString &path)
{
    settings.setValue("subtitles/whisperExe", path);
}

QString BossaSettings::whisperExe()
{
    QDir dir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
    auto exe = "whisper-cli.exe";
#else
    auto exe = "whisper-cli";
#endif
    return settings.value("subtitles/whisperExe", dir.absoluteFilePath(exe)).toString();
}

void BossaSettings::setWhisperModel(const QString &path)
{
    settings.setValue("subtitles/whisperModel", path);
}

QString BossaSettings::whisperModel()
{
    QDir dataPath = QmlApplication::dataDir();
    dataPath.cd("bossa/whisper_models");
    return settings.value("subtitles/whisperModel", "").toString();
}

void BossaSettings::setWhisperUseGpu(bool b)
{
    settings.setValue("subtitles/whisperUseGpu", b);
}

bool BossaSettings::whisperUseGpu() const
{
    return settings.value("subtitles/whisperUseGpu", true).toBool();
}

void BossaSettings::setNotesZoom(int zoom)
{
    settings.setValue("notes/zoom", zoom);
}

int BossaSettings::notesZoom() const
{
    return settings.value("notes/zoom", 0).toInt();
}

void BossaSettings::reset()
{
    for (auto &key : settings.allKeys()) {
        settings.remove(key);
    }
}

int BossaSettings::undoLimit() const
{
    return settings.value("undoLimit", 50).toInt();
}

bool BossaSettings::warnLowMemory() const
{
    return settings.value("warnLowMemory", true).toBool();
}

int BossaSettings::backupPeriod() const
{
    return settings.value("backupPeriod", 24 * 60).toInt();
}

void BossaSettings::setBackupPeriod(int minutes)
{
    settings.setValue("backupPeriod", minutes);
}

QDateTime BossaSettings::lastBackupDateTime(const QString &filePath) const
{
    return settings.value("lastBackupDateTimeMap").toMap().value(filePath).toDateTime();
}

void BossaSettings::setLastBackupDateTime(const QString &filePath, const QDateTime &dt)
{
    static const int kMaxBackupEntries = 100;
    auto map = settings.value("lastBackupDateTimeMap").toMap();
    if (dt.isValid())
        map[filePath] = dt;
    else
        map.remove(filePath);
    // Prune entries for files that no longer exist.
    for (const auto &path : map.keys())
        if (!QFile::exists(path))
            map.remove(path);
    // If still over the limit, remove the oldest entries.
    while (map.size() > kMaxBackupEntries) {
        map.erase(std::min_element(map.begin(), map.end(), [](const QVariant &a, const QVariant &b) {
            return a.toDateTime() < b.toDateTime();
        }));
    }
    settings.setValue("lastBackupDateTimeMap", map);
}

mlt_time_format BossaSettings::timeFormat() const
{
    return (mlt_time_format) settings.value("timeFormat", mlt_time_clock).toInt();
}

void BossaSettings::setTimeFormat(int format)
{
    settings.setValue("timeFormat", format);
    emit timeFormatChanged();
}

bool BossaSettings::askFlatpakWrappers()
{
    return settings.value("flatpakWrappers", true).toBool();
}

void BossaSettings::setAskFlatpakWrappers(bool b)
{
    settings.setValue("flatpakWrappers", b);
}

QString BossaSettings::dockerPath() const
{
#if defined(Q_OS_MAC)
    return settings.value("dockerPath", "/usr/local/bin/docker").toString();
#elif defined(Q_OS_WIN)
    return settings.value("dockerPath", "C:/Program Files/Docker/Docker/resources/bin/docker.exe")
        .toString();
#else
    return settings.value("dockerPath", "docker").toString();
#endif
}

void BossaSettings::setDockerPath(const QString &path)
{
    settings.setValue("dockerPath", path);
}

QString BossaSettings::chromiumPath() const
{
#if defined(Q_OS_MAC)
    return settings.value("chromiumPath", "/Applications/Google Chrome.app").toString();
#elif defined(Q_OS_WIN)
    return settings.value("chromiumPath", "C:/Program Files/Google/Chrome/Application/chrome.exe")
        .toString();
#else
    return settings.value("chromiumPath", "/usr/bin/chromium-browser").toString();
#endif
}

void BossaSettings::setChromiumPath(const QString &path)
{
    settings.setValue("chromiumPath", path);
}

QString BossaSettings::screenRecorderPath() const
{
    return settings.value("screenRecorderPath", "obs").toString();
}

void BossaSettings::setScreenRecorderPath(const QString &path)
{
    settings.setValue("screenRecorderPath", path);
}
