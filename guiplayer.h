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

#ifndef INCLUDED_GUIPLAYER_H
#define INCLUDED_GUIPLAYER_H

#include <QMainWindow>
#include <QProgressDialog>
#include <QObject>
#include <QString>
#include <QList>
#include <QHash>
#include <QPointer>
#include <QThread>

#include "connections.h"
#include "recentfileshelper.h"
#include "pianola.h"
#include "channels.h"
#include "lyrics.h"
#include "prefsdialog.h"
#include "playlist.h"
#include "toolbareditdialog.h"
#include "helpwindow.h"

#if defined(Q_OS_WINDOWS)
#include "winsnap.h"
#endif

class MIDIEvent;

namespace drumstick { namespace rt {
    class MIDIOutput;
}}

namespace Ui {
    class GUIPlayerClass;
}

class SequencePlayer;
class About;

class GUIPlayer : public QMainWindow
{
    Q_OBJECT

public:
    GUIPlayer(QWidget *parent = 0);
    ~GUIPlayer();

    enum PlayerState {
        InvalidState,
        EmptyState,
        PlayingState,
        PausedState,
        StoppedState
    };
    Q_ENUM(PlayerState)

    enum PlaylistRepetition {
        Nothing,
        CurrentSong,
        WholePlaylist
    };
    Q_ENUM(PlaylistRepetition)

    void appendSMFEvent(MIDIEvent* ev);
    void appendWRKEvent(unsigned long ticks, MIDIEvent* ev);
    void appendOVEEvent(unsigned long ticks, MIDIEvent* ev);

    void updateTimeLabel(long milliseconds);
    void updateTempoLabel(float ftempo);
    bool isSupported(QString fileName);
    void connectOutput(const QString &driver, const QString &connection);
    void openFile(const QString &fileName);
    void openFileList(const QStringList &fileNames);
    void updateState(PlayerState newState);
    void applySettings();
    void updNavButtons();
    void updatePositionWidgets();
    
protected:
    void dragEnterEvent( QDragEnterEvent* event ) override;
    void dropEvent( QDropEvent* event ) override;
    void closeEvent( QCloseEvent* event ) override;
    bool nativeEvent( const QByteArray &eventType, void *message,
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
                    long *result
#else
                    qintptr *result
#endif
    ) override;
    void showEvent(QShowEvent *event) override;
    void moveEvent(QMoveEvent *event) override;

public slots:
    void about();
    void play();
    void pause();
    void stop();
    void open();
    void setup();
    void preferences();
    void tempoReset();
    void volumeReset();
    void tempoSlider(int value);
    void quit();
    void volumeSlider(int value);
    void pitchShift(int value);
    void playerFinished();
    void playerStopped();
    void playerEcho(long time, long ticks);
    void nextSong();
    void prevSong();
    void positionSliderPressed();
    void positionSliderMoved(int value);
    void positionSliderReleased();
    void forward();
    void backward();
    void jump();
    void loop();

    void progressDialogInit(const QString& type, int max);
    void progressDialogUpdate(int pos);
    void progressDialogClose();

    void slotShowPianola(bool checked);
    void slotPianolaClosed();
    void slotShowChannels(bool checked);
    void slotChannelsClosed();
    void slotShowLyrics(bool checked);
    void slotLyricsClosed();
    void slotShowRhythm(bool checked);

    void slotAboutTranslation();
    void slotSwitchLanguage(QAction *action);
    void slotFileInfo();
    void slotPlayList();
    void slotEditToolbar();
    void slotPlaylistRepeat(QAction *action);
    void slotHelp();
    void slotOpenWebSite();

    void slotVolume(int channel, qreal vol);
    void slotMuted(int channel, bool mute);
    void slotLocked(int channel, bool lock);
    void slotPatch(int channel, int patch);

    void slotLoadSongSettings();
    void slotSaveSongSettings();
    void slotSearch();

private:
    void createLanguageMenu();
    void retranslateUi();
    void connectOutPort();

    PlayerState m_state;
    PlaylistRepetition m_repeat;
    drumstick::rt::MIDIOutput* m_midiOut;
    SequencePlayer* m_player;
    Ui::GUIPlayerClass* m_ui;
    QPointer<QProgressDialog> m_pd;
    QThread m_playerThread;
    QPointer<RecentFilesHelper> m_recentFiles;
    QPointer<Connections> m_connections;
    QPointer<Pianola> m_pianola;
    QPointer<Channels> m_channels;
    QPointer<Lyrics> m_lyrics;
    QPointer<PrefsDialog> m_preferences;
    QPointer<PlayList> m_playList;
    QPointer<ToolBarEditDialog> m_toolbarEditor;
    QPointer<HelpWindow> m_helpWindow;
    QAction *m_currentLang;
    bool m_firstShown{true};
    int m_newSliderPosition{0};
#if defined(Q_OS_WINDOWS)
    WinSnap m_snapper;
#endif
};

#endif // INCLUDED_GUIPLAYER_H
