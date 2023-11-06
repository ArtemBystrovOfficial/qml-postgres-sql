import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Rectangle {
    property string header_title: "-"
    property string header_updated_at : "-"
    property string header_desc: "-"

    signal opened()

    id: rootTask
    height: 30
    color: "#555555"
    radius: 4
    RowLayout {
        spacing: 0
        anchors.fill: parent
        Loader {
            id: title
            sourceComponent: cellInRow
            onLoaded: {
                item.text = header_title
                item.scale = 0.2
                item.isTitle = true
            }
        }
        Loader {
            id: updatedAt
            sourceComponent: cellInRow
            onLoaded: {
                item.text = header_updated_at
                item.scale = 0.4
                item.isTitle = false
            }
        }
        Loader {
            id: desc
            sourceComponent: cellInRow
            onLoaded: {
                item.text = header_desc
                item.scale = 0.4
                item.isTitle = false
            }
        }
    }

    Component {
        id: cellInRow
        Rectangle {
            property alias text: cellTitle.text
            property real scale
            property color colorBackground
            property bool isTitle: false
            color: isTitle ? "#444444" : "#555555"
            opacity: mouseArea.containsMouse ? 0.8 : 1
            height: rootTask.height
            width: 430 * scale
            Text {
                width: parent.width - 20
                elide: Text.ElideRight
                anchors.centerIn: parent
                font.family: "Helvetica"
                font.pointSize: 9
                font.bold: isTitle
                color: "white"
                id: cellTitle
            }
            MouseArea {
                id: mouseArea
                anchors.fill: parent
                hoverEnabled: true
                onClicked: opened()
            }
            ScaleAnimator {
                target: parent
                from: 1;
                to: 1.1;
                duration: 25
                running: mouseArea.containsMouse
            }
            ScaleAnimator {
                target: parent
                from: 1.1;
                to: 1;
                duration: 12
                running: !mouseArea.containsMouse
            }
        }
    }

}