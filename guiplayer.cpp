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

#include <QDebug>
#include <QFileDialog>
#include <QToolTip>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileInfo>
#include <QTime>
#include <QTimer>
#include <QMimeData>
#include <QtMath>
#include <QActionGroup>
#include <QDesktopServices>
#if QT_VERSION < QT_VERSION_CHECK(5,14,0)
#include <QDesktopWidget>
#else
#include <QScreen>
#endif

#include <drumstick/settingsfactory.h>
#include "guiplayer.h"
#include "ui_guiplayer.h"
#include "playerabout.h"
#include "seqplayer.h"
#include "connections.h"
#include "iconutils.h"
#include "pianola.h"
#include "settings.h"
#include "prefsdialog.h"
#include "loopdialog.h"
#include "helpwindow.h"

using namespace drumstick::rt;
using namespace drumstick::widgets;

void GUIPlayer::connectOutPort()
{
    connect(m_pianola, &Pianola::noteOn, m_midiOut, &MIDIOutput::sendNoteOn);
    connect(m_pianola, &Pianola::noteOff, m_midiOut, &MIDIOutput::sendNoteOff);
    connect(m_channels, &Channels::patch, m_midiOut, &MIDIOutput::sendProgram);
}

GUIPlayer::GUIPlayer(QWidget *parent)
    : QMainWindow(parent),
    m_state(InvalidState),
    m_repeat(Nothing),
    m_midiOut(nullptr),
    m_player(nullptr),
    m_ui(new Ui::GUIPlayerClass),
    m_pd(nullptr),
    m_currentLang(nullptr)
{
    m_ui->setupUi(this);
    setAcceptDrops(true);
    connect(m_ui->actionAbout, &QAction::triggered, this, &GUIPlayer::about);
    connect(m_ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(m_ui->actionPlay, &QAction::triggered, this, &GUIPlayer::play);
    connect(m_ui->actionPause, &QAction::triggered, this, &GUIPlayer::pause);
    connect(m_ui->actionStop, &QAction::triggered, this, &GUIPlayer::stop);
    connect(m_ui->actionOpen, &QAction::triggered, this, &GUIPlayer::open);
    connect(m_ui->actionFileInfo, &QAction::triggered, this, &GUIPlayer::slotFileInfo);
    connect(m_ui->actionPlayList, &QAction::triggered, this, &GUIPlayer::slotPlayList);
    connect(m_ui->actionMIDISetup, &QAction::triggered, this, &GUIPlayer::setup);
    connect(m_ui->actionPreferences, &QAction::triggered, this, &GUIPlayer::preferences);
    connect(m_ui->actionQuit, &QAction::triggered, this, &GUIPlayer::quit);
    connect(m_ui->btnTempo, &QPushButton::clicked, this, &GUIPlayer::tempoReset);
    connect(m_ui->btnVolume, &QPushButton::clicked, this, &GUIPlayer::volumeReset);
    connect(m_ui->sliderTempo, &QSlider::valueChanged, this, &GUIPlayer::tempoSlider);
    connect(m_ui->volumeSlider, &QSlider::valueChanged, this, &GUIPlayer::volumeSlider);
    connect(m_ui->spinPitch, QOverload<int>::of(&QSpinBox::valueChanged), this, &GUIPlayer::pitchShift);
    connect(m_ui->toolBar->toggleViewAction(), &QAction::toggled,
            m_ui->actionShowToolbar, &QAction::setChecked);
    connect(m_ui->actionLyrics, &QAction::toggled, this, &GUIPlayer::slotShowLyrics);
    connect(m_ui->actionRhythm, &QAction::toggled, this, &GUIPlayer::slotShowRhythm);
    connect(m_ui->actionPianoPlayer, &QAction::toggled, this, &GUIPlayer::slotShowPianola);
    connect(m_ui->actionChannels, &QAction::toggled, this, &GUIPlayer::slotShowChannels);
    connect(m_ui->actionNext,  &QAction::triggered, this, &GUIPlayer::nextSong);
    connect(m_ui->actionPrev,  &QAction::triggered, this, &GUIPlayer::prevSong);
    connect(m_ui->positionSlider, &QSlider::sliderPressed, this, &GUIPlayer::positionSliderPressed);
    connect(m_ui->positionSlider, &QSlider::sliderMoved, this, &GUIPlayer::positionSliderMoved);
    connect(m_ui->positionSlider, &QSlider::sliderReleased, this, &GUIPlayer::positionSliderReleased);
    connect(m_ui->customizeToolBar, &QAction::triggered, this, &GUIPlayer::slotEditToolbar);
    connect(m_ui->actionForward, &QAction::triggered, this, &GUIPlayer::forward);
    connect(m_ui->actionBackward, &QAction::triggered, this, &GUIPlayer::backward);
    connect(m_ui->actionJump, &QAction::triggered, this, &GUIPlayer::jump);
    connect(m_ui->actionLoop, &QAction::triggered, this, &GUIPlayer::loop);
    connect(m_ui->actionContents, &QAction::triggered, this, &GUIPlayer::slotHelp);
    connect(m_ui->actionWebSite, &QAction::triggered, this, &GUIPlayer::slotOpenWebSite);
    connect(m_ui->actionLoadSongSettings, &QAction::triggered, this, &GUIPlayer::slotLoadSongSettings);
    connect(m_ui->actionSaveSongSettings, &QAction::triggered, this, &GUIPlayer::slotSaveSongSettings);

    m_ui->actionPlay->setShortcut( Qt::Key_MediaPlay );
    m_ui->actionStop->setShortcut( Qt::Key_MediaStop );

    createLanguageMenu();
    m_ui->actionShowStatusbar->setChecked(Settings::instance()->showStatusBar());
    m_ui->actionShowToolbar->setChecked(Settings::instance()->showToolBar());

    m_recentFiles = new RecentFilesHelper(m_ui->menuRecentFiles);
    connect(m_recentFiles, &RecentFilesHelper::selectedFile, this, &GUIPlayer::openFile);
    m_recentFiles->updateRecentFileActions();

    auto repeatGroup = new QActionGroup(this);
    repeatGroup->addAction(m_ui->actionNothing);
    repeatGroup->addAction(m_ui->actionCurrentSong);
    repeatGroup->addAction(m_ui->actionWholePlaylist);
    connect(repeatGroup, &QActionGroup::triggered, this, &GUIPlayer::slotPlaylistRepeat);

    m_player = new SequencePlayer();
    m_player->moveToThread(&m_playerThread);
    connect(m_player, &SequencePlayer::songFinished, this, &GUIPlayer::playerFinished);
    connect(m_player, &SequencePlayer::songStopped, this, &GUIPlayer::playerStopped);
    connect(m_player, &SequencePlayer::songEchoTime, this, &GUIPlayer::playerEcho);
    connect(m_player, &SequencePlayer::timeSignature, this, [=](const int /*bar*/, const int num, const int /*den*/){
        //qDebug() << "bar:" << bar << "time signature:" << num << "/" << den;
        m_ui->rhythm->setRhythm(num);
    });
    connect(m_player, &SequencePlayer::beat, this, [=](const int bar, const int beat, const int max){
        //qDebug() << "beat(" << bar << beat << max << ")";
        if (m_ui->rhythm->currentRhythm() != max) {
            m_ui->rhythm->setRhythm(max);
        }
        m_ui->rhythm->beat(beat);
        m_ui->lblPos->setText(QString("%1:%2").arg(bar).arg(beat));
    });
    connect(&m_playerThread, &QThread::started, m_player, &SequencePlayer::playerLoop);

    m_pianola = new Pianola(this);
    connect(m_pianola, &Pianola::closed, this, &GUIPlayer::slotPianolaClosed);
    connect(m_player, &SequencePlayer::midiNoteOn, m_pianola, &Pianola::slotNoteOn);
    connect(m_player, &SequencePlayer::midiNoteOff, m_pianola, &Pianola::slotNoteOff);
    connect(m_player, &SequencePlayer::keySignature, m_pianola, &Pianola::slotKeySignature);

    m_channels = new Channels(this);
    connect(m_channels, &Channels::closed, this, &GUIPlayer::slotChannelsClosed);
    connect(m_player, &SequencePlayer::midiNoteOn, m_channels, &Channels::slotNoteOn);
    connect(m_player, &SequencePlayer::midiNoteOff, m_channels, &Channels::slotNoteOff);
    connect(m_player, &SequencePlayer::midiProgram, m_channels, &Channels::slotPatch);
    connect(m_channels, &Channels::name, m_pianola, &Pianola::slotLabel);

    connect(m_channels, &Channels::mute, this, &GUIPlayer::slotMuted);
    connect(m_channels, &Channels::volume, this, &GUIPlayer::slotVolume);
    connect(m_channels, &Channels::lock, this, &GUIPlayer::slotLocked);
    connect(m_channels, &Channels::patch, this, &GUIPlayer::slotPatch);

    m_lyrics = new Lyrics(this);
    connect(m_lyrics, &Lyrics::closed, this, &GUIPlayer::slotLyricsClosed);
    connect(m_player, &SequencePlayer::songStarted, m_lyrics, &Lyrics::displayText);
    connect(m_player, &SequencePlayer::midiText, m_lyrics, &Lyrics::slotMidiText);

    m_preferences = new PrefsDialog(this);
    m_playList = new PlayList(this);
    m_playList->loadPlayList( Settings::instance()->lastPlayList() );
    updNavButtons();

    m_toolbarEditor = new ToolBarEditDialog(m_ui->toolBar, this);
    m_helpWindow = new HelpWindow(this);

    m_ui->frame->setPalette(QPalette(QColor(0x0f,0x0f,0x0f)));
    QFont lblTimeFont = QApplication::font();
    lblTimeFont.setPointSize(36);
    m_ui->lblTime->setFont(lblTimeFont);

    try {
        BackendManager man;
        man.refresh(Settings::instance()->settingsMap());
        QList<MIDIOutput*> outputs = man.availableOutputs();

        m_midiOut = man.findOutput(Settings::instance()->lastOutputBackend());
        if (m_midiOut == nullptr) {
            qWarning() << "MIDI OUT driver not found";
        }

        SettingsFactory settings;
        m_connections = new Connections(this);
        m_connections->setOutputs(outputs);

        if (m_midiOut != 0) {
            MIDIConnection conn;
            QString lastConn = Settings::instance()->lastOutputConnection();
            auto outConnections = m_midiOut->connections(Settings::instance()->advanced());
            auto itr = std::find_if(outConnections.constBegin(), outConnections.constEnd(), [lastConn](const MIDIConnection& c){return c.first == lastConn;});
            if (itr == outConnections.constEnd()) {
                if (!outConnections.isEmpty()) {
                    conn = outConnections.first();
                }
            } else {
                conn = (*itr);
            }
            m_midiOut->initialize(settings.getQSettings());
            m_midiOut->open(conn);
            auto metaObj = m_midiOut->metaObject();
            if ((metaObj->indexOfProperty("status") != -1) &&
                (metaObj->indexOfProperty("diagnostics") != -1)) {
                auto status = m_midiOut->property("status");
                if (status.isValid() && !status.toBool()) {
                    auto diagnostics = m_midiOut->property("diagnostics");
                    if (diagnostics.isValid()) {
                        auto text = diagnostics.toStringList().join(QChar::LineFeed).trimmed();
                        qWarning() << "MIDI Output" << text;
                    }
                }
            }
            m_player->setPort(m_midiOut);
            connectOutPort();
        } else {
            qWarning() << "MIDI Output is not connected";
        }

        tempoReset();
        volumeReset();
        updateState(EmptyState);
        applySettings();
    } catch (...) {
        qWarning() << "Error";
    }
}

GUIPlayer::~GUIPlayer()
{
    if (m_player != 0) {
        stop();
        delete m_player;
    }
    delete m_ui;
}

void GUIPlayer::updateTimeLabel(long milliseconds)
{
    double fracpart, intpart, seconds = milliseconds / 1000.0;
    fracpart = modf(seconds, &intpart);
    QTime t = QTime(0,0).addSecs(intpart).addMSecs(ceil(fracpart*1000));
    m_ui->lblTime->setText(t.toString("mm:ss.zzz").left(8));
}

void GUIPlayer::updNavButtons()
{
    m_ui->actionPrev->setEnabled(false);
    m_ui->actionNext->setEnabled(false);
    if (!m_playList->noItems()) {
        m_ui->actionPrev->setEnabled(!m_playList->atFirstItem());
        m_ui->actionNext->setEnabled(!m_playList->atLastItem());
    }
}

void GUIPlayer::updatePositionWidgets()
{
    m_ui->lblPos->setText(m_player->currentBeatStr());
    m_ui->positionSlider->setValue(m_player->getPosition());
}

void GUIPlayer::updateState(PlayerState newState)
{
    if (m_state == newState)
        return;

    switch (newState) {
    case EmptyState:
        m_ui->actionPlay->setEnabled(false);
        m_ui->actionPause->setEnabled(false);
        m_ui->actionStop->setEnabled(false);
        m_ui->actionForward->setEnabled(false);
        m_ui->actionBackward->setEnabled(false);
        m_ui->actionJump->setEnabled(false);
        m_ui->actionLoop->setEnabled(false);
        statusBar()->showMessage(tr("Please, load a song"));
        break;
    case PlayingState:
        m_ui->actionPlay->setEnabled(false);
        m_ui->actionPause->setEnabled(true);
        m_ui->actionStop->setEnabled(true);
        m_ui->actionForward->setEnabled(true);
        m_ui->actionBackward->setEnabled(true);
        m_ui->actionJump->setEnabled(true);
        m_ui->actionLoop->setEnabled(true);
        statusBar()->showMessage(tr("Playing"));
        break;
    case PausedState:
        m_ui->actionPlay->setEnabled(false);
        m_ui->actionStop->setEnabled(true);
        m_ui->actionForward->setEnabled(true);
        m_ui->actionBackward->setEnabled(true);
        m_ui->actionJump->setEnabled(true);
        m_ui->actionLoop->setEnabled(true);
        statusBar()->showMessage(tr("Paused"));
        break;
    case StoppedState:
        m_ui->actionPause->setChecked(false);
        m_ui->actionPause->setEnabled(false);
        m_ui->actionStop->setEnabled(true);
        m_ui->actionPlay->setEnabled(true);
        m_ui->actionForward->setEnabled(true);
        m_ui->actionBackward->setEnabled(true);
        m_ui->actionJump->setEnabled(true);
        m_ui->actionLoop->setEnabled(true);
        updateTimeLabel(0);
        m_ui->positionSlider->setValue(0);
        statusBar()->showMessage(tr("Stopped"));
        break;
    default:
        statusBar()->showMessage(tr("Not initialized"));
        break;
    }
    m_state = newState;
    //qDebug() << Q_FUNC_INFO << m_state;
}

void GUIPlayer::play()
{
    //qDebug() << Q_FUNC_INFO;
    if (!m_player->song()->isEmpty()) {
        if (m_player->getPosition() == 0) {
            m_player->sendResetMessage();
            m_player->resetControllers();
            m_player->resetPrograms();
            m_player->sendVolumeEvents();
        }
        m_playerThread.start(QThread::HighPriority);
        updateState(PlayingState);
    }
}

void GUIPlayer::pause()
{
    //qDebug() << Q_FUNC_INFO;
    if (m_state == PlayingState || m_player->isRunning()) {
        m_player->pause();
        updateState(PausedState);
    } else {
        QTimer::singleShot(0, this, &GUIPlayer::play);
    }
}

void GUIPlayer::stop()
{
    qDebug() << Q_FUNC_INFO;
    if (m_state == PlayingState || m_state == PausedState || m_player->isRunning()) {
        m_player->stop();
        m_playerThread.wait();
        updateState(StoppedState);
    }
    m_player->resetPosition();
    updatePositionWidgets();
}

void GUIPlayer::progressDialogInit(const QString& type, int max)
{
    m_pd = new QProgressDialog(0, 0, 0, max, this);
    m_pd->setWindowTitle(tr("Loading %1 file...").arg(type));
    m_pd->setMinimumDuration(0); // 1000
    m_pd->setMaximum(max);
    m_pd->setValue(0);
    connect(m_player->song(), &Sequence::loadingProgress, this, &GUIPlayer::progressDialogUpdate);
    connect(m_player->song(), &Sequence::loadingFinished, this, &GUIPlayer::progressDialogClose);
}

void GUIPlayer::progressDialogUpdate(int pos)
{
    if (m_pd != 0) {
        m_pd->setValue(pos);
    }
}

void GUIPlayer::progressDialogClose()
{
    delete m_pd; // set to 0 by QPointer<>
}

void GUIPlayer::openFile(const QString& fileName)
{
    QFileInfo finfo(fileName);
    if (finfo.exists()) {
        QString ext = finfo.suffix().toLower();
        if (ext == "wrk") {
            progressDialogInit("Cakewalk", finfo.size());
        } else if (ext == "rmi") {
            progressDialogInit("RIFF MIDI", finfo.size());
        } else if (ext == "mid" || ext == "midi" || ext == "kar") {
            progressDialogInit("MIDI", finfo.size());
        }
        m_player->loadFile(fileName);
        if (m_player->song()->isEmpty()) {
            m_ui->lblName->clear();
            updNavButtons();
            updateState(EmptyState);
        } else {
            Settings::instance()->setLastDirectory(finfo.absolutePath());
            m_ui->lblName->setText(finfo.fileName());
            m_recentFiles->setCurrentFile(finfo.absoluteFilePath());
            updNavButtons();
            updateState(StoppedState);
            updateTimeLabel(0);
            m_player->resetPosition();
            updateTempoLabel(m_player->currentBPM());
            updatePositionWidgets();
            auto max = m_player->song()->songLengthTicks();
            m_ui->positionSlider->setMaximum(max);
            m_ui->positionSlider->setTickInterval(max / 100);
            m_ui->positionSlider->setValue(0);

            for(int i = 0; i < MIDI_STD_CHANNELS; ++i ) {
                m_player->setLocked(i, false);
                m_player->setMuted(i, false);
            }

            if (m_pianola != nullptr) {
                m_pianola->initSong( m_player->song() );
            }

            if (m_channels != nullptr) {
                m_channels->initSong( m_player->song() );
            }

            if (m_lyrics != nullptr) {
                m_lyrics->initSong( m_player->song() );
            }

            if (Settings::instance()->autoSongSettings()) {
                slotLoadSongSettings();
            }

            if ( Settings::instance()->getAutoPlay() ) {
                QTimer::singleShot(0, this, &GUIPlayer::play);
            }
        }
    } else {
        QMessageBox::warning(this, QSTR_APPNAME, tr("file %1 couldn't be opened").arg(finfo.fileName()));
    }
}

void GUIPlayer::openFileList(const QStringList &fileNames)
{
    m_playList->clearPlayList();
    m_playList->setItems(fileNames);
    if (m_playList->selectFirstItem()) {
        openFile(m_playList->currentItem());
    }
}

void GUIPlayer::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,
          tr("Open MIDI File"), Settings::instance()->lastDirectory(),
          tr("All files (*.kar *.mid *.midi *.rmi *.wrk);;"
          "Karaoke files (*.kar);;"
          "MIDI Files (*.mid *.midi);;"
          "RIFF MIDI Files (*.rmi);;"
          "Cakewalk files (*.wrk)") );
    if (!fileName.isEmpty() && isSupported(fileName)) {
        stop();
        openFile(fileName);
    }
}

