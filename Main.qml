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
        width: 250
        onPlaneSizeChanged: {
            // console.log(planeSize)
            graphics.updatePlaneSize(planeSize, planeSize);
        }
        onNoise_exponentiationChanged: {
            graphics.updatePlaneNoise(noise_scale, noise_octaves, noise_persistence
                                    , noise_lacunarity, noise_exponentiation
                                    , noise_height, noise_type)
        }

        onNoise_octavesChanged: {
            graphics.updatePlaneNoise(noise_scale, noise_octaves, noise_persistence
                                    , noise_lacunarity, noise_exponentiation
                                    , noise_height, noise_type)
        }
        onNoise_persistenceChanged: {
            graphics.updatePlaneNoise(noise_scale, noise_octaves, noise_persistence
                                    , noise_lacunarity, noise_exponentiation
                                    , noise_height, noise_type)
        }
        onNoise_lacunarityChanged: {
            graphics.updatePlaneNoise(noise_scale, noise_octaves, noise_persistence
                                    , noise_lacunarity, noise_exponentiation
                                    , noise_height, noise_type)
        }
        onNoise_heightChanged: {
            graphics.updatePlaneNoise(noise_scale, noise_octaves, noise_persistence
                                    , noise_lacunarity, noise_exponentiation
                                    , noise_height, noise_type)
        }
        onNoise_typeChanged: {
            graphics.updatePlaneNoise(noise_scale, noise_octaves, noise_persistence
                                    , noise_lacunarity, noise_exponentiation
                                    , noise_height, noise_type)
        }
        onMap_heightChanged: {
            graphics.updatePlaneMapHeight(map_height)
        }
        onMap_valueChanged: {
            graphics.updatePlaneMapImage(map_value)
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
