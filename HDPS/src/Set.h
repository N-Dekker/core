#pragma once

#include <CoreInterface.h>

#include <QString>
#include <QVector>

namespace hdps
{

class Set
{
public:
    Set(CoreInterface* core, QString dataName) : _core(core), _dataName(dataName), _all(false) {}
    virtual ~Set() {}

    virtual Set* copy() const = 0;

    QString getName() const
    {
        return _name;
    }

    QString getDataName() const
    {
        return _dataName;
    }

    bool isFull() const
    {
        return _all;
    }

    void setName(QString name)
    {
        _name = name;
    }

    void setAll()
    {
        _all = true;
    }

protected:
    CoreInterface* _core;
private:
    QString _name;
    QString _dataName;
    bool _all;
};

} // namespace hdps
