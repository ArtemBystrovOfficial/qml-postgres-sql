import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3
import CppObjects 1.0

Rectangle {
    id: root
    color:"#222222"
    property int tasksMargin: 30
    property int taskHeight: 30

    signal openTask(int index)

    function addEmptyTask() {
        TaskModel.addEmptyTask();
    }

    ScrollView {
        anchors.fill: parent
        contentHeight: taskContainer.count * (taskHeight + mainColumn.spacing) + tasksMargin
        Item {
            anchors.fill: parent
            Column {
                id: mainColumn
                width: parent.width - tasksMargin
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.topMargin: tasksMargin / 2
                anchors.leftMargin: tasksMargin / 2
                spacing: 2
                Repeater {
                    id: taskContainer
                    model: TaskModel
                    TaskHeader {
                        width: parent.width
                        height: taskHeight
                        header_title: title
                        header_updated_at: updatedAt
                        header_desc: desc
                        onOpened: openTask(index)    
                    }
                }
            }
        }
    }
}