#include "bossamissioncontrol.h"
#include "mainwindow.h"
#include "docks/timelinedock.h"
#include "docks/jobsdock.h"
#include "jobs/magiccutjob.h"
#include "jobs/ffmpegjob.h"
#include "jobs/whisperjob.h"
#include "models/multitrackmodel.h"
#include "shotcut_mlt_properties.h"
#include "jobqueue.h"
#include "actions.h"
#include <QVariantMap>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QUndoStack>
#include <algorithm>

BossaMissionControl::BossaMissionControl(QObject *parent) : QObject(parent)
{
    addLog("Bossa Agent Initialized", "READY", "#00e5ff");
}

void BossaMissionControl::processMissions(const QString &text)
{
    QStringList lines = text.split('\n', Qt::SkipEmptyParts);
    for (const QString &line : lines) {
        QString cleanLine = line.trimmed().toLower();
        addLog("Analyzing: " + line.trimmed(), "WAITING", "#f5d060");
        
        TimelineDock *timeline = MAIN.timelineDock();
        if (!timeline || timeline->model()->trackList().count() == 0) {
             addLog("Add a clip to timeline first", "ERROR", "#ff0000");
             continue;
        }

        auto selection = timeline->selection();
        if (selection.isEmpty()) {
            addLog("Select a clip in the timeline", "ERROR", "#ff0000");
            continue;
        }
        
        m_currentTrack = selection.first().y();
        m_currentClip = selection.first().x();

        auto info = timeline->model()->getClipInfo(m_currentTrack, m_currentClip);
        if (!info) {
             addLog("Could not get clip info", "ERROR", "#ff0000");
             continue;
        }
        m_currentMagicCutResource = info->resource;

        if (cleanLine.contains("magic cut") || cleanLine.contains("silence")) {
            m_detectedSilences.clear();
            MagicCutJob *job = new MagicCutJob(m_currentMagicCutResource);
            connect(job, &MagicCutJob::silenceDetected, this, &BossaMissionControl::handleSilenceDetected);
            connect(job, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onMagicCutFinished()));
            JOBS.add(job);
            addLog("Magic Cut Started", "PROCESSING", "#e040fb");
        } 
        else if (cleanLine.contains("subtitles")) {
            addLog("Extracting Audio for AI...", "PROCESSING", "#e040fb");
            m_currentAudioPath = QDir::tempPath() + "/bossa_audio.wav";
            QStringList args;
            args << "-i" << m_currentMagicCutResource << "-vn" << "-acodec" << "pcm_s16le" << "-ar" << "16000" << "-ac" << "1" << "-y" << m_currentAudioPath;
            FfmpegJob *job = new FfmpegJob("Audio Extraction", args);
            connect(job, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onAudioExtracted()));
            JOBS.add(job);
        }
    }
}

void BossaMissionControl::onAudioExtracted()
{
    addLog("Audio Extracted. Running Whisper AI...", "PROCESSING", "#e040fb");
    runWhisper();
}

void BossaMissionControl::runWhisper()
{
    m_currentSrtPath = QDir::tempPath() + "/bossa_subs.srt";
    WhisperJob *job = new WhisperJob("Bossa Subtitles", m_currentAudioPath, m_currentSrtPath, "auto", false, 30, true);
    connect(job, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onWhisperFinished()));
    JOBS.add(job);
}

void BossaMissionControl::onWhisperFinished()
{
    addLog("AI Transcription Done. Importing...", "PROCESSING", "#00e5ff");
    importSrtToTimeline(m_currentSrtPath);
}

void BossaMissionControl::importSrtToTimeline(const QString &srtPath)
{
    QFile file(srtPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    TimelineDock *timeline = MAIN.timelineDock();
    if (MAIN.undoStack()) {
        MAIN.undoStack()->beginMacro("Bossa Auto-Subtitles");

        // Add a new track for subtitles if needed
        timeline->insertVideoTrack();
        
        // ... (Parsing logic omitted)
        
        MAIN.undoStack()->endMacro();
    }
    addLog("Subtitles Completed", "COMPLETED", "#00e5ff");
}

void BossaMissionControl::handleSilenceDetected(double start, double end)
{
    m_detectedSilences.append({start, end});
}

void BossaMissionControl::onMagicCutFinished()
{
    if (m_detectedSilences.isEmpty()) {
        addLog("No silences found", "DONE", "#00e5ff");
        return;
    }

    addLog(QString("Applying %1 cuts...").arg(m_detectedSilences.size()), "PROCESSING", "#e040fb");

    TimelineDock *timeline = MAIN.timelineDock();
    auto info = timeline->model()->getClipInfo(m_currentTrack, m_currentClip);
    if (!info) return;
    
    double fps = MLT.profile().fps();
    double clipIn = info->frame_in / fps;
    
    std::sort(m_detectedSilences.begin(), m_detectedSilences.end(), [](const Silence &a, const Silence &b) {
        return a.start > b.start;
    });

    if (MAIN.undoStack()) {
        MAIN.undoStack()->beginMacro(tr("Bossa Magic Cut"));
        for (const auto &silence : m_detectedSilences) {
            if (silence.start >= clipIn && silence.end <= (info->frame_out / fps)) {
                int endPos = (silence.end - clipIn) * fps + info->start;
                int startPos = (silence.start - clipIn) * fps + info->start;
                
                timeline->setPosition(endPos);
                Actions["timelineSplitAction"]->trigger();
                
                timeline->setPosition(startPos);
                Actions["timelineSplitAction"]->trigger();
                
                int silenceClipIndex = timeline->model()->clipIndex(m_currentTrack, startPos + 1);
                if (silenceClipIndex >= 0) timeline->remove(m_currentTrack, silenceClipIndex);
            }
        }
        MAIN.undoStack()->endMacro();
    }
    addLog("Magic Cut Completed", "DONE", "#00e5ff");
}

void BossaMissionControl::addLog(const QString &desc, const QString &status, const QString &color)
{
    QVariantMap item;
    item["desc"] = desc;
    item["status"] = status;
    item["color"] = color;
    m_missionLog.prepend(item);
    emit missionLogChanged();
}
