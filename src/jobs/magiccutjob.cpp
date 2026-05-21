#include "magiccutjob.h"
#include "settings.h"
#include "Logger.h"
#include <QRegularExpression>
#include <QApplication>
#include <QFileInfo>
#include <QDir>

MagicCutJob::MagicCutJob(const QString &resource, double threshold, double silenceDuration)
    : AbstractJob("Magic Cut Analysis")
    , m_resource(resource)
    , m_threshold(threshold)
    , m_silenceDuration(silenceDuration)
{
}

void MagicCutJob::start()
{
    QStringList args;
    args << "-i" << m_resource
         << "-af" << QString("silencedetect=noise=%1dB:d=%2").arg(m_threshold).arg(m_silenceDuration)
         << "-f" << "null" << "-";

    QFileInfo ffmpegPath(QDir(qApp->applicationDirPath()), "ffmpeg");
    
    // Connect standard error to our parser
    connect(this, &QProcess::readyReadStandardError, this, &MagicCutJob::onReadyRead);
    
    // Let AbstractJob handle the process execution
    AbstractJob::start(ffmpegPath.absoluteFilePath(), args);
    setLabel(tr("Detecting silences..."));
}

void MagicCutJob::onReadyRead()
{
    QString output = readAllStandardError();
    m_logBuffer += output;
    
    // Parse silence_start and silence_end
    QRegularExpression startRegex("silence_start: (\\d+\\.?\\d*)");
    QRegularExpression endRegex("silence_end: (\\d+\\.?\\d*)");
    
    QStringList lines = m_logBuffer.split('\n');
    m_logBuffer = lines.last(); // Keep the partial line
    lines.removeLast();
    
    for (const QString &line : lines) {
        auto startMatch = startRegex.match(line);
        auto endMatch = endRegex.match(line);
        
        static double lastStart = -1.0;
        
        if (startMatch.hasMatch()) {
            lastStart = startMatch.captured(1).toDouble();
        } else if (endMatch.hasMatch() && lastStart >= 0) {
            double end = endMatch.captured(1).toDouble();
            emit silenceDetected(lastStart, end);
            lastStart = -1.0;
        }
    }
    
    AbstractJob::onReadyRead();
}

void MagicCutJob::onFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    setLabel(tr("Analysis Complete"));
    emit progressUpdated(m_item, 100);
    AbstractJob::onFinished(exitCode, exitStatus);
}
