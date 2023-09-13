// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Project.h"
#include "AbstractProjectManager.h"
#include "CoreInterface.h"
#include "Set.h"

#include "util/Serialization.h"

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps {

Project::Project(QObject* parent /*= nullptr*/) :
    QObject(parent),
    Serializable("Project"),
    _filePath(),
    _applicationVersion(Application::current()->getVersion()),
    _applicationVersionAction(this, "Application Version"),
    _projectVersionAction(this, "Project Version"),
    _readOnlyAction(this, "Read-only"),
    _titleAction(this, "Title"),
    _descriptionAction(this, "Description"),
    _tagsAction(this, "Tags"),
    _commentsAction(this, "Comments"),
    _contributorsAction(this, "Contributors"),
    _compressionAction(this),
    _splashScreenAction(this, *this),
    _studioModeAction(this, "Studio Mode")
{
    initialize();
}

Project::Project(const QString& filePath, bool preview, QObject* parent /*= nullptr*/) :
    Project(parent)
{
    _filePath = filePath;

    try {
        if (!QFileInfo(_filePath).exists())
            throw std::runtime_error("File does not exist");

        QFile projectJsonFile(_filePath);

        if (!projectJsonFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open file for reading");

        QByteArray projectByteArray = projectJsonFile.readAll();

        QJsonDocument jsonDocument;

        jsonDocument = QJsonDocument::fromJson(projectByteArray);

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        fromVariantMap(jsonDocument.toVariant().toMap()["Project"].toMap(), preview);
    }
    catch (std::exception& e)
    {
        qDebug() << "Unable to load project from file:" << e.what();
    }
    catch (...)
    {
        qDebug() << "Unable to load project from file";
    }
}

QString Project::getFilePath() const
{
    return _filePath;
}

void Project::setFilePath(const QString& filePath)
{
    _filePath = filePath;

    emit filePathChanged(_filePath);
}

void Project::fromVariantMap(const QVariantMap& variantMap)
{
    fromVariantMap(variantMap, false);
}

void Project::fromVariantMap(const QVariantMap& variantMap, bool preview)
{
    Serializable::fromVariantMap(variantMap);

    _splashScreenAction.fromParentVariantMap(variantMap);
    _applicationVersion.fromParentVariantMap(variantMap);
    _projectVersionAction.fromParentVariantMap(variantMap);
    _readOnlyAction.fromParentVariantMap(variantMap);
    _titleAction.fromParentVariantMap(variantMap);
    _descriptionAction.fromParentVariantMap(variantMap);
    _tagsAction.fromParentVariantMap(variantMap);
    _commentsAction.fromParentVariantMap(variantMap);
    _contributorsAction.fromParentVariantMap(variantMap);
    _compressionAction.fromParentVariantMap(variantMap);
    _studioModeAction.fromParentVariantMap(variantMap);

    if (!preview) {
        dataHierarchy().fromParentVariantMap(variantMap);
        actions().fromParentVariantMap(variantMap);
        plugins().fromParentVariantMap(variantMap);
    }
}

QVariantMap Project::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();
    
    _splashScreenAction.insertIntoVariantMap(variantMap);
    _applicationVersion.insertIntoVariantMap(variantMap);
    _projectVersionAction.insertIntoVariantMap(variantMap);
    _readOnlyAction.insertIntoVariantMap(variantMap);
    _titleAction.insertIntoVariantMap(variantMap);
    _descriptionAction.insertIntoVariantMap(variantMap);
    _tagsAction.insertIntoVariantMap(variantMap);
    _commentsAction.insertIntoVariantMap(variantMap);
    _contributorsAction.insertIntoVariantMap(variantMap);
    _compressionAction.insertIntoVariantMap(variantMap);
    _studioModeAction.insertIntoVariantMap(variantMap);

    plugins().insertIntoVariantMap(variantMap);
    dataHierarchy().insertIntoVariantMap(variantMap);
    actions().insertIntoVariantMap(variantMap);

    return variantMap;
}

void Project::initialize()
{
    _readOnlyAction.setToolTip("Whether the project is in read-only mode or not");

    _titleAction.setPlaceHolderString("Enter project title here...");
    _titleAction.setClearable(true);

    _descriptionAction.setPlaceHolderString("Enter project description here...");
    _descriptionAction.setClearable(true);

    _tagsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("tag"));
    _tagsAction.setCategory("Tag");
    _tagsAction.setStretch(2);

    _commentsAction.setPlaceHolderString("Enter project comments here...");
    _commentsAction.setClearable(true);
    _commentsAction.setStretch(2);
    _commentsAction.setDefaultWidgetFlags(StringAction::TextEdit);

    _contributorsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("user"));
    _contributorsAction.setCategory("Contributor");
    _contributorsAction.setEnabled(false);
    _contributorsAction.setStretch(1);
    _contributorsAction.setDefaultWidgetFlags(StringsAction::ListView);

    updateContributors();

    _studioModeAction.setIcon(Application::getIconFont("FontAwesome").getIcon("pencil-ruler"));
    
    connect(&_studioModeAction, &ToggleAction::toggled, this, &Project::setStudioMode);

    const auto updateStudioModeActionReadOnly = [&]() -> void {
        _studioModeAction.setEnabled(projects().hasProject());
    };

    updateStudioModeActionReadOnly();

    connect(&projects(), &AbstractProjectManager::projectCreated, this, updateStudioModeActionReadOnly);
    connect(&projects(), &AbstractProjectManager::projectDestroyed, this, updateStudioModeActionReadOnly);
}

