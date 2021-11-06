#pragma once

#include <QHBoxLayout>
#include <QToolButton>

namespace hdps {

namespace gui {

class WidgetAction;

/**
 * Widget action collapsed widget class
 *
 * Displays a tool button that shows the widget in a popup
 * 
 * @author Thomas Kroes
 */
class CollapsedWidget : public QToolButton
{
public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param widgetAction Pointer to the widget action that will be displayed in a popup
     */
    CollapsedWidget(QWidget* parent, QWidget* widget);

    /**
     * Paint event
     * @param paintEvent Pointer to paint event
     */
    void paintEvent(QPaintEvent* paintEvent);
};

}
}
