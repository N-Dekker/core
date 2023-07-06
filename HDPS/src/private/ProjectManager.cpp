// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "ProjectManager.h"
#include "Archiver.h"
#include "PluginManagerDialog.h"
#include "ProjectSettingsDialog.h"
#include "NewProjectDialog.h"

#include <Application.h>
#include <CoreInterface.h>

#include <util/Exception.h>
#include <util/Serialization.h>

#include <widgets/TaskProgressDialog.h>

#include <QFileDialog>
#include <QStandardPaths>
#include <QGridLayout>
#include <QEventLoop>

#include <exception>

#ifdef _DEBUG
    #define PROJECT_MANAGER_VERBOSE
#endif

using namespace hdps;
using namespace hdps::util;
using namespace hdps::gui;

// Definition needed for pre C++17 gcc and clang
#if (__cplusplus < 201703L)
    constexpr bool ProjectManager::DEFAULT_ENABLE_COMPRESSION;
    constexpr std::uint32_t ProjectManager::DEFAULT_COMPRESSION_LEVEL;
#endif

ProjectManager::ProjectManager(QObject* parent /*= nullptr*/) :
    _project(),
    _newBlankProjectAction(this, "Blank"),
    _newProjectFromWorkspaceAction(this, "From Workspace..."),
    _openProjectAction(this, "Open Project"),
    _importProjectAction(this, "Import Project"),
    _saveProjectAction(this, "Save Project"),
    _saveProjectAsAction(this, "Save Project As..."),
    _editProjectSettingsAction(this, "Project Settings..."),
    _recentProjectsAction(this),
    _newProjectMenu(),
    _importDataMenu(),
    _publishAction(this, "Publish"),
    _pluginManagerAction(this, "Plugin Browser..."),
    _showStartPageAction(this, "Start Page...", true)
{
    _newBlankProjectAction.setShortcut(QKeySequence("Ctrl+B"));
    _newBlankProjectAction.setShortcutContext(Qt::ApplicationShortcut);
    _newBlankProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("file"));
    _newBlankProjectAction.setToolTip("Create project without view plugins and data");

    _newProjectFromWorkspaceAction.setShortcut(QKeySequence("Ctrl+N"));
    _newProjectFromWorkspaceAction.setShortcutContext(Qt::ApplicationShortcut);
    _newProjectFromWorkspaceAction.setIcon(workspaces().getIcon());
    _newProjectFromWorkspaceAction.setToolTip("Create new project with workspace");

    _openProjectAction.setShortcut(QKeySequence("Ctrl+O"));
    _openProjectAction.setShortcutContext(Qt::ApplicationShortcut);
    _openProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
    _openProjectAction.setToolTip("Open project from disk");

    _importProjectAction.setShortcut(QKeySequence("Ctrl+I"));
    _importProjectAction.setShortcutContext(Qt::ApplicationShortcut);
    _importProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));
    _importProjectAction.setToolTip("Import project from disk");

    _saveProjectAction.setShortcut(QKeySequence("Ctrl+S"));
    _saveProjectAction.setShortcutContext(Qt::ApplicationShortcut);
    _saveProjectAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveProjectAction.setToolTip("Save project to disk");

    _saveProjectAsAction.setShortcut(QKeySequence("Ctrl+Shift+S"));
    _saveProjectAsAction.setShortcutContext(Qt::ApplicationShortcut);
    _saveProjectAsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("save"));
    _saveProjectAsAction.setToolTip("Save project to disk in a chosen location");

    _editProjectSettingsAction.setShortcut(QKeySequence("Ctrl+Shift+P"));
    _editProjectSettingsAction.setShortcutContext(Qt::ApplicationShortcut);
    _editProjectSettingsAction.setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));

    _newProjectMenu.setIcon(Application::getIconFont("FontAwesome").getIcon("file"));
    _newProjectMenu.setTitle("New Project");
    _newProjectMenu.setToolTip("Create new project");
    _newProjectMenu.addAction(&_newBlankProjectAction);
    _newProjectMenu.addAction(&_newProjectFromWorkspaceAction);

    _importDataMenu.setIcon(Application::getIconFont("FontAwesome").getIcon("file-import"));
    _importDataMenu.setTitle("Import data...");
    _importDataMenu.setToolTip("Import data into HDPS");

    _publishAction.setShortcut(QKeySequence("Ctrl+P"));
    _publishAction.setShortcutContext(Qt::ApplicationShortcut);
    _publishAction.setIcon(Application::getIconFont("FontAwesome").getIcon("cloud-upload-alt"));
    _publishAction.setToolTip("Publish the HDPS application");

    _pluginManagerAction.setShortcut(QKeySequence("Ctrl+M"));
    _pluginManagerAction.setShortcutContext(Qt::ApplicationShortcut);
    _pluginManagerAction.setIcon(Application::getIconFont("FontAwesome").getIcon("plug"));
    _pluginManagerAction.setToolTip("View loaded plugins");

    _showStartPageAction.setShortcut(QKeySequence("Alt+W"));
    _showStartPageAction.setShortcutContext(Qt::ApplicationShortcut);
    _showStartPageAction.setIcon(Application::getIconFont("FontAwesome").getIcon("door-open"));
    _showStartPageAction.setToolTip("Show the HDPS start page");
    //_showStartPageAction.setChecked(!Application::current()->shouldOpenProjectAtStartup());

    auto mainWindow = Application::topLevelWidgets().first();

    mainWindow->addAction(&_newProjectFromWorkspaceAction);
    mainWindow->addAction(&_openProjectAction);
    mainWindow->addAction(&_importProjectAction);
    mainWindow->addAction(&_saveProjectAction);
    mainWindow->addAction(&_showStartPageAction);

    connect(&_newBlankProjectAction, &QAction::triggered, this, [this]() -> void {
        newBlankProject();
    });

    connect(&_newProjectFromWorkspaceAction, &QAction::triggered, this, [this]() -> void {
        auto* dialog = new NewProjectDialog();
        connect(dialog, &NewProjectDialog::finished, dialog, &NewProjectDialog::deleteLater);
        dialog->open();
    });

    connect(&_openProjectAction, &QAction::triggered, this, [this]() -> void {
        openProject();
    });

    connect(&_importProjectAction, &QAction::triggered, this, [this]() -> void {
        importProject();
    });

    connect(&_saveProjectAction, &QAction::triggered, [this]() -> void {
        if (_project.isNull())
            return;

        if (_project->getStudioModeAction().isChecked()) {
            _project->setStudioMode(false);
            {
                saveProject(_project->getFilePath());
            }
            _project->setStudioMode(true);
        }
        else {
            saveProject(_project->getFilePath());
        }
    });

    connect(&_saveProjectAsAction, &QAction::triggered, [this]() -> void {
        if (_project.isNull())
            return;

        saveProjectAs();
    });

    connect(&_editProjectSettingsAction, &TriggerAction::triggered, this, [this]() -> void {
        auto* dialog = new ProjectSettingsDialog();
        connect(dialog, &ProjectSettingsDialog::finished, dialog, &ProjectSettingsDialog::deleteLater);
        dialog->open();
    });

    connect(&_importDataMenu, &QMenu::aboutToShow, this, [this]() -> void {
        _importDataMenu.clear();

        for (auto pluginTriggerAction : plugins().getPluginTriggerActions(plugin::Type::LOADER))
            _importDataMenu.addAction(pluginTriggerAction);

        _importDataMenu.setEnabled(!_importDataMenu.actions().isEmpty());
    });

    connect(&_pluginManagerAction, &TriggerAction::triggered, this, [this]() -> void {
        PluginManagerDialog::create();
    });

    const auto updateActionsReadOnly = [this]() -> void {
        _saveProjectAction.setEnabled(hasProject());
        _saveProjectAsAction.setEnabled(hasProject());
        _saveProjectAsAction.setEnabled(hasProject() && !_project->getFilePath().isEmpty());
        _editProjectSettingsAction.setEnabled(hasProject());
        _importProjectAction.setEnabled(hasProject());
        _importDataMenu.setEnabled(hasProject());
        _pluginManagerAction.setEnabled(hasProject());
        _publishAction.setEnabled(hasProject());
    };

    connect(this, &ProjectManager::projectCreated, this, updateActionsReadOnly);
    connect(this, &ProjectManager::projectDestroyed, this, updateActionsReadOnly);
    connect(this, &ProjectManager::projectOpened, this, updateActionsReadOnly);
    connect(this, &ProjectManager::projectSaved, this, updateActionsReadOnly);

    connect(this, &ProjectManager::projectOpened, this, [this]() -> void {
        if (!hasProject())
            return;

        auto& splashScreenAction = getCurrentProject()->getSplashScreenAction();

        if (splashScreenAction.getEnabledAction().isChecked())
            splashScreenAction.getShowSplashScreenAction().trigger();
    });

    updateActionsReadOnly();

    _recentProjectsAction.initialize("Manager/Project/Recent", "Project", "Ctrl", Application::getIconFont("FontAwesome").getIcon("file"));

    connect(&_recentProjectsAction, &RecentFilesAction::triggered, this, [this](const QString& filePath) -> void {
        openProject(filePath);
    });

    connect(&_publishAction, &TriggerAction::triggered, this, [this]() -> void {
        publishProject();
    });
}

