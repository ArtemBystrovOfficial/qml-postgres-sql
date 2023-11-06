#pragma once

#include <qml_task_model.hpp>
#include <memory>

class QmlSingletonModels {
public:
  static QmlSingletonModels& Instanse();

private: 
	QmlSingletonModels();
	std::unique_ptr<TaskModel> m_task_model;
};