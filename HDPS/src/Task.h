// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "util/Serializable.h"

#include <QObject>
#include <QBitArray>
#include <QTimer>
#include <QIcon>
#include <QSet>

namespace mv {

class AbstractTaskHandler;

/**
 * Task class
 *
 * Convenience class for managing a task.
 *
 * Task progress can be determined in three ways (see Task::setProgressMode() and Task::getProgressMode()):
 *  - Setting progress directly setProgress(...)
 *  - Setting sub tasks items via one of the overloads of Task::setSubTasks() and flagging items as finished 
 *    with Task::setSubtaskFinished(), the percentage is then updated automatically
 *  - Computing the combined progress of child tasks using aggregation (initialize with parent task or use Task::setParentTask())
 * 
 * Tasks have a scope which defines how the content is presented in the user interface (see Task::setScope() and Task::getScope())
 *  - All background tasks are aggregated into one overarching task and presented in the status bar
 *  - Foreground tasks automatically show up in a popup at the right of the status bar 
 *  - Modal tasks show up in a modal tasks dialog which blocks all other user interaction
 * 
 * There is no need to create a user interface to kill tasks, all scopes have controls to kill a task. Connect to the Task::requestAbort() signal
 * to be notified when a task was aborted so that further action can be taken.
 * 
 * Special notes:
 *  - Tasks should work cross threads (this has only been tested using QThread though)
 *  - For a more detail exploration of all tasks in the system, a tasks view system plugin is available
 *  - In case of a tasks hierarchy, all task objects should be in the same QThread context 
 * 
 * @author Thomas Kroes
 */
class Task : public QObject, public util::Serializable
{
    Q_OBJECT

public:

    /** Task: */
    enum class Status {
        Undefined = -1,         /** ...status is undefined */
        Idle,                   /** ...is idle */
        Running,                /** ...is currently running */
        RunningIndeterminate,   /** ...is currently running, but it's operating time is not known */
        Finished,               /** ...has finished successfully */
        AboutToBeAborted,       /** ...is about to be aborted */
        Aborting,               /** ...is in the process of being aborted */
        Aborted                 /** ...has been aborted */
    };

    /** Couples status enum value to status name string */
    static QMap<Status, QString> statusNames;

    /** Progress is tracked by: */
    enum class ProgressMode {
        Manual,     /** ...setting progress percentage manually */
        Subtasks,   /** ...setting a number of subtasks and flagging subtasks as finished (progress percentage is computed automatically) */
        Aggregate   /** ...combining the progress of child tasks */
    };

    /** Couples scope enum value to scope name string */
    static QMap<ProgressMode, QString> progressModeNames;

    /** Task: */
    enum class GuiScope {
        None,               /** ...has no explicit GUI handlers that show it in the gui (the task can still be observed in the tasks plugin) */
        Background,         /** ...will run in the background (not visible by default but can be revealed in the tasks view plugin) */
        Foreground,         /** ...will run in the foreground (tasks with this scope will automatically appear in a tasks popup when running) */
        Modal,              /** ...will run modally (tasks with this scope will automatically appear in a modal tasks dialog when the needed) */
        DataHierarchy,      /** ...will be displayed in the data hierarchy */
    };

    /** Couples scope enum value to scope name string */
    static QMap<GuiScope, QString> guiScopeNames;

    using TasksPtrs             = QVector<Task*>;
    using ProgressTextFormatter = std::function<QString(Task&)>;
    using GuiScopes             = QSet<GuiScope>;
    using Statuses              = QVector<Status>;

public:

    /**
     * Construct task with \p parent object, \p name, initial \p status, whether the task may be killed \p mayKill and possibly a \p taskHandler
     * 
     * @param parent Pointer to parent object (simply denotes the position of the task in the QObject hierarchy and has nothing to do with the task hierarchy)
     * @param name Name of the task
     * @param guiScopes GUI scopes of the task
     * @param status Initial status of the task
     * @param mayKill Boolean determining whether the task may be killed or not
     * @param handler Pointer to task handler
     */
    Task(QObject* parent, const QString& name, const GuiScopes& guiScopes = { GuiScope::None }, const Status& status = Status::Undefined, bool mayKill = false, AbstractTaskHandler* handler = nullptr);

    /** Remove from task manager when destructed */
    ~Task();

