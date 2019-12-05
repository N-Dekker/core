#pragma once

#include <RawData.h>
#include <Set.h>

#include <QString>
#include <QColor>

#include <vector>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

struct Cluster
{
    std::vector<unsigned int> indices;

    QString _name;
    QColor _color;
    std::vector<float> _median;
    std::vector<float> _mean;
    std::vector<float> _stddev;
};

class ClusterData : public RawData
{
public:
    ClusterData() : RawData("Cluster") { }
    ~ClusterData(void) override;
    
    void init() override;

    hdps::DataSet* createDataSet() const override;

    std::vector<Cluster>& getClusters()
    {
        return _clusters;
    }

    void addCluster(Cluster& cluster) {
        _clusters.push_back(cluster);
    }

private:
    std::vector<Cluster> _clusters;
};

// =============================================================================
// Cluster Set
// =============================================================================

class Clusters : public hdps::DataSet
{
public:
    Clusters(hdps::CoreInterface* core, QString dataName) : hdps::DataSet(core, dataName) { }
    ~Clusters() override { }

    ClusterData& getRawData() const
    {
        return dynamic_cast<ClusterData&>(_core->requestRawData(getDataName()));
    }

    std::vector<Cluster>& getClusters()
    {
        return getRawData().getClusters();
    }

    void addCluster(Cluster& cluster)
    {
        getRawData().addCluster(cluster);
    }

    DataSet* copy() const override
    {
        Clusters* clusters = new Clusters(_core, getDataName());
        clusters->setName(getName());
        clusters->indices = indices;
        return clusters;
    }

    void createSubset() const override
    {
        const hdps::DataSet& selection = _core->requestSelection(getDataName());

        _core->createSubsetFromSelection(selection, getDataName(), "Clusters");
    }

    std::vector<unsigned int> indices;
};


// =============================================================================
// Factory
// =============================================================================

class ClusterDataFactory : public RawDataFactory
{
    Q_INTERFACES(hdps::plugin::RawDataFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.ClusterData"
                      FILE  "ClusterData.json")
    
public:
    ClusterDataFactory(void) {}
    ~ClusterDataFactory(void) override {}
    
    RawData* produce() override;
};
