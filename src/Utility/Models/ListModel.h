#pragma once

#include <QAbstractListModel>

namespace Utility::Models {

template <typename DataType>
class ListModel : public QAbstractListModel
{
public:
    Q_DISABLE_COPY(ListModel)
    ListModel(QObject* parent = nullptr)
    {}

    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        return m_data.size();
    }

    QVariant data(const QModelIndex& index = QModelIndex(), int role = Qt::DisplayRole) const override
    {
        if (!index.isValid()) {
            return {};
        }

        if (role == Qt::DisplayRole) {
            return m_data.value(index.row());
        }

        return {};
    }

    QHash<int, QByteArray> roleNames() const override
    {
        return { {Qt::DisplayRole, "modelData"} };
    }

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

    void insert(int index, const DataType& data)
    {
        if (index < 0 || index > m_data.size()) {
            qWarning() << __PRETTY_FUNCTION__ << " incorrect index provided: " << index;
            return;
        }
        emit beginInsertRows({}, index, index);
        m_data.insert(index, data);
        emit endInsertRows();
    }

    void reset()
    {
        emit beginResetModel();
        m_data.clear();
        emit endResetModel();
    }

private:
    QList<DataType> m_data;
};

}
