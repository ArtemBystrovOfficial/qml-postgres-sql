#include "orm_pqxx.hpp"

std::string DataBaseAccess::getFullText(const task_t & task) {
    int id = std::get<0>(task);
    pqxx::work m_work(m_conn);
    pqxx::row out = m_work.exec1((std::string("SELECT COALESCE(full_text,\'\') as text FROM tasks WHERE id = ") + std::to_string(id)).c_str());
    return out[0].as<std::string>();
}

std::shared_ptr<task_list_t> DataBaseAccess::getTaskList(
    std::optional<std::string> search_text) {
    std::shared_ptr<task_list_t> task_list(std::make_unique<task_list_t>());
    pqxx::work m_work(m_conn);

    pqxx::result out = search_text.has_value() ? 
    m_work.exec((std::string("SELECT * FROM search_with_task_header_view (\'") + search_text.value() + std::string("\')")).c_str())
              :  m_work.exec("SELECT * FROM task_header_view");

    for (auto row : out) {
        task_list->push_back(task_t{});
        row.to(task_list->back());
    }
    return task_list;
}

bool DataBaseAccess::addTask(const task_t & task) {
    try {
        pqxx::work m_work(m_conn);
        m_work.exec((std::string("INSERT INTO tasks (title) VALUES (\'") +
                     std::get<1>(task) + std::string("\')").c_str()));
        m_work.commit();
    } catch (...) { return false; }
    return true;
}

void DataBaseAccess::updateTask(const task_t & task) {
  try {
    auto where = std::string("' WHERE id = ") + std::to_string(std::get<0>(task));
    pqxx::work m_work(m_conn);
    m_work.exec((std::string("UPDATE tasks SET title = '") + std::get<1>(task) + where).c_str());
    m_work.exec((std::string("UPDATE tasks SET full_text = '") + std::get<3>(task) + where).c_str());
    m_work.commit();
  } catch (std::exception const &e) {
    std::cerr << e.what() << std::endl;
  }
  //return true;
}
