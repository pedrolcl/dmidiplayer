/*
    Drumstick MIDI File player graphic program
    Copyright (C) 2006-2020, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#ifndef INCLUDED_GUIPLAYER_H
#define INCLUDED_GUIPLAYER_H

#include <QMainWindow>
#include <QProgressDialog>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QList>
#include <QHash>
#include <QPointer>
#include <QThread>
#include "connections.h"

class MIDIEvent;

namespace drumstick { namespace rt {
    class MIDIOutput;
}}

namespace Ui {
    class GUIPlayerClass;
}

class SequencePlayer;
class About;

const QString QSTR_DOMAIN("drumstick.sourceforge.net");
const QString QSTR_APPNAME("Drumstick-GUIPlayer");

enum PlayerState {
    InvalidState,
    EmptyState,
    PlayingState,
    PausedState,
    StoppedState
};

class GUIPlayer : public QMainWindow
{
    Q_OBJECT

public:
    GUIPlayer(QWidget *parent = 0, Qt::WindowFlags flags = 0);
    ~GUIPlayer();

    void appendSMFEvent(MIDIEvent* ev);
    void appendWRKEvent(unsigned long ticks, MIDIEvent* ev);
    void appendOVEEvent(unsigned long ticks, MIDIEvent* ev);

    void updateTimeLabel(int milliseconds);
    void updateTempoLabel(float ftempo);
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    void closeEvent(QCloseEvent* event);
    bool isSupported(QString fileName);
    void connectOutput(const QString &driver, const QString &connection);
    void openFile(const QString &fileName);
    void readSettings(QSettings &settings);
    void writeSettings(QSettings &settings);
    void updateState(PlayerState newState);

public slots:
    void about();
    void play();
    void pause();
    void stop();
    void open();
    void setup();
    void tempoReset();
    void volumeReset();
    void tempoSlider(int value);
    void quit();
    void volumeSlider(int value);
    void pitchShift(int value);
    void playerFinished();
    void playerStopped();
    void playerEcho(int time, int ticks);

    void progressDialogInit(const QString& type, int max);
    void progressDialogUpdate(int pos);
    void progressDialogClose();

private:
    void findOutput(QString name, QList<drumstick::rt::MIDIOutput*> &outputs);

    PlayerState m_state;
    drumstick::rt::MIDIOutput* m_midiOut;
    SequencePlayer* m_player;
    Ui::GUIPlayerClass* m_ui;
    QPointer<QProgressDialog> m_pd;
    bool m_advanced;
    QString m_lastOutputBackend;
    QString m_lastOutputConnection;
    Connections dlgConnections;
    QString m_lastDirectory;
    QThread m_playerThread;
};

#endif // INCLUDED_GUIPLAYER_H
