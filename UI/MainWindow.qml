import QtQuick 2.15
import QtQuick.Controls 2.15
import CppObjects 1.0

ApplicationWindow {
    id: root
    visible: true

    maximumHeight: minimumHeight
    maximumWidth: minimumWidth
    minimumHeight: 720
    minimumWidth: 480
    title: "Tasks"
    property int last_index_opened: -1
    //menuBar: MenuBar {
        // ...
    //}

    function openTask(index) {
        mainStackView.push(editArea,
            { "taskItem" : TaskModel.Get(index),
              "fullText" : TaskModel.getFullText(index) }
        )
        last_index_opened = index
    }

    header: MainToolBar {  
        mainStackView: mainStackView
        onNewTask: {
            checkError(TaskModel.addEmpty())
                openTask(0)
        }
        onSearchRequest: TaskModel.searchText(text) 
        onReturnFromTask: {
            mainStackView.get(1).saveChanges();
            mainStackView.pop()
            TaskModel.unloadChanges();
        }                   
    }

    Component {
        id: taskList
        TaskList {
            onOpenedTask: root.openTask(index)
            onDeletedTask: checkError(TaskModel.Delete(index))
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

    function checkError(er) {
        var is = (er === "");
         if(!is)
            errorBar.text = er
        return is
    }

    footer: Label {
        id: errorBar
        text: "ok"
    }
}