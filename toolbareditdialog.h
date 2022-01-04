/*
    Drumstick MIDI File Player Multiplatform Program
    Copyright (C) 2006-2022, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#ifndef TOOLBAREDITDIALOG_H
#define TOOLBAREDITDIALOG_H

#include <QDialog>
#include <QToolBar>
#include <QStandardItemModel>
#include <QModelIndex>

namespace Ui {
    class ToolBarEditDialog;
}

class ToolBarEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolBarEditDialog(QToolBar *toolBar,  QWidget *parent = nullptr);
    ~ToolBarEditDialog();
    void initialize();
    void apply();
    void enableRightButtons(const QModelIndex &index);

public slots:
    void accept() override;
    void availableClicked(const QModelIndex &index);
    void selectedClicked(const QModelIndex &index);
    void btnAdd();
    void btnRemove();
    void btnUp();
    void btnDown();
    void retranslateUi();

private:
    Ui::ToolBarEditDialog *m_ui;
    QToolBar *m_toolBar;
    QStandardItemModel m_actionsModel;
    QStandardItemModel m_toolbarModel;
    QModelIndex m_actionIndex;
    QModelIndex m_toolbarIndex;
};

#endif // TOOLBAREDITDIALOG_H
