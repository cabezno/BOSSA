import QtQuick

pragma Singleton

QtObject {
    // SODA FUSION Palette - Hyper Vibrant
    readonly property color voidBlack: "#000000"
    readonly property color surface: "#050505"
    readonly property color raised: "#0a0a0a"
    readonly property color cyan: "#00e5ff"
    readonly property color magenta: "#e040fb"
    readonly property color white: "#ffffff"
    readonly property color muted: "#546e7a"
    readonly property color glass: "rgba(255, 255, 255, 0.05)"
    readonly property color glowCyan: "rgba(0, 229, 255, 0.6)"
    readonly property color glowMagenta: "rgba(224, 64, 251, 0.6)"

    // UX Elements
    readonly property int paddingSmall: 4
    readonly property int paddingMedium: 10
    readonly property int paddingLarge: 20
    readonly property int borderRadius: 2
    
    // Fonts
    readonly property string mainFont: "Inter"
    readonly property int fontSizeSmall: 9
    readonly property int fontSizeMedium: 11
    readonly property int fontSizeLarge: 13
}
