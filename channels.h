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

#ifndef CHANNELS_H
#define CHANNELS_H

#include <QMainWindow>
#include <drumstick/rtmidioutput.h>
#include "instrumentset.h"

#if defined(Q_OS_WINDOWS)
#include "winsnap.h"
#endif

class QLabel;
class QToolButton;
class QComboBox;
class Vumeter;
class QLineEdit;

class Channels : public QMainWindow {
    Q_OBJECT

public:
    Channels( QWidget* parent = 0 );
    virtual ~Channels();
    void enableChannel(int channel, bool enable);
    qreal volumeFactor();
    void setVolumeFactor(qreal factor);
    void readSettings();
    void writeSettings();
    void retranslateUi();

    QString channelName(int channel) const;
    bool isChannelMuted(int channel) const;
    bool isChannelSoloed(int channel) const;
    bool isChannelLocked(int channel) const;
    int channelPatch(int channel) const;

    void setChannelName(int channel, const QString& name);
    void setMuteChannel(int channel, bool mute);
    void setSoloChannel(int channel, bool solo);
    void setLockChannel(int channel, bool lock);
    void setPatchChannel(int channel, int patch);

signals:
    void closed();
    void patch(int channel, int value);
    void mute(int channel, bool muted);
    void volume(int channel, qreal factor);
    void lock(int channel, bool locked);
    void name(int channel, const QString& text);

public slots:
    void slotPatch(int channel, int value);
    void slotNoteOn(int channel, int note, int vel);
    void slotNoteOff(int channel, int note, int vel);
    void slotEnableAllChannels();
    void slotDisableAllChannels();
    void slotMuteChannel(int channel);
    void slotSoloChannel(int channel);
    void slotPatchChanged(int channel);
    void slotLockChannel(int channel);
    void slotNameChannel(int channel);
    void allNotesOff();

protected:
    void closeEvent( QCloseEvent *event)  override;
    void timerEvent( QTimerEvent *event ) override;
    bool nativeEvent( const QByteArray &eventType, void *message, long *result ) override;

private:
    int m_timerId;
    qreal m_volumeFactor;
    InstrumentSet m_instSet;
    int m_voices[drumstick::rt::MIDI_STD_CHANNELS];
    qreal m_level[drumstick::rt::MIDI_STD_CHANNELS];
    qreal m_factor[drumstick::rt::MIDI_STD_CHANNELS];
    QLabel *m_lbl1, *m_lbl2, *m_lbl3, *m_lbl4, *m_lbl5, *m_lbl6;
    QToolButton* m_mute[drumstick::rt::MIDI_STD_CHANNELS];
    QToolButton* m_solo[drumstick::rt::MIDI_STD_CHANNELS];
    QToolButton* m_lock[drumstick::rt::MIDI_STD_CHANNELS];
    Vumeter* m_vumeter[drumstick::rt::MIDI_STD_CHANNELS];
    QComboBox* m_patch[drumstick::rt::MIDI_STD_CHANNELS];
    QLineEdit* m_name[drumstick::rt::MIDI_STD_CHANNELS];
#if defined(Q_OS_WINDOWS)
    WinSnap m_snapper;
#endif
};

#endif /* CHANNELS_H */