void ProjectManager::initialize()
{
#ifdef PROJECT_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    AbstractManager::initialize();
}

void ProjectManager::reset()
{
#ifdef PROJECT_MANAGER_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    beginReset();
    {
        auto core = Application::core();

        core->getActionsManager().reset();
        core->getPluginManager().reset();
        core->getDataHierarchyManager().reset();
        core->getDataManager().reset();
        core->getWorkspaceManager().reset();
    }
    endReset();
}

void ProjectManager::newProject(const QString& workspaceFilePath /*= ""*/)
{
    try
    {
#ifdef PROJECT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        createProject();

        if (QFileInfo(workspaceFilePath).exists())
            workspaces().loadWorkspace(workspaceFilePath);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to create new project", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to create new project");
    }
}

void ProjectManager::newProject(const Qt::AlignmentFlag& alignment, bool logging /*= false*/)
{
    newBlankProject();

    auto dockAreaFlag = DockAreaFlag::Right;

    switch (alignment) {
        case Qt::AlignLeft:
            dockAreaFlag = DockAreaFlag::Left;
            break;

        case Qt::AlignRight:
            dockAreaFlag = DockAreaFlag::Right;
            break;

        default:
            break;
    }

    plugin::ViewPlugin* dataHierarchyPlugin = nullptr;

    if (plugins().isPluginLoaded("Data hierarchy"))
        dataHierarchyPlugin = plugins().requestViewPlugin("Data hierarchy", nullptr, dockAreaFlag);

    if (plugins().isPluginLoaded("Data properties"))
        plugins().requestViewPlugin("Data properties", dataHierarchyPlugin, DockAreaFlag::Bottom);

    if (logging && plugins().isPluginLoaded("Logging"))
        plugins().requestViewPlugin("Logging", nullptr, DockAreaFlag::Bottom);
}

