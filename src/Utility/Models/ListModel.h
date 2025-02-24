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
    void pushBack(const QList<DataType>& dataList);
    void remove(int index);
    void remove(const DataType& data);
    void remove(const std::function<bool(const DataType&)>& comparator);
    void reset();
    void updateSelection(int index, bool exclusive, bool value);
    void updateSelection(int startIndex, int endIndex, bool exclusive, bool value);
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
    beginInsertRows({}, index, index);
    m_data.insert(index, data);
    endInsertRows();
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
inline void ListModel<DataType>::pushBack(const QList<DataType>& dataList)
{
    if (dataList.isEmpty()) {
        return;
    }

    int startIndex = rowCount();
    beginInsertRows({}, startIndex, startIndex + dataList.count() - 1);
    for (const auto& data: dataList) {
        m_data.push_back(data);
    }
    endInsertRows();
}

template<typename DataType>
inline void ListModel<DataType>::remove(int index)
{
    if (index < 0 || index >= m_data.size()) {
        qWarning() << __PRETTY_FUNCTION__ << " incorrect index provided: " << index;
        return;
    }
    beginRemoveRows({}, index, index);
    m_data.remove(index);
    endRemoveRows();
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
    beginResetModel();
    m_data.clear();
    m_selectionHelper.clear();
    endResetModel();
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
inline void ListModel<DataType>::updateSelection(int startIndex, int endIndex, bool exclusive, bool value)
{
    if (startIndex < 0 || startIndex >= m_data.size() || endIndex < 0 || endIndex >= m_data.size()) {
        qWarning() << __PRETTY_FUNCTION__ << " some index is out of bounds " << startIndex << " " << endIndex;
        return;
    }

    if (exclusive) {
        resetSelection();
    }

    int from = 0;
    int to = 0;

    if (startIndex <= endIndex) {
        from = startIndex;
        to = endIndex;
    } else {
        from = endIndex;
        to = startIndex;
    }

    for (int i = from; i <= to; i++) {
        auto& item = m_data[i];
        if (item.selected != value) {
            item.selected = value;
            if (item.selected) {
                m_selectionHelper.insert(i);
            } else {
                m_selectionHelper.erase(i);
            }
        }
    }
    emit dataChanged(ListModel::index(from), ListModel::index(to));
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
