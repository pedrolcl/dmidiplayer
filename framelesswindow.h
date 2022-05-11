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

class FramelessWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit FramelessWindow(QWidget *parent = nullptr);
    QWidget *pseudoCaption() const;
    void setPseudoCaption(QWidget *widget);

protected:
    void updateCursor(Qt::Edges edges);
    bool event(QEvent* ev) override;

private:
    QWidget *m_pseudoCaption;
};

#endif
