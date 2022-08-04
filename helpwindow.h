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

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QMainWindow>
#include <QTextBrowser>
#include <QPushButton>
#include <QCloseEvent>
#include <QByteArray>
#include <QString>
#include <QLabel>

#include "framelesswindow.h"

class HelpWindow : public FramelessWindow
{
    Q_OBJECT
public:
    explicit HelpWindow(QWidget *parent = nullptr);
    void readSettings();
    void writeSettings();
    void retranslateUi();
    void applySettings() override;

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent( QCloseEvent *event)  override;

private slots:
    void updateWindowTitle();
    void setNewHome();

private:
    QLabel *m_titleLabel;
    QTextBrowser *m_textBrowser;
    QAction *m_home;
    QAction *m_back;
    QAction *m_close;
    QAction *m_zoomIn;
    QAction *m_zoomOut;
    QString m_path, m_page;
};

#endif // HELPWINDOW_H
