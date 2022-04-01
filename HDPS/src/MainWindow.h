#pragma once

#include "ui_MainWindow.h"
#include "Core.h"
#include "LogDockWidget.h"

#include <QMainWindow>
#include <QAction>
#include <QSharedPointer>

// Advanced docking system
#include "DockManager.h"
#include "DockAreaWidget.h"

namespace Ui
{
    class MainWindow;
}

class StartPageWidget;

namespace hdps
{

namespace plugin
{
    class PluginManager;
    class ViewPlugin;
    enum class Type;
}

namespace gui
{

class LogDockWidget;
class DataHierarchyWidget;
class ActionsViewerWidget;
class DataPropertiesWidget;

class MainWindow : public QMainWindow, private Ui::MainWindow {
    Q_OBJECT

public:

    class RecentProjectLabel : public QLabel
    {
    public:
        RecentProjectLabel(const QString& recentProjectFilePath, QWidget* parent = nullptr) :
            QLabel(recentProjectFilePath, parent),
            _recentProjectFilePath(recentProjectFilePath)
        {
        }

    protected:

        void mousePressEvent(QMouseEvent* mouseEvent)
        {
            Application::current()->loadProject(_recentProjectFilePath);
        }

        void enterEvent(QEvent* event)
        {
            setStyleSheet("QLabel { text-decoration: underline; }");
        }

        void leaveEvent(QEvent* event)
        {
            setStyleSheet("QLabel { text-decoration: none; }");
        }

    protected:
        QString     _recentProjectFilePath;     /** Recent project file path */
    };
public:
    MainWindow(QWidget *parent = nullptr);

    /**
     * Adds a new item to the import menu
     * @param menuName Name of the import plugin
     * @param icon Icon of the import plugin
     */
    QAction* addImportOption(const QString& actionName, const QIcon& icon);

    /**
    * Adds a new item to the menu drop-down for this particular type.
    */
    QAction* addViewAction(const plugin::Type& type, const QString name, const QIcon& icon);

    /**
    * Allows access to the core, which is created by this MainWindow. Useful for
    * unit tests that may need to call addData and requestData.
    */
    CoreInterface& getCore() { return *_core;  }

    /**
     * Callback invoked when the window closes
     * @param closeEvent Close event
     */
    void closeEvent(QCloseEvent* closeEvent) override;

public: // Adding plugins

    /**
     * Dock (visible) plugin to the window
     * @param plugin Plugin to add (view or analysis)
     */
    void addPlugin(plugin::Plugin* plugin);

private: // Docking

    /**
     * TODO
     */
    void addDockWidget(QWidget* widget, const QString& windowTitle, const ads::DockWidgetArea& dockWidgetArea, ads::CDockAreaWidget* dockAreaWidget = nullptr);

private: // Window geometry persistence

    /**
     * Invoked when the window position changes
     * @param moveEvent Move event that occurred
     */
    void moveEvent(QMoveEvent* moveEvent) override;

    /**
     * Invoked when the window size changes
     * @param resizeEvent Resize event that occurred
     */
    void resizeEvent(QResizeEvent* resizeEvent) override;

    /** Restores the window geometry (position and size) from the application settings */
    void restoreWindowGeometryFromSettings();

    /** Save the window geometry (position and size) to application settings */
    void saveWindowGeometryToSettings();

    /**
     * Applies default window geometry (center position and resized with \p coverage)
     * @param coverage Fraction of the total screen size
     */
    void setDefaultWindowGeometry(const float& coverage = 0.7f);

    void checkGraphicsCapabilities();

private: // Docking

    /** Sets up docking */
    void initializeDocking();

    /** Sets up the docking area for view plugins (central widget) */
    void initializeCentralDockingArea();

    /** Sets up the docking area for settings */
    void initializeSettingsDockingArea();

    /** Sets up the docking area for logging */
    void initializeLoggingDockingArea();
    
    /** Updates the visibility of the central dock widget (depending on its content) */
    void updateCentralWidget();

    /**
     * Return a list of (open) view plugin dock widgets
     * @param open Whether to only include open dock widgets
     * @return View plugin dock area widgets
     */
    QList<ads::CDockWidget*> getViewPluginDockWidgets(const bool& openOnly = true);

protected: // Menu

    /** Setup file menu */
    void setupFileMenu();

    /** Setup view menu */
    void setupViewMenu();

    /** Fill the recent projects menu with entries */
    void populateRecentProjectsMenu();

private:
    QSharedPointer<Core>        _core;                          /** HDPS core */
    StartPageWidget*            _startPageWidget;               /** Pointer to the start page widget */
    DataHierarchyWidget*        _dataHierarchyWidget;           /** Pointer to the data hierarchy tree widget */
    ActionsViewerWidget*        _actionsViewerWidget;           /** Pointer to the actions viewer widget */
    DataPropertiesWidget*       _dataPropertiesWidget;          /** Pointer to the data properties widget */

private: // Docking
    ads::CDockManager*          _dockManager;                   /** Manager for docking */
    ads::CDockAreaWidget*       _centralDockArea;               /** Docking area for view plugins */
    ads::CDockAreaWidget*       _settingsDockArea;              /** Docking area for settings */
    ads::CDockAreaWidget*       _loggingDockArea;               /** Docking area for logging */
    ads::CDockWidget*           _centralDockWidget;             /** Dock widget for view plugins */
    ads::CDockWidget*           _startPageDockWidget;           /** Dock widget for the start page */
    ads::CDockWidget*           _dataHierarchyDockWidget;       /** Dock widget for data hierarchy */
    ads::CDockWidget*           _actionsViewerDockWidget;       /** Dock widget for actions viewer */
    ads::CDockWidget*           _dataPropertiesDockWidget;      /** Dock widget for data properties */
    ads::CDockWidget*           _loggingDockWidget;             /** Dock widget for logging */
};

}
}