#include "HeatMapPlugin.h"

#include "ClustersPlugin.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.HeatMapPlugin")

// =============================================================================
// View
// =============================================================================

HeatMapPlugin::~HeatMapPlugin(void)
{
    
}

void HeatMapPlugin::init()
{
    heatmap = new HeatMapWidget();
    heatmap->setPage(":/heatmap/heatmap.html", "qrc:/heatmap/");

    addWidget(heatmap);

    connect(heatmap, SIGNAL(clusterSelectionChanged(QList<int>)), SLOT(clusterSelected(QList<int>)));
    connect(heatmap, SIGNAL(dataSetPicked(QString)), SLOT(dataSetPicked(QString)));
}

void HeatMapPlugin::dataAdded(const QString name)
{
    heatmap->addDataOption(name);
    qDebug() << "Heatmap Data added";
}

void HeatMapPlugin::dataChanged(const QString name)
{
    updateData();
}

void HeatMapPlugin::dataRemoved(const QString name)
{
    
}

void HeatMapPlugin::selectionChanged(const QString dataName)
{

}

QStringList HeatMapPlugin::supportedDataKinds()
{
    QStringList supportedKinds;
    supportedKinds << "Clusters";
    return supportedKinds;
}

void HeatMapPlugin::dataSetPicked(const QString& name)
{
    qDebug() << "DATA PICKED IN HEATMAP";
    updateData();
}

void HeatMapPlugin::clusterSelected(QList<int> selectedClusters)
{
    qDebug() << "CLUSTER SELECTION";
    qDebug() << selectedClusters;
    
    ClusterSet& clusterSet = (ClusterSet&) _core->requestSet(heatmap->getCurrentData());

    const ClustersPlugin& clusterPlugin = clusterSet.getData();

    IndexSet* selection = nullptr;

    int numClusters = clusterPlugin.clusters.size();
    for (int i = 0; i < numClusters; i++)
    {
        IndexSet* cluster = clusterPlugin.clusters[i];
        if (!selection) {
            selection = &dynamic_cast<IndexSet&>(_core->requestSelection(cluster->getDataName()));
            selection->indices.clear();
        }
        
        if (selectedClusters[i]) {
            selection->indices.insert(selection->indices.end(), cluster->indices.begin(), cluster->indices.end());
            _core->notifySelectionChanged(selection->getDataName());
        }
    }
}

void HeatMapPlugin::updateData()
{
    QString currentData = heatmap->getCurrentData();

    qDebug() << "Working on data: " << currentData;
    qDebug() << "Attempting cast to ClusterSet";
    ClusterSet& clusterSet = (ClusterSet&) _core->requestData(currentData);

    qDebug() << "Requesting plugin";
    const ClustersPlugin& clusterPlugin = clusterSet.getData();

    qDebug() << "Calculating data";

    int numClusters = clusterPlugin.clusters.size();
    int numDimensions = 1;

    std::vector<Cluster> clusters;
    clusters.resize(numClusters);
    qDebug() << "Initialize clusters" << numClusters;
    // For every cluster initialize the median, mean, and stddev vectors with the number of dimensions
    for (int i = 0; i < numClusters; i++) {
        const PointsPlugin& points = clusterPlugin.clusters[i]->getData();

        clusters[i]._median.resize(points.getNumDimensions());
        clusters[i]._mean.resize(points.getNumDimensions());
        clusters[i]._stddev.resize(points.getNumDimensions());
        numDimensions = points.getNumDimensions();
    }

    qDebug() << "Calculate cluster statistics";
    for (int i = 0; i < numClusters; i++)
    {
        IndexSet* cluster = clusterPlugin.clusters[i];

        const PointsPlugin& points = cluster->getData();

        for (int d = 0; d < points.getNumDimensions(); d++)
        {
            // Mean calculation
            float mean = 0;

            for (int index : cluster->indices)
                mean += points.data[index * points.getNumDimensions() + d];

            mean /= cluster->indices.size();

            // Standard deviation calculation
            float variance = 0;

            for (int index : cluster->indices)
                variance += pow(points.data[index * points.getNumDimensions() + d] - mean, 2);

            float stddev = sqrt(variance / cluster->indices.size());

            clusters[i]._mean[d] = mean;
            clusters[i]._stddev[d] = stddev;
        }
    }

    qDebug() << "Done calculating data";

    heatmap->setData(clusters, numDimensions);
}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* HeatMapPluginFactory::produce()
{
    return new HeatMapPlugin();
}