    /**
     * Get type (class) name (using QMetaObject)
     * @param humanFriendly Whether to include the task namespace or not
     * @return Task type name
     */
    virtual QString getTypeName(bool humanFriendly = true) const final;

public: // Parent-child

    /**
     * Get parent task (if it has one)
     * @return Pointer to parent task if it has, nullptr otherwise
     */
    Task* getParentTask();

    /**
     * Set parent task to \p parentTask
     * @param parentTask Pointer to parent task
     */
    void setParentTask(Task* parentTask);

    /**
     * Determine whether the task has a parent task
     * @return Boolean determining whether the task has a parent task
     */
    bool hasParentTask();

    /**
     * Get child tasks
     * @param recursively Whether to get child tasks recursively
     * @param enabledOnly Filter out tasks which are disabled
     * @return Vector of pointers to child tasks
     */
    TasksPtrs getChildTasks(bool recursively = false, bool enabledOnly = true) const;

    /**
     * Get child tasks for \p statuses
     * @param recursively Whether to get child tasks recursively
     * @param enabledOnly Filter out tasks which are disabled
     * @param status Filter on task status
     * @return Vector of pointer to child tasks
     */
    TasksPtrs getChildTasksForStatuses(bool recursively = false, bool enabledOnly = true, const Statuses& statuses = Statuses()) const;

    /**
     * Get child tasks for \p guiScopes
     * @param recursively Whether to get child tasks recursively
     * @param enabledOnly Filter out tasks which are disabled
     * @param guiScopes Filter on task GUI scope
     * @return Vector of pointer to child tasks
     */
    TasksPtrs getChildTasksForGuiScopes(bool recursively = false, bool enabledOnly = true, const GuiScopes& guiScopes = GuiScopes()) const;

    /**
     * Get child tasks for \p guiScopes and \p statuses 
     * @param recursively Whether to get child tasks recursively
     * @param enabledOnly Filter out tasks which are disabled
     * @param guiScopes Filter on task GUI scope
     * @param statuses Filter on task status
     * @return Vector of pointer to child tasks
     */
    TasksPtrs getChildTasksForGuiScopesAndStatuses(bool recursively = false, bool enabledOnly = true, const GuiScopes& guiScopes = GuiScopes(), const Statuses& statuses = Statuses()) const;

protected: // Parent-child

    /**
     * Adds \p childTask to children
     * @param childTask Pointer to child task to add
     */
    virtual void addChildTask(Task* childTask);

    /**
     * Removes \p childTask from children
     * @param childTask Pointer to child task to remove
     */
    virtual void removeChildTask(Task* childTask);

public: // Name, description, icon and may kill

    /**
     * Get task name
     * @return Task name
     */
    virtual QString getName() const final;

    /**
     * Set task name to \p name
     * @param name Name of the task
     */
    virtual void setName(const QString& name) final;

    /** Gets the task description */
    virtual QString getDescription() const final;

    /**
     * Sets the task description to \p description
     * @param description Task description
     */
    virtual void setDescription(const QString& description) final;

    /** Gets the task icon */
    virtual QIcon getIcon() const final;

    /**
     * Sets the task icon to \p icon
     * @param icon Task icon
     */
    virtual void setIcon(const QIcon& icon) final;

    /**
     * Get whether the task is enabled or not
     * @return Boolean determining whether the task is enabled or not
     */
    virtual bool getEnabled() const final;

    /**
     * Sets whether the task is enabled or not
     * @param enabled Boolean determining whether the task is enabled or not
     * @param recursive Whether to set all descendants to \p enabled as well
     */
    virtual void setEnabled(bool enabled, bool recursive = false) final;

    /**
     * Get whether the task is visible or not
     * @return Boolean determining whether the task is visible or not
     */
    virtual bool getVisible() const final;

    /**
     * Sets whether the task is visible or not
     * @param visible Boolean determining whether the task is visible or not
     */
    virtual void setVisible(bool visible) final;

    /**
     * Get whether the task may be killed or not
     * @return Whether the task may be killed or not
     */
    virtual bool getMayKill() const final;

    /**
     * Sets whether the task may be killed or not
     * @param mayKill Boolean determining whether the task may be killed or not
     * @param recursive Whether to set all descendants to \p mayKill as well
     */
    virtual void setMayKill(bool mayKill, bool recursive = false) final;

    /**
     * Get whether the task is killable
     * Return true when the following two criteria are met:
     *  - Task#_status is either Task::Status::Running or Task::Status::RunningIndeterminate
     *  - Task#_mayKill is true
     * @return Whether the task is killable
     */
    virtual bool isKillable() const final;

