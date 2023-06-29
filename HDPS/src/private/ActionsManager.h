#pragma once

#include "AbstractActionsManager.h"

#include <models/ActionsListModel.h>

namespace hdps
{

/**
 * Actions manager
 *
 * Extends the abstract actions manager class and adds functionality for serialization and action publishing
 *
 * @author Thomas Kroes
 */
class ActionsManager final : public AbstractActionsManager
{
    Q_OBJECT

public:

    /** Default constructor */
    ActionsManager();

    /** Default destructor */
    ~ActionsManager() override;

    /** Perform manager startup initialization */
    void initialize() override;

    /** Resets the contents of the actions manager */
    void reset() override;

    /**
     * Publish \p privateAction so that other private actions can connect to it
     * @param privateAction Pointer to private action to publish
     * @param name Name of the published widget action (if empty, a name choosing dialog will popup)
     * @param recursive Whether to also publish the child actions recursively
     */
    void publishPrivateAction(gui::WidgetAction* privateAction, const QString& name = "", bool recursive = true) override;

public: // Serialization

    /**
     * Load manager from variant map
     * @param Variant map representation of the manager
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save manager to variant map
     * @return Variant map representation of the manager
     */
    QVariantMap toVariantMap() const override;
};

}
