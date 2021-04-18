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

#include "iconutils.h"
#include <QApplication>
#include <QPainter>

namespace IconUtils
{
    void PaintPixmap(QPixmap &pixmap, const QColor& color)
    {
        QPainter painter(&pixmap);
        painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        painter.fillRect(pixmap.rect(), color);
    }

    QPixmap GetPixmap(const QString& fileName)
    {
        QPixmap pixmap(fileName);
        PaintPixmap(pixmap, qApp->palette().color(QPalette::Active, QPalette::WindowText));
        return pixmap;
    }

    QIcon GetStandardIcon(QStyle::StandardPixmap sp)
    {
        return qApp->style()->standardIcon(sp);
    }

    QIcon GetIcon(const QString &fileName)
    {
        return QIcon(GetPixmap(fileName));
    }

} // namespace IconUtils
