import QtQuick
import QtQuick3D
import QtQuick3D.Helpers

Item {
    id: item
    property var pos: Qt.vector3d(0, 0, 30)
    property var rot: Qt.vector3d(0, 0, 0)
    property real far: 1000
    property real near: 0.01
    property real fov: 45
    // required property var manager
    required property var graphics

    Node {
        id: originNode
        PerspectiveCamera {
            id: cameraNode
            // frustumCullingEnabled: true
            clipFar: far
            clipNear: near
            fieldOfView: fov
            // position: pos
            eulerRotation: rot
            position: pos
            onZChanged: {
                // console.log("z change: ", scenePosition.z)
                if (z < 7) z = 7
                originNode.updateManagerCamera()
            }
        }
        onPositionChanged:  {
            // console.log("position change")
            updateManagerCamera()
        }

        onRotationChanged: {
            // console.log("rotation change")
            updateManagerCamera()
        }

        function updateManagerCamera() {
            graphics.updateCamera(cameraNode.fieldOfView
                               , Qt.size(item.width, item.height)
                               , cameraNode.clipNear, cameraNode.clipFar
                               , cameraNode.scenePosition, cameraNode.forward // 这里都要用scene coordinate
                               , cameraNode.up)
            // console.log(Qt.size(item.width, item.height));
            // console.log(cameraNode.scenePosition);
            // manager.updateGraphics();
        }
        Component.onCompleted: updateManagerCamera()
    }

    OrbitCameraController {
        origin: originNode
        camera: cameraNode
    }

    onWidthChanged: originNode.updateManagerCamera()
    onHeightChanged: originNode.updateManagerCamera()
}
