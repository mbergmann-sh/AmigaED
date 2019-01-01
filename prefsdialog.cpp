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
    QStringList homePath = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    qDebug() << "homedir: " << homePath.first().split(QDir::separator()).last();
    qDebug() << "save prefs clicked";
    // TODO: Collect values, store them in MainWindow global vars

    // TODO: save values to $HOMEDIR/.amigaed/ace.prefs

    myPrefs << ui->lineEdit_getOS13Configfile->text() << ui->lineEdit_getOS20Configfile->text() << ui->lineEdit_getOS3Configfile->text() << ui->lineEdit_getOS4Configfile->text();

                 //Debug:
                 for (int i = 0; i < myPrefs.size(); ++i)
                         qDebug() << myPrefs.at(i).toLocal8Bit().constData();

                QString str = myPrefs.join(", ");

                QString filename = QDir::homePath();
                filename.append(QDir::separator());
                filename.append(".amigaed");
                // check if the folder exists!
                if(!(QDir(filename).exists()))
                {
                    qDebug() << "Folder dows not exist!";
                    QDir dir(filename);
                    dir.mkdir(filename);
                }
                filename.append(QDir::separator());
                filename.append("ace.prefs");

                qDebug() << " prefs dir: " << filename;
                    QFile file(filename);
                    if (file.open(QIODevice::ReadWrite))
                    {
                        QTextStream stream(&file);
                        stream << str << endl;
                    }
                    else
                    {
                        qDebug() << "Dateifehler!";
                    }

               this->close();  // Dialog verlassen

               QMessageBox::information(this, tr("BasicBoss"),
                                   tr("Einstellungen gespeichert\n"
                                      "Bitte starten Sie das Programm neu."),
                                        QMessageBox::Ok);

    // quit dialog when everything is done
    this->close();

}

void PrefsDialog::on_btn_getProjectRootDir_clicked()
{
    // getDir dialog
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    ui->lineEdit_projectsRootDir->setText(dir);


}

void PrefsDialog::on_btn_getGCCexefile_clicked()
{
    QString exestring;
#if defined(__unix__)
    exestring = "/opt/amiga/bin/m68k-amigaos-gcc";
#else
    exestring = "m68k-amigaos-gcc.exe";
#endif
    // ToDO: FIX for executable without file extension!
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Path to gcc"), exestring,
            tr("All Files (*);;Executable (*.exe)"));
    ui->lineEdit_getGCCexefile->setText(fileName);

}

void PrefsDialog::on_btn_getGPPexefile_clicked()
{
    QString exestring;
#if defined(__unix__)
    exestring = "/opt/amiga/bin/m68k-amigaos-g++";
#else
    exestring = "m68k-amigaos-g++.exe";
#endif
    // ToDO: FIX for executable without file extension!
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Path to g++"), exestring,
            tr("All Files (*);;Executable (*.exe)"));
    ui->lineEdit_getGPPexefile->setText(fileName);
}

void PrefsDialog::on_btn_getMAKEexefile_clicked()
{
    QString exestring;
#if defined(__unix__)
    exestring = "/usr/bin/make";
#else
    exestring = "m68k-amigaos-make.exe";
#endif
    // ToDO: FIX for executable without file extension!
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Path to make"), exestring,
            tr("All Files (*);;Executable (*.exe)"));
    ui->lineEdit_getMAKEexefile->setText(fileName);
}

void PrefsDialog::on_btn_getSTRIPexefile_clicked()
{
    QString exestring;
#if defined(__unix__)
    exestring = "/opt/amiga/bin/m68k-amigaos-strip";
#else
    exestring = "m68k-amigaos-strip.exe";
#endif
    // ToDO: FIX for executable without file extension!
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Path to strip"), exestring,
            tr("All Files (*);;Executable (*.exe)"));
    ui->lineEdit_getSTRIPexefile->setText(fileName);
}

void PrefsDialog::on_btn_getVCexefile_clicked()
{
    QString exestring;
#if defined(__unix__)
    exestring = "/opt/amiga/bin/vc";
#else
    exestring = "vc";
#endif
    // ToDO: FIX for executable without file extension!
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Path to vc"), exestring,
            tr("All Files (*);;Executable (*.exe)"));
    ui->lineEdit_getVCexefile->setText(fileName);
}

void PrefsDialog::on_btn_getVASMexefile_clicked()
{
    QString exestring;
#if defined(__unix__)
    exestring = "/opt/amiga/bin/vasmm68k_mot";
#else
    exestring = "vasmm68k_mot.exe";
#endif
    // ToDO: FIX for executable without file extension!
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Path to vasm"), exestring,
            tr("All Files (*);;Executable (*.exe)"));
    ui->lineEdit_getVASMexefile->setText(fileName);
}

void PrefsDialog::on_btn_getVCconfigDir_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open VBCC config Directory"),
                                                 "/etc/",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    ui->lineEdit_getVCconfigDir->setText(dir);
}

void PrefsDialog::on_btn_getEmulatorExefile_clicked()
{
    QString exestring;
#if defined(__unix__)
    exestring = "/usr/bin/fs-uae";
#else
    exestring = "winUAE";
#endif
    // ToDO: FIX for executable without file extension!
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Path to Amiga emulator"), exestring,
            tr("All Files (*);;Executable (*.exe)"));
    ui->lineEdit_getEmulatorExefile->setText(fileName);
}


void PrefsDialog::on_btn_getOS13Configfile_clicked()
{
    QString exestring;
#if defined(__unix__)
    exestring = "/home";
#else
    exestring = "/home";
#endif
    // ToDO: FIX for executable without file extension!
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Path to AmigaOS 1.3 config file"), exestring,
            tr("All Files (*);;Executable (*.exe)"));
    ui->lineEdit_getOS13Configfile->setText(fileName);
}

void PrefsDialog::on_btn_getOS20Configfile_clicked()
{
    QString exestring;
#if defined(__unix__)
    exestring = "/home";
#else
    exestring = "/home";
#endif
    // ToDO: FIX for executable without file extension!
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Path to AmigaOS 2.0 config file"), exestring,
            tr("All Files (*);;Executable (*.exe)"));
    ui->lineEdit_getOS20Configfile->setText(fileName);
}

void PrefsDialog::on_btn_getOS3Configfile_clicked()
{
    QString exestring;
#if defined(__unix__)
    exestring = "/home";
#else
    exestring = "/home";
#endif
    // ToDO: FIX for executable without file extension!
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Path to AmigaOS 3.x config file"), exestring,
            tr("All Files (*);;Executable (*.exe)"));
    ui->lineEdit_getOS3Configfile->setText(fileName);
}

void PrefsDialog::on_btn_getOS4Configfile_clicked()
{
    QString exestring;
#if defined(__unix__)
    exestring = "/home";
#else
    exestring = "/home";
#endif
    // ToDO: FIX for executable without file extension!
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Path to AmigaOS 4.x config file"), exestring,
            tr("All Files (*);;Executable (*.exe)"));
    ui->lineEdit_getOS4Configfile->setText(fileName);
}