    /**
     * Resets the task internals
     * @param recursive Whether to reset all descendants as well
     */
    virtual void reset(bool recursive = false) final;

public: // Status

    /** Get task status */
    virtual Status getStatus() const final;

    /** Check if task is idle */
    virtual bool isIdle() const final;

    /** Check if task is running */
    virtual bool isRunning() const final;

    /** Check if task is running indeterminate */
    virtual bool isRunningIndeterminate() const final;

    /** Check if task is finished */
    virtual bool isFinished() const final;

    /** Check if task is about to be aborted */
    virtual bool isAboutToBeAborted() const final;

    /** Check if task is being aborted */
    virtual bool isAborting() const final;

    /** Check if task is aborted */
    virtual bool isAborted() const final;

    /**
     * Set task status to \p status, possibly \p recursive
     * @param status Task status
     * @param recursive Whether to set all descendants statuses as well
     */
    virtual void setStatus(const Status& status, bool recursive = false) final;

    /**
     * Set task status deferred to \p status, possibly \p recursively and after \p delay
     * @param status Deferred task status
     * @param recursive Whether to set all descendants statuses as well
     * @param delay Delay after which the deferred task status is set
     */
    virtual void setStatusDeferred(const Status& status, bool recursive = false, std::uint32_t delay = DEFERRED_TASK_STATUS_INTERVAL) final;

    /** Convenience method to set task status to undefined */
    virtual void setUndefined() final;

    /** Convenience method to set task status to idle */
    virtual void setIdle() final;

    /** Convenience method to set task status to running */
    virtual void setRunning() final;

    /** Convenience method to set task status to running indeterminate */
    virtual void setRunningIndeterminate() final;

    /** Convenience method to set task status to finished and use a custom progress description */
    virtual void setFinished() final;

    /** Convenience method to set task status to about to be aborted */
    virtual void setAboutToBeAborted() final;

    /** Convenience method to set task status to aborting */
    virtual void setAborting() final;

    /** Convenience method to set task status to aborted */
    virtual void setAborted() final;

    /**
     * Kill the task and trigger Task::abort() signal
     * @param recursive Boolean determining whether to also kill chill tasks recursively
     */
    virtual void kill(bool recursive = true) final;

public:

    /**
     * Get task handler
     * @return Pointer to task handler
     */
    virtual AbstractTaskHandler* getHandler() final;

    /**
     * Set task handler to \p handler
     * @param handler Pointer to task handler
     */
    virtual void setHandler(AbstractTaskHandler* handler) final;

public: // Progress mode

    /**
     * Get progress mode
     * @return Progress mode enum
     */
    virtual ProgressMode getProgressMode() const final;

    /**
     * Sets progress mode to to \p progressMode
     * @param progressMode Progress mode
     */
    void setProgressMode(const ProgressMode& progressMode);

public: // GUI scopes

    /**
     * Get GUI scopes
     * @return GUI scopes
     */
    virtual GuiScopes getGuiScopes() const final;
    
    /**
     * Sets GUI scopes to \p guiScopes
     * @param guiScope GUI scope enum
     */
    virtual void setGuiScopes(const GuiScopes& guiScopes) final;

    /**
     * Add \p guiScope to the GUI scopes set
     * @param guiScope GUI scope to add
     */
    virtual void addGuiScopes(const GuiScope& guiScope) final;

    /**
     * Remove \p guiScope from the GUI scopes set
     * @param guiScope GUI scope to remove
     */
    virtual void removeGuiScopes(const GuiScope& guiScope) final;

    /**
     * Function to establish whether at lease one GUI scope is present in both \p guiScopesA and \p guiScopesB
     * @param guiScopesA GUI scopes A
     * @param guiScopesB GUI scopes B
     */
    static bool doGuiScopesOverlap(const GuiScopes& guiScopesA, const GuiScopes& guiScopesB);

    /**
     * Convert \p guiScopes to string list
     * @return String list of gui scopes
     */
    static QStringList guiScopesToStringlist(const GuiScopes& guiScopes);

public: // Manual

    /** Gets the task progress [0, 1] */
    virtual float getProgress() const final;

