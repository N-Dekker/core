// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QWidget>

#include <QObject>
#include <QDebug>

class QWebEngineView;
class QWebEnginePage;
class QWebChannel;

namespace mv
{
namespace gui
{

class WebCommunicationObject : public QObject
{
    Q_OBJECT

public:


signals:
    void notifyJsBridgeIsAvailable();

public slots:
    void js_debug(QString message)
    {
        qDebug() << "Javascript Debug Info: " << message;
    }

    void js_available()
    {
        emit notifyJsBridgeIsAvailable();
    }
};

class WebWidget : public QWidget
{
    Q_OBJECT
public:
    WebWidget();
    ~WebWidget() override;

    void init(WebCommunicationObject* communicationObject);

    QWebEngineView* getView();
    QWebEnginePage* getPage();
    void setPage(QString htmlPath, QString basePath);

protected:
    void registerFunctions();

public slots:
    void js_debug(QString text);

protected slots:
    virtual void initWebPage() = 0;

private:
    QWebEngineView* _webView;
    QWebChannel* _communicationChannel;

    WebCommunicationObject* _js;

    QString _css;
};

} // namespace gui

} // namespace mv
