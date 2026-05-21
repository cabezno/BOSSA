#include "bossaremotebridge.h"
#include "mainwindow.h"
#include "docks/timelinedock.h"
#include "models/multitrackmodel.h"
#include "player.h"
#include "actions.h"
#include "Logger.h"
#include <QJsonArray>

BossaRemoteBridge::BossaRemoteBridge(QObject *parent) : QObject(parent), m_server(new QTcpServer(this))
{
    connect(m_server, &QTcpServer::newConnection, this, &BossaRemoteBridge::onNewConnection);
}

bool BossaRemoteBridge::start(int port)
{
    if (m_server->listen(QHostAddress::Any, port)) {
        LOG_INFO() << "Bossa Remote Bridge (Total Control) listening on port" << port;
        return true;
    }
    LOG_ERROR() << "Failed to start Bossa Remote Bridge on port" << port;
    return false;
}

void BossaRemoteBridge::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();
        connect(socket, &QTcpSocket::readyRead, this, &BossaRemoteBridge::onReadyRead);
        connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    }
}

void BossaRemoteBridge::onReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!socket) return;

    QByteArray data = socket->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isObject()) {
        processCommand(doc.object(), socket);
    }
}

void BossaRemoteBridge::processCommand(const QJsonObject &json, QTcpSocket *socket)
{
    QString command = json["command"].toString();
    QJsonObject response;
    response["status"] = "ok";
    response["command"] = command;

    if (command == "ping") {
        response["message"] = "Bossa is alive and ready for orders.";
    } 
    else if (command == "timeline_split") {
        int position = json["position"].toInt(-1);
        TimelineDock *timeline = MAIN.timelineDock();
        if (timeline) {
            if (position >= 0) timeline->setPosition(position);
            Actions["timelineSplitAction"]->trigger();
            response["message"] = "Split executed at " + QString::number(timeline->position());
        }
    }
    else if (command == "timeline_remove") {
        int track = json["track"].toInt();
        int clip = json["clip"].toInt();
        if (MAIN.timelineDock()) {
            MAIN.timelineDock()->remove(track, clip);
            response["message"] = QString("Clip %1 on track %2 removed.").arg(clip).arg(track);
        }
    }
    else if (command == "player_play") {
        if (MAIN.player()) MAIN.player()->play();
    }
    else if (command == "player_pause") {
        if (MAIN.player()) MAIN.player()->pause();
    }
    else if (command == "get_project_info") {
        QJsonObject info;
        info["file"] = MAIN.fileName();
        info["fps"] = MLT.profile().fps();
        if (MAIN.timelineDock() && MAIN.timelineDock()->model() && MAIN.timelineDock()->model()->tractor())
            info["duration"] = MAIN.timelineDock()->model()->tractor()->get_length();
        response["data"] = info;
    }
    else {
        response["status"] = "error";
        response["message"] = "Unknown command: " + command;
    }

    sendResponse(socket, response);
}

void BossaRemoteBridge::sendResponse(QTcpSocket *socket, const QJsonObject &response)
{
    socket->write(QJsonDocument(response).toJson());
    socket->flush();
}
