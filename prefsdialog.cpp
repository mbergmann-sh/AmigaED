#include "prefsdialog.h"
#include "ui_prefsdialog.h"
#include "mainwindow.h"

PrefsDialog::PrefsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrefsDialog)
{
    ui->setupUi(this);
}

PrefsDialog::~PrefsDialog()
{
    delete ui;
}

void PrefsDialog::on_btn_SavePrefs_clicked()
{
    qDebug() << "save prefs clicked";
    // TODO: Collect values, store them in MainWindow global vars

    // TODO: save values to $HOMEDIR/.amigaed/ace.prefs

    // quit dialog when everything is done
    this->close();

}
