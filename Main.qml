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

    Manager {
        id: manager
        function updateGraphics() {
            graphics.camera = getCamera();
        }
    }

    Setting {
        id: setting
        z: 1
        width: 40
        height: 40
        visible: true
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.topMargin: 10
        anchors.rightMargin: 200

        onPlaneSizeChanged: {
            // console.log(planeSize)
            graphics.updatePlane(planeSize, planeSize);
        }
        onHeightFactChanged: {
            graphics.heightFact = heightFact
        }
    }

    Graphics {
        id: graphics
        visible: true
        anchors.fill: parent
        camera: manager.getCamera();
        wireframe: setting.wireframe

    }

    CameraController {
        anchors.fill: parent
        manager: manager
    }

}
