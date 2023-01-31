#pragma once

#include <actions/OptionsAction.h>

#include <QSortFilterProxyModel>

/**
 * Logging filter model
 *
 * Provides a sorting and filtering model for the logging model
 *
 * @author Thomas Kroes
 */
class LoggingFilterModel : public QSortFilterProxyModel
{
public:

    /** Constructor
     * @param parent Pointer to parent object
    */
    LoggingFilterModel(QObject* parent = nullptr);

    /**
     * Returns whether a give row with give parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     * @return Boolean indicating whether the item is filtered in or out
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

    /**
     * Compares two cluster items
     * @param lhs Left cluster
     * @param rhs Right cluster
     */
    bool lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const override;

public:
    hdps::gui::OptionsAction& getFilterTypeAction() { return _filterTypeAction; }

private:
    hdps::gui::OptionsAction    _filterTypeAction;          /** Options action for filtering log record item type */
};