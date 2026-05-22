import QtQuick
import org.bossa.qml

Metadata {
    type: Metadata.Filter
    name: qsTr('Reflect')
    keywords: qsTr('mirror repeat', 'search keywords for the Reflect video filter') + ' reflect #yuv'
    objectName: 'reflect'
    mlt_service: 'mirror'
    qml: "ui.qml"
    icon: 'icon.webp'
    help: 'https://forum.bossa.org/t/reflect-video-filter/29279/1'
}