util::Version Project::getVersion() const
{
    return _applicationVersion;
}

void Project::updateContributors()
{
    QString currentUserName;

#ifdef __APPLE__
    currentUserName = getenv("USER");
#else
    currentUserName = getenv("USERNAME");
#endif

    if (!currentUserName.isEmpty() && !_contributorsAction.getStrings().contains(currentUserName))
        _contributorsAction.addString(currentUserName);
}

void Project::setStudioMode(bool studioMode)
{
    auto plugins = hdps::plugins().getPluginsByTypes();  // by default gets all plugin types
    auto& datasets = hdps::data().allSets();

    if (studioMode) {
        for (auto plugin : plugins)
            plugin->cacheConnectionPermissions(true);

        for (auto plugin : plugins)
            plugin->setConnectionPermissionsToAll(true);

        for (auto& dataset : datasets)
        {
            for (auto& action : dataset.get()->getActions())
                action->cacheConnectionPermissions(true);

            for (auto& action : dataset.get()->getActions())
                action->setConnectionPermissionsToAll(true);
        }

    }
    else {
        for (auto plugin : plugins)
            plugin->restoreConnectionPermissions(true);

        for (auto& dataset : datasets)
            for (auto& action : dataset.get()->getActions())
                action->restoreConnectionPermissions(true);
    }

}

Project::CompressionAction::CompressionAction(QObject* parent /*= nullptr*/) :
    WidgetAction(parent, "Compression"),
    _enabledAction(this, "Compression", DEFAULT_ENABLE_COMPRESSION),
    _levelAction(this, "Compression level", 1, 9, DEFAULT_COMPRESSION_LEVEL)
{
    _levelAction.setPrefix("Level: ");

    const auto updateCompressionLevelReadOnly = [this]() -> void {
        _levelAction.setEnabled(_enabledAction.isChecked());
    };

    connect(&_enabledAction, &ToggleAction::toggled, this, updateCompressionLevelReadOnly);

    updateCompressionLevelReadOnly();
}

void Project::CompressionAction::fromVariantMap(const QVariantMap& variantMap)
{
    WidgetAction::fromVariantMap(variantMap);

    _enabledAction.fromParentVariantMap(variantMap);
    _levelAction.fromParentVariantMap(variantMap);
}

QVariantMap Project::CompressionAction::toVariantMap() const
{
    QVariantMap variantMap = WidgetAction::toVariantMap();

    _enabledAction.insertIntoVariantMap(variantMap);
    _levelAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

}
