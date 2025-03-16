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

#include "ListModel.h"
#include <set>

namespace Utility::Models {

template <typename T>
concept Selectable = requires(T) {
    {
        T::selected
    };
};

template <Selectable DataType>
class SelectionListModel : public ListModel<DataType>
{
public:
    Q_DISABLE_COPY(SelectionListModel)

    SelectionListModel(QObject* parent = nullptr);
    void reset() override;
    void updateSelection(int index, bool exclusive, bool value);
    void updateSelection(int startIndex, int endIndex, bool exclusive, bool value);
    void resetSelection();

    const std::set<int>& getSelection() const;

protected:
    std::set<int> m_selectionHelper;
};

template <Selectable DataType>
inline SelectionListModel<DataType>::SelectionListModel(QObject* parent) : ListModel<DataType>(parent)
{}

template <Selectable DataType>
inline void SelectionListModel<DataType>::reset()
{
    this->beginResetModel();
    this->m_data = {};
    m_selectionHelper = {};
    this->endResetModel();
}

template <Selectable DataType>
inline void SelectionListModel<DataType>::updateSelection(int index, bool exclusive, bool value)
{
    if (index < 0 || index >= this->m_data.size()) {
        qWarning() << __PRETTY_FUNCTION__ << " index is out of bounds: " << index;
        return;
    }

    if (exclusive) {
        resetSelection();
    }

    auto item = this->m_data.at(index);
    if (item.selected != value) {
        item.selected = value;
        if (item.selected) {
            m_selectionHelper.insert(index);
        } else {
            m_selectionHelper.erase(index);
        }
        this->update(index, item);
    }
}

template <Selectable DataType>
inline void SelectionListModel<DataType>::updateSelection(int startIndex, int endIndex, bool exclusive, bool value)
{
    if (startIndex < 0 || startIndex >= this->m_data.size() || endIndex < 0 || endIndex >= this->m_data.size()) {
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
        auto& item = this->m_data[i];
        if (item.selected != value) {
            item.selected = value;
            if (item.selected) {
                m_selectionHelper.insert(i);
            } else {
                m_selectionHelper.erase(i);
            }
        }
    }
    emit this->dataChanged(SelectionListModel::index(from), SelectionListModel::index(to));
}

template <Selectable DataType>
inline void SelectionListModel<DataType>::resetSelection()
{
    for (const auto& selectedIndex : m_selectionHelper) {
        if (selectedIndex >= 0 && selectedIndex < this->m_data.size()) {
            auto item = this->m_data.at(selectedIndex);
            if (item.selected) {
                item.selected = false;
                this->update(selectedIndex, item);
            }
        }
    }
    m_selectionHelper = {};
}

template <Selectable DataType>
inline const std::set<int> &SelectionListModel<DataType>::getSelection() const
{
    return m_selectionHelper;
}

}
