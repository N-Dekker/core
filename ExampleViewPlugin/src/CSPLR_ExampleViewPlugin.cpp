#include "CSPLR_ExampleViewPlugin.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.CSPLR_ExampleViewPlugin")

// =============================================================================
// View
// =============================================================================

CSPLR_ExampleViewPlugin::~CSPLR_ExampleViewPlugin(void)
{
    
}

void CSPLR_ExampleViewPlugin::init()
{
}

void CSPLR_ExampleViewPlugin::dataAdded(void)
{
}

void CSPLR_ExampleViewPlugin::dataRefreshed(void)
{
    qDebug() << "YEAH!!!" << this;
}


// =============================================================================
// Factory
// =============================================================================

ViewPlugin* CSPLR_ExampleViewPluginFactory::produce()
{
    return new CSPLR_ExampleViewPlugin();
}
