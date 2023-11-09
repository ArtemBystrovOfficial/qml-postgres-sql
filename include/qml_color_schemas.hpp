#pragma once

#include <QAbstractListModel>
#include <types.hpp>
#include <bitset>
#include <QObject>
#include <QColor>

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

    explicit ColorSchemeModel(QObject* parent = nullptr);

    Q_INVOKABLE ColorScheme* Get(int index);
    Q_INVOKABLE ColorScheme* GetById(int id);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    void selectModel();

    std::vector<std::unique_ptr<ColorScheme>> m_list;
};