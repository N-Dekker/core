// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "AnalysisPlugin.h"
#include "Application.h"

namespace hdps
{

namespace plugin
{

AnalysisPlugin::AnalysisPlugin(const PluginFactory* factory) :
    Plugin(factory),
    _input(),
    _output()
{

}

void AnalysisPlugin::setObjectName(const QString& name)
{
    QObject::setObjectName("Plugins/Analysis/" + name);
}

void AnalysisPlugin::setInputDataset(Dataset<DatasetImpl> inputDataset)
{
    _input = inputDataset;
}

void AnalysisPlugin::setOutputDataset(Dataset<DatasetImpl> outputDataset)
{
    _output = outputDataset;
}

hdps::DataHierarchyItem::TaskStatus AnalysisPlugin::getTaskStatus() const
{
    if (!_output.isValid())
        return DataHierarchyItem::TaskStatus::Undefined;

    return _output->getDataHierarchyItem().getTaskStatus();
}

void AnalysisPlugin::setTaskName(const QString& taskName)
{
    if (!_output.isValid())
        return;

    _output->getDataHierarchyItem().setTaskName(taskName);
}

void AnalysisPlugin::setTaskProgress(const float& taskProgress)
{
    if (!_output.isValid())
        return;

    _output->getDataHierarchyItem().setTaskProgress(taskProgress);
}

void AnalysisPlugin::setTaskDescription(const QString& taskDescription)
{
    if (!_output.isValid())
        return;

    _output->getDataHierarchyItem().setTaskDescription(taskDescription);
}

void AnalysisPlugin::setTaskRunning()
{
    if (!_output.isValid())
        return;

    _output->getDataHierarchyItem().setTaskRunning();
}

void AnalysisPlugin::setTaskFinished()
{
    if (!_output.isValid())
        return;

    _output->getDataHierarchyItem().setTaskFinished();
}

void AnalysisPlugin::setTaskAborted()
{
    if (!_output.isValid())
        return;

    _output->getDataHierarchyItem().setTaskAborted();
}

QIcon AnalysisPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("square-root-alt", color);
}

}

}
