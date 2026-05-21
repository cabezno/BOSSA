import QtQuick
import org.bossa.qml

Metadata {
    type: Metadata.Filter
    name: qsTr("Sketch")
    keywords: qsTr('drawing painting cartoon', 'search keywords for the Sketch video filter') + ' sketch #yuv'
    mlt_service: "charcoal"
    qml: "ui.qml"
    icon: 'icon.webp'
    help: 'https://forum.bossa.org/t/sketch/12882/1'
}
