/*
    Drumstick MIDI File Player Multiplatform Program
    Copyright (C) 2006-2021, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include <QMessageBox>
#include "loopdialog.h"
#include "ui_loopdialog.h"

LoopDialog::LoopDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::LoopDialog)
{
    m_ui->setupUi(this);
}

LoopDialog::~LoopDialog()
{
    delete m_ui;
}

int LoopDialog::getFromBar()
{
    return m_ui->spinFrom->value();
}

int LoopDialog::getToBar()
{
    return m_ui->spinTo->value();
}

void LoopDialog::setLastBar(int bar)
{
    m_ui->spinFrom->setMinimum(1);
    m_ui->spinFrom->setMaximum(bar);
    m_ui->spinTo->setMinimum(1);
    m_ui->spinTo->setMaximum(bar);
}

void LoopDialog::initValues(int from, int to)
{
    m_ui->spinFrom->setValue(from > m_ui->spinFrom->minimum() ? from : m_ui->spinFrom->minimum());
    m_ui->spinTo->setValue(to < m_ui->spinTo->maximum() ? to : m_ui->spinTo->maximum());
}

void LoopDialog::accept()
{
    auto from = m_ui->spinFrom->value();
    auto to = m_ui->spinTo->value();
    if (to > from) {
        QDialog::accept();
    } else {
        QMessageBox::critical(this, tr("Input Error"), tr("%1 is not greater than %2").arg(to).arg(from));
    }
}
