/*
 * Logger - simple log viewer application.
 * Copyright (C) 2025 Dmytro Martyniuk <digitizenetwork@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

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

    void addUserRole(int role, const QByteArray& name, const std::function<QVariant(const DataType&)>& handler);
    void insert(int index, const DataType& data);
    void update(int index, const DataType& data);
    void update(int index, const std::function<void(DataType&)>& updater);
    void pushBack(const DataType& data);
    void pushBack(const QList<DataType>& dataList);
    void remove(int index);
    void remove(const DataType& data);
    void move(int from, int to);
    virtual void reset();
    const QList<DataType>& getRawData() const;

protected:
    QList<DataType> m_data;
    QHash<int, QByteArray> m_roles;
    QHash<int, std::function<QVariant(const DataType&)>> m_userRoles;
};


//TODO_SUPER_LOW: maybe add automatic roles creation from data type?
//SUPER_LOW as app almost always needs whole data and only for 1 usecase custom role is needed and it can be set manually
template <typename DataType>
inline ListModel<DataType>::ListModel(QObject* parent)
{
    addUserRole(Qt::DisplayRole, "modelData", [](const auto& value) {
        return QVariant::fromValue(value);
    });
}

template <typename DataType>
inline int ListModel<DataType>::rowCount(const QModelIndex& parent) const
{
    return m_data.size();
}

template <typename DataType>
inline QVariant ListModel<DataType>::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return {};
    }


    const auto handler = m_userRoles.value(role);
    if (handler) {
        return handler(m_data.value(index.row()));
    } else {
        qWarning() << __PRETTY_FUNCTION__ << " request for non existent role " << role;
    }

    return {};
}

template <typename DataType>
inline QHash<int, QByteArray> ListModel<DataType>::roleNames() const
{
    return m_roles;
}

template <typename DataType>
inline void ListModel<DataType>::addUserRole(int role, const QByteArray& name, const std::function<QVariant(const DataType&)>& handler)
{
    m_roles.insert(role, name);
    m_userRoles.insert(role, handler);
}

template <typename DataType>
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

template <typename DataType>
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
inline void ListModel<DataType>::update(int index, const std::function<void(DataType&)>& updater)
{
    if (index < 0 || index > m_data.size()) {
        qWarning() << __PRETTY_FUNCTION__ << " incorrect index provided: " << index;
        return;
    }
    updater(m_data[index]);
    const auto modelIndex = ListModel::index(index);
    emit dataChanged(modelIndex, modelIndex);
}

template <typename DataType>
inline void ListModel<DataType>::pushBack(const DataType& data)
{
    insert(rowCount(), data);
}

template <typename DataType>
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

template <typename DataType>
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

template <typename DataType>
inline void ListModel<DataType>::remove(const DataType& data)
{
    const auto itemIndex = m_data.indexOf(data);
    if (itemIndex != -1) {
        remove(itemIndex);
    }
}

template <typename DataType>
inline void ListModel<DataType>::move(int from, int to)
{
    const auto moveToBottom = from < to;
    beginMoveRows({}, from, from, {}, to + (moveToBottom ? 1 : 0));
    for (int i = from; moveToBottom ? i < to : i > to; moveToBottom ? i++ : i--) {
        m_data.swapItemsAt(i, i + (moveToBottom ? 1 : -1));
    }
    endMoveRows();
    emit dataChanged(index(from), index(to));
}

template <typename DataType>
inline void ListModel<DataType>::reset()
{
    beginResetModel();
    m_data = {};
    endResetModel();
}

template <typename DataType>
inline const QList<DataType>& ListModel<DataType>::getRawData() const
{
    return m_data;
}

}
