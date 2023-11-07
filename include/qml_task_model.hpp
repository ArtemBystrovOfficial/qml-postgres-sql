#pragma once

#include <QAbstractListModel>
#include <types.hpp>
#include <bitset>

//TODO import meta factory object and models from another project and adapt from proto to tuples

class Task : public QObject {
    Q_OBJECT

    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QString updatedAt READ updatedAt WRITE setUpdatedAt NOTIFY updatedAtChanged)
    Q_PROPERTY(QString desc READ desc WRITE setDesc NOTIFY descChanged)

signals:
    void titleChanged();
    void updatedAtChanged();
    void descChanged();

public:
    using update_pool = std::bitset<std::tuple_size_v<task_t>>;

    Task(QObject* parent = nullptr) : QObject(parent) {};

    int id() const { return std::get<0>(m_data); };
    QString title() const { return QString::fromStdString(std::get<1>(m_data)); }
    QString updatedAt() const { return QString::fromStdString(std::get<2>(m_data)); }
    QString desc() const { return QString::fromStdString(std::get<3>(m_data)); }

    void setTitle(const QString& str) { std::get<1>(m_data) = str.toStdString(); m_to_update_.set(1); emit titleChanged(); };
    void setUpdatedAt(const QString& str) { std::get<2>(m_data) = str.toStdString(); m_to_update_.set(2); emit updatedAtChanged(); };
    void setDesc(const QString& str) { std::get<3>(m_data) = str.toStdString(); m_to_update_.set(3); emit descChanged(); };

    const task_t & getData() const { return m_data; }
    void setData(const task_t& data) { m_data = data; }
    
    bool isSomeToUpdate() { return m_to_update_.any(); } 
    update_pool unload() { update_pool out = m_to_update_; m_to_update_.reset(); return out; }
private:
    task_t m_data;
    update_pool m_to_update_;
};

class TaskModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit TaskModel(QObject* parent = nullptr);

    Q_INVOKABLE bool addEmpty();
    Q_INVOKABLE void searchText(const QString &);
    Q_INVOKABLE void unloadChanges();
    Q_INVOKABLE Task* Get(int index);
    Q_INVOKABLE bool Delete(int index);

    Q_INVOKABLE QString getFullText(int index);

    enum class role_t { item = Qt::UserRole + 1};

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

private:
    void selectModel();

    QString m_current_filter_search;
    std::vector<std::unique_ptr<Task>> m_list;
};