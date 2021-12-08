#include "TextData.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "hdps.TextData")

TextData::~TextData(void)
{
}

void TextData::init()
{
}

Dataset<DatasetImpl> TextData::createDataSet() const
{
    return Dataset<DatasetImpl>(new Text(_core, getName()));
}

QIcon TextDataFactory::getIcon() const
{
    return Application::getIconFont("FontAwesome").getIcon("font");
}

hdps::plugin::RawData* TextDataFactory::produce()
{
    return new TextData(this);
}

QIcon Text::getIcon() const
{
    return Application::getIconFont("FontAwesome").getIcon("font");
}
