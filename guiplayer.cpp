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

using namespace drumstick::rt;
using namespace drumstick::widgets;

#if defined(Q_OS_LINUX)
    const QString defaultBackend = QStringLiteral("SonivoxEAS");
    const QString defaultConn = QStringLiteral("SonivoxEAS");
#elif defined(Q_OS_OSX)
    const QString defaultBackend = QStringLiteral("DLS Synth");
    const QString defaultConn = QStringLiteral("DLS Synth");
#elif defined(Q_OS_WIN)
    const QString defaultBackend = QStringLiteral("Windows MM");
    const QString defaultConn = QStringLiteral("Microsoft GS Wavetable Synth");
#else
    const QString defaultBackend = QStringLiteral("Network");
    const QString defaultConn = QStringLiteral("21928");
#endif

GUIPlayer::GUIPlayer(QWidget *parent, Qt::WindowFlags flags)
    : QMainWindow(parent, flags),
    m_state(InvalidState),
    m_midiOut(nullptr),
    m_player(nullptr),
    m_ui(new Ui::GUIPlayerClass),
    m_pd(nullptr)
{
	m_ui->setupUi(this);
	setAcceptDrops(true);
    connect(m_ui->actionAbout, &QAction::triggered, this, &GUIPlayer::about);
    connect(m_ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(m_ui->actionPlay, &QAction::triggered, this, &GUIPlayer::play);
    connect(m_ui->actionPause, &QAction::triggered, this, &GUIPlayer::pause);
    connect(m_ui->actionStop, &QAction::triggered, this, &GUIPlayer::stop);
    connect(m_ui->actionOpen, &QAction::triggered, this, &GUIPlayer::open);
    connect(m_ui->actionMIDISetup, &QAction::triggered, this, &GUIPlayer::setup);
    connect(m_ui->actionQuit, &QAction::triggered, this, &GUIPlayer::quit);
    connect(m_ui->btnTempo, &QPushButton::clicked, this, &GUIPlayer::tempoReset);
    connect(m_ui->btnVolume, &QPushButton::clicked, this, &GUIPlayer::volumeReset);
    connect(m_ui->sliderTempo, &QSlider::valueChanged, this, &GUIPlayer::tempoSlider);
    connect(m_ui->volumeSlider, &QSlider::valueChanged, this, &GUIPlayer::volumeSlider);
    connect(m_ui->spinPitch, QOverload<int>::of(&QSpinBox::valueChanged), this, &GUIPlayer::pitchShift);
    connect(m_ui->toolBar->toggleViewAction(), &QAction::toggled,
            m_ui->actionShowToolbar, &QAction::setChecked);
    connect(m_ui->actionPianoPlayer, &QAction::toggled, this, &GUIPlayer::slotShowPianola);
    connect(m_ui->actionChannels, &QAction::toggled, this, &GUIPlayer::slotShowChannels);

    m_ui->actionPlay->setIcon(QIcon(IconUtils::GetPixmap(this, ":/resources/play.png")));
    m_ui->actionPlay->setShortcut( Qt::Key_MediaPlay );
    m_ui->actionStop->setIcon(QIcon(IconUtils::GetPixmap(this, ":/resources/stop.png")));
    m_ui->actionStop->setShortcut( Qt::Key_MediaStop );
    m_ui->actionPause->setIcon(QIcon(IconUtils::GetPixmap(this, ":/resources/pause.png")));
    m_ui->actionMIDISetup->setIcon(QIcon(IconUtils::GetPixmap(this, ":/resources/setup.png")));
    SettingsFactory settings;
    readSettings(*settings.getQSettings());

    settings->beginGroup(BackendManager::QSTR_DRUMSTICKRT_GROUP);
    settings->setValue(BackendManager::QSTR_DRUMSTICKRT_PUBLICNAMEOUT, QLatin1String("MIDIPlayerOUT"));
    settings->setValue(BackendManager::QSTR_DRUMSTICKRT_PUBLICNAMEIN, QLatin1String("MIDIPlayerIN"));
    settings->endGroup();
    settings->sync();

    m_player = new SequencePlayer();
    m_player->moveToThread(&m_playerThread);
    connect(m_player, &SequencePlayer::songFinished, this, &GUIPlayer::playerFinished);
    connect(m_player, &SequencePlayer::songStopped, this, &GUIPlayer::playerStopped);
    connect(m_player, &SequencePlayer::songEchoTime, this, &GUIPlayer::playerEcho);
    connect(&m_playerThread, &QThread::started, m_player, &SequencePlayer::playerLoop);

    m_pianola = new Pianola(this);
    connect(m_pianola, &Pianola::closed, this, &GUIPlayer::slotPianolaClosed);
    connect(m_player, &SequencePlayer::midiNoteOn, m_pianola.data(), &Pianola::slotNoteOn);
    connect(m_player, &SequencePlayer::midiNoteOff, m_pianola.data(), &Pianola::slotNoteOff);

    m_channels = new Channels(this);
    connect(m_channels.data(), &Channels::closed, this, &GUIPlayer::slotChannelsClosed);
    connect(m_player, &SequencePlayer::midiNoteOn, m_channels.data(), &Channels::slotNoteOn);
    connect(m_player, &SequencePlayer::midiNoteOff, m_channels.data(), &Channels::slotNoteOff);
    connect(m_player, &SequencePlayer::midiProgram, m_channels.data(), &Channels::slotPatch);
    connect(m_channels.data(), &Channels::name, m_pianola.data(), &Pianola::slotLabel);

    connect(m_channels.data(), &Channels::mute, m_player, &SequencePlayer::setMuted);
    connect(m_channels.data(), &Channels::volume, m_player, &SequencePlayer::setVolume);
    connect(m_channels.data(), &Channels::lock, m_player, &SequencePlayer::setLocked);
    connect(m_channels.data(), &Channels::patch, m_player, &SequencePlayer::setPatch);

    try {
        BackendManager man;
        man.refresh(settings.getQSettings());
        QList<MIDIOutput*> outputs = man.availableOutputs();

        findOutput(m_lastOutputBackend, outputs);
        if (m_midiOut == 0) {
            findOutput(defaultBackend, outputs);
        }
        if (m_midiOut == 0) {
            qFatal("Can't find a suitable MIDI out port.");
        }

        dlgConnections.setOutputs(outputs);
        dlgConnections.setOutput(m_midiOut);
        dlgConnections.setAdvanced(m_advanced);

        if (m_midiOut != 0 && !m_lastOutputConnection.isEmpty()) {
            auto outConnections = m_midiOut->connections(m_advanced);
            MIDIConnection conn = outConnections.first();
            foreach(const auto& c, outConnections) {
                if (c.first == m_lastOutputConnection) {
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

void GUIPlayer::findOutput(QString name, QList<MIDIOutput *> &outputs)
{
    foreach(MIDIOutput* output, outputs) {
        //qDebug() << Q_FUNC_INFO << output->backendName();
        if (m_midiOut == 0 && (output->backendName() == name))  {
            m_midiOut = output;
            break;
        }
    }
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
        play();
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
            m_lastDirectory = finfo.absolutePath();
            m_ui->lblName->setText(finfo.fileName());
            updateState(StoppedState);
            updateTimeLabel(0);
            m_player->resetPosition();
            updateTempoLabel(m_player->currentBPM());
            m_ui->progressBar->setMaximum(m_player->song()->songLengthTicks());
            m_ui->progressBar->setValue(0);

            if (m_pianola != 0) {
                int loNote = m_player->song()->lowestMidiNote();
                int hiNote = m_player->song()->highestMidiNote();
                m_pianola->setNoteRange(loNote, hiNote);
                for(int i = 0; i < MIDI_STD_CHANNELS; ++i ) {
                    m_pianola->enableChannel(i, m_player->song()->channelUsed(i));
                    m_pianola->slotLabel(i, m_player->song()->channelLabel(i));
                }
            }
            if (m_channels != 0) {
                for(int i = 0; i < MIDI_STD_CHANNELS; ++i ) {
                    m_player->setLocked(i, false);
                    m_player->setMuted(i, false);
                    m_channels->setLockChannel(i, false);
                    m_channels->setSoloChannel(i, false);
                    m_channels->setMuteChannel(i, false);
                    m_channels->enableChannel(i, m_player->song()->channelUsed(i));
                    m_channels->setChannelName(i, m_player->song()->channelLabel(i));
                }
            }
        }
    }
}

void GUIPlayer::open()
{
    QString fileName = QFileDialog::getOpenFileName(this,
          "Open MIDI File", m_lastDirectory,
          "All files (*.kar *.mid *.midi *.wrk);;"
          "Karaoke files (*.kar);;"
          "MIDI Files (*.mid *.midi);;"
          "Cakewalk files (*.wrk)" );
    if (!fileName.isEmpty() && isSupported(fileName)) {
        stop();
        openFile(fileName);
    }
}

void GUIPlayer::setup()
{
    dlgConnections.refresh();
    if (dlgConnections.exec() == QDialog::Accepted) {
        if (m_midiOut != 0) {
            m_midiOut->disconnect();
        }
        m_midiOut = dlgConnections.getOutput();
        m_advanced = dlgConnections.advanced();
        m_player->setPort(m_midiOut);
    }
}

void GUIPlayer::playerFinished()
{
    //qDebug() << Q_FUNC_INFO;
    m_player->resetPosition();
    updateTimeLabel(0);
    m_ui->progressBar->setValue(0);
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
    QString tip = QString("%1%%").arg(factor*100.0, 0, 'f', 0);
    m_ui->sliderTempo->setToolTip(tip);
    QToolTip::showText(QCursor::pos(), tip, this);
}

void GUIPlayer::volumeSlider(int value)
{
    QString tip = QString::number(value)+'%';
    m_ui->lblVolume->setText(tip);
    m_ui->volumeSlider->setToolTip(tip);
    m_player->setVolumeFactor(value);
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
    m_lastOutputBackend = driver;
    m_lastOutputConnection = connection;
}

void GUIPlayer::readSettings(QSettings &settings)
{
    settings.beginGroup("Window");
    restoreGeometry(settings.value("Geometry").toByteArray());
    restoreState(settings.value("State").toByteArray());
    settings.endGroup();

    settings.beginGroup("Connections");
    m_lastOutputBackend = settings.value("outputBackend", defaultBackend).toString();
    m_lastOutputConnection = settings.value("outputConnection", defaultConn).toString();
    m_advanced = settings.value("advanced", false).toBool();
    settings.endGroup();

    settings.beginGroup("Preferences");
    m_lastDirectory = settings.value("LastDirectory").toString();
    settings.endGroup();
}

void GUIPlayer::writeSettings(QSettings &settings)
{
    settings.beginGroup("Window");
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("State", saveState());
    settings.endGroup();

    settings.beginGroup("Connections");
    if (m_midiOut != nullptr) {
        settings.setValue("outputBackend", m_midiOut->backendName());
        settings.setValue("outputConnection", m_midiOut->currentConnection().first);
    }
    settings.setValue("advanced", m_advanced);
    settings.endGroup();

    settings.beginGroup("Preferences");
    settings.setValue("LastDirectory", m_lastDirectory);
    settings.endGroup();

    settings.sync();
}

void GUIPlayer::closeEvent( QCloseEvent *event )
{
    SettingsFactory settings;
    writeSettings(*settings.getQSettings());
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
    if (m_pianola != 0) {
        m_pianola->setVisible(checked);
    }
}

void GUIPlayer::slotPianolaClosed()
{
    m_ui->actionPianoPlayer->setChecked(false);
}

void GUIPlayer::slotShowChannels(bool checked)
{
    if (m_channels != 0) {
        m_channels->setVisible(checked);
    }
}

void GUIPlayer::slotChannelsClosed()
{
    m_ui->actionChannels->setChecked(false);
}
