import QtQuick
import org.bossa.qml

Metadata {
    type: Metadata.Filter
    isAudio: true
    name: qsTr("Compressor")
    keywords: qsTr('loudness dynamics range', 'search keywords for the Compressor audio filter') + ' compressor'
    mlt_service: 'ladspa.1882'
    qml: 'ui.qml'
    help: 'https://forum.bossa.org/t/compressor-audio-filter/12899/1'
}
