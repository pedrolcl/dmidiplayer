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

#ifndef PIANOLA_H
#define PIANOLA_H

#include <QMainWindow>
#include <drumstick/rtmidioutput.h>
#include <mutex>

#include "framelesswindow.h"

class QFrame;
class QLabel;
class QToolButton;
class Sequence;

namespace drumstick { namespace widgets {
    class PianoKeybd;
}}

class Pianola : public FramelessWindow {
    Q_OBJECT

public:
    Pianola( QWidget* parent = 0 );
    virtual ~Pianola();

    void enableChannel(int channel, bool enable);
    void setNoteRange(int lowerNote, int upperNote);
    void readSettings();
    void writeSettings();
    void retranslateUi();
    void applySettings() override;
    void initSong( Sequence *song );

signals:
    void closed();
    void noteOn(int channel, int note, int vel);
    void noteOff(int channel, int note, int vel);

public slots:
    void playNoteOn(int note, int vel);
    void playNoteOff(int note, int vel);
    void slotNoteOn(int channel, int note, int vel);
    void slotNoteOff(int channel, int note, int vel);
    void slotShowChannel(int chan);
    void slotShowAllChannels();
    void slotHideAllChannels();
    void allNotesOff();
    void slotLabel(int channel, const QString& text);
    void tightenKeys(bool enabled);
    void slotKeySignature(int track, int alt, bool minor);
    void toggleFullScreen(bool enabled);

protected:
    void closeEvent( QCloseEvent *event ) override;
    void showEvent( QShowEvent * event ) override;

private:
    Sequence *m_song;
    drumstick::widgets::PianoKeybd* m_piano[drumstick::rt::MIDI_STD_CHANNELS];
    QMenu *m_chmenu;
    QLabel *m_title;
    QToolButton* m_toolBtn;
    QAction *m_a1, *m_a2, *m_a3, *m_a4;
    QFrame* m_frame[drumstick::rt::MIDI_STD_CHANNELS];
    QAction* m_action[drumstick::rt::MIDI_STD_CHANNELS];
    QLabel* m_label[drumstick::rt::MIDI_STD_CHANNELS];
    bool m_tightenKeys;
    int m_lowerNote;
    int m_upperNote;
    std::once_flag m_firstTime;
};

#endif /* PIANOLA_H */