void ProjectManager::newBlankProject()
{
    try
    {
#ifdef PROJECT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__;
#endif

        createProject();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to create blank project", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to create blank project");
    }
}

void ProjectManager::openProject(QString filePath /*= ""*/, bool importDataOnly /*= false*/, bool loadWorkspace /*= true*/)
{
    try
    {
#ifdef PROJECT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << filePath;
#endif

        emit projectAboutToBeOpened(*(_project.get()));
        {
            const auto scopedState = ScopedState(this, State::OpeningProject);

            if (QFileInfo(filePath).isDir())
                throw std::runtime_error("Project file path may not be a directory");

            const auto loadedDatasets = Application::core()->requestAllDataSets();

            QTemporaryDir temporaryDirectory;

            const auto temporaryDirectoryPath = temporaryDirectory.path();

            Application::setSerializationTemporaryDirectory(temporaryDirectoryPath);
            Application::setSerializationAborted(false);

            ToggleAction disableReadOnlyAction(this, "Allow edit of published project");

            if (filePath.isEmpty()) {
                QFileDialog fileDialog;

                fileDialog.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("folder-open"));
                fileDialog.setWindowTitle("Open ManiVault Project");
                fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
                fileDialog.setFileMode(QFileDialog::ExistingFile);
                fileDialog.setNameFilters({ "ManiVault project files (*.mv)" });
                fileDialog.setDefaultSuffix(".mv");
                fileDialog.setDirectory(Application::current()->getSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());
                fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);

                StringAction    titleAction(this, "Title");
                StringAction    descriptionAction(this, "Description");
                StringAction    tagsAction(this, "Tags");
                StringAction    commentsAction(this, "Comments");
                StringAction    contributorsAction(this, "Contributors");

                titleAction.setEnabled(false);
                descriptionAction.setEnabled(false);
                tagsAction.setEnabled(false);
                commentsAction.setEnabled(false);
                contributorsAction.setEnabled(false);
                disableReadOnlyAction.setEnabled(false);

                auto fileDialogLayout   = dynamic_cast<QGridLayout*>(fileDialog.layout());
                auto rowCount           = fileDialogLayout->rowCount();

                fileDialogLayout->addWidget(titleAction.createLabelWidget(&fileDialog), rowCount, 0);
                fileDialogLayout->addWidget(titleAction.createWidget(&fileDialog), rowCount, 1, 1, 2);

                fileDialogLayout->addWidget(descriptionAction.createLabelWidget(&fileDialog), rowCount + 1, 0);
                fileDialogLayout->addWidget(descriptionAction.createWidget(&fileDialog), rowCount + 1, 1, 1, 2);

                fileDialogLayout->addWidget(tagsAction.createLabelWidget(&fileDialog), rowCount + 2, 0);
                fileDialogLayout->addWidget(tagsAction.createWidget(&fileDialog), rowCount + 2, 1, 1, 2);

                fileDialogLayout->addWidget(commentsAction.createLabelWidget(&fileDialog), rowCount + 3, 0);
                fileDialogLayout->addWidget(commentsAction.createWidget(&fileDialog), rowCount + 3, 1, 1, 2);

                fileDialogLayout->addWidget(contributorsAction.createLabelWidget(&fileDialog), rowCount + 4, 0);
                fileDialogLayout->addWidget(contributorsAction.createWidget(&fileDialog), rowCount + 4, 1, 1, 2);
       
                fileDialogLayout->addWidget(disableReadOnlyAction.createWidget(&fileDialog), rowCount + 5, 1, 1, 2);

                connect(&fileDialog, &QFileDialog::currentChanged, this, [&](const QString& filePath) -> void {
                    if (!QFileInfo(filePath).isFile())
                        return;
                    
                    QTemporaryDir temporaryDir;

                    Project project(extractProjectFileFromHdpsFile(filePath, temporaryDir), true);

                    titleAction.setString(project.getTitleAction().getString());
                    descriptionAction.setString(project.getDescriptionAction().getString());
                    tagsAction.setString(project.getTagsAction().getStrings().join(", "));
                    commentsAction.setString(project.getCommentsAction().getString());
                    contributorsAction.setString(project.getContributorsAction().getStrings().join(","));
                    disableReadOnlyAction.setEnabled(project.getReadOnlyAction().isChecked());
                });

                fileDialog.open();

                QEventLoop eventLoop;
                QObject::connect(&fileDialog, &QDialog::finished, &eventLoop, &QEventLoop::quit);
                eventLoop.exec();

                if (fileDialog.result() != QDialog::Accepted)
                    return;

                if (fileDialog.selectedFiles().count() != 1)
                    throw std::runtime_error("Only one file may be selected");

                filePath = fileDialog.selectedFiles().first();

                Application::current()->setSetting("Projects/WorkingDirectory", QFileInfo(filePath).absolutePath());
            }

            if (!importDataOnly)
                newProject();

            qDebug().noquote() << "Open ManiVault project from" << filePath;

            Archiver archiver;

            QStringList tasks = archiver.getTaskNamesForDecompression(filePath) << "Import data model" << "Load workspace";

            TaskProgressDialog taskProgressDialog(nullptr, tasks, "Open ManiVault project from " + filePath, Application::getIconFont("FontAwesome").getIcon("folder-open"));

            connect(&taskProgressDialog, &TaskProgressDialog::canceled, this, [this]() -> void {
                Application::setSerializationAborted(true);

                throw std::runtime_error("Canceled before project was loaded");
            });

            connect(&Application::core()->getDataHierarchyManager(), &AbstractDataHierarchyManager::itemLoading, this, [&taskProgressDialog](DataHierarchyItem& loadingItem) {
                //taskProgressDialog.setCurrentTask("Importing dataset: " + loadingItem.getFullPathName());
            });

            connect(&archiver, &Archiver::taskStarted, &taskProgressDialog, &TaskProgressDialog::setCurrentTask);
            connect(&archiver, &Archiver::taskFinished, &taskProgressDialog, &TaskProgressDialog::setTaskFinished);

            archiver.decompress(filePath, temporaryDirectoryPath);

            taskProgressDialog.setCurrentTask("Import data model");
            {
                projects().fromJsonFile(QFileInfo(temporaryDirectoryPath, "project.json").absoluteFilePath());
            }
            taskProgressDialog.setTaskFinished("Import data model");

            if (loadWorkspace) {
                taskProgressDialog.setCurrentTask("Load workspace");
                {
                    const QFileInfo workspaceFileInfo(temporaryDirectoryPath, "workspace.json");

                    if (workspaceFileInfo.exists())
                        workspaces().loadWorkspace(workspaceFileInfo.absoluteFilePath(), false);

                    workspaces().setWorkspaceFilePath("");
                }
                taskProgressDialog.setTaskFinished("Load workspace");
            }

            _recentProjectsAction.addRecentFilePath(filePath);

            _project->setFilePath(filePath);
            _project->updateContributors();

            if (disableReadOnlyAction.isEnabled() && disableReadOnlyAction.isChecked())
                _project->getReadOnlyAction().setChecked(disableReadOnlyAction.isChecked());

            qDebug().noquote() << filePath << "loaded successfully";
        }
        emit projectOpened(*(_project.get()));
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to load ManiVault project", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to load ManiVault project");
    }
}

