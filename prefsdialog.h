#ifndef PREFSDIALOG_H
#define PREFSDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include <QStandardPaths>
#include <QStringList>
#include <QDir>

class MainWindow;

namespace Ui {
class PrefsDialog;
}

class PrefsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PrefsDialog(QWidget *parent = 0);
    ~PrefsDialog();

    QStringList myPrefs;

private slots:
    void on_btn_SavePrefs_clicked();
    void on_btn_getProjectRootDir_clicked();
    void on_btn_getGCCexefile_clicked();
    void on_btn_getGPPexefile_clicked();
    void on_btn_getMAKEexefile_clicked();
    void on_btn_getSTRIPexefile_clicked();
    void on_btn_getVCexefile_clicked();
    void on_btn_getVASMexefile_clicked();
    void on_btn_getVCconfigDir_clicked();
    void on_btn_getEmulatorExefile_clicked();
    void on_btn_getOS13Configfile_clicked();
    void on_btn_getOS20Configfile_clicked();
    void on_btn_getOS3Configfile_clicked();
    void on_btn_getOS4Configfile_clicked();

private:
    Ui::PrefsDialog *ui;
};

#endif // PREFSDIALOG_H
