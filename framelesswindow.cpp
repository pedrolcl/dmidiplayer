/*
    Drumstick MIDI File Player Multiplatform Program
    Copyright (C) 2006-2023, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#include <QFlags>
#include <QWindow>
#include <QEvent>
#include <QHoverEvent>
#include <QStyle>
#include "settings.h"
#include "framelesswindow.h"

FramelessWindow::FramelessWindow(QWidget *parent)
    : QMainWindow(parent, Qt::FramelessWindowHint),
      m_pseudoCaption(nullptr)
{ 
#if defined(Q_OS_MACOS)
    //setUnifiedTitleAndToolBarOnMac(true);
    setAttribute(Qt::WA_MacMiniSize, true);
#else
    setWindowFlag(Qt::Tool, true);
#endif
    setAttribute(Qt::WA_DeleteOnClose, false);
    connect(this, &FramelessWindow::sizeAdjustNeeded, this, &FramelessWindow::performAdjustSize, Qt::QueuedConnection);
}

void FramelessWindow::updateCursor(Qt::Edges edges)
{
    if ( edges == 0 )
    {
        setCursor(Qt::ArrowCursor);
    }
    else if ( (edges == Qt::LeftEdge) ||
              (edges == Qt::RightEdge) )
    {
        setCursor(Qt::SizeHorCursor);
    }
    else if ( (edges == Qt::TopEdge) ||
              (edges == Qt::BottomEdge))
    {
        setCursor(Qt::SizeVerCursor);
    }
    else if ( (edges == (Qt::LeftEdge | Qt::TopEdge)) ||
              (edges == (Qt::RightEdge | Qt::BottomEdge)) )
    {
        setCursor(Qt::SizeFDiagCursor);
    }
    else if ( (edges == (Qt::RightEdge | Qt::TopEdge)) ||
              (edges == (Qt::LeftEdge | Qt::BottomEdge)) )
    {
        setCursor(Qt::SizeBDiagCursor);
    }
}

bool FramelessWindow::event(QEvent* ev)
{
    static Qt::Edges edges = Qt::Edges();
    const int margin = 5;

    switch (ev->type()) {
    case QEvent::MouseButtonPress:
        updateCursor(edges);
        if ((m_pseudoCaption != nullptr) && m_pseudoCaption->underMouse()) {
            /*auto ok =*/ windowHandle()->startSystemMove();
            //qDebug() << "startSystemMove()" << (ok ? "succeeded" : "failed");
        } else if (edges != 0) {
            /*auto ok =*/ windowHandle()->startSystemResize(edges);
            //qDebug() << "startSystemResize()" << (ok ? "succeeded" : "failed");
        }
        break;
    case QEvent::MouseButtonRelease:
        //qDebug() << Q_FUNC_INFO << ev;
        edges = Qt::Edges();
        updateCursor(edges);
        setFocus();
        break;
    case QEvent::HoverEnter:
        edges = Qt::Edges();
        break;
    case QEvent::HoverLeave:
        edges = Qt::Edges();
        break;
    case QEvent::HoverMove: {
        edges = Qt::Edges();
        QHoverEvent *event = static_cast<QHoverEvent*>(ev);
        QPoint p =
    #if QT_VERSION < QT_VERSION_CHECK(6,0,0)
                   event->pos();
    #else
                   event->position().toPoint();
    #endif
        if ( p.x() < margin ) {
            edges |= Qt::LeftEdge;
        }
        if ( p.x() > (width() - margin) ) {
            edges |= Qt::RightEdge;
        }
        if ( p.y() < margin ) {
            edges |= Qt::TopEdge;
        }
        if ( p.y() > (height() - margin) ) {
            edges |= Qt::BottomEdge;
        }
        updateCursor(edges);
        break;
    }
    default:
        break;
    }
    return QMainWindow::event(ev);
}

QWidget *FramelessWindow::pseudoCaption() const
{
    return m_pseudoCaption;
}

void FramelessWindow::setPseudoCaption(QWidget *widget)
{
    m_pseudoCaption = widget;
}

void FramelessWindow::performAdjustSize()
{
    centralWidget()->adjustSize();
    adjustSize();
}

void FramelessWindow::applySettings()
{
#if defined(Q_OS_WINDOWS)
    m_snapper.SetEnabled(Settings::instance()->winSnap());
#endif
}

bool FramelessWindow::nativeEvent(const QByteArray &eventType, void *message,
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
                            long *result
#else
                            qintptr *result
#endif
                          )
{
#if defined(Q_OS_WINDOWS)
    if (m_snapper.HandleMessage(message)) {
        result = 0;
        return true;
    }
#endif
    return QMainWindow::nativeEvent(eventType, message, result);
}
