#pragma once

#include "Dataset.h"

#include "GroupAction.h"
#include "ToggleAction.h"

namespace hdps {
    class DatasetImpl;
}

using namespace hdps;
using namespace hdps::gui;

/**
 * Linked data action class
 *
 * Action class for configuring linked data
 *
 * @author Thomas Kroes
 */
class LinkedDataAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param points Smart pointer to points dataset
     */
    LinkedDataAction(QObject* parent, Dataset<DatasetImpl> dataset);

protected:
    Dataset<DatasetImpl>    _dataset;               /** Smart pointer to dataset */
    ToggleAction            _mayReceiveAction;      /** May receive linked data action */
    ToggleAction            _maySendAction;         /** May send linked data action */
};
