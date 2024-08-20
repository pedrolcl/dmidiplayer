/*
    Drumstick MIDI File Player Multiplatform Program
    Copyright (C) 2006-2024, Pedro Lopez-Cabanillas <plcl@users.sf.net>

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

#ifndef PREFSDIALOG_H
#define PREFSDIALOG_H

#include <QDialog>

namespace Ui {
class PrefsDialog;
}

class PrefsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrefsDialog(QWidget *parent = nullptr);
    ~PrefsDialog();
    void apply();
    void setFutureColor(QColor c);
    void setPastColor(QColor c);
    void setSingleColor(QColor c);
    void retranslateUi();

public slots:
    void restoreDefaults();
    void slotFutureColor();
    void slotPastColor();
    void slotLyricsFont();
    void slotNotesFont();
    void slotSingleColor();
    void darkModeChanged(bool state);
    void iconThemeChanged(bool state);
    void accept() override;

protected:
    void showEvent ( QShowEvent *event ) override;

private:
    Ui::PrefsDialog *ui;
};

#endif // PREFSDIALOG_H
