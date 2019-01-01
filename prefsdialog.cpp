#include "prefsdialog.h"
#include "ui_prefsdialog.h"
#include "mainwindow.h"

PrefsDialog::PrefsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PrefsDialog)
{
    ui->setupUi(this);
    // load prefs from file (if exists!)
    QString Meldung = getPrefs();
}

PrefsDialog::~PrefsDialog()
{
    delete ui;
}

void PrefsDialog::on_btn_SavePrefs_clicked()
{
    // Collect values, store them in $HOMEDIR/.amigaed/ace.prefs
    // TAB: Project
    myPrefs << ui->lineEdit_author->text() << ui->lineEdit_email->text() << ui->lineEdit_website->text() << ui->lineEdit_description->text() << ui->lineEdit_purpose->text() << ui->lineEdit_projectsRootDir->text();
    // TAB: GCC
    myPrefs << ui->lineEdit_getGCCexefile->text() << ui->lineEdit_getGPPexefile->text() << ui->lineEdit_getMAKEexefile->text() << ui->lineEdit_getSTRIPexefile->text() << ui->lineEdit_GCCdefaultOpts->text();
    // TAB: VBCC
    myPrefs << ui->lineEdit_getVCexefile->text() << ui->lineEdit_getVASMexefile->text() << ui->lineEdit_getVCconfigDir->text() << ui->lineEdit_VCdefaultOpts->text();
    // TAB: Emulator
    myPrefs << ui->lineEdit_getEmulatorExefile->text() << ui->lineEdit_getOS13Configfile->text() << ui->lineEdit_getOS20Configfile->text() << ui->lineEdit_getOS3Configfile->text() << ui->lineEdit_getOS4Configfile->text() << ui->comboBox_defaultEmulator->currentText();
    // TAB: Misc
    // no nothing up to now.

     //Debug:
     for (int i = 0; i < myPrefs.size(); ++i)
             qDebug() << myPrefs.at(i).toLocal8Bit().constData();

    QString str = myPrefs.join(", ");

    // Construct file path to store values:
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
    // TODO: save values to $HOMEDIR/.amigaed/ace.prefs
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

   this->close();  // quit PrefsDialog

   QMessageBox::information(this, tr("Amiga Cross Editor"),
                       tr("Prefs saved!\n"
                          "Please restart Amiga Cross Editor to activate changes!"),
                            QMessageBox::Ok);
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

QString PrefsDialog::getPrefs()
{
    QString line;
    QStringList fields;
    // Construct file path to store values:
    QString filename = QDir::homePath();
    filename.append(QDir::separator());
    filename.append(".amigaed");
    filename.append(QDir::separator());
    filename.append("ace.prefs");

    QFile file(filename);

    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "Amiga Cross Editor Prefs", "Error while opening prefs file: \n" + file.errorString() + "\n\nPlease edit and save prefs first and\nthen restart Amiga Cross Editor!");
        return "Fuck you!";
    }
    else
    {
        QTextStream in(&file);
        while(!in.atEnd())
        {
            line = in.readLine();
            fields = line.split(", ");
        }

        file.close();

        // TAB: Project
        ui->lineEdit_author->setText(fields[0]);
        ui->lineEdit_email->setText(fields[1]);
        ui->lineEdit_website->setText(fields[2]);
        ui->lineEdit_description->setText(fields[3]);
        ui->lineEdit_purpose->setText(fields[4]);
        ui->lineEdit_projectsRootDir->setText(fields[5]);
        // TAB: GCC
        ui->lineEdit_getGCCexefile->setText(fields[6]);
        ui->lineEdit_getGPPexefile->setText(fields[7]);
        ui->lineEdit_getMAKEexefile->setText(fields[8]);
        ui->lineEdit_getSTRIPexefile->setText(fields[9]);
        ui->lineEdit_GCCdefaultOpts->setText(fields[10]);
        // TAB: VBCC
        ui->lineEdit_getVCexefile->setText(fields[11]);
        ui->lineEdit_getVASMexefile->setText(fields[12]);
        ui->lineEdit_getVCconfigDir->setText(fields[13]);
        ui->lineEdit_VCdefaultOpts->setText(fields[14]);
        // TAB: Emulator
        ui->lineEdit_getEmulatorExefile->setText(fields[15]);
        ui->lineEdit_getOS13Configfile->setText(fields[16]);
        ui->lineEdit_getOS20Configfile->setText(fields[17]);
        ui->lineEdit_getOS3Configfile->setText(fields[18]);
        ui->lineEdit_getOS4Configfile->setText(fields[19]);
        ui->comboBox_defaultEmulator->setCurrentText(fields[20]);
    }

    return line;
}


void PrefsDialog::on_btn_CancelSave_clicked()
{
    this->close();
}