void GUIPlayer::setup()
{
    MIDIConnection conn;
    m_connections->setOutput(m_midiOut);
    m_connections->setAdvanced(Settings::instance()->advanced());
    m_connections->refresh();
    if (m_connections->exec() == QDialog::Accepted) {
        if (m_midiOut != m_connections->getOutput()) {
            if (m_midiOut != 0) {
                m_midiOut->disconnect();
            }
            m_midiOut = m_connections->getOutput();
            m_player->setPort(m_midiOut);
            connectOutPort();
        }
        conn = m_midiOut->currentConnection();
        Settings::instance()->setAdvanced(m_connections->advanced());
        Settings::instance()->setLastOutputBackend(m_midiOut->backendName());
        Settings::instance()->setLastOutputConnection(conn.first);
    }
}

void GUIPlayer::applySettings()
{
    static QPalette defaultPalette = qApp->palette(); //style()->standardPalette();
    static QPalette darkPalette(QColor(0x30,0x30,0x30));
#if defined(Q_OS_WINDOWS)
    m_snapper.SetEnabled(Settings::instance()->winSnap());
#endif
    qApp->setPalette( Settings::instance()->getDarkMode() ? darkPalette : defaultPalette );
    qApp->setStyle( Settings::instance()->getStyle() );

    m_ui->actionFileInfo->setIcon(IconUtils::GetIcon("dialog-information"));
    m_ui->actionPlayList->setIcon(IconUtils::GetIcon("view-media-playlist"));
    m_ui->actionAbout->setIcon(IconUtils::GetIcon("help-about"));
    m_ui->actionAboutQt->setIcon(IconUtils::GetStandardIcon(QStyle::SP_TitleBarMenuButton));
    m_ui->actionOpen->setIcon(IconUtils::GetIcon("document-open"));
    m_ui->menuRecentFiles->setIcon(IconUtils::GetIcon("document-open-recent"));
    m_ui->actionQuit->setIcon(IconUtils::GetIcon("system-shutdown"));
    m_ui->actionPlay->setIcon(IconUtils::GetIcon("media-playback-start"));
    m_ui->actionStop->setIcon(IconUtils::GetIcon("media-playback-stop"));
    m_ui->actionPause->setIcon(IconUtils::GetIcon("media-playback-pause"));
    m_ui->actionPrev->setIcon(IconUtils::GetIcon("media-skip-backward"));
    m_ui->actionNext->setIcon(IconUtils::GetIcon("media-skip-forward"));
    m_ui->actionMIDISetup->setIcon(IconUtils::GetIcon("midi"));
    m_ui->actionPreferences->setIcon(IconUtils::GetIcon("settings"));
    m_ui->btnVolume->setIcon(IconUtils::GetIcon("player-volume"));
    m_ui->menuLanguage->setIcon(IconUtils::GetIcon("preferences-desktop-locale"));
    m_ui->actionForward->setIcon(IconUtils::GetIcon("media-seek-forward"));
    m_ui->actionBackward->setIcon(IconUtils::GetIcon("media-seek-backward"));
    m_ui->actionJump->setIcon(IconUtils::GetIcon("go-jump"));
    m_ui->customizeToolBar->setIcon(IconUtils::GetIcon("settings"));
    m_ui->menuPlaylistRepeat->setIcon(IconUtils::GetIcon("media-playlist-repeat"));
    m_ui->actionLoop->setIcon(IconUtils::GetIcon("looping"));
    m_ui->actionContents->setIcon(IconUtils::GetIcon("help-contents"));
    m_ui->actionWebSite->setIcon(IconUtils::GetIcon("viewhtml"));
    m_ui->actionLoadSongSettings->setIcon(IconUtils::GetIcon("document-open"));
    m_ui->actionSaveSongSettings->setIcon(IconUtils::GetIcon("document-save"));

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    foreach(QMenu *mnu, m_ui->menuBar->findChildren<QMenu*>()) {
        mnu->setPalette(qApp->palette());
    }
#endif
    m_lyrics->applySettings();
    m_pianola->applySettings();
    m_channels->applySettings();
    m_helpWindow->applySettings();
}

