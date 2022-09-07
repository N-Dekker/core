#include "Serializable.h"
#include "Application.h"

#include "actions/WidgetAction.h"
#include "util/Exception.h"

#include <QDebug>
#include <QFileDialog>
#include <QJsonArray>

#ifdef _DEBUG
    #define SERIALIZABLE_VERBOSE
#endif

using namespace hdps::util;

namespace hdps {

Serializable::Serializable(const QString& name /*= ""*/) :
    _name(name)
{
}

QString Serializable::getSerializationName() const
{
    if (_name.isEmpty()) {
        auto widgetAction = dynamic_cast<const WidgetAction*>(this);

        if (widgetAction)
            return !widgetAction->objectName().isEmpty() ? widgetAction->objectName() : widgetAction->text();
        else
            return "unnamed";
    }
    
    return _name;
}

void Serializable::fromVariantMap(const QVariantMap& variantMap)
{
}

QVariantMap Serializable::toVariantMap() const
{
    return QVariantMap();
}

void Serializable::fromJsonDocument(const QJsonDocument& jsonDocument)
{
    const auto variantMap = jsonDocument.toVariant().toMap();

    fromVariantMap(const_cast<Serializable*>(this), variantMap[getSerializationName()].toMap());
}

QJsonDocument Serializable::toJsonDocument() const
{
    QVariantMap variantMap;

    variantMap[getSerializationName()] = const_cast<Serializable*>(this)->toVariantMap(this);

    return QJsonDocument::fromVariant(variantMap);
}

void Serializable::fromJsonFile(const QString& filePath /*= ""*/)
{
    if (Application::isSerializationAborted())
        return;

    try
    {
        if (!QFileInfo(filePath).exists())
            throw std::runtime_error("File does not exist");

        QFile jsonFile(filePath);

        if (!jsonFile.open(QIODevice::ReadOnly))
            throw std::runtime_error("Unable to open file for reading");

        QByteArray data = jsonFile.readAll();

        QJsonDocument jsonDocument;

        jsonDocument = QJsonDocument::fromJson(data);

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

        fromJsonDocument(jsonDocument);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to load data from JSON file", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to load data from JSON file");
    }
}

void Serializable::toJsonFile(const QString& filePath /*= ""*/)
{
    if (Application::isSerializationAborted())
        return;

    try
    {
        QFile jsonFile(filePath);

        if (!jsonFile.open(QFile::WriteOnly))
            throw std::runtime_error("Unable to open file for writing");

        auto jsonDocument = toJsonDocument();

        if (jsonDocument.isNull() || jsonDocument.isEmpty())
            throw std::runtime_error("JSON document is invalid");

#ifdef SERIALIZABLE_VERBOSE
        qDebug().noquote() << jsonDocument.toJson(QJsonDocument::Indented);
#endif

        jsonFile.write(jsonDocument.toJson());
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to save data to JSON file", e);
    }
    catch (...) {
        exceptionMessageBox("Unable to save data to JSON file");
    }
}

void Serializable::fromVariantMap(Serializable* serializable, const QVariantMap& variantMap)
{
#ifdef SERIALIZABLE_VERBOSE
    qDebug().noquote() << QString("From variant map: %1").arg(serializable->getSerializationName());
#endif

    serializable->fromVariantMap(variantMap);

    auto object = dynamic_cast<const QObject*>(serializable);

    if (object == nullptr)
        return;

    for (auto child : object->children()) {
        auto childSerializable = dynamic_cast<Serializable*>(child);

        if (!childSerializable)
            continue;

        fromVariantMap(childSerializable, variantMap[childSerializable->getSerializationName()].toMap());
    }
}

QVariantMap Serializable::toVariantMap(const Serializable* serializable)
{
#ifdef SERIALIZABLE_VERBOSE
    qDebug().noquote() << QString("To variant map: %1").arg(serializable->getSerializationName());
#endif

    return serializable->toVariantMap();
}

}