void ProjectManager::importProject(QString filePath /*= ""*/)
{
    try
    {
#ifdef PROJECT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << filePath;
#endif

        const auto scopedState = ScopedState(this, State::ImportingProject);

        emit projectAboutToBeImported(filePath);
        {
            openProject(filePath, true, false);
        }
        emit projectImported(filePath);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to import project", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to import project");
    }
}

void ProjectManager::saveProject(QString filePath /*= ""*/, const QString& password /*= ""*/)
{
    try
    {
#ifdef PROJECT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << filePath;
#endif

        const auto scopedState = ScopedState(this, State::SavingProject);

        emit projectAboutToBeSaved(*(_project.get()));
        {
            if (QFileInfo(filePath).isDir())
                throw std::runtime_error("Project file path may not be a directory");

            QTemporaryDir temporaryDirectory;

            const auto temporaryDirectoryPath = temporaryDirectory.path();

            auto currentProject = getCurrentProject();

            if (filePath.isEmpty()) {

                QFileDialog fileDialog;

                fileDialog.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("save"));
                fileDialog.setWindowTitle("Save ManiVault Project");
                fileDialog.setAcceptMode(QFileDialog::AcceptSave);
                fileDialog.setNameFilters({ "ManiVault project files (*.mv)" });
                fileDialog.setDefaultSuffix(".mv");
                fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
                fileDialog.setDirectory(Application::current()->getSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());

                auto fileDialogLayout   = dynamic_cast<QGridLayout*>(fileDialog.layout());
                auto rowCount           = fileDialogLayout->rowCount();

                QCheckBox   passwordProtectedCheckBox("Password protected");
                QLineEdit   passwordLineEdit;

                passwordProtectedCheckBox.setChecked(false);
                passwordLineEdit.setPlaceholderText("Enter encryption password...");

                auto compressionLayout = new QHBoxLayout();

                compressionLayout->addWidget(currentProject->getCompressionAction().getEnabledAction().createWidget(&fileDialog));
                compressionLayout->addWidget(currentProject->getCompressionAction().getLevelAction().createWidget(&fileDialog), 1);
                
                fileDialogLayout->addLayout(compressionLayout, rowCount, 1, 1, 2);
                
                //fileDialogLayout->addWidget(&passwordProtectedCheckBox, ++rowCount, 0);
                //fileDialogLayout->addWidget(&passwordLineEdit, rowCount, 1);

                auto& titleAction = currentProject->getTitleAction();

                fileDialogLayout->addWidget(titleAction.createLabelWidget(nullptr), rowCount + 2, 0);

                GroupAction settingsGroupAction(this, "Settings");

                settingsGroupAction.setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
                settingsGroupAction.setToolTip("Edit project settings");
                settingsGroupAction.setPopupSizeHint(QSize(420, 320));
                settingsGroupAction.setLabelSizingType(GroupAction::LabelSizingType::Auto);

                settingsGroupAction.addAction(&currentProject->getTitleAction());
                settingsGroupAction.addAction(&currentProject->getDescriptionAction());
                settingsGroupAction.addAction(&currentProject->getTagsAction());
                settingsGroupAction.addAction(&currentProject->getCommentsAction());

                auto titleLayout = new QHBoxLayout();

                titleLayout->addWidget(titleAction.createWidget(&fileDialog));
                titleLayout->addWidget(settingsGroupAction.createCollapsedWidget(&fileDialog));

                fileDialogLayout->addLayout(titleLayout, rowCount + 2, 1, 1, 2);

                //const auto updatePassword = [&]() -> void {
                //    passwordLineEdit.setEnabled(passwordProtectedCheckBox.isChecked());
                //};

                //connect(&passwordProtectedCheckBox, &QCheckBox::toggled, this, updatePassword);

                //updatePassword();

                connect(&fileDialog, &QFileDialog::currentChanged, this, [this, currentProject](const QString& path) -> void {
                    if (!QFileInfo(path).isFile())
                        return;

                    QTemporaryDir temporaryDir;

                    const auto projectJsonFilePath = projects().extractProjectFileFromHdpsFile(path, temporaryDir);

                    Project project(projectJsonFilePath, true);

                    currentProject->getCompressionAction().getEnabledAction().setChecked(project.getCompressionAction().getEnabledAction().isChecked());
                    currentProject->getCompressionAction().getLevelAction().setValue(project.getCompressionAction().getLevelAction().getValue());
                });

                fileDialog.open();

                QEventLoop eventLoop;
                QObject::connect(&fileDialog, &QDialog::finished, &eventLoop, &QEventLoop::quit);
                eventLoop.exec();

                if (fileDialog.result() != QDialog::Accepted)
                    return;

                if (fileDialog.selectedFiles().count() != 1)
                    throw std::runtime_error("Only one file may be selected");

                filePath = fileDialog.selectedFiles().first();

                Application::current()->setSetting("Projects/WorkingDirectory", QFileInfo(filePath).absolutePath());
            }
            
            if (filePath.isEmpty() || QFileInfo(filePath).isDir())
                return;

            if (currentProject->getCompressionAction().getEnabledAction().isChecked())
                qDebug().noquote() << "Saving ManiVault project to" << filePath << "with compression level" << currentProject->getCompressionAction().getLevelAction().getValue();
            else
                qDebug().noquote() << "Saving ManiVault project to" << filePath << "without compression";

            Archiver archiver;

            QStringList tasks;

            tasks << "Export data model" << "Temporary task";

            TaskProgressDialog taskProgressDialog(nullptr, tasks, "Saving ManiVault project to " + filePath, Application::current()->getIconFont("FontAwesome").getIcon("save"));

            taskProgressDialog.setCurrentTask("Export data model");

            connect(&taskProgressDialog, &TaskProgressDialog::canceled, this, [this]() -> void {
                Application::setSerializationAborted(true);

                throw std::runtime_error("Canceled before project was saved");
            });

            QFileInfo jsonFileInfo(temporaryDirectoryPath, "project.json");

            Application::setSerializationTemporaryDirectory(temporaryDirectoryPath);
            Application::setSerializationAborted(false);

            connect(&Application::core()->getDataHierarchyManager(), &AbstractDataHierarchyManager::itemSaving, this, [&taskProgressDialog](DataHierarchyItem& savingItem) {
                taskProgressDialog.setCurrentTask("Exporting dataset: " + savingItem.getLocation());
            });

            projects().toJsonFile(jsonFileInfo.absoluteFilePath());

            taskProgressDialog.setTaskFinished("Export data model");
            taskProgressDialog.addTasks(archiver.getTaskNamesForDirectoryCompression(temporaryDirectoryPath));
            taskProgressDialog.setTaskFinished("Temporary task");

            connect(&archiver, &Archiver::taskStarted, &taskProgressDialog, &TaskProgressDialog::setCurrentTask);
            connect(&archiver, &Archiver::taskFinished, &taskProgressDialog, &TaskProgressDialog::setTaskFinished);

            QFileInfo workspaceFileInfo(temporaryDirectoryPath, "workspace.json");

            workspaces().saveWorkspace(workspaceFileInfo.absoluteFilePath(), false);

            archiver.compressDirectory(temporaryDirectoryPath, filePath, true, currentProject->getCompressionAction().getEnabledAction().isChecked() ? currentProject->getCompressionAction().getLevelAction().getValue() : 0, password);

            _recentProjectsAction.addRecentFilePath(filePath);

            _project->setFilePath(filePath);

            qDebug().noquote() << filePath << "saved successfully";
        }
        emit projectSaved(*(_project.get()));
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to save project", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to save project");
    }
}

