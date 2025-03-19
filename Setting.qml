import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

Item {

    property bool wireframe: false
    property real planeSize
    property real heightFact
    ColumnLayout {
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
        RowLayout {
            Slider {
                from: 1.0
                to: 30.0
                value:  heightFact
                onValueChanged: heightFact = value
            }
            Label {
                text: "heightFact: " + heightFact.toString()
            }
        }
    }

}
