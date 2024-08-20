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

//#include <QDebug>
#include <QPainter>
#include <QStyleOption>
#include "vumeter.h"

Vumeter::Vumeter(QWidget *parent) :
    QWidget(parent),
    m_img(0),
    m_max(1.0),
    m_value(0.0)
{
    setMinimumWidth( 100 );
    setFixedHeight( 20 );
    initImage();
}

Vumeter::~Vumeter()
{
    delete m_img;
}

void Vumeter::initImage()
{
    delete m_img;
    m_img = new QImage(width(), height(), QImage::Format_ARGB32);
    m_img->fill(0);
}

void Vumeter::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QStyleOptionFrame option;
    option.initFrom(this);
    option.rect = contentsRect();
    option.lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &option, this);
    option.midLineWidth = 0;
    option.state |= QStyle::State_Sunken;
    option.state |= QStyle::State_ReadOnly;
    option.features = QStyleOptionFrame::None;
    style()->drawPrimitive(QStyle::PE_PanelLineEdit, &option, &painter, this);
    if (isEnabled()) {
        painter.drawImage( 0, 0,
            m_img->copy(0, 0, width() * m_value / m_max, height()) );
    }
}

void Vumeter::resizeEvent(QResizeEvent *)
{
    static const QColor colHi(Qt::red);
    static const QColor colLo(Qt::green);
    static const QColor colBg(0, 50, 0);
    static const qreal  margin(1.0);

    initImage();
    QPainter painter(m_img);
    painter.setRenderHint(QPainter::Antialiasing);
    // background
    painter.setBrush(colBg);
    painter.drawRect(QRectF(0, 0, width(), height()));
    // value
    QLinearGradient grad(0, 0, width(), height());
    grad.setColorAt(0, colLo);
    grad.setColorAt(1, colHi);
    painter.setBrush(grad);
    painter.drawRect( QRectF( margin, margin,
            width() - 2.0 * margin, height() - 2.0 * margin));
    // ticks
    painter.setPen(colBg);
    painter.setBrush(colBg);
    qreal c = width() / (4.0 * margin);
    int m = c / m_max;
    for ( int i = 0; i < m; ++i )
        painter.drawRect(QRectF(width()*i/c, 0, margin, height()));
}

void Vumeter::setValue(qreal value)
{
    if (isEnabled()) {
        if (value > m_max)
            m_value = m_max;
        else
            m_value = value;
        update();
    }
}

qreal Vumeter::value() const
{
    return m_value;
}

qreal Vumeter::decay(qreal pct)
{
    if (isEnabled()) {
        m_value -= (m_max * pct / 100);
        if (m_value < 0) m_value = 0.0;
        update();
    }
    return m_value;
}

void Vumeter::setMaxValue(qreal maxValue)
{
    m_max = maxValue;
    update();
}

qreal Vumeter::maxValue() const
{
    return m_max;
}

QSize Vumeter::minimumSizeHint() const
{
    return QSize(100, 20);
}

QSize Vumeter::sizeHint() const
{
    return QSize(100, 20);
}
