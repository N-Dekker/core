#include "ScatterplotSettings.h"

#include "ScatterplotPlugin.h"

#include <QGridLayout>
#include <QVBoxLayout>

#include <cassert>

PointSettingsWidget::PointSettingsWidget(const ScatterplotPlugin& plugin)
    :
    _pointSizeLabel("Point Size:"),
    _pointSizeSlider(Qt::Horizontal)
{
    connect(&_pointSizeSlider, &QSlider::valueChanged, plugin._scatterPlotWidget, &ScatterplotWidget::pointSizeChanged);
    
    _pointSizeSlider.setRange(MIN_POINT_SIZE, MAX_POINT_SIZE);
    _pointSizeSlider.setValue(10);

    QVBoxLayout* pointSettingsLayout = new QVBoxLayout();
    pointSettingsLayout->addWidget(&_pointSizeLabel);
    pointSettingsLayout->addWidget(&_pointSizeSlider);
    setLayout(pointSettingsLayout);
}

DensitySettingsWidget::DensitySettingsWidget(const ScatterplotPlugin& plugin)
    :
    _sigmaLabel("Sigma:"),
    _sigmaSlider(Qt::Horizontal)
{
    connect(&_sigmaSlider, &QSlider::valueChanged, plugin._scatterPlotWidget, &ScatterplotWidget::sigmaChanged);

    _sigmaSlider.setRange(MIN_SIGMA, MAX_SIGMA);
    _sigmaSlider.setValue(30);

    QVBoxLayout* densitySettingsLayout = new QVBoxLayout();
    densitySettingsLayout->addWidget(&_sigmaLabel);
    densitySettingsLayout->addWidget(&_sigmaSlider);
    setLayout(densitySettingsLayout);
}

PlotSettingsStack::PlotSettingsStack(const ScatterplotPlugin& plugin)
    :
    _pointSettingsWidget(plugin),
    _densitySettingsWidget(plugin)
{
    addWidget(&_pointSettingsWidget);
    addWidget(&_densitySettingsWidget);
}

DimensionPicker::DimensionPicker(const ScatterplotPlugin* plugin)
    :
    _xDimLabel("X:"),
    _yDimLabel("Y:"),
    _cDimLabel("Color:")
{
    _layout.addWidget(&_xDimLabel, 0, 0);
    _layout.addWidget(&_yDimLabel, 1, 0);
    _layout.addWidget(&_cDimLabel, 2, 0);

    _layout.addWidget(&_xDimOptions, 0, 1);
    _layout.addWidget(&_yDimOptions, 1, 1);
    _layout.addWidget(&_cDimOptions, 2, 1);

    connect(&_xDimOptions, SIGNAL(currentIndexChanged(int)), plugin, SLOT(xDimPicked(int)));
    connect(&_yDimOptions, SIGNAL(currentIndexChanged(int)), plugin, SLOT(yDimPicked(int)));
    connect(&_cDimOptions, SIGNAL(currentIndexChanged(int)), plugin, SLOT(cDimPicked(int)));
}

QGridLayout& DimensionPicker::getLayout()
{
    return _layout;
}

void DimensionPicker::setDimensions(unsigned int numDimensions, std::vector<QString> names)
{
    bool hasNames = numDimensions == names.size();

    std::vector<QComboBox*> allBoxes = { &_xDimOptions, &_yDimOptions, &_cDimOptions };

    for (auto* dimensionBox : allBoxes)
    {
        dimensionBox->blockSignals(true);
        dimensionBox->clear();

        for (unsigned int i = 0; i < numDimensions; i++)
        {
            QString name = hasNames ? names[i] : "Dim " + QString::number(i);
            dimensionBox->addItem(name);
        }
        dimensionBox->blockSignals(true);
    }

    if (numDimensions >= 2)
    {
        _xDimOptions.setCurrentIndex(0);
        _yDimOptions.setCurrentIndex(1);
    }

    for (auto* dimensionBox : allBoxes)
    {
        dimensionBox->blockSignals(false);
    }
}

int DimensionPicker::getDimensionX()
{
    return _xDimOptions.currentIndex();
}

int DimensionPicker::getDimensionY()
{
    return _yDimOptions.currentIndex();
}

int DimensionPicker::getDimensionColor()
{
    return _cDimOptions.currentIndex();
}

ScatterplotSettings::ScatterplotSettings(const ScatterplotPlugin* plugin)
:
    _baseColor(DEFAULT_BASE_COLOR),
    _selectionColor(DEFAULT_SELECTION_COLOR)
{
    setFixedHeight(100);
    
    _subsetButton.setText("Create Subset");
    _subsetButton.setFixedWidth(100);

    _settingsLayout = new QHBoxLayout();

    QVBoxLayout* dataLayout = new QVBoxLayout();
    dataLayout->addWidget(&_dataOptions);
    dataLayout->addWidget(&_subsetButton);
    
    QVBoxLayout* renderLayout = new QVBoxLayout();
    _renderMode.addItem("Scatterplot");
    _renderMode.addItem("Density map");
    _renderMode.addItem("Contour map");
    _renderMode.setFixedWidth(100);
    renderLayout->addWidget(&_renderMode);

    _settingsStack = new PlotSettingsStack(*plugin);
    renderLayout->addWidget(_settingsStack);

    _dimensionPicker = new DimensionPicker(plugin);

    _settingsLayout->addLayout(dataLayout);
    _settingsLayout->addLayout(renderLayout, 1);
    _settingsLayout->addLayout(&_dimensionPicker->getLayout());

    setLayout(_settingsLayout);

    connect(&_dataOptions, SIGNAL(currentIndexChanged(QString)), plugin, SLOT(dataSetPicked(QString)));
    connect(&_subsetButton, SIGNAL(clicked()), plugin, SLOT(subsetCreated()));

    connect(&_renderMode, SIGNAL(currentIndexChanged(int)), plugin->_scatterPlotWidget, SLOT(renderModePicked(int)));
    connect(&_renderMode, SIGNAL(currentIndexChanged(int)), this, SLOT(renderModePicked(int)));
}

ScatterplotSettings::~ScatterplotSettings()
{

}

int ScatterplotSettings::getXDimension()
{
    return _dimensionPicker->getDimensionX();
}

int ScatterplotSettings::getYDimension()
{
    return _dimensionPicker->getDimensionY();
}

int ScatterplotSettings::getColorDimension()
{
    return _dimensionPicker->getDimensionColor();
}

hdps::Vector3f ScatterplotSettings::getBaseColor()
{
    return _baseColor;
}

hdps::Vector3f ScatterplotSettings::getSelectionColor()
{
    return _selectionColor;
}

void ScatterplotSettings::showPointSettings()
{
    _settingsStack->setCurrentIndex(0);
}

void ScatterplotSettings::showDensitySettings()
{
    _settingsStack->setCurrentIndex(1);
}

QString ScatterplotSettings::currentData()
{
    return _dataOptions.currentText();
}

void ScatterplotSettings::initDimOptions(const unsigned int nDim)
{
    _dimensionPicker->setDimensions(nDim);
}

void ScatterplotSettings::initDimOptions(const std::vector<QString> dimNames)
{
    _dimensionPicker->setDimensions(dimNames.size(), dimNames);
}

void ScatterplotSettings::addDataOption(const QString option)
{
    _dataOptions.addItem(option);
}

int ScatterplotSettings::numDataOptions()
{
    return _dataOptions.count();
}

void ScatterplotSettings::renderModePicked(const int index)
{
    switch (index)
    {
    case 0: showPointSettings(); break;
    case 1: showDensitySettings(); break;
    case 2: showDensitySettings(); break;
    }
}
