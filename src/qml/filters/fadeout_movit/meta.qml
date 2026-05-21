import QtQuick
import org.bossa.qml

Metadata {
    type: Metadata.Filter
    objectName: 'fadeOutMovit'
    name: qsTr("Fade Out Video")
    keywords: qsTr('brightness lightness opacity alpha', 'search keywords for the Fade Out video filter') + ' fade out video #gpu #10bit'
    mlt_service: "movit.opacity"
    needsGPU: true
    qml: "ui.qml"
    icon: 'icon.webp'
    isFavorite: true
    allowMultiple: false
    help: 'https://forum.bossa.org/t/fade-out-video/12846/1'

    keyframes {
        allowTrim: false
        allowAnimateOut: true
    }
}