void ProjectManager::saveProjectAs()
{
    saveProject("");
}

void ProjectManager::publishProject(QString filePath /*= ""*/)
{
    try
    {
#ifdef PROJECT_MANAGER_VERBOSE
        qDebug() << __FUNCTION__ << filePath;
#endif

        if (!hasProject())
            return;

        const auto scopedState = ScopedState(this, State::PublishingProject);

        auto& readOnlyAction        = getCurrentProject()->getReadOnlyAction();
        auto& splashScreenAction    = getCurrentProject()->getSplashScreenAction();

        readOnlyAction.cacheState();
        splashScreenAction.cacheState();
        
        readOnlyAction.setChecked(true);
        splashScreenAction.getEnabledAction().setChecked(true);

        emit projectAboutToBePublished(*(_project.get()));
        {
            if (QFileInfo(filePath).isDir())
                throw std::runtime_error("Project file path may not be a directory");

            QTemporaryDir temporaryDirectory;

            const auto temporaryDirectoryPath = temporaryDirectory.path();

            auto currentProject = getCurrentProject();

            ToggleAction    passwordProtectedAction(this, "Password Protected");
            StringAction    passwordAction(this, "Password");

            if (filePath.isEmpty()) {

                QFileDialog fileDialog;

                fileDialog.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cloud-upload-alt"));
                fileDialog.setWindowTitle("Publish ManiVault Project");
                fileDialog.setAcceptMode(QFileDialog::AcceptSave);
                fileDialog.setNameFilters({ "ManiVault project files (*.mv)" });
                fileDialog.setDefaultSuffix(".mv");
                fileDialog.setOption(QFileDialog::DontUseNativeDialog, true);
                fileDialog.setDirectory(Application::current()->getSetting("Projects/WorkingDirectory", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)).toString());

                auto fileDialogLayout   = dynamic_cast<QGridLayout*>(fileDialog.layout());
                auto rowCount           = fileDialogLayout->rowCount();

                QStringList options{ "Compression", "Title" };

                if (options.contains("Password")) {
                    passwordProtectedAction.setToolTip("Whether to password-protect the project");

                    passwordAction.setToolTip("Project password");
                    passwordAction.setPlaceHolderString("Enter encryption password...");
                    passwordAction.setClearable(true);

                    auto passwordLayout = new QHBoxLayout();

                    passwordLayout->addWidget(passwordProtectedAction.createWidget(&fileDialog));
                    passwordLayout->addWidget(passwordAction.createWidget(&fileDialog), 1);

                    const auto updatePasswordActionReadOnly = [&]() -> void {
                        passwordAction.setEnabled(passwordProtectedAction.isChecked());
                    };

                    connect(&passwordProtectedAction, &ToggleAction::toggled, this, updatePasswordActionReadOnly);

                    updatePasswordActionReadOnly();

                    fileDialogLayout->addLayout(passwordLayout, rowCount + 3, 1, 1, 2);
                }

                if (options.contains("Compression")) {
                    auto compressionLayout = new QHBoxLayout();

                    compressionLayout->addWidget(currentProject->getCompressionAction().getEnabledAction().createWidget(&fileDialog));
                    compressionLayout->addWidget(currentProject->getCompressionAction().getLevelAction().createWidget(&fileDialog), 1);

                    fileDialogLayout->addLayout(compressionLayout, rowCount, 1, 1, 2);
                }
                    
                GroupAction settingsGroupAction(this, "Settings");

                if (options.contains("Title")) {
                    auto& titleAction = currentProject->getTitleAction();

                    fileDialogLayout->addWidget(titleAction.createLabelWidget(nullptr), rowCount + 2, 0);

                    settingsGroupAction.setIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
                    settingsGroupAction.setToolTip("Edit project settings");
                    settingsGroupAction.setPopupSizeHint(QSize(420, 0));
                    settingsGroupAction.setLabelSizingType(GroupAction::LabelSizingType::Auto);

                    settingsGroupAction.addAction(&currentProject->getTitleAction());
                    settingsGroupAction.addAction(&currentProject->getDescriptionAction());
                    settingsGroupAction.addAction(&currentProject->getTagsAction());
                    settingsGroupAction.addAction(&currentProject->getCommentsAction());
                    settingsGroupAction.addAction(&currentProject->getSplashScreenAction());

                    auto titleLayout = new QHBoxLayout();

                    titleLayout->addWidget(titleAction.createWidget(&fileDialog));
                    titleLayout->addWidget(settingsGroupAction.createCollapsedWidget(&fileDialog));

                    fileDialogLayout->addLayout(titleLayout, rowCount + 2, 1, 1, 2);
                }
                    
                connect(&fileDialog, &QFileDialog::currentChanged, this, [this, currentProject](const QString& path) -> void {
                    if (!QFileInfo(path).isFile())
                        return;

                    QTemporaryDir temporaryDir;

                    const auto projectJsonFilePath = projects().extractProjectFileFromHdpsFile(path, temporaryDir);

                    Project project(projectJsonFilePath, true);

                    currentProject->getCompressionAction().getEnabledAction().setChecked(project.getCompressionAction().getEnabledAction().isChecked());
                    currentProject->getCompressionAction().getLevelAction().setValue(project.getCompressionAction().getLevelAction().getValue());
                });

                fileDialog.open();

                QEventLoop eventLoop;
                QObject::connect(&fileDialog, &QDialog::finished, &eventLoop, &QEventLoop::quit);
                eventLoop.exec();

                if (fileDialog.result() != QDialog::Accepted)
                    return;

                if (fileDialog.selectedFiles().count() != 1)
                    throw std::runtime_error("Only one file may be selected");

                filePath = fileDialog.selectedFiles().first();

                Application::current()->setSetting("Projects/WorkingDirectory", QFileInfo(filePath).absolutePath());
            }

            if (filePath.isEmpty() || QFileInfo(filePath).isDir())
                return;
                
            auto& workspaceLockingAction = workspaces().getCurrentWorkspace()->getLockingAction();

            const auto cacheWorkspaceLocked = workspaceLockingAction.isLocked();

            workspaceLockingAction.setLocked(true);
            {
                saveProject(filePath, passwordAction.getString());
            }
            workspaceLockingAction.setLocked(cacheWorkspaceLocked);
        }
        emit projectPublished(*(_project.get()));
        
        readOnlyAction.restoreState();
        splashScreenAction.restoreState();
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to publish ManiVault project", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to publish ManiVault project");
    }
}

