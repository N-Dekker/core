#include "ColorMapSettingsAction.h"
#include "ColorMapAction.h"
#include "Application.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

using namespace hdps::util;

namespace hdps {

namespace gui {

ColorMapSettingsAction::ColorMapSettingsAction(ColorMapAction& colorMapAction) :
    WidgetAction(&colorMapAction),
    _colorMapAction(colorMapAction),
    _horizontalAxisAction(*this, "Horizontal Axis"),
    _verticalAxisAction(*this, "Vertical Axis"),
    _discreteAction(*this)
{
    setText("Settings");
    setIcon(hdps::Application::getIconFont("FontAwesome").getIcon("sliders-h"));
}

void ColorMapSettingsAction::connectToPublicAction(WidgetAction* publicAction)
{
    auto publicColorMapSettingsAction = dynamic_cast<ColorMapSettingsAction*>(publicAction);

    Q_ASSERT(publicColorMapSettingsAction != nullptr);

    _horizontalAxisAction.connectToPublicAction(&publicColorMapSettingsAction->getHorizontalAxisAction());
    _verticalAxisAction.connectToPublicAction(&publicColorMapSettingsAction->getVerticalAxisAction());
    _discreteAction.connectToPublicAction(&publicColorMapSettingsAction->getDiscreteAction());
}

void ColorMapSettingsAction::disconnectFromPublicAction()
{
    _horizontalAxisAction.disconnectFromPublicAction();
    _verticalAxisAction.disconnectFromPublicAction();
    _discreteAction.disconnectFromPublicAction();
}

ColorMapSettingsAction::Widget::Widget(QWidget* parent, ColorMapSettingsAction* colorMapSettingsAction) :
    WidgetActionWidget(parent, colorMapSettingsAction),
    _colorMapViewAction(colorMapSettingsAction->getColorMapAction())
{
    auto mainLayout     = new QHBoxLayout();
    auto settingsLayout = new QVBoxLayout();
    auto rangeLayout    = new QHBoxLayout();

    mainLayout->setMargin(0);

    mainLayout->addLayout(settingsLayout);

    settingsLayout->addLayout(rangeLayout);
    settingsLayout->addWidget(colorMapSettingsAction->getDiscreteAction().createWidget(this));

    switch (colorMapSettingsAction->getColorMapAction().getColorMapType())
    {
        case ColorMap::Type::OneDimensional:
        {
            rangeLayout->addWidget(colorMapSettingsAction->getHorizontalAxisAction().createWidget(this));
            break;
        }

        case ColorMap::Type::TwoDimensional:
        {
            rangeLayout->addWidget(colorMapSettingsAction->getHorizontalAxisAction().createWidget(this));
            rangeLayout->addWidget(colorMapSettingsAction->getVerticalAxisAction().createWidget(this));
            mainLayout->addWidget(_colorMapViewAction.createWidget(this));
            break;
        }

        default:
            break;
    }

    setLayout(mainLayout);
}

}
}
