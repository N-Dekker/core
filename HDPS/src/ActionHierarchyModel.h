#pragma once

#include "actions/WidgetAction.h"

#include <QAbstractItemModel>
#include <QMimeData>

namespace hdps
{

class ActionHierarchyModelItem;

/**
 * Action hierarchy model class
 *
 * Class for action hierarchy model
 *
 * @author Thomas Kroes
 */
class ActionHierarchyModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param rootAction Pointer to non-owning root action
     */
    explicit ActionHierarchyModel(QObject* parent, gui::WidgetAction* rootAction);

    /** Destructor */
    ~ActionHierarchyModel();

    /**
     * Mandatory override for QAbstractItemModel.
     */
    QVariant data(const QModelIndex& index, int role) const override;

    /**
     * Sets the data value for the given model index and data role
     * @param index Model index
     * @param value Data value in variant form
     * @param role Data role
     * @return Whether the data was properly set or not
     */
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;

    /**
     * Mandatory override for QAbstractItemModel. Provides an index associated
     * to a particular data item at location (row, column).
     */
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Mandatory override for QAbstractItemModel. Returns the index of the parent
     * of this item. If this item is not a child, an invalid index is returned.
     */
    QModelIndex parent(const QModelIndex& index) const override;

    /**
     * Mandatory override for QAbstractItemModel.
     * Returns the number of children of this parent item.
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    /**
     * Mandatory override for QAbstractItemModel.
     * Returns number of columns associated with this parent item.
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

   /**
    * Get action hierarchy model item
    * @param index Model index of the item
    * @param role Data role
    */
    ActionHierarchyModelItem* getItem(const QModelIndex& index, int role) const;

    /**
     * Get item flags
     * @param index Model index
     * @return Item flags
     */
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    /**
     * Get header data
     * @param section Section
     * @param orientation Orientation
     * @param role Data role
     * @return Header
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:

    /**
     * Fetches model indices of \p modelIndex and its children recursively
     * @param modelIndex Filter model index
     * @param childrenOnly Whether to only include children
     * @return Fetched model indices
     */
    QModelIndexList fetchModelIndices(QModelIndex modelIndex = QModelIndex(), bool childrenOnly = false) const;

private:
    ActionHierarchyModelItem*     _rootItem;      /** Root node of the action hierarchy */
};

}
