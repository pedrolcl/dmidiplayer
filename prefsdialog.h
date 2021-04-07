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

public slots:
    void restoreDefaults();
    void slotFutureColor();
    void slotPastColor();
    void slotLyricsFont();
    void slotNotesFont();
    void accept() override;

protected:
    void showEvent ( QShowEvent *event ) override;

private:
    Ui::PrefsDialog *ui;
};

#endif // PREFSDIALOG_H
