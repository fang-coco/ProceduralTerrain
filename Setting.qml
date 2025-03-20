import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {

    property bool drawdVisible: false
    property bool wireframe: false
    property real planeSize

    property bool noiseVisible: false
    property real noise_scale
    property real noise_octaves
    property real noise_persistence
    property real noise_lacunarity
    property real noise_exponentiation
    property real noise_height
    property int  noise_type

    property bool hightmapVisible: false
    property real map_height : 0
    property string map_value : "heightmap-hi.png"

    z: 1
    anchors.right: parent.right

    Button {
        id: but
        width: parent.width
        text: drawdVisible ? "Close" : "Setting"
        onClicked: {
            drawdVisible = !drawdVisible
        }
    }
    Rectangle {
        id: drawer
        anchors.top: but.bottom
        height: drawdVisible ? layout.height : 0
        width: parent.width
        ColumnLayout {
            id: layout
            anchors.fill: parent
            visible: drawdVisible
            spacing: 5
            Switch {
                text: "WireFrame"
                checked: wireframe
                onCheckedChanged: wireframe = checked
            }

            RowLayout {
                Slider {
                    from: 5
                    to: 500
                    stepSize: 1
                    value:  planeSize
                    onValueChanged: planeSize = value
                }
                Label {
                    text: "planeSize: " + planeSize.toString()
                          + "x" + planeSize.toString()
                }
            }

            // 添加分割线
            Rectangle {
                width: parent.width
                height: 1
                color: "gray"
                opacity: 0.5
            }

            Label {
                width: parent.width
                text: "Noise Properities"
                background: Rectangle {
                    color: "black"
                    opacity: noiseVisible ? 0.6 : 0.2
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: noiseVisible = !noiseVisible
                }
            }
            Column {
                visible: noiseVisible

                RowLayout {
                    RadioButton {
                        text: "Perlin"
                        onCheckedChanged: noise_type = checkable ? 1 : 0
                    }
                    RadioButton {
                        text: "Simpler2"
                        onCheckedChanged: noise_type = checkable ? 0 : 1
                    }
                }

                RowLayout {
                    Slider {
                        from: 64.0
                        to: 1024.0
                        stepSize: 0.5
                        value:  noise_scale
                        onValueChanged: noise_scale = value
                    }
                    Label {
                        text: "scale: " + noise_scale.toString()
                    }
                }
                RowLayout {
                    Slider {
                        from: 1.0
                        to: 20.0
                        stepSize: 1.0
                        value:  noise_octaves
                        onValueChanged: noise_octaves = value
                    }
                    Label {
                        text: "ovtaves: " + noise_octaves.toString()
                    }
                }

                RowLayout {
                    Slider {
                        from: 0.01
                        to: 1.0
                        stepSize: 0.01
                        value:  noise_persistence
                        onValueChanged: noise_persistence = value
                    }
                    Label {
                        text: "persistence: " + noise_persistence.toString()
                    }
                }
                RowLayout {
                    Slider {
                        from: 0.01
                        to: 4.0
                        stepSize: 0.01
                        value:  noise_lacunarity
                        onValueChanged: noise_lacunarity = value
                    }
                    Label {
                        text: "lacunarity: " + noise_lacunarity.toString()
                    }
                }

                RowLayout {
                    Slider {
                        from: 0.1
                        to: 10.0
                        stepSize: 0.5
                        value:  noise_exponentiation
                        onValueChanged: noise_exponentiation = value
                    }
                    Label {
                        text: "exponentiation: " + noise_exponentiation.toString()
                    }
                }
                RowLayout {
                    Slider {
                        from: 0.0
                        to: 256.0
                        stepSize: 0.5
                        value:  noise_height
                        onValueChanged: noise_height= value
                    }
                    Label {
                        text: "height: " + noise_height.toString()
                    }
                }

            }

            // 添加分割线
            Rectangle {
                width: parent.width
                height: 1
                color: "gray"
                opacity: 0.5
            }
            Label {
                width: parent.width
                text: "HightMap"
                background: Rectangle {
                    color: "black"
                    opacity: hightmapVisible ? 0.6 : 0.2
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked: hightmapVisible = !hightmapVisible
                }
            }
            Column {
                visible: hightmapVisible

                RowLayout {
                    Label {
                        text: "Image: "
                    }

                    ComboBox {
                        model: [
                            "heightmap-hi.png",
                            "heightmap-test.jpg"
                        ]
                        onCurrentIndexChanged: {
                            map_value = model[currentIndex]
                            // console.log(map_value)
                        }

                    }
                }

                RowLayout {
                    Slider {
                        from: 0.0
                        to: 256.0
                        stepSize: 0.5
                        value:  map_height
                        onValueChanged: map_height = value
                    }
                    Label {
                        text: "height: " + map_height.toString()
                    }
                }

            }



            // 添加分割线
            Rectangle {
                width: parent.width
                height: 1
                color: "gray"
                opacity: 0.5
            }
        }
    }
}
