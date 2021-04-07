#include <QDebug>
#include <QPushButton>
#include <QShowEvent>
#include <QFontDialog>
#include <QColorDialog>
#include <drumstick/rtmidioutput.h>
#include "settings.h"
#include "prefsdialog.h"
#include "ui_prefsdialog.h"

PrefsDialog::PrefsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrefsDialog)
{
    ui->setupUi(this);
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
}

PrefsDialog::~PrefsDialog()
{
    delete ui;
}

void PrefsDialog::restoreDefaults()
{
   using namespace drumstick::rt;

    QColor futureColor = qApp->palette().color(QPalette::Text);
    QColor pastColor = Qt::gray;

    ui->chkSnapping->setChecked(true);
    ui->chkDarkMode->setChecked(false);
    ui->spinPercChannel->setValue(MIDI_GM_STD_DRUM_CHANNEL+1);
    ui->editTextFont->setText("Sans Serif,36");
    ui->editFutureColor->setText(futureColor.name(QColor::HexRgb));
    ui->editPastColor->setText(pastColor.name(QColor::HexRgb));

    ui->chkVelocityColor->setChecked(true);
    ui->cboHighlight->setCurrentIndex(0);
    ui->cboNoteNames->setCurrentIndex(0);
    ui->editNoteFont->setText("Sans Serif,50");
}

void PrefsDialog::slotFutureColor()
{
    QColor color = QColorDialog::getColor(Settings::instance()->getFutureColor(), this);
    if (color.isValid()) {
        ui->editFutureColor->setText(color.name(QColor::HexRgb));
    }
}

void PrefsDialog::slotPastColor()
{
    QColor color = QColorDialog::getColor(Settings::instance()->getPastColor(), this);
    if (color.isValid()) {
        ui->editPastColor->setText(color.name(QColor::HexRgb));
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

void PrefsDialog::accept()
{
    apply();
    QDialog::accept();
}

void PrefsDialog::showEvent ( QShowEvent *event )
{
    if (event->type() == QEvent::Show) {
//        qDebug() << Q_FUNC_INFO;
//        qDebug() << "lyrics:" << Settings::instance()->lyricsFont();
//        qDebug() << "notes:" << Settings::instance()->notesFont();

        ui->chkDarkMode->setChecked( Settings::instance()->getDarkMode() );
        ui->spinPercChannel->setValue( Settings::instance()->drumsChannel() );
#if defined(Q_OS_WINDOWS)
        ui->chkSnapping->setChecked( Settings::instance()->winSnap() );
#endif
        ui->editTextFont->setText( Settings::instance()->lyricsFont().toString() );
        ui->editFutureColor->setText( Settings::instance()->getFutureColor().name(QColor::HexRgb) );
        ui->editPastColor->setText( Settings::instance()->getPastColor().name(QColor::HexRgb) );

        ui->cboHighlight->clear();
        for(int i=0; i<Settings::instance()->availablePalettes(); ++i) {
            PianoPalette p = Settings::instance()->getPalette(i);
            if (p.isHighLight()) {
                qDebug() << "palette name:" << p.paletteName() << "id:" << p.paletteId();
                ui->cboHighlight->addItem(p.paletteName(), p.paletteId());
                if (p.paletteId() == Settings::instance()->highlightPaletteId()) {
                    ui->cboHighlight->setCurrentText(p.paletteName());
                }
            }
        }
        ui->chkVelocityColor->setChecked( Settings::instance()->velocityColor() );
        ui->editNoteFont->setText( Settings::instance()->notesFont().toString() );
        ui->cboNoteNames->setCurrentIndex(static_cast<int>(Settings::instance()->namesVisibility()));

        ui->tabWidget->setCurrentIndex(0);

//        qDebug() << Q_FUNC_INFO;
//        qDebug() << "lyrics:" << ui->editTextFont->text();
//        qDebug() << "notes:" << ui->editNoteFont->text();
    }
}

void PrefsDialog::apply()
{
//    qDebug() << Q_FUNC_INFO;
//    qDebug() << "lyrics:" << ui->editTextFont->text();
//    qDebug() << "notes:" << ui->editNoteFont->text();

    Settings::instance()->setDrumsChannel(ui->spinPercChannel->value());
    Settings::instance()->setDarkMode(ui->chkDarkMode->isChecked());
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
    QFont noteFont;
    if (noteFont.fromString(ui->editNoteFont->text())) {
        Settings::instance()->setNotesFont(noteFont);
    }
    Settings::instance()->setNamesVisibility(static_cast<LabelVisibility>(ui->cboNoteNames->currentIndex()));

//    qDebug() << Q_FUNC_INFO;
//    qDebug() << "lyrics:" << Settings::instance()->lyricsFont();
//    qDebug() << "notes:" << Settings::instance()->notesFont();
}
