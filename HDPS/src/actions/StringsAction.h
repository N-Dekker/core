#pragma once

#include "WidgetAction.h"

#include "widgets/HierarchyWidget.h"

#include <QStringListModel>
#include <QSortFilterProxyModel>

namespace hdps::gui {

/**
 * Strings widget action class
 *
 * Stores a string list and creates widgets to interact with it (add/remove/rename)
 *
 * @author Thomas Kroes
 */
class StringsAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget configurations */
    enum WidgetFlag {
        MayEdit             = 0x00001,              /** Strings may be added and removed from the user interface */
        ListView            = 0x00002,              /** Widget includes a list view */
        EditableListView    = ListView | MayEdit,   /** Widget includes a list view with which strings may be added and removed from the user interface */

        Default = EditableListView
    };

public:

    /** Widget class for strings action */
    class ListWidget : public WidgetActionWidget
    {
    protected:

        /** Qt native string list model does not support icons, this class solves that for the strings action */
        class IconStringListModel final : public QStringListModel {
        public:

            /**
             * Construct string list model from \p icon and \p parent object
             * @param icon Global model icon
             * @param parent Pointer to parent object
             */
            explicit IconStringListModel(const QIcon& icon, QObject* parent = nullptr) :
                QStringListModel(parent),
                _icon(icon)
            {
            }

            /**
             * Override string list model to also support data decoration role
             * @param index Index to fetch the data for
             * @param role Data role
             * @return Data in variant form
             */
            QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
                switch (role) {
                    case Qt::DisplayRole:
                    case Qt::EditRole:
                        return QStringListModel::data(index, role);

                    case Qt::DecorationRole:
                        return _icon;
                }

                return QVariant();
            }

        private:
            const QIcon   _icon;  /** Global icon */
        };

    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param stringsAction Pointer to strings action
         * @param widgetFlags Widget flags for the configuration of the widget (type)
         */
        ListWidget(QWidget* parent, StringsAction* stringsAction, const std::int32_t& widgetFlags);

    private:
        IconStringListModel     _model;             /** Strings model */
        QSortFilterProxyModel   _filterModel;       /** Strings filter model */
        HierarchyWidget         _hierarchyWidget;   /** Hierarchy widget for show the strings */
        StringAction            _nameAction;        /** String name action */
        TriggerAction           _addAction;         /** Add string action */
        TriggerAction           _removeAction;      /** Remove string action */

        friend class StringsAction;
    };

protected:

    /**
     * Get widget representation of the strings action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param strings Strings
     * @param defaultStrings Default strings
     */
    StringsAction(QObject* parent, const QString& title = "", const QStringList& strings = QStringList(), const QStringList& defaultStrings = QStringList());

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;

    /**
     * Initialize the strings action
     * @param strings Strings
     * @param defaultStrings Default strings
     */
    void initialize(const QStringList& strings = QStringList(), const QStringList& defaultStrings = QStringList());

    /**
     * Get string category
     * @return String category
     */
    QString getCategory() const;

    /**
     * Set string category
     * @param category Category
     */
    void setCategory(const QString& category);

    /**
     * Get strings
     * @return Strings as string list
     */
    QStringList getStrings() const;

    /**
     * Set strings
     * @param strings Strings
     */
    void setStrings(const QStringList& strings);

    /**
     * Get default strings
     * @return Default strings as string list
     */
    QStringList getDefaultStrings() const;

    /**
     * Set default strings
     * @param defaultStrings Default strings
     */
    void setDefaultStrings(const QStringList& defaultStrings);

    /**
     * Add string
     * @param string String to add
     */
    void addString(const QString& string);

    /**
     * Remove string
     * @param string String to remove
     */
    void removeString(const QString& string);

    /**
     * Remove strings
     * @param strings Strings to remove
     */
    void removeStrings(const QStringList& strings);

public: // Settings

    /**
     * Determines whether the action can be reset to its default
     * @param recursive Check recursively
     * @return Whether the action can be reset to its default
     */
    bool isResettable() override final;

    /**
     * Reset to factory default
     * @param recursive Reset to factory default recursively
     */
    void reset() override final;

public: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     */
    void connectToPublicAction(WidgetAction* publicAction) override;

    /** Disconnect this action from a public action */
    void disconnectFromPublicAction() override;

protected:  // Linking

    /**
     * Get public copy of the action (other compatible actions can connect to it)
     * @return Pointer to public copy of the action
     */
    WidgetAction* getPublicCopy() const override;

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

signals:

    /**
     * Signals that the strings changed
     * @param strings Updated strings
     */
    void stringsChanged(const QStringList& strings);

    /**
     * Signals that the default strings changed
     * @param defaultStrings Updated default strings
     */
    void defaultStringsChanged(const QStringList& defaultString);

protected:
    QString         _category;          /** Type of string */
    QStringList     _strings;           /** Current strings */
    QStringList     _defaultStrings;    /** Default strings */
};

}
