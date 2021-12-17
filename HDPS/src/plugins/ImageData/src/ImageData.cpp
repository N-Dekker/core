#include "ImageData.h"
#include "Images.h"
#include "Application.h"

#include "PointData.h"
#include "DataHierarchyItem.h"

#include "util/Exception.h"

#include <QDebug>

#include <stdexcept>

Q_PLUGIN_METADATA(IID "nl.BioVault.ImageData")

using namespace hdps;
using namespace hdps::util;

ImageData::ImageData(const hdps::plugin::PluginFactory* factory) :
    hdps::plugin::RawData(factory, ImageType),
    _type(Type::Undefined),
    _numberOfImages(0),
    _imageOffset(),
    _imageSize(),
    _numberOfComponentsPerPixel(0),
    _imageFilePaths(),
    _dimensionNames()
{
}

void ImageData::init()
{
}

ImageData::Type ImageData::getType() const
{
    return _type;
}

void ImageData::setType(const ImageData::Type& type)
{
    _type = type;
}

QSize ImageData::getImageSize() const
{
    return _imageSize;
}

void ImageData::setImageSize(const QSize& imageSize)
{
    _imageSize = imageSize;
}

std::uint32_t ImageData::getNumberOfComponentsPerPixel() const
{
    return _numberOfComponentsPerPixel;
}

void ImageData::setNumberOfComponentsPerPixel(const std::uint32_t& numberOfComponentsPerPixel)
{
    _numberOfComponentsPerPixel = numberOfComponentsPerPixel;
}

QStringList ImageData::getImageFilePaths() const
{
    return _imageFilePaths;
}

void ImageData::setImageFilePaths(const QStringList& imageFilePaths)
{
    _imageFilePaths = imageFilePaths;
}

std::uint32_t ImageData::getNumberOfImages() const
{
    return _numberOfImages;
}

void ImageData::setNumberImages(const std::uint32_t& numberOfImages)
{
    _numberOfImages = numberOfImages;
}

Dataset<DatasetImpl> ImageData::createDataSet() const
{
    return Dataset<DatasetImpl>(new Images(_core, getName()));
}

QIcon ImageDataFactory::getIcon() const
{
    return hdps::Application::getIconFont("FontAwesome").getIcon("images");
}

hdps::plugin::RawData* ImageDataFactory::produce()
{
    return new ImageData(this);
}