void GUIPlayer::preferences()
{
    if (m_preferences->exec() == QDialog::Accepted) {
        applySettings();
    }
}

void GUIPlayer::playerFinished()
{
    qDebug() << Q_FUNC_INFO;
    m_player->resetPosition();
    updateTimeLabel(0);
    updatePositionWidgets();
    if (Settings::instance()->autoSongSettings()) {
        slotSaveSongSettings();
    }
    if (m_repeat == CurrentSong) {
        QTimer::singleShot(0, this, &GUIPlayer::play);
    } else if ( Settings::instance()->autoAdvance()) {
        if (m_playList->atLastItem()) {
            if (m_repeat == WholePlaylist) {
                if (m_playList->selectFirstItem()) {
                    openFile(m_playList->currentItem());
                }
            }
        } else {
            nextSong();
        }
    }
}

void GUIPlayer::playerStopped()
{
    qDebug() << Q_FUNC_INFO;
    m_playerThread.wait();
    m_player->allNotesOff();
    if (m_pianola != nullptr) {
        m_pianola->allNotesOff();
    }
    if (m_channels != nullptr) {
        m_channels->allNotesOff();
    }
    if (m_state == PlayingState) {
        updateState(StoppedState);
    }
    updatePositionWidgets();
    if (Settings::instance()->autoSongSettings()) {
        slotSaveSongSettings();
    }
}

