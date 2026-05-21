#ifndef MAGICCUTJOB_H
#define MAGICCUTJOB_H

#include "abstractjob.h"
#include <QStringList>

class MagicCutJob : public AbstractJob
{
    Q_OBJECT
public:
    MagicCutJob(const QString &resource, double threshold = -30.0, double silenceDuration = 0.5);
    void start() override;

signals:
    void silenceDetected(double start, double end);

private slots:
    void onReadyRead();
    void onFinished();

private:
    QString m_resource;
    double m_threshold;
    double m_silenceDuration;
    QString m_logBuffer;
};

#endif // MAGICCUTJOB_H
