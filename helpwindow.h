/*
    Drumstick MIDI File Player Multiplatform Program
    Copyright (C) 2006-2024, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include <QByteArray>
#include <QCloseEvent>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QString>
#include <QTextBrowser>
#include <mutex>

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

private:
    QLabel *m_titleLabel;
    QTextBrowser *m_textBrowser;
    QAction *m_home;
    QAction *m_back;
    QAction *m_close;
    QAction *m_zoomIn;
    QAction *m_zoomOut;
    std::once_flag m_firstTime;
};

#endif // HELPWINDOW_H
