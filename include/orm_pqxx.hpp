#pragma once 

#include <iostream>
#include <stdexcept>
#include <pqxx/pqxx>
#include <tuple>
#include <qml_task.hpp>

class DataBaseAccess {
 public:
  static DataBaseAccess & Instanse() {
    static DataBaseAccess base;
    return base;
  }

  std::string getFullText(const task_t &);

  std::shared_ptr<task_list_t> getTaskList(std::optional<std::string> search_text = std::nullopt);
  bool addTask(const task_t &); 
  void updateTask(const task_t &);

  //void updateTask(task_list_t);
  //void updateTask(task_list_t);



 private:
  DataBaseAccess() 
      : m_conn(
            "user=postgres "
            "host=localhost "
            "password=9000 "
            "dbname=task_app"
      ) {}
  pqxx::connection m_conn;
};