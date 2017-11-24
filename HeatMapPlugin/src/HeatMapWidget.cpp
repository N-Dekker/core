#include "HeatMapWidget.h"

#include "ClustersPlugin.h"
#include "util/FileUtil.h"

#include <QWebView>
#include <QWebFrame>

#include <QVBoxLayout>

#include <cassert>

HeatMapWidget::HeatMapWidget()
{
    Q_INIT_RESOURCE(resources);
}

HeatMapWidget::~HeatMapWidget()
{

}

void HeatMapWidget::addDataOption(const QString option)
{
    _dataOptions.addItem(option);
    qt_addAvailableData(option);
}

QString HeatMapWidget::getCurrentData() const
{
    return _dataOptions.currentText();
}

void HeatMapWidget::setData(const std::vector<Cluster>& clusters, const int numDimensions)
{
    std::string _jsonObject = "";

    unsigned int numClusters = clusters.size();
    qDebug() << "Setting data";
    //// Nodes
    std::string nodes = "\"nodes\":[\n";
    for (int i = 0; i < numClusters; i++)
    {
        nodes = nodes + "{\"name\":\"" + "Cluster name " + std::to_string(i) + "\", ";
        nodes = nodes + "\"size\":" + std::to_string(numClusters) + ", ";
        nodes = nodes + "\"expression\":[";

        for (int j = 0; j < numDimensions; j++)
        {
            nodes = nodes + std::to_string(clusters[i]._mean[j]);

            if (j < numDimensions - 1) nodes = nodes + ",";
        }
        nodes = nodes + "], \"stddev\":[";
        for (int j = 0; j < numDimensions; j++)
        {
            nodes = nodes + std::to_string(clusters[i]._stddev[j]);

            if (j < numDimensions - 1) nodes = nodes + ",";
        }
        if (i < numClusters - 1) nodes = nodes + "]},\n";
        else nodes = nodes + "]}\n]";
    }

    // TODO: multi files
    std::string names = ",\n\"names\":[";
    for (int i = 0; i < numDimensions; i++) {

        names = names + "\"" + "markerName " + std::to_string(i) + "\"";

        if (i < numDimensions - 1) names = names + ",";
        else names = names + "]";
    }

    _jsonObject = "{\n" + nodes + names + "\n}";

    qDebug() << _jsonObject.c_str();

    qt_setData(QString(_jsonObject.c_str()));
}

void HeatMapWidget::mousePressEvent(QMouseEvent *event)
{

}

void HeatMapWidget::mouseMoveEvent(QMouseEvent *event)
{

}

void HeatMapWidget::mouseReleaseEvent(QMouseEvent *event)
{

}

void HeatMapWidget::onSelection(QRectF selection)
{

}

void HeatMapWidget::cleanup()
{

}

void HeatMapWidget::connectJs()
{
    qDebug() << "CONNECT JS";
    registerFunctions(this);
}

void HeatMapWidget::webViewLoaded(bool ok)
{
    qDebug() << "HEATMAP LOADED!" << ok;
}

void HeatMapWidget::js_selectData(QString name)
{
    qDebug() << "SELECTED SOME DATA: " << name;
    int index = _dataOptions.findText(name);
    _dataOptions.setCurrentIndex(index);
}

void HeatMapWidget::js_highlightUpdated(int highlightId)
{
    //if (!_data) return;
    //_data->setHighlight(highlightId);
}

void HeatMapWidget::js_selectionUpdated(QList<int> selectedClusters)
{
    emit clusterSelectionChanged(selectedClusters);
}
