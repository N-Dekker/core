#include "Core.h"

#include "MainWindow.h"
#include "PluginManager.h"

namespace hdps {

Core::Core(gui::MainWindow& mainWindow)
: _mainWindow(mainWindow)
{
    
}

Core::~Core() {

}

void Core::init() {
    _pluginManager = std::unique_ptr<plugin::PluginManager>(new plugin::PluginManager(*this));

    _pluginManager->LoadPlugins();
}

gui::MainWindow& Core::gui() const {
    return _mainWindow;
}

} // namespace hdps
