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

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QMainWindow>
#include <QTextBrowser>
#include <QPushButton>
#include <QCloseEvent>
#include <QByteArray>
#include <QString>

#if defined(Q_OS_WINDOWS)
#include "winsnap.h"
#endif

class HelpWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit HelpWindow(QWidget *parent = nullptr);
    void readSettings();
    void writeSettings();
    void retranslateUi();
    void showPage(const QString &path, const QString &page);

protected:
    void closeEvent( QCloseEvent *event)  override;
    bool nativeEvent( const QByteArray &eventType, void *message,
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
                  long *result
#else
                  qintptr *result
#endif
    ) override;

private slots:
    void updateWindowTitle();

private:
    QTextBrowser *m_textBrowser;
    QPushButton *m_homeButton;
    QPushButton *m_backButton;
    QPushButton *m_closeButton;
    QString m_path, m_page;
#if defined(Q_OS_WINDOWS)
    WinSnap m_snapper;
#endif
};

#endif // HELPWINDOW_H
