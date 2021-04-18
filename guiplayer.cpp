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

#include <QDebug>
#include <QFileDialog>
#include <QToolTip>
#include <QMessageBox>
#include <QFileInfo>
#include <QTime>
#include <QTimer>
#include <QMimeData>
#include <QtMath>
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

using namespace drumstick::rt;
using namespace drumstick::widgets;

GUIPlayer::GUIPlayer(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags),
    m_state(InvalidState),
    m_midiOut(nullptr),
    m_player(nullptr),
    m_ui(new Ui::GUIPlayerClass),
    m_pd(nullptr)
{
    m_trq = new QTranslator(this);
    m_trp = new QTranslator(this);
    m_trl = new QTranslator(this);
    QString lang = Settings::instance()->language();
    if (!m_trq->load("qt_" + lang, Settings::systemLocales()) && !lang.startsWith("en")) {
        qWarning() << "Failure loading Qt5 system translations for" << lang
                   << "from" << Settings::systemLocales();
    }
    if (!m_trp->load("dmidiplayer_" + lang, Settings::localeDirectory()) && !lang.startsWith("en")) {
        qWarning() << "Failure loading application translations for" << lang
                   << "from" << Settings::localeDirectory();
    }
    if (!m_trl->load("drumstick-widgets_" + lang, Settings::drumstickLocales()) && !lang.startsWith("en")) {
        qWarning() << "Failure loading widgets library translations for" << lang
                   << "from" << Settings::drumstickLocales();
    }
    QCoreApplication::installTranslator(m_trq);
    QCoreApplication::installTranslator(m_trp);
    QCoreApplication::installTranslator(m_trl);
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

    m_ui->actionPlay->setShortcut( Qt::Key_MediaPlay );
    m_ui->actionStop->setShortcut( Qt::Key_MediaStop );

    createLanguageMenu();
    m_ui->actionShowStatusbar->setChecked(Settings::instance()->showStatusBar());
    m_ui->actionShowToolbar->setChecked(Settings::instance()->showToolBar());

    m_recentFiles = new RecentFilesHelper(m_ui->menuRecentFiles);
    connect(m_recentFiles, &RecentFilesHelper::selectedFile, this, &GUIPlayer::openFile);
    m_recentFiles->updateRecentFileActions();

    m_player = new SequencePlayer();
    m_player->moveToThread(&m_playerThread);
    connect(m_player, &SequencePlayer::songFinished, this, &GUIPlayer::playerFinished);
    connect(m_player, &SequencePlayer::songStopped, this, &GUIPlayer::playerStopped);
    connect(m_player, &SequencePlayer::songEchoTime, this, &GUIPlayer::playerEcho);
    connect(m_player, &SequencePlayer::timeSignature, this, [=](const int /*bar*/, const int num, const int /*den*/){
        //qDebug() << "bar:" << bar << "time signature:" << num << "/" << den;
        m_ui->rhythm->setRhythm(num);
    });
    connect(m_player, &SequencePlayer::beat, this, [=](const int /*bar*/, const int beat, const int max){
        //qDebug() << "beat(" << bar << beat << max << ")";
        if (m_ui->rhythm->currentRhythm() != max) {
            m_ui->rhythm->setRhythm(max);
        }
        m_ui->rhythm->beat(beat);
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

    connect(m_channels, &Channels::mute, m_player, &SequencePlayer::setMuted);
    connect(m_channels, &Channels::volume, m_player, &SequencePlayer::setVolume);
    connect(m_channels, &Channels::lock, m_player, &SequencePlayer::setLocked);
    connect(m_channels, &Channels::patch, m_player, &SequencePlayer::setPatch);

    m_lyrics = new Lyrics(this);
    connect(m_lyrics, &Lyrics::closed, this, &GUIPlayer::slotLyricsClosed);
    connect(m_player, &SequencePlayer::songStarted, m_lyrics, &Lyrics::displayText);
    connect(m_player, &SequencePlayer::midiText, m_lyrics, &Lyrics::slotMidiText);

    m_preferences = new PrefsDialog(this);
    m_playList = new PlayList(this);
    m_playList->loadPlayList( Settings::instance()->lastPlayList() );

    try {
        BackendManager man;
        man.refresh(Settings::instance()->settingsMap());
        QList<MIDIOutput*> outputs = man.availableOutputs();

        m_midiOut = man.outputBackendByName(Settings::instance()->lastOutputBackend());
        if (m_midiOut == nullptr) {
            if (Settings::instance()->lastOutputBackend() != Settings::instance()->nativeOutput()) {
                qWarning() << "Last MIDI OUT driver" << Settings::instance()->lastOutputBackend() << "not found";
            }
            m_midiOut = man.outputBackendByName(Settings::instance()->nativeOutput());
            if (m_midiOut == nullptr) {
                qWarning() << "Default MIDI OUT driver" << Settings::instance()->nativeOutput() << "not found";
            }
        }

        SettingsFactory settings;
        m_connections = new Connections(this);
        m_connections->setOutputs(outputs);
        m_connections->setOutput(m_midiOut);
        m_connections->setAdvanced(Settings::instance()->advanced());

        if (m_midiOut != 0 && !Settings::instance()->lastOutputConnection().isEmpty()) {
            auto outConnections = m_midiOut->connections(Settings::instance()->advanced());
            MIDIConnection conn = outConnections.first();
            foreach(const auto& c, outConnections) {
                if (c.first == Settings::instance()->lastOutputConnection()) {
                    conn = c;
                    break;
                }
            }
            m_midiOut->initialize(settings.getQSettings());
            if (!conn.first.isEmpty())
                m_midiOut->open(conn);
            m_player->setPort(m_midiOut);
            connect(m_pianola.data(), &Pianola::noteOn, m_midiOut, &MIDIOutput::sendNoteOn);
            connect(m_pianola.data(), &Pianola::noteOff, m_midiOut, &MIDIOutput::sendNoteOff);
            connect(m_channels.data(), &Channels::patch, m_midiOut, &MIDIOutput::sendProgram);
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
        m_player->stop();
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

void GUIPlayer::updateState(PlayerState newState)
{
    if (m_state == newState)
        return;
    switch (newState) {
    case EmptyState:
        m_ui->actionPlay->setEnabled(false);
        m_ui->actionPause->setEnabled(false);
        m_ui->actionStop->setEnabled(false);
        statusBar()->showMessage("Please, load a song");
        break;
    case PlayingState:
        m_ui->actionPlay->setEnabled(false);
        m_ui->actionPause->setEnabled(true);
        m_ui->actionStop->setEnabled(true);
        statusBar()->showMessage("Playing");
        break;
    case PausedState:
        m_ui->actionPlay->setEnabled(false);
        m_ui->actionStop->setEnabled(true);
        statusBar()->showMessage("Paused");
        break;
    case StoppedState:
        m_ui->actionPause->setChecked(false);
        m_ui->actionPause->setEnabled(false);
        m_ui->actionStop->setEnabled(false);
        m_ui->actionPlay->setEnabled(true);
        updateTimeLabel(0);
        m_ui->progressBar->setValue(0);
        statusBar()->showMessage("Stopped");
        break;
    default:
        statusBar()->showMessage("Not initialized");
        break;
    }
    m_state = newState;
}

void GUIPlayer::play()
{
    //qDebug() << Q_FUNC_INFO;
    if (!m_player->song()->isEmpty()) {
        if (m_player->getPosition() == 0) {
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
    //qDebug() << Q_FUNC_INFO;
    if (m_state == PlayingState || m_state == PausedState || m_player->isRunning()) {
        m_player->stop();
        m_playerThread.wait();
        m_player->resetPosition();
        updateState(StoppedState);
    }
}

void GUIPlayer::progressDialogInit(const QString& type, int max)
{
    m_pd = new QProgressDialog(0, 0, 0, max, this);
    m_pd->setWindowTitle(QString("Loading %1 file...").arg(type));
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
        } else if (ext == "mid" || ext == "midi" || ext == "kar") {
            progressDialogInit("MIDI", finfo.size());
        }
        m_player->loadFile(fileName);
        if (m_player->song()->isEmpty()) {
            m_ui->lblName->clear();
            updateState(EmptyState);
        } else {
            Settings::instance()->setLastDirectory(finfo.absolutePath());
            m_ui->lblName->setText(finfo.fileName());
            m_recentFiles->setCurrentFile(finfo.absoluteFilePath());
            updateState(StoppedState);
            updateTimeLabel(0);
            m_player->resetPosition();
            updateTempoLabel(m_player->currentBPM());
            m_ui->progressBar->setMaximum(m_player->song()->songLengthTicks());
            m_ui->progressBar->setValue(0);

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

            if ( Settings::instance()->getAutoPlay() ) {
                QTimer::singleShot(0, this, &GUIPlayer::play);
            }
        }
    }
}

void GUIPlayer::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,
          tr("Open MIDI File"), Settings::instance()->lastDirectory(),
          tr("All files (*.kar *.mid *.midi *.wrk);;"
          "Karaoke files (*.kar);;"
          "MIDI Files (*.mid *.midi);;"
          "Cakewalk files (*.wrk)") );
    if (!fileName.isEmpty() && isSupported(fileName)) {
        stop();
        openFile(fileName);
    }
}

void GUIPlayer::setup()
{
    m_connections->refresh();
    if (m_connections->exec() == QDialog::Accepted) {
        if (m_midiOut != 0) {
            m_midiOut->disconnect();
        }
        m_midiOut = m_connections->getOutput();
        m_player->setPort(m_midiOut);
        Settings::instance()->setAdvanced(m_connections->advanced());
        Settings::instance()->setLastOutputBackend(m_midiOut->backendName());
        Settings::instance()->setLastOutputConnection(m_midiOut->currentConnection().first);
    }
}

void GUIPlayer::applySettings()
{
    static QPalette defaultPalette = qApp->palette(); //(QColor(0xa0,0xa0,0xa0));
    static QPalette darkPalette(QColor(0x30,0x30,0x30));
#if defined(Q_OS_WINDOWS)
    m_snapper.SetEnabled(Settings::instance()->winSnap());
#endif
    qApp->setPalette( Settings::instance()->getDarkMode() ? darkPalette : defaultPalette );

    //m_ui->actionPianoPlayer->setIcon(IconUtils::GetIcon(":/resources/audio-midi.png"));
    m_ui->actionFileInfo->setIcon(IconUtils::GetIcon(":/resources/help-about.png"));
    m_ui->actionPlayList->setIcon(IconUtils::GetIcon(":/resources/view-media-playlist.png"));
    m_ui->actionAbout->setIcon(IconUtils::GetIcon(":/resources/help-about.png"));
    m_ui->actionAboutQt->setIcon(IconUtils::GetStandardIcon(QStyle::SP_TitleBarMenuButton));
    m_ui->actionOpen->setIcon(IconUtils::GetIcon(":/resources/document-open.png"));
    m_ui->actionQuit->setIcon(IconUtils::GetIcon(":/resources/system-shutdown.png"));
    m_ui->actionPlay->setIcon(IconUtils::GetIcon(":/resources/media-playback-start.png"));
    m_ui->actionStop->setIcon(IconUtils::GetIcon(":/resources/media-playback-stop.png"));
    m_ui->actionPause->setIcon(IconUtils::GetIcon(":/resources/media-playback-pause.png"));
    m_ui->actionPrev->setIcon(IconUtils::GetIcon(":/resources/media-skip-backward.png"));
    m_ui->actionNext->setIcon(IconUtils::GetIcon(":/resources/media-skip-forward.png"));
    m_ui->actionMIDISetup->setIcon(IconUtils::GetIcon(":/resources/midi.png"));
    m_ui->actionPreferences->setIcon(IconUtils::GetIcon(":/resources/settings.png"));
    m_ui->btnVolume->setIcon(IconUtils::GetIcon(":/resources/player-volume.png"));

    m_lyrics->applySettings();
    m_pianola->applySettings();
    m_channels->applySettings();
}

void GUIPlayer::preferences()
{
    if (m_preferences->exec() == QDialog::Accepted) {
        applySettings();
    }
}

void GUIPlayer::playerFinished()
{
    //qDebug() << Q_FUNC_INFO;
    m_player->resetPosition();
    updateTimeLabel(0);
    m_ui->progressBar->setValue(0);
    nextSong();
}

void GUIPlayer::playerStopped()
{
    //qDebug() << Q_FUNC_INFO;
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
    m_ui->progressBar->setValue(ticks);
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
    QString data = event->mimeData()->text();
    QString fileName = QUrl(data).path().trimmed();
    while (fileName.endsWith(QChar::Null)) fileName.chop(1);
    if ( isSupported(fileName) ) {
        stop();
        openFile(fileName);
        event->accept();
    } else {
        QMessageBox::warning(this, QSTR_APPNAME,
            QString("Dropped file %1 is not supported").arg(fileName));
    }
}

bool GUIPlayer::isSupported(QString fileName)
{
    return  fileName.endsWith(".mid", Qt::CaseInsensitive) ||
            fileName.endsWith(".midi", Qt::CaseInsensitive) ||
            fileName.endsWith(".kar", Qt::CaseInsensitive) ||
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

bool GUIPlayer::nativeEvent(const QByteArray &eventType, void *message, long *result)
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
        restoreGeometry(Settings::instance()->mainWindowGeometry());
        restoreState(Settings::instance()->mainWindowState());
        m_firstShown = false;
    }
}

void GUIPlayer::createLanguageMenu()
{
    QString currentLang = Settings::instance()->language();
    QActionGroup *languageGroup = new QActionGroup(this);
    languageGroup->setExclusive(true);
    connect(languageGroup, &QActionGroup::triggered, this, &GUIPlayer::slotSwitchLanguage);
    QDir dir(Settings::localeDirectory());
    QStringList fileNames = dir.entryList({"*.qm"}, QDir::NoFilter, QDir::NoSort);
    QStringList locales;
    locales << "en";
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
        QString localeName = loc == "en" ? QLocale::languageToString(qlocale.language()) : qlocale.nativeLanguageName();
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
    m_trq->load("qt_" + Settings::instance()->language(), Settings::systemLocales());
    m_trp->load("dmidiplayer_" + Settings::instance()->language(), Settings::localeDirectory());
    m_trl->load("drumstick-widgets_" + Settings::instance()->language(), Settings::drumstickLocales());
    Settings::instance()->retranslatePalettes();
    m_ui->retranslateUi(this);
    m_connections->retranslateUi();
    m_pianola->retranslateUi();
    m_channels->retranslateUi();
    m_lyrics->retranslateUi();
}

void GUIPlayer::slotSwitchLanguage(QAction *action)
{
    QString lang = action->data().toString();
    QLocale qlocale(lang);
    QString localeName = qlocale.nativeLanguageName();
    if ( QMessageBox::question (this, tr("Language Changed"),
            tr("The language for this application is going to change to %1. "
               "Do you want to continue?").arg(localeName),
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
        int mib = m_player->song()->detectedUchardetMIB();
        infostr = tr("<b>File:</b><br>%1<br>").arg(m_player->song()->currentFile());

        QString s = m_player->song()->getText(Sequence::Copyright, mib).join(tr("<br>"));
        if (!s.isEmpty())
            infostr += tr("<b>Copyright:</b>%1<br>").arg(s);

        s = m_player->song()->getText(Sequence::KarFileType, mib).join(tr("<br>"));
        if (!s.isEmpty())
            infostr += tr("<b>Karaoke type:</b>%1<br>").arg(s);

        s = m_player->song()->getText(Sequence::KarVersion, mib).join(tr("<br>"));
        if (!s.isEmpty())
            infostr += tr("<b>Karaoke version:</b>%1<br>").arg(s);

        s = m_player->song()->getText(Sequence::KarInformation, mib).join(tr("<br>"));
        if (!s.isEmpty())
            infostr += tr("<b>Karaoke info:</b>%1<br>").arg(s);

        s = m_player->song()->getText(Sequence::KarLanguage, mib).join(tr("<br>"));
        if (!s.isEmpty())
            infostr += tr("<b>Karaoke language:</b>%1<br>").arg(s);

        s = m_player->song()->getText(Sequence::KarTitles, mib).join(tr("<br>"));
        if (!s.isEmpty())
            infostr += tr("<b>Karaoke title:</b>%1<br>").arg(s);

        s = m_player->song()->getText(Sequence::KarWhatever, mib).join(tr("<br>"));
        if (!s.isEmpty())
            infostr += tr("<b>Karaoke things:</b>%1<br>").arg(s);
    }
    infostr.replace(QChar::LineSeparator, tr("<br>"));
    QMessageBox::information(this, tr("SMF Information"), infostr );
}

void GUIPlayer::slotPlayList()
{
    if (m_playList->exec() == QDialog::Accepted) {
        auto current = m_playList->currentItem();
        if (!current.isEmpty()) {
            if (m_state == PlayingState) {
                stop();
            }
            openFile(current);
        }
    }
}
