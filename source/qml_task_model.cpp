#include <qml_task_model.hpp>
#include <orm_pqxx.hpp>

TaskModel::TaskModel(QObject* parent)
    : QAbstractListModel(parent) {
    selectModel();
}

bool TaskModel::addEmpty() {
    bool is = DataBaseAccess::Instanse().Insert(task_t{});
    if (is)
        selectModel();
    return is;
}

void TaskModel::searchText(const QString& str) {
    m_current_filter_search = str;
    selectModel();
}

QString TaskModel::getFullText(int index) {
    auto opt = DataBaseAccess::Instanse().specialSelect11<std::string>(
        std::format("SELECT full_text FROM {} WHERE id = {} ", types_impl::tuple_info_name<task_t>(),
        m_list[index]->id())
     );
    return QString::fromStdString(opt.has_value() ? opt.value() : "");
}

Q_INVOKABLE void TaskModel::unloadChanges() {
    bool is_any = false;
    for (auto& elem : m_list) 
        if (elem->isSomeToUpdate()) {
            DataBaseAccess::Instanse().Update(elem->getData(), elem->unload());
            is_any = true;
        }
    if (is_any)
        selectModel();
}

Q_INVOKABLE bool TaskModel::Delete(int index) {
    bool is = DataBaseAccess::Instanse().Delete(m_list[index]->getData());
    if(is)
        selectModel();
    return is;
}

Q_INVOKABLE Task* TaskModel::Get(int index) {
    return m_list[index].get();
}

int TaskModel::rowCount(const QModelIndex& parent) const {
    Q_UNUSED(parent);
    return  m_list.size();
}

QVariant TaskModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return {};

    switch (static_cast<role_t>(role)) {
        case role_t::item: return QVariant::fromValue(m_list[index.row()].get()); break;
    }

    return {};
}

QHash<int, QByteArray> TaskModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[static_cast<int>(role_t::item)] = "item";
    return roles;
}

void TaskModel::selectModel() { 
    emit beginResetModel(); 
    auto opt = DataBaseAccess::Instanse().Select<task_t>(DataBaseAccess::FilterSelectPack{
            false,
            "updated_at",
            (m_current_filter_search.isEmpty() ? std::nullopt : std::make_optional(m_current_filter_search.toStdString())),
            {"title", "full_text"}
    });
    if (opt.has_value()) {
        auto& list = opt.value();
        m_list.clear();
        m_list.resize(list.size());
        std::transform(list.begin(), list.end(), m_list.begin(), [this](const task_t& task_i) {
            std::unique_ptr<Task> task_o(std::make_unique<Task>(this));
            task_o->setData(task_i);
            return std::move(task_o);
        });
    }
    emit endResetModel();
}
