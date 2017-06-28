#include "CsvReaderPlugin.h"

#include "PointsPlugin.h"

#include <QtCore>
#include <QtDebug>

#include <QFileDialog>
#include <vector>

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

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList tokens = line.split(",");

        for (int i = 0; i < tokens.size(); i++)
        {
            if (tokens[i].isEmpty())
                continue;
            float f = tokens[i].toFloat();

            data.push_back(f);
        }
    }

    QString name = _core->addData("Points");
    DataTypePlugin* dataSet = _core->requestPlugin(name);
    PointsPlugin* points = dynamic_cast<PointsPlugin*>(dataSet);
    points->data.resize(data.size());
    for (int i = 0; i < points->data.size(); i++) {
        points->data[i] = data[i];
    }

    _core->notifyDataChanged(name);

    qDebug() << "CSV file loaded. Num data points: " << points->data.size();
}

// =============================================================================
// Factory
// =============================================================================

LoaderPlugin* CsvReaderPluginFactory::produce()
{
    return new CsvReaderPlugin();
}