void GUIPlayer::updateTempoLabel(float ftempo)
{
    QString stempo = QString("%1 bpm").arg(ftempo, 0, 'f', 2);
    m_ui->lblOther->setText(stempo);
}

void GUIPlayer::playerEcho(long millis, long ticks)
{
    updateTempoLabel(m_player->currentBPM());
    updateTimeLabel(millis);
    m_ui->positionSlider->setValue(ticks);
}

void GUIPlayer::nextSong()
{
    if (m_state == PlayingState) {
        stop();
    }
    if (m_playList->selectNextItem()) {
        openFile(m_playList->currentItem());
    }
}

void GUIPlayer::prevSong()
{
    if (m_state == PlayingState) {
        stop();
    }
    if (m_playList->selectPrevItem()) {
        openFile(m_playList->currentItem());
    }
}

void GUIPlayer::positionSliderPressed()
{
    if (m_state == PlayingState || m_player->isRunning()) {
        m_player->pause();
        updateState(PausedState);
    }
}

void GUIPlayer::positionSliderMoved(int value)
{
    if (m_ui->positionSlider->isSliderDown()) {
        m_newSliderPosition = value;
        m_player->beatByTickPosition(m_newSliderPosition);
        m_ui->lblPos->setText(m_player->currentBeatStr());
        // tooltip
        QString tip = m_player->currentBeatStr();
        m_ui->positionSlider->setToolTip(tip);
        QToolTip::showText(QCursor::pos(), tip, this);
    }
}

