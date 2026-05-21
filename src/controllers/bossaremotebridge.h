#ifndef BOSSAREMOTEBRIDGE_H
#define BOSSAREMOTEBRIDGE_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>

class BossaRemoteBridge : public QObject
{
    Q_OBJECT
public:
    explicit BossaRemoteBridge(QObject *parent = nullptr);
    bool start(int port = 8080);

private slots:
    void onNewConnection();
    void onReadyRead();

private:
    void processCommand(const QJsonObject &json, QTcpSocket *socket);
    void sendResponse(QTcpSocket *socket, const QJsonObject &response);

    QTcpServer *m_server;
};

#endif // BOSSAREMOTEBRIDGE_H
