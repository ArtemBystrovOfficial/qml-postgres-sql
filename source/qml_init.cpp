#include <qml_init.hpp>

#include <QQmlApplicationEngine>

#define REGISTER_QML_TYPES "CppObjects", 1, 0

QmlSingletonModels& QmlSingletonModels::Instanse() {
	static QmlSingletonModels model;
	return model;
}

QmlSingletonModels::QmlSingletonModels() 
: m_task_model(std::make_unique <TaskModel>()) {
//TastModel
    qmlRegisterSingletonType<TaskModel>(REGISTER_QML_TYPES, "TaskModel",
    [&](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject* {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return m_task_model.get();
    });
}
