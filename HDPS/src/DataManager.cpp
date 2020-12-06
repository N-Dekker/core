#include "DataManager.h"

#include <QRegularExpression>
#include <cassert>
#include <iostream>

namespace hdps
{

void DataManager::addRawData(RawData* rawData)
{
    _rawDataMap.emplace(rawData->getName(), std::unique_ptr<RawData>(rawData));
}

QString DataManager::addSet(QString requestedName, DataSet* set)
{
    QString uniqueName = getUniqueSetName(requestedName);
    set->setName(uniqueName);
    _dataSetMap.emplace(set->getName(), std::unique_ptr<DataSet>(set));

    emit dataChanged();
    return uniqueName;
}

void DataManager::addSelection(QString dataName, DataSet* selection)
{
    _selections.emplace(dataName, std::unique_ptr<DataSet>(selection));
}

void DataManager::renameSet(QString oldName, QString requestedName)
{
    auto& dataSet = _dataSetMap[oldName];

    // Find a unique name from the requested name and set it in the dataset
    QString newName = getUniqueSetName(requestedName);
    dataSet->setName(newName);

    // Put the renamed set back into the map
    _dataSetMap.emplace(newName, std::unique_ptr<DataSet>(dataSet.release()));

    // Erase the old entry in the map
    _dataSetMap.erase(oldName);

    emit dataChanged();
}

QStringList DataManager::removeRawData(QString name)
{
    // Convert any derived data referring to this data to non-derived data
    for (auto& pair : _rawDataMap)
    {
        RawData& rawData = *pair.second;

        // Set as non-derived data
        if (rawData.isDerivedData() && rawData.getSourceDataName() == name)
        {
            rawData.setDerived(false, QString());
        }
        
        // Generate a selection set for the previously derived data
        DataSet* selection = rawData.createDataSet();
        addSelection(rawData.getName(), selection);
    }

    // Remove any sets referring to this data, and keep track of the removed set names
    QStringList removedSets;
    for (auto it = _dataSetMap.begin(); it != _dataSetMap.end();)
    {
        const DataSet& set = *it->second;
        if (set.getDataName() == name)
        {
            removedSets.append(set.getName());
            it = _dataSetMap.erase(it);
        }
        else
            it++;
    }

    // Remove the selection belonging to the raw data
    _selections.erase(name);

    // Remove the raw data
    _rawDataMap.erase(name);

    return removedSets;
}

RawData& DataManager::getRawData(QString name)
{
    if (_rawDataMap.find(name) == _rawDataMap.end())
        throw DataNotFoundException(name);

    return *_rawDataMap[name];
}

DataSet& DataManager::getSet(QString name)
{
    if (_dataSetMap.find(name) == _dataSetMap.end())
        throw SetNotFoundException(name);

    return *_dataSetMap[name];
}

DataSet& DataManager::getSelection(QString name)
{
    RawData& rawData = getRawData(name);
    if (rawData.isDerivedData())
    {
        return getSelection(rawData.getSourceData().getName());
    }

    DataSet* selection = _selections[name].get();

    if (!selection)
        throw SelectionNotFoundException(name);

    return *selection;
}

const std::unordered_map<QString, std::unique_ptr<DataSet>>& DataManager::allSets() const
{
    return _dataSetMap;
}

const QString DataManager::getUniqueSetName(QString request)
{
    for (const auto& pair : allSets())
    {
        const DataSet& set = *pair.second;
        if (set.getName() == request)
        {
            // Index in the string where the underscore followed by digits starts
            int index = request.lastIndexOf(QRegularExpression("_\\d+"));

            // If the regular expression was not found create the first copy
            if (index == -1)
            {
                return getUniqueSetName(request + "_1");
            }
            else
            {
                // Number of characters used by the digits we need to replace
                int numChars = request.length() - (index + 1);
                // The digit we want to increment and place back
                int digit = request.right(numChars).toInt() + 1;
                return getUniqueSetName(request.left(request.length() - numChars) + QString::number(digit));
            }
        }
    }
    return request;
}

} // namespace hdps
