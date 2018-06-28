#pragma once

#include "RawData.h"

#include "Set.h"
#include <QString>
#include <vector>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class PointsPlugin : public RawData
{
public:
    PointsPlugin() : RawData("Points") { }
    ~PointsPlugin(void);
    
    void init();

    unsigned int getNumPoints() const
    {
        return data.size() / numDimensions;
    }

    unsigned int getNumDimensions() const
    {
        return numDimensions;
    }

    hdps::Set* createSet() const;

    std::vector<QString> dimNames;
    std::vector<float> data;
    unsigned int numDimensions = 1;
};

class IndexSet : public hdps::Set
{
public:
    IndexSet(hdps::CoreInterface* core, QString dataName) : Set(core, dataName) { }
    ~IndexSet() { }
    
    PointsPlugin& getData() const
    {
        return dynamic_cast<PointsPlugin&>(_core->requestData(getDataName()));
    }

    Set* copy() const override;

    std::vector<unsigned int> indices;
};

// =============================================================================
// Factory
// =============================================================================

class PointsPluginFactory : public RawDataFactory
{
    Q_INTERFACES(hdps::plugin::RawDataFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.PointsPlugin"
                      FILE  "PointsPlugin.json")
    
public:
    PointsPluginFactory(void) {}
    ~PointsPluginFactory(void) {}
    
    RawData* produce();
};
