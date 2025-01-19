#pragma once

#include <QAbstractListModel>

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
    void reset();
    const QList<DataType>& getRawData();

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
inline void ListModel<DataType>::reset()
{
    emit beginResetModel();
    m_data.clear();
    emit endResetModel();
}

template<typename DataType>
inline const QList<DataType>& ListModel<DataType>::getRawData()
{
    return m_data;
}

}
