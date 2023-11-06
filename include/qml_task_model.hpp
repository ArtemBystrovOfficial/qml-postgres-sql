#pragma once

#include <QAbstractListModel>
#include <qml_task.hpp>

class TaskModel : public QAbstractListModel {
    Q_OBJECT

public:
    explicit TaskModel(QObject* parent = nullptr);

    Q_INVOKABLE void addEmptyTask();
    Q_INVOKABLE void searchText(const QString &);

    Q_INVOKABLE QString getFullText(int index);
    Q_INVOKABLE QString getTitle(int index);
    Q_INVOKABLE void saveTask(int index, const QString& title, const QString& full_text);

    // Define the roles for your model (e.g., displayRole for the data)
    enum class role_t {
        id = Qt::UserRole + 1,
        title,
        updatedAt,
        desc
    };

    // Implement the required virtual functions for QAbstractListModel
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role) const override;

    QHash<int, QByteArray> roleNames() const override;

private:
    void selectModel();

    QString m_current_filter_search;
    mutable std::shared_ptr<task_list_t> m_temp_list_;
};