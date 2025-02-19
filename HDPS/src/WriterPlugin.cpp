// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WriterPlugin.h"
#include "Application.h"

namespace mv
{
namespace plugin
{

WriterPlugin::WriterPlugin(const PluginFactory* factory) :
    Plugin(factory),
    _input()
{

}

void WriterPlugin::setInputDataset(Dataset<DatasetImpl> inputDataset)
{
    _input = inputDataset;
}

WriterPluginFactory::WriterPluginFactory() :
    PluginFactory(Type::WRITER)
{

}

QIcon WriterPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("file-export", color);
}

}

}
