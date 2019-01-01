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
    explicit PrefsDialog(QWidget *parent = 0);
    ~PrefsDialog();

private slots:
    void on_btn_SavePrefs_clicked();

private:
    Ui::PrefsDialog *ui;
};

#endif // PREFSDIALOG_H
