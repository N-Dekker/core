#ifndef HDPS_DATASET_H
#define HDPS_DATASET_H

#include "RawData.h"

#include "CoreInterface.h"

#include <QString>
#include <QVector>
#include <memory>

namespace hdps
{

class DataSet
{
public:
    DataSet(CoreInterface* core, QString dataName) :
        _core(core),
        _dataName(dataName),
        _all(false),
        _rawData(nullptr)
    {
        
    }

    virtual ~DataSet() {}

    virtual DataSet* copy() const = 0;

    virtual QString createSubset() const = 0;

    QString getName() const
    {
        return _name;
    }

    void setName(QString name)
    {
        _name = name;
    }

    /**
     * Returns true if this set represents the full data and false if it's a subset.
     */
    bool isFull() const
    {
        return _all;
    }

    bool isDerivedData() const
    {
        return _derived;
    }

    /**
     * Returns the data type of the raw data associated with this dataset.
     */
    DataType getDataType() const
    {
        return _core->requestRawData(getDataName()).getDataType();
    }

    /**
     * If the given set is derived from another set, then calls this function on the other set.
     * If the given set is not derived from another set, then returns the given set.
     */
    template <class T>
    static T& getSourceData(T& set)
    {
        return set.isDerivedData() ? getSourceData(static_cast<T&>(set._core->requestData(set._sourceSetName))) : set;
    }

    /**
     * Gets the selection associated with this data set. If the data set is
     * derived then the selection of the source data will be returned. Otherwise,
     * the selection of the set's data will be returned.
     *
     * @return The selection associated with this data set
     */
    DataSet& getSelection() const
    {
        return _core->requestSelection(getSourceData(*this).getDataName());
    }

    void setSelection(std::vector<unsigned int> indices)
    {

    }

public: // Properties

    /**
     * Get property in variant form
     * @param name Name of the property
     * @param defaultValue Default value
     * @return Property in variant form
     */
    QVariant getProperty(const QString& name, const QVariant& defaultValue = QVariant()) const
    {
        return _rawData->getProperty(name, defaultValue);
    }

    /**
    * Set property
    * @param name Name of the property
    * @param value Property value
    */
    void setProperty(const QString& name, const QVariant& value)
    {
        _rawData->setProperty(name, value);
    }

    /**
    * Determines whether a property with a give name exists
    * @param name Name of the property
    * @param value If property with the given name exists
    */
    bool hasProperty(const QString& name) const
    {
        return _rawData->hasProperty(name);
    }

    /** Returns a list of available property names */
    QStringList propertyNames() const
    {
        return _rawData->propertyNames();
    }

protected:
    template <class DataType>
    DataType& getRawData() const
    {
        if (_rawData == nullptr)
            _rawData = &dynamic_cast<DataType&>(_core->requestRawData(getDataName()));
        return *static_cast<DataType*>(_rawData);
    }

    QString getDataName() const
    {
        return _dataName;
    }

    QString getSourceName() const
    {
        return _sourceSetName;
    }

    /**
     * Set whether this set represents all the data or only a subset
     */
    void setAll(bool all)
    {
        _all = all;
    }

    CoreInterface* _core;
private:
    mutable RawData* _rawData;

    QString _name;
    QString _dataName;
    bool _all;

    bool _derived = false;
    QString _sourceSetName;

    friend class Core;
    friend class DataManager;
    friend class EventListener;
};

} // namespace hdps

#endif // HDPS_DATASET_H
