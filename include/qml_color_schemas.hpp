#pragma once

#include <QAbstractListModel>
#include <types.hpp>
#include <bitset>
#include <QObject>
#include <QColor>
#include <orm_pqxx.hpp>

class ColorScheme : public QObject {
    Q_OBJECT

    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(QColor color READ color CONSTANT)

signals:
    void titleChanged();
    void updatedAtChanged();
    void descChanged();

public:

    ColorScheme(QObject* parent = nullptr) : QObject(parent) {};

    int id() const { return std::get<0>(m_data.tp); };
    QColor color() const { return QString::fromStdString(std::get<1>(m_data.tp)); }

    const Color_Tuple& getData() const { return m_data; }
    void setData(const Color_Tuple& data) { m_data = data; }

private:
    Color_Tuple m_data;
};

class ColorSchemeModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum class role_t { item = Qt::UserRole + 1 };

    explicit ColorSchemeModel(QObject* parent = nullptr) : QAbstractListModel(parent) {
        selectModel();
    }

    Q_INVOKABLE ColorScheme* Get(int index) {
        return m_list[index].get();
    }

    Q_INVOKABLE ColorScheme* GetById(int id) {
        auto it = std::find_if(m_list.begin(), m_list.end(), [=](auto & ptr) { return id == ptr->id(); });
        return (it == m_list.end() ? nullptr : it->get());
    }

    int rowCount(const QModelIndex& parent = QModelIndex()) const override {
        return m_list.size();
    }
    QVariant data(const QModelIndex& index, int role) const override {
        if (!index.isValid())
            return {};

        switch (static_cast<role_t>(role)) {
        case role_t::item: return QVariant::fromValue(m_list[index.row()].get()); break;
        }

        return {};
    }
    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[static_cast<int>(role_t::item)] = "item";
        return roles;
    }

private:
    void selectModel() {
        emit beginResetModel();
        auto opt = DataBaseAccess::Instanse().Select<Color_Tuple>(DataBaseAccess::FilterSelectPack{
                std::nullopt,
                "",
                std::nullopt,
                {}
        });
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

    std::vector<std::unique_ptr<ColorScheme>> m_list;
};