#pragma once

#include <QAbstractListModel>
#include <set>

namespace Utility::Models {

template <typename DataType>
class ListModel : public QAbstractListModel
{
public:
    Q_DISABLE_COPY(ListModel)
    ListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index = QModelIndex(), int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void insert(int index, const DataType& data);
    void update(int index, const DataType& data);
    void pushBack(const DataType& data);
    void remove(int index);
    void remove(const DataType& data);
    void remove(const std::function<bool(const DataType&)>& comparator);
    void reset();
    void updateSelection(int index, bool exclusive, bool value);
    void resetSelection();
    const QList<DataType>& getRawData() const;
    const std::set<int>& getSelection() const;

    // void move(int from, int to)
    // {
    //     const auto moveToBottom = from < to;
    //     beginMoveRows({}, from, from, {}, to + (moveToBottom ? 1 : 0));
    //     for (int i = from; moveToBottom ? i < to : i > to; moveToBottom ? i++ : i--) {
    //         m_data.swapItemsAt(i, i + (moveToBottom ? 1 : -1));
    //     }
    //     endMoveRows();
    //     emit dataChanged(index(from), index(to));
    // }

private:
    QList<DataType> m_data;
    std::set<int> m_selectionHelper;
};



template<typename DataType>
inline ListModel<DataType>::ListModel(QObject* parent)
{}

template<typename DataType>
inline int ListModel<DataType>::rowCount(const QModelIndex& parent) const
{
    return m_data.size();
}

template<typename DataType>
inline QVariant ListModel<DataType>::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    if (role == Qt::DisplayRole) {
        return QVariant::fromValue(m_data.value(index.row()));
    }

    return {};
}

template<typename DataType>
inline QHash<int, QByteArray> ListModel<DataType>::roleNames() const
{
    return { {Qt::DisplayRole, "modelData"} };
}

template<typename DataType>
inline void ListModel<DataType>::insert(int index, const DataType& data)
{
    if (index < 0 || index > m_data.size()) {
        qWarning() << __PRETTY_FUNCTION__ << " incorrect index provided: " << index;
        return;
    }
    emit beginInsertRows({}, index, index);
    m_data.insert(index, data);
    emit endInsertRows();
}

template<typename DataType>
inline void ListModel<DataType>::update(int index, const DataType& data)
{
    if (index < 0 || index > m_data.size()) {
        qWarning() << __PRETTY_FUNCTION__ << " incorrect index provided: " << index;
        return;
    }
    m_data[index] = data;
    const auto modelIndex = ListModel::index(index);
    emit dataChanged(modelIndex, modelIndex);
}

template<typename DataType>
inline void ListModel<DataType>::pushBack(const DataType& data)
{
    insert(rowCount(), data);
}

template<typename DataType>
inline void ListModel<DataType>::remove(int index)
{
    if (index < 0 || index >= m_data.size()) {
        qWarning() << __PRETTY_FUNCTION__ << " incorrect index provided: " << index;
        return;
    }
    emit beginRemoveRows({}, index, index);
    m_data.remove(index);
    emit endRemoveRows();
}

template<typename DataType>
inline void ListModel<DataType>::remove(const DataType& data)
{
    const auto itemIndex = m_data.indexOf(data);
    if (itemIndex != -1) {
        remove(itemIndex);
    }
}

template<typename DataType>
inline void ListModel<DataType>::remove(const std::function<bool(const DataType&)>& comparator)//NOT TESTED
{
    if (comparator) {
        const auto iter = std::find_if(m_data.cbegin(), m_data.cend(), comparator);
        if (iter != m_data.cend()) {
            remove(std::distance(m_data.cbegin(), iter));
        }
    }
}

template<typename DataType>
inline void ListModel<DataType>::reset()
{
    emit beginResetModel();
    m_data.clear();
    m_selectionHelper.clear();
    emit endResetModel();
}

template<typename DataType>
inline void ListModel<DataType>::updateSelection(int index, bool exclusive, bool value)
{
    if (index < 0 || index >= m_data.size()) {
        qWarning() << __PRETTY_FUNCTION__ << " index is out of bounds: " << index;
        return;
    }

    if (exclusive) {
        resetSelection();
    }

    auto item = m_data.at(index);
    if (item.selected != value) {
        item.selected = value;
        if (item.selected) {
            m_selectionHelper.insert(index);
        } else {
            m_selectionHelper.erase(index);
        }
        update(index, item);
    }
}

template<typename DataType>
inline void ListModel<DataType>::resetSelection()
{
    for (const auto& selectedIndex : m_selectionHelper) {
        if (selectedIndex >= 0 && selectedIndex < m_data.size()) {
            auto item = m_data.at(selectedIndex);
            if (item.selected) {
                item.selected = false;
                update(selectedIndex, item);
            }
        }
    }
    m_selectionHelper.clear();
}

template<typename DataType>
inline const QList<DataType>& ListModel<DataType>::getRawData() const
{
    return m_data;
}

template<typename DataType>
inline const std::set<int> &ListModel<DataType>::getSelection() const
{
    return m_selectionHelper;
}

}
