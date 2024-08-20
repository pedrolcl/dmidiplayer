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

#ifndef INSTRUMENTSET_H
#define INSTRUMENTSET_H

#include <QCoreApplication>
#include <QVector>
#include <QStringList>

class InstrumentSet {
    Q_DECLARE_TR_FUNCTIONS(InstrumentSet)
public:
    InstrumentSet();
    virtual ~InstrumentSet();

    void reloadNames();

    QString name(const int patch, bool percussion = false) const;

    QStringList names(bool percussion = false) const;

private:
    QVector<QString> m_names;
    QStringList m_numbers;
};

#endif /* INSTRUMENTSET_H */
