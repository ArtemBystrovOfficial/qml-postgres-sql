import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

Rectangle {
    property alias title: titleArea.text
    property alias desc: descArea.text
    id: root
    //width: 480
    //height: 600
    
    color:"#444444"

    ColumnLayout {
        anchors.fill:parent
        spacing:0
        TextArea {
            id: titleArea
            color: "white"
            font.family: "Helvetica"
            font.pointSize: 12
            onTextChanged: {
                if (text.length > 50)
                    text = text.slice(0, -1); 
            }
            Layout.leftMargin: 10
            width: 200
            height: 30
        }
        Rectangle {
            color: "silver"
            Layout.leftMargin: 10
            width: 200
            height: 1
        }
        TextArea {
            id: descArea
            wrapMode: Text.WordWrap
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "white"
            font.family: "Helvetica"
            font.pointSize: 9
        }
    }

}