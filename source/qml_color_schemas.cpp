#include <qml_color_schemas.hpp>
#include <orm_pqxx.hpp>

ColorSchemeModel::ColorSchemeModel(QObject* parent) : QAbstractListModel(parent) {
    selectModel();
}

ColorScheme* ColorSchemeModel::Get(int index) {
    return m_list[index].get();
}

ColorScheme* ColorSchemeModel::GetById(int id) {
    auto it = std::find_if(m_list.begin(), m_list.end(), [=](auto& ptr) { return id == ptr->id(); });
    return (it == m_list.end() ? nullptr : it->get());
}

int ColorSchemeModel::rowCount(const QModelIndex& parent) const {
    return m_list.size();
}

QVariant ColorSchemeModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid())
        return {};

    switch (static_cast<role_t>(role)) {
    case role_t::item: return QVariant::fromValue(m_list[index.row()].get()); break;
    }

    return {};
}

QHash<int, QByteArray> ColorSchemeModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[static_cast<int>(role_t::item)] = "item";
    return roles;
}

void ColorSchemeModel::selectModel() {
    emit beginResetModel();
    DataBaseAccess::ExceptionHandler _;
    auto opt = DataBaseAccess::Instanse().Select<Color_Tuple>(DataBaseAccess::FilterSelectPack{
        std::nullopt,
        "",
        std::nullopt,
        {}
        }, _);
    if (opt.has_value()) {
        auto& list = opt.value();
        m_list.clear();
        m_list.resize(list.size());
        std::transform(list.begin(), list.end(), m_list.begin(), [this](const Color_Tuple& task_i) {
            std::unique_ptr<ColorScheme> task_o(std::make_unique<ColorScheme>(this));
            task_o->setData(task_i);
            return std::move(task_o);
            });
    }
    emit endResetModel();
}