QMenu& ProjectManager::getNewProjectMenu()
{
    return _newProjectMenu;
}

QMenu& ProjectManager::getImportDataMenu()
{
    return _importDataMenu;
}

void ProjectManager::createProject()
{
    emit projectAboutToBeCreated();
    {
        reset();

        _project.reset(new Project());
    }
    emit projectCreated(*(_project.get()));

    _showStartPageAction.setChecked(false);

    workspaces().reset();
}

bool ProjectManager::hasProject() const
{
    return getCurrentProject() != nullptr;
}

const hdps::Project* ProjectManager::getCurrentProject() const
{
    return _project.get();
}

hdps::Project* ProjectManager::getCurrentProject()
{
    return _project.get();
}

QString ProjectManager::extractProjectFileFromHdpsFile(const QString& hdpsFilePath, QTemporaryDir& temporaryDir)
{
    const auto temporaryDirectoryPath = temporaryDir.path();

    const QString projectFile("project.json");

    QFileInfo projectFileInfo(temporaryDirectoryPath, projectFile);

    Archiver archiver;
    QString projectFilePath = "";

    try
    {
        archiver.extractSingleFile(hdpsFilePath, projectFile, projectFileInfo.absoluteFilePath());
        projectFilePath = projectFileInfo.absoluteFilePath();
    }
    catch (const std::runtime_error& e)
    {
        qDebug() << "ProjectManager: exception caught in extractProjectFileFromHdpsFile, given file path " << hdpsFilePath << ": " << e.what();
    }

    return projectFilePath;
}

void ProjectManager::fromVariantMap(const QVariantMap& variantMap)
{
    createProject();

    _project->fromVariantMap(variantMap);
}

QVariantMap ProjectManager::toVariantMap() const
{
    if (hasProject())
        return _project->toVariantMap();

    return QVariantMap();
}

QImage ProjectManager::getPreviewImage(const QString& projectFilePath, const QSize& targetSize /*= QSize(500, 500)*/) const
{
    Archiver archiver;

    const QString workspaceFile("workspace.json");

    QTemporaryDir temporaryDirectory;

    const auto temporaryDirectoryPath = temporaryDirectory.path();

    QFileInfo workspaceFileInfo(temporaryDirectoryPath, workspaceFile);

    try
    {
        archiver.extractSingleFile(projectFilePath, workspaceFile, workspaceFileInfo.absoluteFilePath());
    }
    catch (const std::runtime_error& e)
    {
        qDebug() << "ProjectManager: exception caught in getPreviewImage, given file path " << projectFilePath << ": " << e.what();
    }

    if (workspaceFileInfo.exists())
        return Workspace::getPreviewImage(workspaceFileInfo.absoluteFilePath());

    return QImage();
}
