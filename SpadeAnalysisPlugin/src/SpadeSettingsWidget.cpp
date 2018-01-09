#include "SpadeSettingsWidget.h"

#include "SpadeAnalysisPlugin.h"

#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>

#include <QIntValidator>
#include <QDoubleValidator>

using namespace hdps::plugin;

SpadeSettingsWidget::SpadeSettingsWidget(const SpadeAnalysisPlugin* analysis)
{
    setFixedWidth(200);

    connect(&ui_dataOptions,   SIGNAL(currentIndexChanged(QString)), analysis, SLOT(dataSetPicked(QString)));
    connect(&ui_startButton,   SIGNAL(clicked()), analysis, SLOT(startComputation()));

    connect(&ui_targetEvents,     SIGNAL(valueChanged(double)), SLOT(targetEventsChanged(double)));
    connect(&ui_targetNodes,      SIGNAL(valueChanged(int)),    SLOT(targetNodesChanged(int)));
    connect(&ui_heuristicSamples, SIGNAL(valueChanged(int)),    SLOT(heuristicSamplesChanged(int)));
    connect(&ui_alpha,            SIGNAL(valueChanged(double)), SLOT(alphaChanged(double)));
    connect(&ui_targetDensity,    SIGNAL(valueChanged(double)), SLOT(targetDensityChanged(double)));
    connect(&ui_outlierDensity,   SIGNAL(valueChanged(double)), SLOT(outlierDensityChanged(double)));

    QGroupBox* settingsBox = new QGroupBox("Basic settings");
    QGroupBox* advancedSettingsBox = new QGroupBox("Advanced Settings");
    advancedSettingsBox->setCheckable(true);
    advancedSettingsBox->setChecked(false);

    QLabel* targetEventsLabel = new QLabel("Target Events %");
    QLabel* targetNodesLabel = new QLabel("Target Nodes");

    QLabel* heuristicSamplesLabel = new QLabel("Heuristic Samples");
    QLabel* alphaLabel = new QLabel("Alpha");
    QLabel* targetDensityLabel = new QLabel("Target Density %");
    QLabel* outlierDensityLabel = new QLabel("Outlier Density %");

    ui_targetEvents.setToolTip("Target number of points to keep after downsampling in percent.\nThe number might not be reached depending on the outlier and target density parameters.");
    ui_targetEvents.setRange(0, 100.0);
    ui_targetEvents.setValue(spadeSettings._densityLimit);
    ui_targetNodes.setToolTip("Target number of nodes in the SPADE tree.");
    ui_targetNodes.setRange(0, 99999);
    ui_targetNodes.setValue(spadeSettings._targetNumClusters);

    ui_heuristicSamples.setToolTip("The number of random Points used to determine the median density.");
    ui_heuristicSamples.setRange(0, 999999);
    ui_heuristicSamples.setValue(spadeSettings._maxRandomSampleSize);
    ui_alpha.setToolTip("Scales the neighborhood size for the density computation for downsampling.");
    ui_alpha.setValue(spadeSettings._alpha);
    ui_targetDensity.setToolTip("Percentile of the upper limit of the target density. Points with a density between the outlier density and the target density are considered rare, but not noise and kept.\n A value of 3.0 (and an outlier density of 1.0) means the cells in between the first and third percentile, ordered by density are considered rare and kept.\nPoints with a density above this value are kept only with a probability according to their density, the larger the density, the lower the probability.");
    ui_targetDensity.setValue(spadeSettings._targetDensityPercentile);
    ui_outlierDensity.setToolTip("Percentile of the upper limit of the outlier density. Points with a density below this threshold are considered as noise.\nA value of 1.0 means the first percentile of the cells ordered by density is discarded. Use larger values if you expect more noise.");
    ui_outlierDensity.setValue(spadeSettings._outlierDensityPercentile);

    ui_startButton.setText("Cluster");
    ui_startButton.setFixedSize(QSize(150, 50));

    QGridLayout *settingsLayout = new QGridLayout;
    settingsLayout->setAlignment(Qt::AlignTop);
    settingsLayout->addWidget(targetEventsLabel, 0, 0);
    settingsLayout->addWidget(&ui_targetEvents, 0, 1);
    settingsLayout->addWidget(targetNodesLabel, 1, 0);
    settingsLayout->addWidget(&ui_targetNodes);
    settingsLayout->setColumnStretch(0, 0);
    settingsLayout->setColumnStretch(1, 1);
    settingsBox->setLayout(settingsLayout);

    QGridLayout* advancedSettingsLayout = new QGridLayout;
    advancedSettingsLayout->setAlignment(Qt::AlignTop);
    advancedSettingsLayout->addWidget(heuristicSamplesLabel, 0, 0);
    advancedSettingsLayout->addWidget(&ui_heuristicSamples, 0, 1);
    advancedSettingsLayout->addWidget(alphaLabel, 1, 0);
    advancedSettingsLayout->addWidget(&ui_alpha, 1, 1);
    advancedSettingsLayout->addWidget(targetDensityLabel, 2, 0);
    advancedSettingsLayout->addWidget(&ui_targetDensity, 2, 1);
    advancedSettingsLayout->addWidget(outlierDensityLabel, 3, 0);
    advancedSettingsLayout->addWidget(&ui_outlierDensity, 3, 1);
    advancedSettingsLayout->setColumnStretch(0, 0);
    advancedSettingsLayout->setColumnStretch(1, 1);
    advancedSettingsBox->setLayout(advancedSettingsLayout);

    addWidget(&ui_dataOptions);
    addWidget(settingsBox);
    addWidget(advancedSettingsBox);
    addWidget(&ui_startButton);
}

void SpadeSettingsWidget::addDataOption(QString option)
{
    ui_dataOptions.addItem(option);
}

QString SpadeSettingsWidget::getCurrentDataOption()
{
    return ui_dataOptions.currentText();
}

const SpadeSettings& SpadeSettingsWidget::getSpadeSettings() const
{
    return spadeSettings;
}

void SpadeSettingsWidget::targetEventsChanged(double value)
{
    spadeSettings._maxRandomSampleSize = value;
}

void SpadeSettingsWidget::targetNodesChanged(int value)
{
    spadeSettings._targetNumClusters = value;
}

void SpadeSettingsWidget::heuristicSamplesChanged(int value)
{
    spadeSettings._densityLimit = value;
}

void SpadeSettingsWidget::alphaChanged(double value)
{
    spadeSettings._alpha = value;
}

void SpadeSettingsWidget::targetDensityChanged(double value)
{
    spadeSettings._targetDensityPercentile = value;
}

void SpadeSettingsWidget::outlierDensityChanged(double value)
{
    spadeSettings._outlierDensityPercentile = value;
}
