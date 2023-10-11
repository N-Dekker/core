// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Task.h"
#include "ModalTask.h"

namespace hdps {

/**
 * Project serialization task class
 *
 * Defines a task for use during project serialization (used during application startup and use).
 *
 * @author Thomas Kroes
 */
class ProjectSerializationTask final : public Task
{
    Q_OBJECT

public:

    /**
    * Construct task with \p parent object, \p name and initial \p status
    * @param parent Pointer to parent object
    * @param name Name of the task
    * @param status Initial status of the task
    * @param mayKill Boolean determining whether the task may be killed or not
    */
    ProjectSerializationTask(QObject* parent, const QString& name, const Status& status = Status::Undefined, bool mayKill = false);

public: // Task getters

    ModalTask& getDataTask();               /** Get task for serializing the project data */
    ModalTask& getWorkspaceTask();          /** Get task for serializing the workspace */
    ModalTask& getSystemViewPluginsTask();  /** Get task for serializing the system view plugins */
    ModalTask& getViewPluginsTask();        /** Get task for serializing the view plugins */

private:
    ModalTask   _dataTask;                  /** Task for serializing the project data */
    ModalTask   _workspaceTask;             /** Task for serializing the workspace */
    ModalTask   _systemViewPluginsTask;     /** Task for serializing the system view plugins */
    ModalTask   _viewPluginsTask;           /** Task for serializing the view plugins */
};

}
