#include "TsneAnalysisPlugin.h"

#include "PointsPlugin.h"

#include <QtCore>
#include <QMessageBox>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.TsneAnalysisPlugin")

// =============================================================================
// View
// =============================================================================

TsneAnalysisPlugin::~TsneAnalysisPlugin(void)
{
    stopComputation();
}

void TsneAnalysisPlugin::init()
{
    _settings = std::make_unique<TsneSettingsWidget>(this);

    _tsne = std::make_unique<TsneAnalysis>();

    connect(_tsne.get(), SIGNAL(newEmbedding()), this, SLOT(onNewEmbedding()));
}

void TsneAnalysisPlugin::dataAdded(const QString name)
{
    _settings->dataOptions.addItem(name);

    IndexSet& set = (IndexSet&)_core->requestSet(name);
    PointsPlugin& rawData = set.getData();
    
    _settings->onNumDimensionsChanged(this, rawData.getNumDimensions(), rawData.dimNames);
}

void TsneAnalysisPlugin::dataChanged(const QString name)
{
    IndexSet& set = (IndexSet&) _core->requestSet(name);
    PointsPlugin& rawData = set.getData();

    _settings->onNumDimensionsChanged(this, rawData.getNumDimensions(), rawData.dimNames);
}

void TsneAnalysisPlugin::dataRemoved(const QString name)
{
    
}

void TsneAnalysisPlugin::selectionChanged(const QString dataName)
{

}


QStringList TsneAnalysisPlugin::supportedDataKinds()
{
    QStringList supportedKinds;
    supportedKinds << "Points";
    return supportedKinds;
}

SettingsWidget* const TsneAnalysisPlugin::getSettings()
{
    return _settings.get();
}

void TsneAnalysisPlugin::dataSetPicked(const QString& name)
{

}

void TsneAnalysisPlugin::startComputation()
{
    // Do nothing if we have no data set selected
    if (_settings->dataOptions.currentText().isEmpty()) {
        return;
    }

    // Check if the tSNE settings are valid before running the computation
    if (!_settings->hasValidSettings()) {
        QMessageBox warningBox;
        warningBox.setText(tr("Some settings are invalid or missing. Continue with default values?"));
        QPushButton *continueButton = warningBox.addButton(tr("Continue"), QMessageBox::ActionRole);
        QPushButton *abortButton = warningBox.addButton(QMessageBox::Abort);

        warningBox.exec();

        if (warningBox.clickedButton() == abortButton) {
            return;
        }
    }

    initializeTsne();

    // Run the computation
    QString setName = _settings->dataOptions.currentText();
    const IndexSet& set = dynamic_cast<const IndexSet&>(_core->requestSet(setName));
    const PointsPlugin& points = set.getData();

    // Create list of data from the enabled dimensions
    std::vector<float> data;
    
    std::vector<bool> enabledDimensions = _settings->getEnabledDimensions();
    for (int i = 0; i < points.getNumPoints(); i++)
    {
        for (int j = 0; j < points.getNumDimensions(); j++)
        {
            if (enabledDimensions[j])
                data.push_back(points.data[i * points.getNumDimensions() + j]);
        }
    }
    unsigned int numDimensions = count_if(enabledDimensions.begin(), enabledDimensions.end(), [](bool b) { return b; } );

    _embedSetName = _core->createDerivedData("Points", "Embedding", points.getName());
    const IndexSet& embedSet = dynamic_cast<const IndexSet&>(_core->requestSet(_embedSetName));
    PointsPlugin& embedPoints = embedSet.getData();

    embedPoints.numDimensions = 2;
    _core->notifyDataAdded(_embedSetName);

    // Compute t-SNE with the given data
    _tsne->initTSNE(data, numDimensions);

    _tsne->start();
}

void TsneAnalysisPlugin::onNewEmbedding() {
    std::vector<float>* output = _tsne->output();
    const IndexSet& embedSet = dynamic_cast<const IndexSet&>(_core->requestSet(_embedSetName));
    PointsPlugin& embedPoints = embedSet.getData();

    embedPoints.data = *output;

    _core->notifyDataChanged(_embedSetName);
}

void TsneAnalysisPlugin::initializeTsne() {
    // Initialize the tSNE computation with the settings from the settings widget
    _tsne->setIterations(_settings->numIterations.text().toInt());
    _tsne->setPerplexity(_settings->perplexity.text().toInt());
    _tsne->setExaggerationIter(_settings->exaggeration.text().toInt());
    _tsne->setExpDecay(_settings->expDecay.text().toInt());
    _tsne->setNumTrees(_settings->numTrees.text().toInt());
    _tsne->setNumChecks(_settings->numChecks.text().toInt());
}

void TsneAnalysisPlugin::stopComputation() {
    if (_tsne)
    {
        if (_tsne->isRunning())
        {
            // Request interruption of the computation
            _tsne->stopGradientDescent();
            _tsne->exit();

            // Wait until the thread has terminated (max. 3 seconds)
            if (!_tsne->wait(3000))
            {
                qDebug() << "tSNE computation thread did not close in time, terminating...";
                _tsne->terminate();
                _tsne->wait();
            }
            qDebug() << "tSNE computation stopped.";
        }
    }
}

// =============================================================================
// Factory
// =============================================================================

AnalysisPlugin* TsneAnalysisPluginFactory::produce()
{
    return new TsneAnalysisPlugin();
}
