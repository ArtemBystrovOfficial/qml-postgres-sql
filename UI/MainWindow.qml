import QtQuick 2.15
import QtQuick.Controls 2.15
import CppObjects 1.0

ApplicationWindow {
    visible: true
    width: 480
    height: 720
    title: "Tasks"
    property int last_index_opened: -1
    //menuBar: MenuBar {
        // ...
    //}

    header: MainToolBar {  
        mainStackView: mainStackView
        onNewTask: mainStackView.get(0).addEmptyTask()
        onSearchRequest: TaskModel.searchText(text) 
        onReturnFromTask: {
            TaskModel.saveTask(last_index_opened,
                               mainStackView.get(1).title,
                               mainStackView.get(1).desc)
            mainStackView.pop()
        }                   
    }

    Component {
        id: taskList
        TaskList {
            onOpenTask: {
                mainStackView.push(editArea,
                    {"title" : TaskModel.getTitle(index),
                    "desc" : TaskModel.getFullText(index)}
                )
                last_index_opened = index
            }
        }
    }

    Component {
        id: editArea
        EditArea {}
    }

    //footer: TabBar {
    //    // ...
    //}

    StackView {
        id: mainStackView
        anchors.fill: parent
    }
    Component.onCompleted: {
        mainStackView.push(taskList)
    }
}