// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "LoggingWidget.h"

#include <Application.h>

#include <util/FileUtil.h>

#include <QDebug>
#include <QHeaderView>
#include <QMenu>
#include <QGuiApplication>
#include <QClipboard>
#include <QAbstractEventDispatcher>
#include <QMessageBox>

using namespace mv;
using namespace mv::util;

LoggingWidget::LoggingWidget(QWidget* parent) :
    QWidget(parent),
    _model(),
    _filterModel(nullptr),
    _hierarchyWidget(this, "Log record", _model, &_filterModel),
    _idleUpdateConnection(),
    _findLogFileAction(this, "Find log file")
{
    setAutoFillBackground(true);

    auto layout = new QVBoxLayout();

    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(&_hierarchyWidget, 1);

    setLayout(layout);

    _hierarchyWidget.setWindowIcon(Application::getIconFont("FontAwesome").getIcon("scroll"));

    _hierarchyWidget.getCollapseAllAction().setVisible(false);
    _hierarchyWidget.getExpandAllAction().setVisible(false);
    
    auto& settingsGroupAction = _hierarchyWidget.getSettingsGroupAction();

    settingsGroupAction.setVisible(true);

    settingsGroupAction.addAction(&_model.getWordWrapAction());
    settingsGroupAction.addAction(&_findLogFileAction);

    auto& filterGroupAction = _hierarchyWidget.getFilterGroupAction();

    filterGroupAction.setLabelWidthFixed(60);
    filterGroupAction.setPopupSizeHint(QSize(340, 10));

    filterGroupAction.addAction(&_filterModel.getFilterTypeAction());
    
    _findLogFileAction.setToolTip("Find the location where the log file resides");

    auto& treeView = _hierarchyWidget.getTreeView();

    _idleUpdateConnection = connect(QAbstractEventDispatcher::instance(), &QAbstractEventDispatcher::awake, &_model, &LoggingModel::synchronizeLogRecords);

    treeView.setSortingEnabled(true);
    treeView.setRootIsDecorated(false);
    treeView.setColumnHidden(static_cast<int>(LoggingModel::Column::Category), true);
    treeView.setColumnHidden(static_cast<int>(LoggingModel::Column::FileAndLine), true);
    treeView.setColumnHidden(static_cast<int>(LoggingModel::Column::Function), true);

    auto treeViewHeader = treeView.header();

    treeViewHeader->resizeSection(static_cast<int>(LoggingModel::Column::Number), 50);
    treeViewHeader->resizeSection(static_cast<int>(LoggingModel::Column::Type), 60);

    treeViewHeader->setSectionResizeMode(static_cast<int>(LoggingModel::Column::Number), QHeaderView::Fixed);
    treeViewHeader->setSectionResizeMode(static_cast<int>(LoggingModel::Column::Type), QHeaderView::Fixed);

    connect(&treeView, &QTreeView::customContextMenuRequested, [this, &treeView](const QPoint& point)
    {
        const auto selectedRows = treeView.selectionModel()->selectedRows();

        if (selectedRows.isEmpty())
            return;

        QMenu contextMenu;

        auto* const copyAction = contextMenu.addAction(tr("&Copy"), [this, selectedRows] {
            QStringList messageRecordsString;

            for (const auto& selectedRow : selectedRows) {
                const auto index            = _filterModel.mapToSource(selectedRows.first());
                const auto messageRecord    = static_cast<MessageRecord*>(index.internalPointer());

                messageRecordsString << messageRecord->toString();
            }
            
            QGuiApplication::clipboard()->setText(messageRecordsString.join("\n"));
        });
        
        contextMenu.exec(QCursor::pos());
    });

    connect(&_findLogFileAction, &QAction::triggered, [this](bool) {
        const auto filePath = Logger::GetFilePathName();

        if (!mv::util::ShowFileInFolder(filePath))
            QMessageBox::information(this, QObject::tr("Log file not found"), QObject::tr("The log file is not found:\n%1").arg(filePath));
        });
}

QSize LoggingWidget::sizeHint() const
{
    return QSize(100, 100);
}