    /**
     * Sets the task progress percentage to \p progress
     * This method only has an effect when Task#_progressMode is set to ProgressMode::Manual
     * @param progress Task progress, clamped to [0, 1]
     * @param subtaskDescription Subtask description associated with the progress update
     */
    virtual void setProgress(float progress, const QString& subtaskDescription = "");

    /**
     * Re-sets the task progress percentage to zero
     * @param recursive Boolean determining whether to also set the descendant tasks
     */
    virtual void resetProgress(bool recursive = false);

public: // Subtasks

    /**
     * Initializes the subtasks with \p numberOfSubtasks
     * This method sets the Task#_progressMode to ProgressMode::Subtasks
     * @param numberOfSubtasks Number of subtasks
     */
    virtual void setSubtasks(std::uint32_t numberOfSubtasks) final;

    /**
     * Initializes the subtasks with \p subtasksNames
     * This method sets the Task#_progressMode to ProgressMode::Subtasks
     * @param subtasksNames Subtasks names
     */
    virtual void setSubtasks(const QStringList& subtasksNames) final;

    /**
     * Flag item with \p subtaskIndex as started
     * Assumes the number of tasks has been set prior with Task::setSubtasks()
     * This method only has an effect when Task#_progressMode is set to ProgressMode::Subtasks
     * @param subtaskIndex Index of the subtask
     * @param progressDescription Override the default progress description when set to a non-empty string
     */
    virtual void setSubtaskStarted(std::uint32_t subtaskIndex, const QString& progressDescription = QString()) final;

    /**
     * Flag item with \p subtaskName as started
     * Assumes the number of tasks has been set prior with Task::setSubtasks()
     * This method only has an effect when Task#_progressMode is set to ProgressMode::Subtasks
     * @param subtaskName Name of the subtask
     * @param progressDescription Override the default progress description when set to a non-empty string
     */
    virtual void setSubtaskStarted(const QString& subtaskName, const QString& progressDescription = QString()) final;

    /**
     * Flag item with \p subtaskIndex as finished, the progress percentage will be computed automatically
     * Assumes the number of tasks has been set prior with Task::setSubtasks()
     * This method only has an effect when Task#_progressMode is set to ProgressMode::Subtasks
     * @param subtaskIndex Index of the subtask
     * @param progressDescription Override the default progress description when set to a non-empty string
     */
    virtual void setSubtaskFinished(std::uint32_t subtaskIndex, const QString& progressDescription = QString()) final;

    /**
     * Flag item with \p subtaskName as finished, the progress percentage will be computed automatically
     * Assumes the number of tasks has been set prior with Task::setSubtasks()
     * If \p subtaskName is not found, the progress will not be updated (ensure that subtasks names are set with Task::setSubtasksNames() or Task::setSubtasksName())
     * This method only has an effect when Task#_progressMode is set to ProgressMode::Subtasks
     * @param subtaskName Name of the subtask
     * @param progressDescription Override the default progress description when set to a non-empty string
     */
    virtual void setSubtaskFinished(const QString& subtaskName, const QString& progressDescription = QString()) final;

    /**
     * Set subtask name to \p subtaskName for \p subtaskIndex
     * This method only has an effect when Task#_progressMode is set to ProgressMode::Subtasks
     * @param subtaskIndex Subtask index to set the description for
     * @param subtaskName Name of the subtask
     */
    virtual void setSubtaskName(std::uint32_t subtaskIndex, const QString& subtaskName) final;

    /**
     * Get subtask names
     * @return Subtasks names
     */
    virtual QStringList getSubtasksNames() const final;

    /**
     * Get subtask index for \p subtaskName
     * Returns -1 when \p subtaskName is not found or Task#_progressMode is set to ProgressMode::Manual
     * @param subtaskName Name of the subtask
     */
    virtual std::int32_t getSubtaskIndex(const QString& subtaskName) const final;

public: // Progress description

    /**
     * Get progress description
     * @return Progress description
     */
    virtual QString getProgressDescription() const final;

    /**
     * Set progress description to \p progressDescription
     * @param progressDescription Progress description
     * @param clearDelay Progress description will be cleared after this delay (do not clear if zero)
     */
    virtual void setProgressDescription(const QString& progressDescription, std::uint32_t clearDelay = 0) final;

public: // Progress text

    /** Gets the task progress text */
    virtual QString getProgressText() const final;

    /**
     * Set progress text formatter to \p progressTextFormatter
     * @param progressTextFormatter Progress formatter function
     */
    virtual void setProgressTextFormatter(const ProgressTextFormatter& progressTextFormatter) final;

private:

