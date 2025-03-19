import QtQuick
import QtQuick3D
import QtQuick3D.Helpers
import ProceduralTerrain

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    CameraController {
        anchors.fill: parent
        manager: manager
    }


    Manager {
        id: manager
        function updateGraphics() {
            graphics.camera = getCamera();
        }
    }

    Graphics {
        id: graphics
        visible: true
        anchors.fill: parent
        camera: manager.getCamera();
    }

}
