#include "Set.h"
#include "DataHierarchyItem.h"

namespace hdps
{

const DataHierarchyItem& DatasetImpl::getDataHierarchyItem() const
{
    return const_cast<DatasetImpl*>(this)->getDataHierarchyItem();
}

DataHierarchyItem& DatasetImpl::getDataHierarchyItem()
{
    return _core->getDataHierarchyItem(_guid);
}

hdps::Dataset<hdps::DatasetImpl> DatasetImpl::getParent() const
{
    return getDataHierarchyItem().getParent().getDataset();
}

QVector<Dataset<DatasetImpl>> DatasetImpl::getChildren(const QVector<DataType>& dataTypes /*= QVector<DataType>()*/) const
{
    // Found children
    QVector<Dataset<DatasetImpl>> children;

    // Loop over all data hierarchy children and add to the list if occur in the data types
    for (auto dataHierarchyChild : getDataHierarchyItem().getChildren())
        if (dataTypes.contains(dataHierarchyChild->getDataType()))
            children << dataHierarchyChild->getDataset();

    return children;
}

std::int32_t DatasetImpl::getSelectionSize() const
{
    return static_cast<std::int32_t>(const_cast<DatasetImpl*>(this)->getSelectionIndices().size());
}

void DatasetImpl::lock()
{
    getDataHierarchyItem().setLocked(true);
}

void DatasetImpl::unlock()
{
    getDataHierarchyItem().setLocked(false);
}

bool DatasetImpl::isLocked() const
{
    return getDataHierarchyItem().getLocked();
}

std::int32_t DatasetImpl::getGroupIndex() const
{
    return _groupIndex;
}

void DatasetImpl::setGroupIndex(const std::int32_t& groupIndex)
{
    _groupIndex = groupIndex;

    _core->notifyDataSelectionChanged(this);
}

void DatasetImpl::addAction(hdps::gui::WidgetAction& widgetAction)
{
    _core->getDataHierarchyItem(_guid).addAction(widgetAction);
}

hdps::gui::WidgetActions DatasetImpl::getActions() const
{
    return _core->getDataHierarchyItem(_guid).getActions();
}

QMenu* DatasetImpl::getContextMenu(QWidget* parent /*= nullptr*/)
{
    return _core->getDataHierarchyItem(_guid).getContextMenu(parent);
}

void DatasetImpl::populateContextMenu(QMenu* contextMenu)
{
    return _core->getDataHierarchyItem(_guid).populateContextMenu(contextMenu);
}

}
