import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {

    property bool drawdVisible: false
    property bool useWireFrame: false
    property bool useImagery: false
    property bool useElevation: false
    property real subdivision: 1

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
                checked: useWireFrame
                onCheckedChanged: useWireFrame = checked
            }

            Switch {
                text: "Elevation"
                checked: useElevation
//                onCheckedChanged: useElevation = checked
            }
            Switch {
                text: "Imagery"
                checked: useImagery
                onCheckedChanged: useImagery = checked
            }

            RowLayout {
                Slider {
                    from: 1
                    to: 16
                    stepSize: 1
                    value: subdivision
                    onValueChanged: subdivision = value
                }
                Label {
                    text: "Subdivision: " + subdivision.toString()
                }
            }
        }
    }
}
