// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Application.h"

#include "CoreInterface.h"
#include "util/FontAwesome.h"
#include "util/Exception.h"
#include "actions/WidgetAction.h"

#include <stdexcept>

#include <QDebug>
#include <QMessageBox>
#include <QTemporaryDir>
#include <QFileDialog>
#include <QDir>
#include <QDateTime>

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps {

hdps::Application::Application(int& argc, char** argv) :
    QApplication(argc, argv),
    _core(nullptr),
    _version({ 3, 0 }),
    _iconFonts(),
    _settings(),
    _serializationTemporaryDirectory(),
    _serializationAborted(false),
    _logger(),
    _startupProjectFilePath()
{
    _iconFonts.add(QSharedPointer<IconFont>(new FontAwesome(5, 14)));
    
    _logger.initialize();
}

hdps::Application* hdps::Application::current()
{
    try
    {
        auto hdpsApplication = dynamic_cast<Application*>(instance());

        if (hdpsApplication == nullptr)
            throw std::runtime_error("Current application instance is not an HDPS application");

        return hdpsApplication;
    }
    catch (std::exception& e)
    {
        QMessageBox::critical(nullptr, "An application error occurred", e.what());
        return nullptr;
    }
}

const IconFont& hdps::Application::getIconFont(const QString& name, const std::int32_t& majorVersion /*= -1*/, const std::int32_t& minorVersion /*= -1*/)
{
    return current()->_iconFonts.getIconFont(name, majorVersion, minorVersion);
}

hdps::CoreInterface* Application::getCore()
{
    Q_ASSERT(_core != nullptr);

    return _core;
}

void Application::setCore(CoreInterface* core)
{
    Q_ASSERT(core != nullptr);

    _core = core;

    emit coreSet(_core);
}

hdps::CoreInterface* Application::core()
{
    return current()->getCore();
}

util::Version Application::getVersion() const
{
    return _version;
}

QString Application::getName()
{
    return "ManiVault";
}

QString Application::getStartupProjectFilePath() const
{
    return _startupProjectFilePath;
}

void Application::setStartupProjectFilePath(const QString& startupProjectFilePath)
{
    _startupProjectFilePath = startupProjectFilePath;
}

bool Application::shouldOpenProjectAtStartup() const
{
    return !_startupProjectFilePath.isEmpty() && QFileInfo(_startupProjectFilePath).exists();
}

QVariant Application::getSetting(const QString& path, const QVariant& defaultValue /*= QVariant()*/) const
{
    return _settings.value(path, defaultValue);
}

void Application::setSetting(const QString& path, const QVariant& value)
{
    _settings.setValue(path, value);
}

Logger& Application::getLogger()
{
    return current()->_logger;
}

QString Application::getSerializationTemporaryDirectory()
{
    return current()->_serializationTemporaryDirectory;
}

void Application::setSerializationTemporaryDirectory(const QString& serializationTemporaryDirectory)
{
    current()->_serializationTemporaryDirectory = serializationTemporaryDirectory;
}

bool Application::isSerializationAborted()
{
    return current()->_serializationAborted;
}

void Application::setSerializationAborted(bool serializationAborted)
{
    current()->_serializationAborted = serializationAborted;
}

}
