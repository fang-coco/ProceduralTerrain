import QtQuick
import QtQuick3D
import QtQuick3D.Helpers
import ProceduralTerrain

Window {
    id: window
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    Setting {
        id: setting
        width: 250
    }

    Graphics {
        id: graphics
        visible: true
        anchors.fill: parent
        useElevation: setting.useElevation
        useImagery: setting.useImagery
        subdivision: setting.subdivision
        useWireFrame: setting.useWireFrame
    }

    CameraController {
        anchors.fill: parent
        graphics: graphics
    }

}
