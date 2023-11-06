#include <qml_task_model.hpp>
#include <orm_pqxx.hpp>

TaskModel::TaskModel(QObject* parent)
    : QAbstractListModel(parent) {
    selectModel();
}

void TaskModel::addEmptyTask() {
    if (DataBaseAccess::Instanse().addTask({0, "No name", "", ""})) 
        selectModel();
}

void TaskModel::searchText(const QString& str) {
    m_current_filter_search = str;
    selectModel();
}

QString TaskModel::getFullText(int index) {
    return QString::fromStdString(DataBaseAccess::Instanse().getFullText(m_temp_list_->at(index)));
}

Q_INVOKABLE QString TaskModel::getTitle(int index) {
    return QString::fromStdString(std::get<1>(m_temp_list_->at(index))); 
}

Q_INVOKABLE void TaskModel::saveTask(int index,const QString& title, const QString& full_text) {
    DataBaseAccess::Instanse().updateTask({std::get<0>(m_temp_list_->at(index)),
                                            title.toStdString(), 
                                            "",
                                            full_text.toStdString()});
    selectModel();
}

int TaskModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return m_temp_list_ ? m_temp_list_->size() : 0;
}

QVariant TaskModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() && m_temp_list_)
        return QVariant();

    int idx = index.row();  

    auto& task = m_temp_list_->at(idx);

    QVariant out;
    switch (static_cast<role_t>(role)) {
      case role_t::id: out  = std::get<0>(task);break;
      case role_t::title: out = QString::fromStdString(std::get<1>(task)); break;
      case role_t::updatedAt: out = QString::fromStdString(std::get<2>(task)); break;
      case role_t::desc: out = QString::fromStdString(std::get<3>(task)); break;
    }

    return out;
}

QHash<int, QByteArray> TaskModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[static_cast<int>(role_t::id)] = "id";
    roles[static_cast<int>(role_t::title)] = "title";
    roles[static_cast<int>(role_t::updatedAt)] = "updatedAt";
    roles[static_cast<int>(role_t::desc)] = "desc";
    return roles;
}

void TaskModel::selectModel() { 
    emit beginResetModel(); 
            m_temp_list_ = m_current_filter_search.isEmpty()
            ? DataBaseAccess::Instanse().getTaskList()
            : DataBaseAccess::Instanse().getTaskList(m_current_filter_search.toStdString());
    emit endResetModel();
}
