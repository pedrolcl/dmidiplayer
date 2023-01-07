/*
    Drumstick MIDI File Player Multiplatform Program
    Copyright (C) 2006-2023, Pedro Lopez-Cabanillas <plcl@users.sf.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <QRegularExpression>
#include "toolbareditdialog.h"
#include "ui_toolbareditdialog.h"
#include "iconutils.h"

ToolBarEditDialog::ToolBarEditDialog(QToolBar *toolBar, QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::ToolBarEditDialog),
    m_toolBar(toolBar)
{
    m_ui->setupUi(this);
    retranslateUi();
    connect(m_ui->lstActions, &QListView::clicked, this, &ToolBarEditDialog::availableClicked);
    connect(m_ui->lstToolbar, &QListView::clicked, this, &ToolBarEditDialog::selectedClicked);
    connect(m_ui->btnAdd, &QPushButton::clicked, this, &ToolBarEditDialog::btnAdd);
    connect(m_ui->btnRemove, &QPushButton::clicked, this, &ToolBarEditDialog::btnRemove);
    connect(m_ui->btnMoveUp, &QPushButton::clicked, this, &ToolBarEditDialog::btnUp);
    connect(m_ui->btnMoveDown, &QPushButton::clicked, this, &ToolBarEditDialog::btnDown);
}

ToolBarEditDialog::~ToolBarEditDialog()
{
    m_actionsModel.clear();
    m_toolbarModel.clear();
    delete m_ui;
}

void ToolBarEditDialog::retranslateUi()
{
    setWindowTitle(tr("Customize %1").arg(m_toolBar->windowTitle()));
    m_ui->retranslateUi(this);
}

void ToolBarEditDialog::initialize()
{
    m_actionsModel.clear();
    m_toolbarModel.clear();
    m_ui->btnAdd->setEnabled(false);
    m_ui->btnRemove->setEnabled(false);
    m_ui->btnMoveUp->setEnabled(false);
    m_ui->btnMoveDown->setEnabled(false);
    m_ui->btnAdd->setIcon(IconUtils::GetIcon("go-next"));
    m_ui->btnRemove->setIcon(IconUtils::GetIcon("go-previous"));
    m_ui->btnMoveUp->setIcon(IconUtils::GetIcon("go-up"));
    m_ui->btnMoveDown->setIcon(IconUtils::GetIcon("go-down"));
    QList<QAction*> available = parent()->findChildren<QAction*>(QRegularExpression("action.*"), Qt::FindDirectChildrenOnly);
    QList<QAction*> selected = m_toolBar->actions();
    foreach(QAction *action, available) {
        if (!selected.contains(action)) {
            auto item = new QStandardItem(action->icon(), action->text());
            item->setData(action->objectName(), Qt::UserRole);
            m_actionsModel.appendRow(item);
        }
    }
    foreach(QAction *action, selected) {
        auto item = new QStandardItem(action->icon(), action->text());
        item->setData(action->objectName(), Qt::UserRole);
        m_toolbarModel.appendRow(item);
    }
    m_ui->lstActions->setModel(&m_actionsModel);
    m_ui->lstToolbar->setModel(&m_toolbarModel);
    m_ui->cbBtnStyle->setCurrentIndex(m_toolBar->toolButtonStyle());
}

void ToolBarEditDialog::apply()
{
    m_toolBar->clear();
    for(int row = 0; row < m_toolbarModel.rowCount(); ++row) {
        QModelIndex index = m_toolbarModel.index(row, 0);
        QString name = m_toolbarModel.data(index, Qt::UserRole).toString();
        QAction *action = parent()->findChild<QAction*>(name, Qt::FindDirectChildrenOnly);
        if (action != nullptr) {
            m_toolBar->addAction(action);
        }
    }
    m_toolBar->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(m_ui->cbBtnStyle->currentIndex()));
}

void ToolBarEditDialog::accept()
{
    apply();
    QDialog::accept();
}

void ToolBarEditDialog::availableClicked(const QModelIndex &index)
{
    m_ui->lstToolbar->clearSelection();
    m_ui->btnAdd->setEnabled(true);
    m_ui->btnRemove->setEnabled(false);
    m_ui->btnMoveUp->setEnabled(false);
    m_ui->btnMoveDown->setEnabled(false);
    m_actionIndex = index;
    m_toolbarIndex = QModelIndex();
}

void ToolBarEditDialog::enableRightButtons(const QModelIndex &index)
{
    m_ui->btnAdd->setEnabled(false);
    m_ui->btnRemove->setEnabled(true);
    m_ui->btnMoveUp->setEnabled(index.row() > 0);
    m_ui->btnMoveDown->setEnabled(index.row() < m_toolbarModel.rowCount()-1);
}

void ToolBarEditDialog::selectedClicked(const QModelIndex &index)
{
    m_ui->lstActions->clearSelection();
    enableRightButtons(index);
    m_toolbarIndex = index;
    m_actionIndex = QModelIndex();
}

void ToolBarEditDialog::btnAdd()
{
    if (m_actionIndex.isValid()) {
        m_toolbarModel.appendRow(m_actionsModel.takeRow(m_actionIndex.row()));
    }
}

void ToolBarEditDialog::btnRemove()
{
    if (m_toolbarIndex.isValid()) {
        m_actionsModel.appendRow(m_toolbarModel.takeRow(m_toolbarIndex.row()));
    }
}

void ToolBarEditDialog::btnUp()
{
    if (m_toolbarIndex.isValid()) {
        int newRow = m_toolbarIndex.row() - 1;
        auto item = m_toolbarModel.takeRow(m_toolbarIndex.row());
        m_toolbarModel.insertRow(newRow, item);
        m_toolbarIndex = m_toolbarModel.index(newRow, 0);
        m_ui->lstToolbar->setCurrentIndex(m_toolbarIndex);
        enableRightButtons(m_toolbarIndex);
    }
}

void ToolBarEditDialog::btnDown()
{
    if (m_toolbarIndex.isValid()) {
        int newRow = m_toolbarIndex.row() + 1;
        auto item = m_toolbarModel.takeRow(m_toolbarIndex.row());
        m_toolbarModel.insertRow(newRow, item);
        m_toolbarIndex = m_toolbarModel.index(newRow, 0);
        m_ui->lstToolbar->setCurrentIndex(m_toolbarIndex);
        enableRightButtons(m_toolbarIndex);
    }
}