    /** Gets the standard task progress text */
    virtual QString getStandardProgressText() const final;

private:

    /** Updates the progress percentage depending on the task settings */
    void updateProgress();

    /** Updates the progress text depending on the task settings */
    void updateProgressText();

private: // For aggregate task

    /**
     * Registers \p childTask
     * @param childTask Pointer to child task to register
     */
    void registerChildTask(Task* childTask);

    /**
     * Unregisters \p childTask
     * @param childTask Pointer to child task to unregister
     */
    void unregisterChildTask(Task* childTask);

    /**
     * Update status of aggregate task
     * This method only has an effect when Task#_progressMode is set to ProgressMode::Aggregate
     */
    virtual void updateAggregateStatus() final;

private: // Private setters (these call private signals under the hood, an essential part to make cross thread task usage possible)

    void privateSetParentTask(Task* parentTask);
    void privateAddChildTask(Task* childTask);
    void privateRemoveChildTask(Task* childTask);
    void privateSetName(const QString& name);
    void privateSetDescription(const QString& description);
    void privateSetIcon(const QIcon& icon);
    void privateSetEnabled(bool enabled, bool recursive = false);
    void privateSetVisible(bool visible);
    void privateSetMayKill(bool mayKill, bool recursive = false);
    void privateReset(bool recursive = false);
    void privateSetStatus(const Status& status, bool recursive = false);
    void privateSetStatusDeferred(const Status& status, bool recursive = false, std::uint32_t delay = DEFERRED_TASK_STATUS_INTERVAL);
    void privateSetUndefined();
    void privateSetIdle();
    void privateSetRunning();
    void privateSetRunningIndeterminate();
    void privateSetFinished();
    void privateSetAboutToBeAborted();
    void privateSetAborting();
    void privateSetAborted();
    void privateKill(bool recursive = true);
    void privateSetProgressMode(const ProgressMode& progressMode);
    void privateSetGuiScopes(const GuiScopes& guiScopes);
    void privateAddGuiScope(const GuiScope& guiScope);
    void privateRemoveGuiScope(const GuiScope& guiScope);
    void privateResetProgress(bool recursive = false);
    void privateSetProgress(float progress, const QString& subtaskDescription = "");
    void privateSetSubtasks(std::uint32_t numberOfSubtasks);
    void privateSetSubtasks(const QStringList& subtasksNames);
    void privateSetSubtaskStarted(std::uint32_t subtaskIndex, const QString& progressDescription);
    void privateSetSubtaskStarted(const QString& subtaskName, const QString& progressDescription);
    void privateSetSubtaskFinished(std::uint32_t subtaskIndex, const QString& progressDescription);
    void privateSetSubtaskFinished(const QString& subtaskName, const QString& progressDescription);
    void privateSetSubtaskName(std::uint32_t subtaskIndex, const QString& subtaskName);
    void privateSetProgressDescription(const QString& progressDescription, std::uint32_t clearDelay = 0);
    void privateSetProgressText(const QString& progressText, std::uint32_t clearDelay = 0);
    void privateSetProgressTextFormatter(const ProgressTextFormatter& progressTextFormatter);

private:

    void privateEmitProgressChanged();
    void privateEmitProgressDescriptionChanged();
    void privateEmitProgressTextChanged();

signals:

    /**
     * Signals that the task name changed to \p name
     * @param name Modified name
     */
    void nameChanged(const QString& name);

    /**
     * Signals that the task description changed to \p description
     * @param description Modified description
     */
    void descriptionChanged(const QString& description);

    /**
     * Signals that the task icon changed to \p icon
     * @param icon Modified icon
     */
    void iconChanged(const QIcon& icon);

    /**
     * Signals that the task enabled state changed to \p enabled
     * @param enabled Whether the task is enabled
     */
    void enabledChanged(bool enabled);

    /**
     * Signals that the task visibility changed to \p visible
     * @param visible Whether the task is visible in the user interface
     */
    void visibileChanged(bool visible);

    /**
     * Signals that the task handler changed to \p handler
     * @param handler Task handler
     */
    void handlerChanged(AbstractTaskHandler* handler);

    /**
     * Signals that the task status changed from \p previousStatus to \p status
     * @param previousStatus Status before the modification
     * @param status Modified name
     */
    void statusChanged(const Status& previousStatus, const Status& status);