void GUIPlayer::positionSliderReleased()
{
    m_player->setPosition(m_newSliderPosition);
    m_player->beatByTickPosition(m_newSliderPosition);
    m_ui->lblPos->setText(m_player->currentBeatStr());
    // tooltip
    QString tip = m_player->currentBeatStr();
    m_ui->positionSlider->setToolTip(tip);
    QToolTip::showText(QCursor::pos(), tip, this);
    if (m_state == PausedState) {
        QTimer::singleShot(0, this, &GUIPlayer::play);
    }
}

void GUIPlayer::forward()
{
    if (m_state == PlayingState || m_player->isRunning()) {
        m_player->pause();
        updateState(PausedState);
    }
    m_player->beatForward();
    updatePositionWidgets();
    if (m_state == PausedState) {
        QTimer::singleShot(0, this, &GUIPlayer::play);
    }
}

void GUIPlayer::backward()
{
    if (m_state == PlayingState || m_player->isRunning()) {
        m_player->pause();
        updateState(PausedState);
    }
    m_player->beatBackward();
    updatePositionWidgets();
    if (m_state == PausedState) {
        QTimer::singleShot(0, this, &GUIPlayer::play);
    }
}

void GUIPlayer::jump()
{
    bool ok;
    int bar = QInputDialog::getInt(this, tr("Jump to Bar"),
                tr("Bar number:"), 1, 1, m_player->song()->lastBar(), 1, &ok);
    if (ok) {
        if (m_state == PlayingState || m_player->isRunning()) {
            m_player->pause();
            updateState(PausedState);
        }
        m_player->jumpToBar(bar);
        updatePositionWidgets();
        if (m_state == PausedState) {
            QTimer::singleShot(0, this, &GUIPlayer::play);
        }
    }
}

void GUIPlayer::loop()
{
    auto active = m_ui->actionLoop->isChecked();
    //qDebug() << Q_FUNC_INFO << active;
    if (active) {
        LoopDialog dlg;
        dlg.setLastBar(m_player->song()->lastBar());
        dlg.initValues(m_player->loopStart(), m_player->loopEnd());
        if (dlg.exec() == QDialog::Accepted) {
            m_player->setLoop(dlg.getFromBar(), dlg.getToBar());
        } else {
            m_ui->actionLoop->setChecked(false);
            m_player->setLoop(false);
        }
    } else {
        m_player->setLoop(false);
    }
}

void GUIPlayer::pitchShift(int value)
{
    m_player->setPitchShift(value);
}

void GUIPlayer::tempoReset()
{
    m_ui->sliderTempo->setValue(100);
    tempoSlider(100);
}

