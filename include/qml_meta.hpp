#pragma once

#include <QAbstractListModel>
#include <types.hpp>
#include <bitset>
#include <orm_pqxx.hpp>
#include <QQmlApplicationEngine>
//TYPES

#define INT_NULL_PROPERTY(PROPERTY_NAME, INDEX_PROPERTY, DEFAULT) \
    Q_PROPERTY(int PROPERTY_NAME READ PROPERTY_NAME WRITE set_##PROPERTY_NAME NOTIFY PROPERTY_NAME##Changed) \
    Q_SIGNAL void PROPERTY_NAME##Changed(); \
    int PROPERTY_NAME() const { \
        auto& val = std::get<INDEX_PROPERTY>(m_data.tp); \
        return null_values::is_null(val) ? DEFAULT : val; \
    } \
    void set_##PROPERTY_NAME(int id) { \
        std::get<INDEX_PROPERTY>(m_data.tp) = id; \
        m_to_update_.set(INDEX_PROPERTY); \
        emit PROPERTY_NAME##Changed(); \
    }

#define INT_PRIMARY_PROPERTY(PROPERTY_NAME, INDEX_PROPERTY) \
    Q_PROPERTY(int PROPERTY_NAME READ PROPERTY_NAME CONSTANT) \
    int PROPERTY_NAME() const { \
        return std::get<INDEX_PROPERTY>(m_data.tp); \
    }

#define STRING_NULL_PROPERTY(PROPERTY_NAME, INDEX_PROPERTY, DEFAULT) \
    Q_PROPERTY(QString PROPERTY_NAME READ PROPERTY_NAME WRITE set_##PROPERTY_NAME NOTIFY PROPERTY_NAME##Changed) \
    Q_SIGNAL void PROPERTY_NAME##Changed(); \
    QString PROPERTY_NAME() const { \
        auto& val = std::get<INDEX_PROPERTY>(m_data.tp); \
        return null_values::is_null(val) ? DEFAULT : QString::fromStdString(val); \
    } \
    void set_##PROPERTY_NAME(const QString &str) { \
        std::get<INDEX_PROPERTY>(m_data.tp) = str.toStdString(); \
        m_to_update_.set(INDEX_PROPERTY); \
        emit PROPERTY_NAME##Changed(); \
    }

#define META_MODEL_QML_FUNCTIONS \
public: \
    Q_INVOKABLE QString Add(meta_type_t* obj = nullptr) { return add(obj); } \
    Q_INVOKABLE QString CommitChanges() { return update_all_to_bd(); } \
    Q_INVOKABLE QString Delete(int index) { return remove(index); } \
    Q_INVOKABLE meta_type_t* itemAt(int index) { return item_at(index); } 

//#define META_MODEL_REGISTER(CHILD_NAME) \
    //qmlRegisterInterface<CHILD_NAME>(#CHILD_NAME,1);

template <class Tuple>
class MetaQmlObject : public QObject {
public:

    using tuple_t = Tuple;
    using update_pool = std::bitset<Tuple::tuple_size>;

    MetaQmlObject(QObject* parent = nullptr) : QObject(parent) {};


    const Tuple& getData() const { return m_data; }
    void setData(const Tuple& data) { m_data = data; }

    bool isSomeToUpdate() { return m_to_update_.any(); }
    update_pool unload() { update_pool out = m_to_update_; m_to_update_.reset(); return out; }

    ~MetaQmlObject() {}
protected:
    Tuple m_data;
    update_pool m_to_update_;
};

template <class MetaObject>
class MetaQmlModel : public QAbstractListModel {
public:
//CONSTRUCTORS
    explicit MetaQmlModel(DataBaseAccess::FilterSelectPack filter_pack = {},
       QObject * parent = nullptr) : QAbstractListModel(parent), m_fill_pack(filter_pack) {};

//TYPES
    using meta_type_t = MetaObject;
    using tuple_t = MetaObject::tuple_t;
    enum class role_t { item = Qt::UserRole + 1 };

//BASIC OPERATIONS

     QString add(MetaObject* obj = nullptr) {
         DataBaseAccess::Instanse().Insert(obj ? obj->getData() : tuple_t{}, eh);
         if (!eh)
             select_model();
         return QString::fromStdString(eh.what);
     }

     QString update_all_to_bd() {
         bool is_any = false;
         for (auto& elem : m_list)
             if (elem->isSomeToUpdate()) {
                 DataBaseAccess::Instanse().Update(elem->getData(), elem->unload(), eh);
                 if(eh)
                     return QString::fromStdString(eh.what);
                 is_any = true;
             }
         if (is_any)
             select_model();
         return QString::fromStdString(eh.what);
     }

     QString remove(int index) {
         DataBaseAccess::Instanse().Delete(m_list[index]->getData(), eh);
         if (!eh)
             select_model();
         return QString::fromStdString(eh.what);
     }

//WORKING WITH LIST

     meta_type_t* item_at(int index) {
         return m_list[index].get();
     }

//SORT, FILTER AND OTHER SETTINGS

     void set_filter(const DataBaseAccess::FilterSelectPack & pack) {
         m_fill_pack = pack;
     }

     DataBaseAccess::FilterSelectPack& mutable_filter() {
         return m_fill_pack;
     }

//ABSTRACT MODEL
     int rowCount(const QModelIndex& parent = QModelIndex()) const override {
         Q_UNUSED(parent);
         return  m_list.size();
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

protected:

    void select_model() {
        emit beginResetModel();
        auto opt = DataBaseAccess::Instanse().Select<tuple_t>(m_fill_pack, eh);
        if (opt.has_value()) {
            auto& list = opt.value();
            m_list.clear();
            m_list.resize(list.size());
            std::transform(list.begin(), list.end(), m_list.begin(), [this](const tuple_t& task_i) {
                std::unique_ptr<MetaObject> task_o(std::make_unique<MetaObject>(this));
                task_o->setData(task_i);
                return std::move(task_o);
            });
        }
        emit endResetModel();
    }

    std::vector<std::unique_ptr<MetaObject>> m_list;

private:
    DataBaseAccess::FilterSelectPack m_fill_pack;
    DataBaseAccess::ExceptionHandler eh;
};
