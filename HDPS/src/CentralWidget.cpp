#include "CentralWidget.h"

#include "ViewPlugin.h"

#include <QMainWindow>

namespace hdps
{
    namespace gui
    {
        void CentralWidget::addView(plugin::ViewPlugin* plugin)
        {
            QMainWindow* mainWidget = new QMainWindow();
            mainWidget->addDockWidget(Qt::TopDockWidgetArea, plugin);
            addWidget(mainWidget);
        }
    }
}
