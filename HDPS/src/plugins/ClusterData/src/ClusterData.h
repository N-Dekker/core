#pragma once

#include "clusterdata_export.h"

#include "ClusterDataVisitor.h"
#include "Cluster.h"
#include "event/EventListener.h"

#include <RawData.h>
#include <Set.h>

#include <QString>
#include <QColor>
#include <QUuid>

#include <vector>

using namespace hdps;

const hdps::DataType ClusterType = hdps::DataType(QString("Clusters"));

class InfoAction;

class CLUSTERDATA_EXPORT ClusterData : public hdps::plugin::RawData, public hdps::util::Visitable<ClusterDataVisitor>
{
public:
    ClusterData(const hdps::plugin::PluginFactory* factory);
    ~ClusterData(void) override;

    void init() override;

    /** Create dataset for raw data */
    Dataset<DatasetImpl> createDataSet() const override;

    /** Returns reference to the clusters */
    std::vector<Cluster>& getClusters();

    /** Accept cluster data visitor for visiting
     * @param visitor Reference to visitor that will visit this component
     */
    void accept(ClusterDataVisitor* visitor) const override;

    /**
     * Adds a cluster
     * @param cluster Cluster to add
     */
    void addCluster(Cluster& cluster);

    /**
     * Removes a cluster by its unique identifier
     * @param id Unique identifier of the cluster to remove
     */
    void removeClusterById(const QString& id);

    /**
     * Remove clusters by their unique identifiers
     * @param ids Unique identifiers of the clusters to remove
     */
    void removeClustersById(const QStringList& ids);

    /**
     * Get cluster index by name
     * @param clusterName Name of the cluster to search for
     * @return Index of the cluster (-1 if not found)
     */
    std::int32_t getClusterIndex(const QString& clusterName) const;

private:
    std::vector<Cluster>    _clusters;      /** Clusters data */
};

// =============================================================================
// Cluster Set
// =============================================================================

class CLUSTERDATA_EXPORT Clusters : public DatasetImpl, public hdps::EventListener
{
public:
    Clusters(CoreInterface* core, QString dataName) : DatasetImpl(core, dataName) { }
    ~Clusters() override { }

    void init() override;

    std::vector<Cluster>& getClusters()
    {
        return getRawData<ClusterData>().getClusters();
    }

    const std::vector<Cluster>& getClusters() const
    {
        return getRawData<ClusterData>().getClusters();
    }

    /**
     * Adds a cluster
     * @param cluster Cluster to add
     */
    void addCluster(Cluster& cluster);

    /**
     * Removes a cluster by its unique identifier
     * @param id Unique identifier of the cluster to remove
     */
    void removeClusterById(const QString& id);

    /**
     * Remove clusters by their unique identifiers
     * @param ids Unique identifiers of the clusters to remove
     */
    void removeClustersById(const QStringList& ids);

    /**
     * Get a copy of the dataset
     * @return Smart pointer to copy of dataset
     */
    Dataset<DatasetImpl> copy() const override
    {
        auto clusters = new Clusters(_core, getRawDataName());

        clusters->setGuiName(getGuiName());
        clusters->indices = indices;
        
        return Dataset<DatasetImpl>(clusters);
    }

    /**
     * Create subset and specify where the subset will be placed in the data hierarchy
     * @param guiName Name of the subset in the GUI
     * @param parentDataSet Smart pointer to parent dataset in the data hierarchy (default is below the set)
     * @param visible Whether the subset will be visible in the UI
     * @return Smart pointer to the created subset
     */
    Dataset<DatasetImpl> createSubset(const QString& guiName, const Dataset<DatasetImpl>& parentDataSet = Dataset<DatasetImpl>(), const bool& visible = true) const  override
    {
        return _core->createSubsetFromSelection(getSelection(), toSmartPointer(), guiName, parentDataSet, visible);
    }

    /** Get icon for the dataset */
    QIcon getIcon() const override;

    /** Loads cluster from variant list */
    void fromVariant(const QVariant& variant);

    /** Returns a variant representation of the clusters */
    QVariant toVariant() const;

    /** Gets concatenated indices for all selected clusters */
    std::vector<std::uint32_t> getSelectedIndices() const;

    /** Get names of the selected clusters */
    QStringList getSelectedClusterNames() const;

    /** Select clusters by name */
    void setSelection(const QStringList& clusterNames);

    /**
     * Set selection
     * @param indices Selection indices
     */
    void setSelection(const std::vector<std::uint32_t>& indices) override;

    std::vector<unsigned int>       indices;
    QSharedPointer<InfoAction>      _infoAction;        /** Shared pointer to info action */
};

// =============================================================================
// Factory
// =============================================================================

class ClusterDataFactory : public hdps::plugin::RawDataFactory
{
    Q_INTERFACES(hdps::plugin::RawDataFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "hdps.ClusterData"
                      FILE  "ClusterData.json")
    
public:
    ClusterDataFactory(void) {}
    ~ClusterDataFactory(void) override {}
    
    /** Returns the plugin icon */
    QIcon getIcon() const override;

    hdps::plugin::RawData* produce() override;
};
