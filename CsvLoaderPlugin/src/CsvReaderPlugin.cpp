#include "CsvReaderPlugin.h"

#include "PointsPlugin.h"
#include "Set.h"

#include <QtCore>
#include <QtDebug>

#include <QFileDialog>
#include <vector>
#include <QInputDialog>

Q_PLUGIN_METADATA(IID "nl.tudelft.CsvReaderPlugin")

// =============================================================================
// View
// =============================================================================

CsvReaderPlugin::~CsvReaderPlugin(void)
{
    
}

void CsvReaderPlugin::init()
{

}

void CsvReaderPlugin::loadData()
{
    QString fileName = QFileDialog::getOpenFileName(Q_NULLPTR, "Load File", "", "CSV Files (*.csv *)");
    qDebug() << "Loading CSV file: " << fileName;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    std::vector<float> data;
    int numDimensions = 1;

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList tokens = line.split(",");

        numDimensions = tokens.size();
        for (int i = 0; i < tokens.size(); i++)
        {
            if (tokens[i].isEmpty())
                continue;
            float f = tokens[i].toFloat();

            data.push_back(f);
        }
    }

    bool ok;
    QString dataSetName = QInputDialog::getText(nullptr, "Add New Dataset",
        "Dataset name:", QLineEdit::Normal, "DataSet", &ok);

    if (ok && !dataSetName.isEmpty()) {
        QString name = _core->addData("Points", dataSetName);
        const hdps::Set* set = _core->requestData(name);
        DataTypePlugin* dataPlugin = _core->requestPlugin(set->getDataName());
        PointsPlugin* points = dynamic_cast<PointsPlugin*>(dataPlugin);
        points->data.resize(data.size());
        for (int i = 0; i < points->data.size(); i++) {
            points->data[i] = data[i];
        }
        points->numDimensions = numDimensions;

        _core->notifyDataAdded(name);

        qDebug() << "CSV file loaded. Num data points: " << points->data.size();
    }
}

// =============================================================================
// Factory
// =============================================================================

LoaderPlugin* CsvReaderPluginFactory::produce()
{
    return new CsvReaderPlugin();
}
