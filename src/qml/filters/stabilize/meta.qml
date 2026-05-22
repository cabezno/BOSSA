import QtQuick
import org.bossa.qml

Metadata {
    type: Metadata.Filter
    name: qsTr("Stabilize")
    keywords: qsTr('smooth deshake', 'search keywords for the Stabilize video filter') + ' vid.stab stabilize #yuv'
    mlt_service: "vidstab"
    qml: "ui.qml"
    icon: 'icon.webp'
    isClipOnly: true
    allowMultiple: false
    help: 'https://forum.bossa.org/t/stabilize/12884/1'

    keyframes {
        allowTrim: false
    }
}