    /**
     * Signals that may kill changed to \p mayKill
     * @param mayKill Boolean determining whether the task may be killed or not
     */
    void mayKillChanged(bool mayKill);

    /**
     * Signals that killable changed to \p killable
     * @param killable Boolean determining whether the task is killable or not
     */
    void isKillableChanged(bool killable);

    /** Signals that the task status changed to undefined */
    void statusChangedToUndefined();

    /** Signals that the task became idle */
    void statusChangedToIdle();

    /** Signals that the task started running */
    void statusChangedToRunning();

    /** Signals that the task started running indeterminately */
    void statusChangedToRunningIndeterminate();

    /** Signals that the task finished */
    void statusChangedToFinished();

    /** Signals that the task is about to be aborted */
    void statusChangedToAboutToBeAborted();

    /** Signals that the task is aborting */
    void statusChangedToAborting();

    /** Signals that the task is aborted */
    void statusChangedToAborted();

    /** Requests the involved task algorithms to abort their work */
    void requestAbort();

    /**
     * Signals that the task progress mode changed to \p progressMode
     * @param progressMode Modified progress mode
     */
    void progressModeChanged(const ProgressMode& progressMode);

    /**
     * Signals that the task GUI scopes changed to \p guiScopes
     * @param guiScopes Modified GUI scopes
     */
    void guiScopesChanged(const GuiScopes& guiScopes);

    /**
     * Signals that the task progress changed to \p progress
     * @param progress Modified progress
     */
    void progressChanged(float progress);

    /**
     * Signals that subtasks with \p subtasksNames have been added
     * @param subtasksNames Added subtasks names
     */
    void subtasksAdded(const QStringList& subtasksNames);

    /**
     * Signals that subtasks changed to \p subtasks
     * @param subtasks Modified subtasks
     * @param subtasksNames Modified subtasks names
     */
    void subtasksChanged(const QBitArray& subtasks, const QStringList& subtasksNames);

    /**
     * Signals that subtask with \p name started
     * @param subTaskName Name of the subtask that started
     */
    void subtaskStarted(const QString& subTaskName);

    /**
     * Signals that subtask with \p name finished
     * @param subTaskName Name of the subtask that finished
     */
    void subtaskFinished(const QString& subTaskName);

    /**
     * Signals that the progress description changed to \p progressDescription
     * @param progressDescription Modified progress description
     */
    void progressDescriptionChanged(const QString& progressDescription);

    /**
     * Signals that the progress text changed to \p progressText
     * @param progressText Current progress text
     */
    void progressTextChanged(const QString& progressText);

    /**
     * Signals that the parent task changed from \p previousParentTask to \p currentParentTask
     * @param previousParentTask Pointer to previous parent task (maybe nullptr)
     * @param currentParentTask Pointer to previous parent task (maybe nullptr)
     */
    void parentTaskChanged(Task* previousParentTask, Task* currentParentTask);

    /**
     * Signals that \p childTask has been added
     * @param childTask Pointer to child task that was added
     */
    void childTaskAdded(Task* childTask);

    /**
     * Signals that \p childTask is about to be removed from the list of children
     * @param childTask Pointer to child task that is about to be removed from the list of children
     */
    void childTaskAboutToBeRemoved(Task* childTask);

    /**
     * Signals that \p childTask has been removed from the list of children
     * @param childTask Pointer to child task that has been removed from the list of children
     */
    void childTaskRemoved(Task* childTask);