void GUIPlayer::volumeReset()
{
    m_ui->volumeSlider->setValue(100);
    volumeSlider(100);
}

void GUIPlayer::tempoSlider(int value)
{
    qreal factor = (value*value + 100.0*value + 20000.0) / 40000.0;
    m_player->song()->setTempoFactor(factor);
    updateTempoLabel(m_player->currentBPM());
    // Slider tooltip
    QString tip = QString("%1%").arg(factor*100.0, 0, 'f', 0);
    m_ui->sliderTempo->setToolTip(tip);
    QToolTip::showText(QCursor::pos(), tip, this);
}

void GUIPlayer::volumeSlider(int value)
{
    QString tip = QString::number(value)+'%';
    m_ui->lblVolume->setText(tip);
    m_ui->volumeSlider->setToolTip(tip);
    if (m_player != nullptr) {
        m_player->setVolumeFactor(value);
    }
    if (m_channels != nullptr) {
        m_channels->setVolumeFactor(value*0.01);
    }
    QToolTip::showText(QCursor::pos(), tip, this);
}

void GUIPlayer::dragEnterEvent( QDragEnterEvent * event )
{
    if (event->mimeData()->hasFormat("text/uri-list"))
        event->acceptProposedAction();
}

void GUIPlayer::dropEvent( QDropEvent * event )
{
    if (event->mimeData()->hasText()) {
        QStringList list, data;
#if (QT_VERSION < QT_VERSION_CHECK(5,15,0))
        data = event->mimeData()->text().split(QChar::LineFeed, QString::SkipEmptyParts);
#else
        data = event->mimeData()->text().split(QChar::LineFeed, Qt::SkipEmptyParts);
#endif
        foreach(const QString& fileName, data) {
            QString localFileName = QUrl(fileName).toLocalFile();
            if ( isSupported(localFileName) ) {
                list.append(localFileName);
            } else {
                QMessageBox::warning(this, QSTR_APPNAME,
                    tr("Dropped file %1 is not supported").arg(fileName));
            }
        }
        if (!list.isEmpty()) {
            stop();
            openFileList(list);
            event->accept();
        }
    }
}

bool GUIPlayer::isSupported(QString fileName)
{
    return  fileName.endsWith(".mid", Qt::CaseInsensitive) ||
            fileName.endsWith(".midi", Qt::CaseInsensitive) ||
            fileName.endsWith(".kar", Qt::CaseInsensitive) ||
            fileName.endsWith(".rmi", Qt::CaseInsensitive) ||
            fileName.endsWith(".wrk", Qt::CaseInsensitive);
}

void GUIPlayer::connectOutput(const QString &driver, const QString &connection)
{
    Settings::instance()->setLastOutputBackend(driver);
    Settings::instance()->setLastOutputConnection(connection);
}

void GUIPlayer::closeEvent( QCloseEvent *event )
{
    Settings::instance()->setShowStatusBar(m_ui->actionShowStatusbar->isChecked());
    Settings::instance()->setShowToolBar(m_ui->actionShowToolbar->isChecked());
    Settings::instance()->setMainWindowGeometry(saveGeometry());
    Settings::instance()->setMainWindowState(saveState());
    QStringList actionNames;
    foreach(auto a, m_ui->toolBar->actions()) {
        actionNames << a->objectName();
    }
    Settings::instance()->setToolbarActions(actionNames);
    Settings::instance()->setToolbarButtonStyle(m_ui->toolBar->toolButtonStyle());
    Settings::instance()->SaveSettings();
    event->accept();
}

void GUIPlayer::about()
{
    About aboutDlg(this);
    aboutDlg.exec();
}

void GUIPlayer::quit()
{
    stop();
    close();
}

void GUIPlayer::slotShowPianola(bool checked)
{
    if (m_pianola != nullptr) {
        m_pianola->setVisible(checked);
    }
}

void GUIPlayer::slotPianolaClosed()
{
    m_ui->actionPianoPlayer->setChecked(false);
}

void GUIPlayer::slotShowChannels(bool checked)
{
    if (m_channels != nullptr) {
        m_channels->setVisible(checked);
    }
}

void GUIPlayer::slotChannelsClosed()
{
    m_ui->actionChannels->setChecked(false);
}

void GUIPlayer::slotShowLyrics(bool checked)
{
    if (m_lyrics != nullptr) {
        m_lyrics->setVisible(checked);
    }
}

void GUIPlayer::slotLyricsClosed()
{
    m_ui->actionLyrics->setChecked(false);
}

void GUIPlayer::slotShowRhythm(bool checked)
{
    m_ui->rhythm->setVisible(checked);
}

bool GUIPlayer::nativeEvent(const QByteArray &eventType, void *message,
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
                            long *result
#else
                            qintptr *result
#endif
                           )
{
#if defined(Q_OS_WINDOWS)
    if (Settings::instance()->winSnap() && m_snapper.HandleMessage(message)) {
        result = 0;
        return true;
    }
#endif
    return QWidget::nativeEvent(eventType, message, result);
}

void GUIPlayer::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    if (m_firstShown) {
        const QByteArray geometry = Settings::instance()->mainWindowGeometry();
        const QByteArray state = Settings::instance()->mainWindowState();
        if (geometry.isEmpty()) {
            const QRect availableGeometry =
#if QT_VERSION < QT_VERSION_CHECK(5,14,0)
                    QApplication::desktop()->availableGeometry(this);
#else
                    screen()->availableGeometry();
#endif
            //qDebug() << Q_FUNC_INFO << availableGeometry;
            setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter,
                                            size(), availableGeometry));
        } else {
            restoreGeometry(geometry);
        }
        if (!state.isEmpty()) {
            restoreState(state);
        }
        QStringList actions = Settings::instance()->toolbarActions();
        if (!actions.empty()) {
            m_ui->toolBar->clear();
            foreach(const auto a, actions) {
                QAction *action = findChild<QAction*>(a, Qt::FindDirectChildrenOnly);
                if (action != nullptr) {
                    m_ui->toolBar->addAction(action);
                }
            }
        }
        int s = Settings::instance()->toolbarButtonStyle();
        m_ui->toolBar->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(s));
        m_firstShown = false;
    }
}

