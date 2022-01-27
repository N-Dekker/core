#pragma once

#include "WidgetAction.h"
#include "StringAction.h"
#include "TriggerAction.h"

#include <QLineEdit>
#include <QString>
#include <QDirModel>
#include <QCompleter>

class QWidget;

namespace hdps {

namespace gui {

/**
 * Directory picker action class
 *
 * Action class for picking a directory
 *
 * @author Thomas Kroes
 */
class DirectoryPickerAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget configurations */
    enum WidgetFlag {
        LineEdit    = 0x00001,      /** Widget includes a line edit */
        PushButton  = 0x00002,      /** There is a button to pick a directory */

        Default = LineEdit | PushButton
    };

protected:

    /** Widget class for directory picker action */
    class Widget : public QLineEdit
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param directoryPickerAction Pointer to directory picker action
         */
        Widget(QWidget* parent, DirectoryPickerAction* directoryPickerAction);

        friend class DirectoryPickerAction;
    };

    /**
     * Get widget representation of the directory picker action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param directory Directory
     * @param defaultDirectory Default directory
     */
    DirectoryPickerAction(QObject* parent, const QString& title = "", const QString& directory = QString(), const QString& defaultDirectory = QString());

    /**
     * Initialize the directory picker action
     * @param directory Directory
     * @param defaultDirectory Default directory
     */
    void initialize(const QString& directory = QString(), const QString& defaultDirectory = QString());

    /**
     * Get the current directory
     * @return Current directory
     */
    QString getDirectory() const;

    /**
     * Set the current directory
     * @param directory Current directory
     */
    void setDirectory(const QString& directory);

    /**
     * Get the default directory
     * @return Default directory
     */
    QString getDefaultDirectory() const;

    /**
     * Set the default directory
     * @param defaultDirectory Default directory
     */
    void setDefaultDirectory(const QString& defaultDirectory);

    /** Determines whether the current directory can be reset to its default directory */
    bool isResettable() const override;

    /** Reset the current directory to the default directory */
    void reset() override;

    /**
     * Get placeholder text
     * @return Placeholder string
     */
    QString getPlaceholderString() const;

    /**
     * Set placeholder text
     * @param placeholderText Placeholder text
     */
    void setPlaceHolderString(const QString& placeholderString);

    /**
     * Get directory name
     * @return Directory name
     */
    QString getDirectoryName() const;

    /**
     * Get whether the directory is valid or not
     * @return Boolean indication whether the directory is valid or not
     */
    bool isValid() const;

signals:

    /**
     * Signals that the directory changed
     * @param directory Directory that changed
     */
    void directoryChanged(const QString& directory);

    /**
     * Signals that the default directory changed
     * @param defaultString Default directory that changed
     */
    void defaultDirectoryChanged(const QString& defaultDirectory);

    /**
     * Signals that the placeholder string changed
     * @param placeholderString Placeholder string that changed
     */
    void placeholderStringChanged(const QString& placeholderString);

protected:
    QDirModel       _dirModel;              /** Directory model */
    QCompleter      _completer;             /** Completer */
    StringAction    _directoryAction;       /** Directory action */
    TriggerAction   _pickAction;            /** Pick directory action */
};

}
}
