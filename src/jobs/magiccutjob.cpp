#include "magiccutjob.h"
#include "settings.h"
#include "Logger.h"
#include <QRegularExpression>

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

    m_process.setProgram(Settings.ffmpegPath());
    m_process.setArguments(args);
    
    connect(&m_process, &QProcess::readyReadStandardError, this, &MagicCutJob::onReadyRead);
    connect(&m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onFinished()));
    
    m_process.start();
    setStatus(tr("Detecting silences..."));
}

void MagicCutJob::onReadyRead()
{
    QString output = m_process.readAllStandardError();
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
}

void MagicCutJob::onFinished()
{
    setStatus(tr("Analysis Complete"));
    setPercent(100);
}