void GUIPlayer::createLanguageMenu()
{
    QString currentLang = Settings::instance()->language();
    if (currentLang.isEmpty()) {
        QLocale locale;
        if (locale.language() == QLocale::C || locale.language() == QLocale::English)
            currentLang = "C";
        else
            currentLang = locale.name().left(2);
    }
    QActionGroup *languageGroup = new QActionGroup(this);
    languageGroup->setExclusive(true);
    connect(languageGroup, &QActionGroup::triggered, this, &GUIPlayer::slotSwitchLanguage);
    QDir dir(Settings::localeDirectory());
    QStringList fileNames = dir.entryList({"dmidiplayer*.qm"}, QDir::NoFilter, QDir::NoSort);
    QStringList locales;
    locales << "C";
    foreach (const QString& fileName, fileNames) {
        QFileInfo f(fileName);
        QString locale = f.fileName();
        if (locale.startsWith("dmidiplayer_")) {
            locale.remove(0, locale.indexOf('_') + 1);
            locale.truncate(locale.lastIndexOf('.'));
            locales << locale;
        }
    }
    locales.sort();
    m_ui->menuLanguage->clear();
    foreach (const QString& loc, locales) {
        QLocale qlocale(loc);
        QString localeName = loc == "C" ? QLocale::languageToString(QLocale::English) : qlocale.nativeLanguageName();
        QAction *action = new QAction(localeName.section(" ", 0, 0), this);
        action->setCheckable(true);
        action->setData(loc);
        m_ui->menuLanguage->addAction(action);
        languageGroup->addAction(action);
        if (currentLang.startsWith(loc)) {
            action->setChecked(true);
            m_currentLang = action;
        }
    }
}

void GUIPlayer::slotAboutTranslation()
{
    QString common = tr("<p>This program is developed and translated thanks to the "
        "volunteer work of many people from around the world. If you want to "
        "join the team or have any question, please visit the web site at "
        "<a href='http://sourceforge.net/projects/dmidiplayer/'>SourceForge</a>"
        "</p>");
    QMessageBox::information(this, tr("Translation Information"), common);
}

void GUIPlayer::retranslateUi()
{
    Settings::instance()->loadTranslations();
    m_ui->retranslateUi(this);
    m_connections->retranslateUi();
    m_pianola->retranslateUi();
    m_channels->retranslateUi();
    m_lyrics->retranslateUi();

    m_preferences->retranslateUi();
    m_recentFiles->retranslateUi();
    m_playList->retranslateUi();
    m_toolbarEditor->retranslateUi();
    m_helpWindow->retranslateUi();
}

