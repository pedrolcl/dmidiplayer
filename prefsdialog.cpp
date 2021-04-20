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

//#include <QDebug>
#include <QPushButton>
#include <QShowEvent>
#include <QFontDialog>
#include <QColorDialog>
#include <drumstick/rtmidioutput.h>
#include "settings.h"
#include "iconutils.h"
#include "prefsdialog.h"
#include "ui_prefsdialog.h"

PrefsDialog::PrefsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrefsDialog)
{
    ui->setupUi(this);
    ui->tabWidget->setTabIcon(0, IconUtils::GetIcon("settings"));
    ui->tabWidget->setTabIcon(1, IconUtils::GetIcon("view-media-lyrics"));
    ui->tabWidget->setTabIcon(2, IconUtils::GetIcon("audio-midi"));
#if !defined(Q_OS_WINDOWS)
    ui->chkSnapping->setVisible(false);
#endif
    restoreDefaults();
    QPushButton *btnDefaults = ui->buttonBox->button(QDialogButtonBox::RestoreDefaults);
    connect(btnDefaults, &QPushButton::clicked, this, &PrefsDialog::restoreDefaults);
    connect(ui->btnFutureColor, &QToolButton::clicked, this, &PrefsDialog::slotFutureColor);
    connect(ui->btnPastColor, &QToolButton::clicked, this, &PrefsDialog::slotPastColor);
    connect(ui->btnNoteFont, &QToolButton::clicked, this, &PrefsDialog::slotNotesFont);
    connect(ui->btnTextFont, &QToolButton::clicked, this, &PrefsDialog::slotLyricsFont);
    connect(ui->btnSingleColor, &QToolButton::clicked, this, &PrefsDialog::slotSingleColor);
    connect(ui->chkDarkMode, &QCheckBox::toggled, this, &PrefsDialog::darkModeChanged);
    connect(ui->chkInternalIcons, &QCheckBox::toggled, this, &PrefsDialog::iconThemeChanged);
}

PrefsDialog::~PrefsDialog()
{
    delete ui;
}

void PrefsDialog::restoreDefaults()
{
   using namespace drumstick::rt;

    QColor futureColor = qApp->palette().color(QPalette::WindowText);
    QColor pastColor = QColor(Qt::gray);

    ui->chkSnapping->setChecked(true);
    ui->chkDarkMode->setChecked(false);
    ui->spinPercChannel->setValue(MIDI_GM_STD_DRUM_CHANNEL+1);
    ui->editTextFont->setText("Sans Serif,36");
    setFutureColor(futureColor);
    setPastColor(pastColor);
    ui->chkAutoPlay->setChecked(true);

    ui->chkVelocityColor->setChecked(true);
    ui->cboHighlight->setCurrentIndex(0);
    ui->cboNoteNames->setCurrentIndex(0);
    ui->editNoteFont->setText("Sans Serif,50");

    PianoPalette p = Settings::instance()->getPalette(PAL_SINGLE);
    setSingleColor(p.getColor(0));
}

void PrefsDialog::slotFutureColor()
{
    QColor color = QColorDialog::getColor(Settings::instance()->getFutureColor(), this);
    if (color.isValid()) {
        setFutureColor(color);
    }
}

void PrefsDialog::slotPastColor()
{
    QColor color = QColorDialog::getColor(Settings::instance()->getPastColor(), this);
    if (color.isValid()) {
        setPastColor(color);
    }
}

void PrefsDialog::slotLyricsFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, Settings::instance()->lyricsFont(), this);
    if (ok) {
        ui->editTextFont->setText(font.toString());
    }
}

void PrefsDialog::slotNotesFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(&ok, Settings::instance()->notesFont(), this);
    if (ok) {
        ui->editNoteFont->setText(font.toString());
    }
}

void PrefsDialog::slotSingleColor()
{
    QColor color = QColorDialog::getColor(Settings::instance()->getSingleColor(), this);
    if (color.isValid()) {
        setSingleColor(color);
    }
}

void PrefsDialog::darkModeChanged(bool state)
{
    QColor future = QColor( state ? Qt::white : qApp->palette().color(QPalette::WindowText) );
    setFutureColor(future);
}

