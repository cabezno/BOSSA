#ifndef BOSSAMISSIONCONTROL_H
#define BOSSAMISSIONCONTROL_H

#include <QObject>
#include <QStringList>
#include <QVariantList>

class BossaMissionControl : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList missionLog READ missionLog NOTIFY missionLogChanged)

public:
    explicit BossaMissionControl(QObject *parent = nullptr);

    Q_INVOKABLE void processMissions(const QString &text);
    QVariantList missionLog() const { return m_missionLog; }

public slots:
    void handleSilenceDetected(double start, double end);
    void onMagicCutFinished();
    void onAudioExtracted();
    void onWhisperFinished();

signals:
    void missionLogChanged();

private:
    void addLog(const QString &desc, const QString &status, const QString &color);
    void runWhisper();
    void importSrtToTimeline(const QString &srtPath);

    QVariantList m_missionLog;
    struct Silence {
        double start;
        double end;
    };
    QList<Silence> m_detectedSilences;
    QString m_currentMagicCutResource;
    QString m_currentAudioPath;
    QString m_currentSrtPath;
    int m_currentTrack;
    int m_currentClip;
};

#endif // BOSSAMISSIONCONTROL_H
