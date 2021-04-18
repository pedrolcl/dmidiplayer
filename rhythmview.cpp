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

#include <QColor>
#include <QPalette>
#include <QApplication>
#include <QPixmap>
#include "rhythmview.h"


RhythmView::RhythmView (QWidget *parent)
    : QWidget(parent),
    m_last(0)
{
    led_off = QPixmap(":/icons/led_off.png");
    led_on = QPixmap(":/icons/led_on.png");
    setRhythm( 4 );
}

RhythmView::~RhythmView()
{
    clearLamps();
}

void RhythmView::clearLamps()
{
    if (!m_lamps.isEmpty()) {
        for (int i=0; i<m_lamps.size(); ++i) {
            delete m_lamps[i];
        }
        m_lamps.clear();
    }
}

void RhythmView::setRhythm( int numerator )
{
    if (m_lamps.size() != numerator) {
        int w = qMin(width() / numerator, height());
        int h = qMin(w, height());
        int x = 0;
        clearLamps();
        for (int i=0; i<numerator; ++i ) {
            QLabel *led = new QLabel( this );
            led->setGeometry(x+2,0,w-4,h);
            led->setScaledContents(true);
            led->setPixmap(led_off);
            led->show();
            m_lamps.insert(i, led);
            x += w;
        }
        lampOn(m_lamps[0]);
        m_last = 0;
    }
}

int RhythmView::currentRhythm() const
{
    return m_lamps.size();
}

void RhythmView::beat(int j)
{
    if (j>m_lamps.size()) {
        // This is a preventive case
        setRhythm(j);
    }
    for (int i=0; i<m_lamps.size(); ++i ) {
        lampOff(m_lamps[i]);
    }
    if (j > 0) {
        m_last = j-1;
        lampOn(m_lamps[m_last]);
    }
}

void RhythmView::beat()
{
    for (int i=0; i<m_lamps.size(); ++i ) {
        lampOff(m_lamps[i]);
    }
    ++m_last;
    if (m_last >= m_lamps.size()) {
        m_last = 0;
    }
    lampOn(m_lamps[m_last]);
}

void RhythmView::resizeEvent(QResizeEvent *)
{
    int w = qMin(width() / m_lamps.size(), height());
    int h = qMin(w, height());
    int x = 0;
    for (int i=0; i<m_lamps.size(); i++ ) {
        m_lamps[i]->setGeometry(x+2,0,w-4,h);
        x += w;
    }
}

QSize RhythmView::sizeHint() const
{
    return QSize(10,10);
}

QSizePolicy RhythmView::sizePolicy()
{
    return QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Minimum);
}

void RhythmView::lampOn(QLabel *lamp)
{
    lamp->setPixmap(led_on);
    lamp->update();
}

void RhythmView::lampOff(QLabel *lamp)
{
    lamp->setPixmap(led_off);
    lamp->update();
}
