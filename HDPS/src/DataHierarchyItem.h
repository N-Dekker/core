// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "DataType.h"
#include "actions/WidgetAction.h"
#include "event/Event.h"

#include <QObject>
#include <QMap>
#include <QString>
#include <QDebug>
#include <QIcon>
#include <QTimer>
#include <QBitArray>
#include <QVector>

namespace mv
{

class DatasetImpl;

/** Vector of data hierarchy item pointers */
using DataHierarchyItems = QVector<DataHierarchyItem*>;

/**
 * Data hierarchy item class
 *
 * Represents a dataset as an item in a dataset hierarchy
 *
 * @author Thomas Kroes
 */
class DataHierarchyItem final : public mv::gui::WidgetAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param dataset Smart pointer to dataset
     * @param parentDataset Smart pointer to parent dataset (if any)
     * @param visible Whether the dataset is visible
     * @param selected Whether the dataset is selected
     */
    DataHierarchyItem(QObject* parent, Dataset<DatasetImpl> dataset, Dataset<DatasetImpl> parentDataset, const bool& visible = true, const bool& selected = false);

    /** Destructor */
    ~DataHierarchyItem() = default;

    /** Get reference to parent hierarchy item */
    DataHierarchyItem& getParent() const;

    /**
     * Walks up the hierarchy of the data hierarchy item and returns all parents
     * @param dataHierarchyItem Item to fetch the parents for
     * @param parents Item parents
     */
    static void getParents(DataHierarchyItem& dataHierarchyItem, DataHierarchyItems& parents);

    /** Returns whether the data hierarchy item has a parent */
    bool hasParent() const;

    /**
     * Gets the names of the children name
     * @param recursive Recursive
     */
    DataHierarchyItems getChildren(const bool& recursive = false) const;

    /** Gets the number of children */
    std::uint32_t getNumberOfChildren() const;

    /** Establishes whether the item has any children */
    bool hasChildren() const;

    /**
     * Set visibility
     * @param visible Whether the data hierarchy item is visible or not
     */
    void setVisible(bool visible);

public: // Selection

    /** Gets whether the hierarchy item is selected */
    bool isSelected() const;

    /**
     * Sets the item selection status
     * @param selected Whether the hierarchy item is selected
     */
    void setSelected(const bool& selected);

    /** Selects the hierarchy item */
    void select();

    /** De-selects the hierarchy item */
    void deselect();

protected:

    /** Set reference to parent hierarchy item */
    void setParent(DataHierarchyItem& parent);

public: // Hierarchy

    /**
     * Add a child
     * @param child Reference to child data hierarchy item
     */
    void addChild(DataHierarchyItem& child);

public: // Miscellaneous

    /** Get the dataset */
    Dataset<DatasetImpl> getDataset();

    /** Get the dataset */
    template<typename DatasetType>
    Dataset<DatasetType> getDataset() const {
        return Dataset<DatasetType>(const_cast<DataHierarchyItem*>(this)->getDataset().get<DatasetType>());
    };

    /**
     * Get reference to dataset smart pointer
     * @return Reference to dataset smart pointer
     */
    Dataset<DatasetImpl>& getDatasetReference();

    /**
     * Get reference to dataset smart pointer of a specific type
     * @return Reference to dataset smart pointer of a specific type
     */
    template<typename DatasetType>
    Dataset<DatasetType>& getDatasetReference() const {
        return Dataset<DatasetType>(const_cast<DataHierarchyItem*>(this)->getDataset().get<DatasetType>());
    };

    /** Get the dataset type */
    DataType getDataType() const;

public: // Actions

    /** Add action */
    void addAction(mv::gui::WidgetAction& widgetAction);

    /** Returns list of shared action widgets*/
    mv::gui::WidgetActions getActions() const;

    /**
     * Get the context menu
     * @param parent Parent widget
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent = nullptr) override;

    /**
     * Populates existing menu with actions menus
     * @param contextMenu Context menu to populate
     */
    void populateContextMenu(QMenu* contextMenu);;

public: // Locked

    /** Get locked status */
    bool getLocked() const;

    /**
     * Set locked status
     * @param locked Whether the dataset is locked
     */
    void setLocked(const bool& locked);

public: // Expanded

    /**
     * Get expanded status
     * @return Boolean indicating whether the item is expanded or not
     */
    bool isExpanded() const;

    /**
     * Set expanded status
     * @param expanded Whether the dataset is expanded
     */
    void setExpanded(bool expanded);

public: // Named icons

    /** Get icon */
    QIcon getIcon() const;

    /**
     * Set icon
     * @param icon Icon
     */
    void setIcon(const QIcon& icon);

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
     * Signals that the item got selected
     * @param selected Whether the item is selected
     */
    void selectionChanged(const bool& selected);

    /**
     * Signals that a widget action was added
     * @param widgetAction Widget action that was added
     */
    void actionAdded(mv::gui::WidgetAction& widgetAction);

    /** Signals that the set icon has changed */
    void iconChanged();

    /**
     * Signals that the dataset name changed
     * @param datasetName New name of the dataset
     */
    void datasetNameChanged(const QString& datasetName);

    /**
     * Signals that the locked status changed
     * @param locked Locked
     */
    void lockedChanged(bool locked);

    /**
     * Signals that the visibility status changed
     * @param visibility Visibility
     */
    void visibilityChanged(bool visibility);

    /**
     * Signals that the expansion status changed
     * @param expanded Whether the item is expanded or not
     */
    void expandedChanged(bool expanded);

    /** Signals that the data hierarchy item is being loaded */
    void loading();

    /** Signals that the data hierarchy item has been loaded */
    void loaded();

    /** Signals that the data hierarchy item is being saved */
    void saving();

    /** Signals that the data hierarchy item has been saved */
    void saved();

protected:
    Dataset<DatasetImpl>        _dataset;       /** Smart pointer to dataset */
    DataHierarchyItem*          _parent;        /** Pointer to parent data hierarchy item */
    DataHierarchyItems          _children;      /** Pointers to child items (if any) */
    bool                        _selected;      /** Whether the hierarchy item is selected */
    bool                        _expanded;      /** Whether the item is expanded or not (when it has children) */
    QIcon                       _icon;          /** Icon */
    mv::gui::WidgetActions    _actions;       /** Widget actions */

protected:
    friend class DataHierarchyManager;
    friend class DataManager;

    /** Single shot task update timer interval */
    static constexpr std::uint32_t TASK_UPDATE_TIMER_INTERVAL = 100;

    /** Single shot message disappear timer interval */
    static constexpr std::uint32_t MESSAGE_DISAPPEAR_INTERVAL = 1500;
};

}
