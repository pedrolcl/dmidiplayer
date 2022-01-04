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

#ifndef VUMETER_H
#define VUMETER_H

#include <QWidget>

class QImage;

class Vumeter : public QWidget
{
    Q_OBJECT
public:

    Vumeter(QWidget *parent = 0);
    virtual ~Vumeter();

    qreal value() const;
    qreal maxValue() const;
    qreal decay(qreal pct = 10.0);

public slots:
    void setValue(qreal);
    void setMaxValue(qreal);

protected:
    void initImage();
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

private:
    QImage *m_img;
    qreal m_max;
    qreal m_value;
};

#endif /* VUMETER_H */