void PrefsDialog::iconThemeChanged(bool state)
{
    Settings::instance()->setInternalIcons(state);
    ui->tabWidget->setTabIcon(0, IconUtils::GetIcon("settings"));
    ui->tabWidget->setTabIcon(1, IconUtils::GetIcon("view-media-lyrics"));
    ui->tabWidget->setTabIcon(2, IconUtils::GetIcon("audio-midi"));
}

void PrefsDialog::accept()
{
    apply();
    QDialog::accept();
}

void PrefsDialog::showEvent ( QShowEvent *event )
{
    if (event->type() == QEvent::Show) {

        ui->chkDarkMode->setChecked( Settings::instance()->getDarkMode() );
        ui->chkInternalIcons->setChecked( Settings::instance()->useInternalIcons() );
        ui->spinPercChannel->setValue( Settings::instance()->drumsChannel() );
        ui->chkAutoPlay->setChecked( Settings::instance()->getAutoPlay() );
#if defined(Q_OS_WINDOWS)
        ui->chkSnapping->setChecked( Settings::instance()->winSnap() );
#endif
        ui->editTextFont->setText( Settings::instance()->lyricsFont().toString() );
        setFutureColor(Settings::instance()->getFutureColor());
        setPastColor( Settings::instance()->getPastColor());

        ui->cboHighlight->clear();
        for(int i=0; i<Settings::instance()->availablePalettes(); ++i) {
            PianoPalette p = Settings::instance()->getPalette(i);
            if (p.isHighLight()) {
                //qDebug() << "palette name:" << p.paletteName() << "id:" << p.paletteId();
                ui->cboHighlight->addItem(p.paletteName(), p.paletteId());
                if (p.paletteId() == Settings::instance()->highlightPaletteId()) {
                    ui->cboHighlight->setCurrentText(p.paletteName());
                }
            }
        }
        ui->chkVelocityColor->setChecked( Settings::instance()->velocityColor() );
        ui->editNoteFont->setText( Settings::instance()->notesFont().toString() );
        ui->cboNoteNames->setCurrentIndex(static_cast<int>(Settings::instance()->namesVisibility()));
        setSingleColor( Settings::instance()->getSingleColor() );

        ui->tabWidget->setCurrentIndex(0);
    }
}

void PrefsDialog::apply()
{
    Settings::instance()->setDrumsChannel(ui->spinPercChannel->value());
    Settings::instance()->setDarkMode(ui->chkDarkMode->isChecked());
    Settings::instance()->setAutoPlay(ui->chkAutoPlay->isChecked());
    Settings::instance()->setInternalIcons(ui->chkInternalIcons->isChecked());
#if defined(Q_OS_WINDOWS)
    Settings::instance()->setWinSnap( ui->chkSnapping->isChecked() );
#endif
    QFont textFont;
    if (textFont.fromString(ui->editTextFont->text())) {
        Settings::instance()->setLyricsFont(textFont);
    }
    Settings::instance()->setFutureColor(ui->editFutureColor->text());
    Settings::instance()->setPastColor(ui->editPastColor->text());

    Settings::instance()->setHighlightPaletteId(ui->cboHighlight->currentData().toInt());
    Settings::instance()->setVelocityColor(ui->chkVelocityColor->isChecked());
    Settings::instance()->setSingleColor(ui->editSingle->text());
    QFont noteFont;
    if (noteFont.fromString(ui->editNoteFont->text())) {
        Settings::instance()->setNotesFont(noteFont);
    }
    Settings::instance()->setNamesVisibility(static_cast<LabelVisibility>(ui->cboNoteNames->currentIndex()));
}

void PrefsDialog::setFutureColor(QColor c)
{
    int x = ui->lblFuture->height();
    QPixmap p(x, x);
    p.fill(c);
    ui->lblFuture->setPixmap(p);
    ui->editFutureColor->setText(c.name(QColor::HexRgb));
}

void PrefsDialog::setPastColor(QColor c)
{
    int x = ui->lblPast->height();
    QPixmap p(x, x);
    p.fill(c);
    ui->lblPast->setPixmap(p);
    ui->editPastColor->setText(c.name(QColor::HexRgb));
}

void PrefsDialog::setSingleColor(QColor c)
{
    int x = ui->lblSingle->height();
    QPixmap p(x, x);
    p.fill(c);
    ui->lblSingle->setPixmap(p);
    ui->editSingle->setText(c.name(QColor::HexRgb));
}
