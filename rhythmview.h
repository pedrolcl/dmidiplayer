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

#ifndef RHYTHMVIEW_H
#define RHYTHMVIEW_H

#include <QWidget>
#include <QResizeEvent>
#include <QLabel>

class RhythmView : public QWidget
{
public:
    RhythmView (QWidget *parent = 0);
    ~RhythmView();

    void setRhythm(int numerator);
    int currentRhythm() const;

    void beat(int i); // Sets the beat number

    void beat(); // Just increase the beat

    QSize sizeHint() const;
    QSizePolicy sizePolicy();

private:
    void lampOn(QLabel* lamp);
    void lampOff(QLabel* lamp);

    void resizeEvent(QResizeEvent *);
    void clearLamps();

    int m_last;
    QList<QLabel*> m_lamps;
};

#endif /* RHYTHMVIEW_H */
