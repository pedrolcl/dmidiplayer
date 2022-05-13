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

#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include <QMainWindow>

#if defined(Q_OS_WINDOWS)
#include "winsnap.h"
#endif

class FramelessWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit FramelessWindow(QWidget *parent = nullptr);
    QWidget *pseudoCaption() const;
    void setPseudoCaption(QWidget *widget);

protected:
	virtual void applySettings();
    void updateCursor(Qt::Edges edges);
    bool event(QEvent* ev) override;
	bool nativeEvent( const QByteArray &eventType, void *message,
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
                      long *result
#else
                      qintptr *result
#endif
    ) override;

private:
    QWidget *m_pseudoCaption;
#if defined(Q_OS_WINDOWS)
    WinSnap m_snapper;
#endif
};

#endif
