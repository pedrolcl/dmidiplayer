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

#ifndef LYRICS_H
#define LYRICS_H

#include <QMainWindow>
#include <QObject>
#include <QMap>
#include <QTextCodec>
#include "framelesswindow.h"

class QGridLayout;
class QFrame;
class QHBoxLayout;
class QLabel;
class QComboBox;
class QToolButton;
class QTextEdit;
class Sequence;

class Lyrics : public FramelessWindow
{
    Q_OBJECT
public:
    explicit Lyrics(QWidget *parent = nullptr);
    void readSettings();
    void writeSettings();
    void retranslateUi();
    void initSong( Sequence *song );
    void applySettings() override;

public slots:
    void slotMidiText(const int track, const int type, int ticks, const QByteArray &text);
    void trackChanged(int index);
    void typeChanged(int index);
    void codecChanged(int index);
    void changeFont();
    void displayText();
    void toggleFullScreen(bool enabled);
    void slotCopy();
    void slotSave();
    void slotPrint();

signals:
    void closed();

protected:
    void showEvent(QShowEvent *event) override;
    void closeEvent( QCloseEvent *event)  override;

private: // methods
    void populateCodecsCombo();
    void populateTracksCombo();
    QString sanitizeText(const QByteArray& text);

private: // member vars
    int m_track;
    int m_mib;
    int m_type;
    Sequence *m_song;
    QTextCodec *m_codec;
    QColor m_normalColor;
    QColor m_otherColor;
    QAction *m_actionCopy;
    QAction *m_actionSave;
    QAction *m_actionPrint;
    QAction *m_actionFont;
    QAction *m_actionFullScreen;
    QLabel *m_label1;
    QComboBox *m_comboTrack;
    QLabel *m_label2;
    QComboBox *m_comboType;
    QLabel *m_label3;
    QComboBox *m_comboCodec;
    QToolButton *m_toolButton;
    QTextEdit *m_textViewer;
    QMenu *m_chmenu;
    QMap<int,int> m_textPos;
};

#endif // LYRICS_H