    /**
     * The signals below are private signals and can/should only be called from within this Task class
     * These signals provide a way to make cross thread task usage possible
     */
    void privateSetParentTaskSignal(Task* parentTask, QPrivateSignal);
    void privateAddChildTaskSignal(Task* childTask, QPrivateSignal);
    void privateRemoveChildTaskSignal(Task* childTask, QPrivateSignal);
    void privateSetNameSignal(const QString& name, QPrivateSignal);
    void privateSetDescriptionSignal(const QString& description, QPrivateSignal);
    void privateSetIconSignal(const QIcon& icon, QPrivateSignal);
    void privateSetEnabledSignal(bool enabled, bool recursive, QPrivateSignal);
    void privateSetVisibleSignal(bool visible, QPrivateSignal);
    void privateSetMayKillSignal(bool mayKill, bool recursive, QPrivateSignal);
    void privateResetSignal(bool recursive, QPrivateSignal);
    void privateSetStatusSignal(const Status& status, bool recursive, QPrivateSignal);
    void privateSetStatusDeferredSignal(const Status& status, bool recursive, std::uint32_t delay, QPrivateSignal);
    void privateSetUndefinedSignal(QPrivateSignal);
    void privateSetIdleSignal(QPrivateSignal);
    void privateSetRunningSignal(QPrivateSignal);
    void privateSetRunningIndeterminateSignal(QPrivateSignal);
    void privateSetFinishedSignal(QPrivateSignal);
    void privateSetAboutToBeAbortedSignal(QPrivateSignal);
    void privateSetAbortingSignal(QPrivateSignal);
    void privateSetAbortedSignal(QPrivateSignal);
    void privateKillSignal(bool, QPrivateSignal);
    void privateSetProgressModeSignal(const ProgressMode& progressMode, QPrivateSignal);
    void privateSetGuiScopesSignal(const GuiScopes& guiScopes, QPrivateSignal);
    void privateAddGuiScopeSignal(const GuiScope& guiScope, QPrivateSignal);
    void privateRemoveGuiScopeSignal(const GuiScope& guiScope, QPrivateSignal);
    void privateResetProgressSignal(bool recursive, QPrivateSignal);
    void privateSetProgressSignal(float progress, const QString& subtaskDescription, QPrivateSignal);
    void privateSetSubtasksSignal(std::uint32_t numberOfSubtasks, QPrivateSignal);
    void privateSetSubtasksSignal(const QStringList& subtasksNames, QPrivateSignal);
    void privateSetSubtaskStartedSignal(std::uint32_t subtaskIndex, const QString& progressDescription, QPrivateSignal);
    void privateSetSubtaskStartedSignal(const QString& subtaskName, const QString& progressDescription, QPrivateSignal);
    void privateSetSubtaskFinishedSignal(std::uint32_t subtaskIndex, const QString& progressDescription, QPrivateSignal);
    void privateSetSubtaskFinishedSignal(const QString& subtaskName, const QString& progressDescription, QPrivateSignal);
    void privateSetSubtaskNameSignal(std::uint32_t subtaskIndex, const QString& subtaskName, QPrivateSignal);
    void privateSetProgressDescriptionSignal(const QString& progressDescription, std::uint32_t clearDelay, QPrivateSignal);
    void privateSetProgressTextFormatterSignal(const ProgressTextFormatter& progressTextFormatter, QPrivateSignal);

private:
    QString                 _name;                                          /** Task name */
    QString                 _description;                                   /** Task description */
    QIcon                   _icon;                                          /** Task icon */
    bool                    _enabled;                                       /** Whether the task is enabled, disabled tasks are not included in task aggregation */
    bool                    _visible;                                       /** Whether the task is visible in the user interface */
    Status                  _status;                                        /** Task status */
    Status                  _deferredStatus;                                /** Task status which is set after a delay */
    bool                    _deferredStatusRecursive;                       /** Whether to set the task status deferred recursively */
    QTimer                  _deferredStatusTimer;                           /** Deferred status setter timer */
    bool                    _mayKill;                                       /** Whether the task may be killed or not */
    AbstractTaskHandler*    _handler;                                       /** Task handler */
    ProgressMode            _progressMode;                                  /** The way progress is recorded */
    GuiScopes               _guiScopes;                                      /** The gui scope(s) in which the task will present itself to the user */
    float                   _progress;                                      /** Task progress */
    QBitArray               _subtasks;                                      /** Subtasks status */
    QStringList             _subtasksNames;                                 /** Subtasks names */
    QString                 _progressDescription;                           /** Current item description */
    Task*                   _parentTask;                                    /** Pointer to the parent task */
    TasksPtrs               _childTasks;                                    /** Pointers to child tasks */
    QString                 _progressText;                                  /** Progress text */
    ProgressTextFormatter   _progressTextFormatter;                         /** Progress text formatter function (overrides Task::getProgressText() when set) */

private:
    static constexpr std::uint32_t TASK_UPDATE_TIMER_INTERVAL           = 250;      /** Single shot task progress and description timer interval */
    static constexpr std::uint32_t TASK_DESCRIPTION_DISAPPEAR_INTERVAL  = 1500;     /** Single shot task description disappear timer interval */
    static constexpr std::uint32_t DEFERRED_TASK_STATUS_INTERVAL        = 1500;     /** Delay after which the deferred task status is set */
};

}
