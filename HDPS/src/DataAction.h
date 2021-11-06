#pragma once

#include "DataAnalysisAction.h"
#include "DataExportAction.h"
#include "DataRemoveAction.h"

using namespace hdps::gui;
using namespace hdps::util;

namespace hdps {

/**
 * Data action class
 *
 * Data action class
 *
 * @author Thomas Kroes
 */
class DataAction : public hdps::gui::WidgetAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param datasetName Name of the dataset
     */
    DataAction(QObject* parent, const QString& datasetName);

protected:
    DatasetRef<DataSet>     _dataset;                /** Dataset reference */
    DataAnalysisAction      _dataAnalysisAction;    /** Data analysis action */
    DataExportAction        _dataExportAction;      /** Data export action */
    DataRemoveAction        _dataRemoveAction;      /** Data remove action */
};

}
