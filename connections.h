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

#ifndef CONNECTIONS_H
#define CONNECTIONS_H

#include <QObject>
#include <QDialog>
#include <QShowEvent>
#include <drumstick/rtmidioutput.h>
#include "ui_connections.h"

class Connections : public QDialog
{
    Q_OBJECT

public:
    Connections(QWidget *parent = 0);
    void setOutput(drumstick::rt::MIDIOutput *out);
    void setOutputs(QList<drumstick::rt::MIDIOutput *> outs);
    drumstick::rt::MIDIOutput *getOutput();
    void setAdvanced(bool value);
    bool advanced();
    void retranslateUi();

public slots:
    void configureOutputDriver();
    void clickedAdvanced(bool value);
    void refreshOutputs(int idx);
    void refresh();
    void reopen();
    void accept() override;
    void reject() override;

private:
    bool m_advanced;
    bool m_settingsChanged;
    drumstick::rt::MIDIOutput *m_midiOut, *m_savedOut;
    drumstick::rt::MIDIConnection m_connOut;
    Ui::ConnectionsClass ui;
};

#endif // CONNECTIONS_H