void GUIPlayer::slotSwitchLanguage(QAction *action)
{
    QString lang = action->data().toString();
    QLocale qlocale(lang);
    QString langName = lang == "C" ? QLocale::languageToString(QLocale::English) : qlocale.nativeLanguageName();
    if ( QMessageBox::question (this, tr("Language Changed"),
            tr("The language for this application is going to change to %1. "
               "Do you want to continue?").arg(langName),
            QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes )
    {
        Settings::instance()->setLanguage(lang);
        retranslateUi();
    } else {
        if (m_currentLang == nullptr) {
            m_currentLang = action;
        }
        m_currentLang->setChecked(true);
    }
}

void GUIPlayer::slotFileInfo()
{
    QString infostr;
    if (m_player->song()->currentFile().isEmpty())
        infostr = tr("<b>No file loaded</b>");
    else {
        infostr = tr("File: <b>%1</b><br>").arg(m_player->song()->currentFile());
        QString s = m_player->song()->getFileFormat();
        if (!s.isEmpty())
            infostr += tr("Format: <b>%1</b><br>").arg(s);

        s = m_player->song()->getText(Sequence::Copyright).join(tr("<br>"));
        if (!s.isEmpty())
            infostr += tr("Copyright: <b>%1</b><br>").arg(s);

        s = m_player->song()->getText(Sequence::KarFileType).join(tr("<br>"));
        if (!s.isEmpty())
            infostr += tr("Karaoke type: <b>%1</b><br>").arg(s);

        s = m_player->song()->getText(Sequence::KarVersion).join(tr("<br>"));
        if (!s.isEmpty())
            infostr += tr("Karaoke version: <b>%1</b><br>").arg(s);

        s = m_player->song()->getText(Sequence::KarInformation).join(tr("<br>"));
        if (!s.isEmpty())
            infostr += tr("Karaoke info: <b>%1</b><br>").arg(s);

        s = m_player->song()->getText(Sequence::KarLanguage).join(tr("<br>"));
        if (!s.isEmpty())
            infostr += tr("Karaoke language: <b>%1</b><br>").arg(s);

        s = m_player->song()->getText(Sequence::KarTitles).join(tr("<br>"));
        if (!s.isEmpty())
            infostr += tr("Karaoke title: <b>%1</b><br>").arg(s);

        s = m_player->song()->getText(Sequence::KarWhatever).join(tr("<br>"));
        if (!s.isEmpty())
            infostr += tr("Karaoke things: <b>%1</b><br>").arg(s);

        s = m_player->song()->getMetadataInfo();
        if (!s.isEmpty())
            infostr += s;
    }
    infostr.replace(QChar::LineSeparator, tr("<br>"));
    QMessageBox::information(this, tr("SMF Information"), infostr );
}

void GUIPlayer::slotPlayList()
{
    if (m_playList->exec() == QDialog::Accepted) {
        if (m_playList->isDirty()) {
            QMessageBox::warning(this, tr("Playlist not saved"),
                tr("The current playlist has changed, but it is not saved. "
                   "The changes will be lost when closing the program."));
        }
        auto current = m_playList->currentItem();
        if (!current.isEmpty()) {
            if (m_state == PlayingState) {
                stop();
            }
            openFile(current);
        }
    }
}

void GUIPlayer::slotEditToolbar()
{
    m_toolbarEditor->initialize();
    m_toolbarEditor->exec();
}

void GUIPlayer::slotPlaylistRepeat(QAction *action)
{
    if (action == m_ui->actionNothing) {
        m_repeat = Nothing;
    } else if (action == m_ui->actionCurrentSong) {
        m_repeat = CurrentSong;
    } else if (action == m_ui->actionWholePlaylist) {
        m_repeat = WholePlaylist;
    } else {
        m_repeat = Nothing;
    }
}

void GUIPlayer::slotHelp()
{
    QString lang = Settings::instance()->language();
    if (lang == "C") {
        lang = "en";
    }
    QDir hdir(":/help");
    QString hname = QStringLiteral("%1/index.html").arg(lang);
    QFileInfo finfo(hdir, hname);
    if (!finfo.exists()) {
        hname = "en/index.html";
    }
    m_helpWindow->showPage(hdir.path(), hname);
}

void GUIPlayer::slotOpenWebSite()
{
    QUrl url(QStringLiteral("https://dmidiplayer.sourceforge.io"));
    QDesktopServices::openUrl(url);
}

void GUIPlayer::slotVolume(int channel, qreal vol)
{
    m_player->setVolume(channel, vol);
}

void GUIPlayer::slotMuted(int channel, bool mute)
{
    m_player->setMuted(channel, mute);
}

void GUIPlayer::slotLocked(int channel, bool lock)
{
    m_player->setLocked(channel, lock);
}

void GUIPlayer::slotPatch(int channel, int patch)
{
    m_player->setPatch(channel, patch);
}

void GUIPlayer::slotSaveSongSettings()
{
    QString songName = m_player->song()->currentFile();
    qDebug() << Q_FUNC_INFO << songName;
    if (!songName.isEmpty()) {
        QDir dataDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.dmidiplayer");
        QString fileName = QString("%1/%2.cfg").arg(dataDir.absolutePath(), songName);
        QSettings songSettings(fileName, QSettings::IniFormat);
        songSettings.beginGroup("Global");
        songSettings.setValue("file", m_player->song()->currentFullFileName());
        songSettings.setValue("encoding", QString::fromLatin1(m_player->song()->currentCharset()));
        songSettings.setValue("volume", m_ui->volumeSlider->value());
        songSettings.setValue("pitch", m_ui->spinPitch->value());
        songSettings.setValue("timeskew", m_ui->sliderTempo->value());
        songSettings.endGroup();
        for(int i = 0; i < MIDI_STD_CHANNELS; ++i ) {
            if ( m_player->song()->channelUsed(i) ) {
                songSettings.beginGroup(QString("MIDI Channel %1").arg(i+1,2));
                songSettings.setValue("name", m_channels->channelName(i));
                songSettings.setValue("muted", m_channels->isChannelMuted(i));
                songSettings.setValue("solo", m_channels->isChannelSoloed(i));
                songSettings.setValue("locked", m_channels->isChannelLocked(i));
                songSettings.setValue("patch", m_channels->channelPatch(i));
                songSettings.setValue("level", m_channels->channelLevel(i));
                songSettings.endGroup();
            }
        }
        songSettings.sync();
    }
}

void GUIPlayer::slotLoadSongSettings()
{
    int vol, pitch, skew, pgm;
    bool locked, muted, solo;
    QString songName = m_player->song()->currentFile();
    qDebug() << Q_FUNC_INFO << songName;
    if (!songName.isEmpty()) {
        QDir dataDir(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/.dmidiplayer");
        QString fileName = QString("%1/%2.cfg").arg(dataDir.absolutePath(), songName);
        QSettings songSettings(fileName, QSettings::IniFormat);
        songSettings.beginGroup("Global");
        //QString songfile = songSettings.value("file").toString();
        //qDebug() << Q_FUNC_INFO << songfile;
        QString encoding = songSettings.value("encoding").toString();
        if (!encoding.isEmpty()) {
            m_player->song()->setCurrentCharset(encoding.toLatin1());
            m_lyrics->initSong(m_player->song());
        }
        vol = songSettings.value("volume", 100).toInt();
        m_ui->volumeSlider->setValue(vol);
        volumeSlider(vol);
        pitch = songSettings.value("pitch", 0).toInt();
        m_ui->spinPitch->setValue(pitch);
        pitchShift(pitch);
        skew = songSettings.value("timeskew", 100).toInt();
        m_ui->sliderTempo->setValue(skew);
        tempoSlider(skew);
        songSettings.endGroup();
        for(int i = 0; i < MIDI_STD_CHANNELS; ++i ) {
            QString grpName = QString("MIDI Channel %1").arg(i+1,2);
            if ( songSettings.childGroups().contains(grpName) ) {
                songSettings.beginGroup(grpName);
                QString name = songSettings.value("name").toString();
                if (name.isEmpty())
                    name =  m_player->song()->channelLabel(i);
                if (!name.isEmpty()) {
                    m_channels->setChannelName(i, name);
                    m_pianola->slotLabel(i, name);
                }
                muted = songSettings.value("muted", false).toBool();
                m_channels->setMuteChannel(i, muted);
                solo = songSettings.value("solo", false).toBool();
                m_channels->setSoloChannel(i, solo);
                pgm = songSettings.value("patch", -1).toInt();
                m_channels->setPatchChannel(i, pgm);
                locked = songSettings.value("locked", false).toBool();
                m_channels->setLockChannel(i, locked);
                vol = songSettings.value("level", 100).toInt();
                m_channels->setLevelChannel(i, vol);
                songSettings.endGroup();
            }
        }
    }
}
