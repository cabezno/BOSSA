import QtQuick
import org.bossa.qml

Metadata {
    type: Metadata.Filter
    isAudio: true
    name: qsTr("Limiter")
    keywords: qsTr('dynamics range loudness', 'search keywords for the Limiter audio filter') + ' limiter'
    mlt_service: 'ladspa.1913'
    qml: 'ui.qml'
    help: 'https://forum.bossa.org/t/limiter-audio-filter/12908/1'
}
