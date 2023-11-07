#include <qml_init.hpp>

#include <QQmlApplicationEngine>

#define REGISTER_QML_TYPES "CppObjects", 1, 0

QmlSingletonModels& QmlSingletonModels::Instanse() {
	static QmlSingletonModels model;
	return model;
}

QmlSingletonModels::QmlSingletonModels() {
//TaskModel
    static auto task_model = new TaskModel;

    qmlRegisterType<Task>(REGISTER_QML_TYPES, "Task");
    qmlRegisterSingletonType<TaskModel>(REGISTER_QML_TYPES, "TaskModel",
    [&](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject* {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return task_model;
    });
}
