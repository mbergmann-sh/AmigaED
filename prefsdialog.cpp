#include "prefsdialog.h"
#include "ui_prefsdialog.h"
#include "mainwindow.h"

PrefsDialog::PrefsDialog(QWidget *parent, int tabindex) :
    QDialog(parent),
    ui(new Ui::PrefsDialog)
{
    ui->setupUi(this);
    // Remove Close-, help-, size gadgets
    this->setWindowFlags(Qt::Dialog | Qt::Desktop | Qt::Window | Qt::WindowTitleHint | Qt::CustomizeWindowHint);

    // Start Tabwidget with first tab visible allways
    ui->tabWidget->setCurrentIndex(tabindex);

    // set items for default style combobox:
    p_style_items << QStyleFactory::keys();
    ui->comboBoxDefaultStyle->addItems(p_style_items);

    // set items for default compiler combobox:
    ui->comboBoxDefaultCompiler->addItems(p_Compilers);

    // make "Cancel" the default button
    ui->btn_CancelSave->setFocus();

    // load global configuration
    load_mySettings();
    simpleStatusbar();

}

PrefsDialog::~PrefsDialog()
{
    delete ui;
}

void PrefsDialog::on_btn_SavePrefs_clicked()
{
   save_mySettings();
   this->close();  // quit PrefsDialog

   QMessageBox::information(this, tr("Amiga Cross Editor"),
                       tr("Prefs saved.\n"
                          "Changes will be activated after restarting the application!"
                          "\n\nYou might consider saving all your work and restart now."),
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



void PrefsDialog::on_btn_CancelSave_clicked()
{
    this->close();
}

void PrefsDialog::save_mySettings()
{
    // TAB: Project
    mySettings.setValue("Project/Author", ui->lineEdit_author->text());
    mySettings.setValue("Project/Email", ui->lineEdit_email->text());
    mySettings.setValue("Project/Website", ui->lineEdit_website->text());
    mySettings.setValue("Project/Description", ui->lineEdit_description->text());
    mySettings.setValue("Project/Purpose", ui->lineEdit_purpose->text());
    mySettings.setValue("Project/ProjectRootDir", ui->lineEdit_projectsRootDir->text());
    mySettings.setValue("Project/DefaultIcon", ui->lineEdit_getDefaultIcon->text());

    // TAB: GCC
    mySettings.setValue("GCC/GccPath", ui->lineEdit_getGCCexefile->text());
    mySettings.setValue("GCC/GppPath", ui->lineEdit_getGPPexefile->text());
    mySettings.setValue("GCC/MakePath", ui->lineEdit_getMAKEexefile->text());
    mySettings.setValue("GCC/StripPath", ui->lineEdit_getSTRIPexefile->text());
    mySettings.setValue("GCC/GccDefaultOpts", ui->lineEdit_GCCdefaultOpts->text());
    mySettings.setValue("GCC/GppDefaultOpts", ui->lineEdit_GPPdefaultOpts->text());
    mySettings.setValue("GCC/ShowGccDefaultOpts", ui->checkBox_ShowGccOpts->isChecked());

    // TAB: VBCC
    mySettings.setValue("VBCC/VcPath", ui->lineEdit_getVCexefile->text());
    mySettings.setValue("VBCC/VasmPath", ui->lineEdit_getVASMexefile->text());
    mySettings.setValue("VBCC/VcConfigPath", ui->lineEdit_getVCconfigDir->text());
    mySettings.setValue("VBCC/VcDefaultOpts13", ui->lineEdit_VCdefaultOptsOS13->text());
    mySettings.setValue("VBCC/VcDefaultOpts30", ui->lineEdit_VCdefaultOptsOS30->text());
    mySettings.setValue("VBCC/VcDefaultOpts40", ui->lineEdit_VCdefaultOptsOS40->text());
    mySettings.setValue("VBCC/VcDefaultTarget", ui->comboBoxVbccDefaultTargetOS->currentIndex());
    mySettings.setValue("VBCC/ShowVbccDefaultOpts", ui->checkBox_ShowVbccOpts->isChecked());

    // TAB: Emulator
     mySettings.setValue("UAE/UaePath", ui->lineEdit_getEmulatorExefile->text());
     mySettings.setValue("UAE/Os13ConfigPath", ui->lineEdit_getOS13Configfile->text());
     mySettings.setValue("UAE/Os20ConfigPath", ui->lineEdit_getOS20Configfile->text());
     mySettings.setValue("UAE/Os30ConfigPath", ui->lineEdit_getOS3Configfile->text());
     mySettings.setValue("UAE/Os40ConfigPath", ui->lineEdit_getOS4Configfile->text());
     mySettings.setValue("UAE/DefaultConfig", ui->comboBox_defaultEmulator->currentIndex());

     // TAB: Misc
     mySettings.setValue("MISC/DefaultStyle", ui->comboBoxDefaultStyle->currentText());
     mySettings.setValue("MISC/UseBlackishStyle", ui->checkBoxStylesheet->isChecked());
     mySettings.setValue("MISC/ShowIndentGuide", ui->checkBoxIndentationLines->isChecked());
     mySettings.setValue("MISC/ShowDebugOutput", ui->checkBoxDebugOutput->isChecked());
     mySettings.setValue("MISC/NoLCDstatusbar", ui->checkBoxNoLCD->isChecked());
     mySettings.setValue("MISC/NoCompileButton", ui->checkBoxNoCompileButton->isChecked());
     mySettings.setValue("MISC/SimpleStatusbar", ui->checkBoxSimpleStatusbar->isChecked());
     mySettings.setValue("MISC/DefaultCrossCompiler", ui->comboBoxDefaultCompiler->currentIndex());
     mySettings.setValue("MISC/CreateIcon", ui->checkBoxCreateIcon->isChecked());
     mySettings.setValue("MISC/OpenConsoleOnFail", ui->checkBoxOpenOnFail->isChecked());
     mySettings.setValue("MISC/NoWarnRequester", ui->checkBoxWarnRequesters->isChecked());
}

void PrefsDialog::load_mySettings()
{
    // TAB: Project
    ui->lineEdit_author->setText(mySettings.value("Project/Author").toString());
    ui->lineEdit_email->setText(mySettings.value("Project/Email").toString());
    ui->lineEdit_website->setText(mySettings.value("Project/Website").toString());
    ui->lineEdit_description->setText(mySettings.value("Project/Description").toString());
    ui->lineEdit_purpose->setText(mySettings.value("Project/Purpose").toString());
    ui->lineEdit_projectsRootDir->setText(mySettings.value("Project/ProjectRootDir").toString());
    ui->lineEdit_getDefaultIcon->setText(mySettings.value("Project/DefaultIcon").toString());

    // TAB: GCC
    ui->lineEdit_getGCCexefile->setText(mySettings.value("GCC/GccPath").toString());
    ui->lineEdit_getGPPexefile->setText(mySettings.value("GCC/GppPath").toString());
    ui->lineEdit_getMAKEexefile->setText(mySettings.value("GCC/MakePath").toString());
    ui->lineEdit_getSTRIPexefile->setText(mySettings.value("GCC/StripPath").toString());
    ui->lineEdit_GCCdefaultOpts->setText(mySettings.value("GCC/GccDefaultOpts").toString());
    ui->lineEdit_GPPdefaultOpts->setText(mySettings.value("GCC/GppDefaultOpts").toString());
    ui->checkBox_ShowGccOpts->setChecked(mySettings.value("GCC/ShowGccDefaultOpts").toBool());

    // TAB: VBCC
    ui->lineEdit_getVCexefile->setText(mySettings.value("VBCC/VcPath").toString());
    ui->lineEdit_getVASMexefile->setText(mySettings.value("VBCC/VasmPath").toString());
    ui->lineEdit_getVCconfigDir->setText(mySettings.value("VBCC/VcConfigPath").toString());
    ui->lineEdit_VCdefaultOptsOS13->setText(mySettings.value("VBCC/VcDefaultOpts13").toString());
    ui->lineEdit_VCdefaultOptsOS30->setText(mySettings.value("VBCC/VcDefaultOpts30").toString());
    ui->lineEdit_VCdefaultOptsOS40->setText(mySettings.value("VBCC/VcDefaultOpts40").toString());
    ui->checkBox_ShowVbccOpts->setChecked(mySettings.value("VBCC/ShowVbccDefaultOpts").toBool());
    ui->comboBoxVbccDefaultTargetOS->setCurrentIndex(mySettings.value("VBCC/VcDefaultTarget").toInt());

    // TAB: Emulator
    ui->lineEdit_getEmulatorExefile->setText(mySettings.value("UAE/UaePath").toString());
    ui->lineEdit_getOS13Configfile->setText(mySettings.value("UAE/Os13ConfigPath").toString());
    ui->lineEdit_getOS20Configfile->setText(mySettings.value("UAE/Os20ConfigPath").toString());
    ui->lineEdit_getOS3Configfile->setText(mySettings.value("UAE/Os30ConfigPath").toString());
    ui->lineEdit_getOS4Configfile->setText(mySettings.value("UAE/Os40ConfigPath").toString());
    ui->comboBox_defaultEmulator->setCurrentIndex(mySettings.value("UAE/DefaultConfig").toInt());

    // TAB: Misc
    ui->comboBoxDefaultStyle->setCurrentText(mySettings.value("MISC/DefaultStyle").toString());
    ui->checkBoxStylesheet->setChecked(mySettings.value("MISC/UseBlackishStyle").toBool());
    ui->checkBoxIndentationLines->setChecked(mySettings.value("MISC/ShowIndentGuide").toBool());
    ui->checkBoxDebugOutput->setChecked(mySettings.value("MISC/ShowDebugOutput").toBool());
    ui->checkBoxNoLCD->setChecked(mySettings.value("MISC/NoLCDstatusbar").toBool());
    ui->checkBoxNoCompileButton->setChecked(mySettings.value("MISC/NoCompileButton").toBool());
    ui->checkBoxSimpleStatusbar->setChecked(mySettings.value("MISC/SimpleStatusbar").toBool());
    ui->comboBoxDefaultCompiler->setCurrentIndex(mySettings.value("MISC/DefaultCrossCompiler").toInt());
    ui->checkBoxCreateIcon->setChecked(mySettings.value("MISC/CreateIcon").toBool());
    ui->checkBoxOpenOnFail->setChecked(mySettings.value("MISC/OpenConsoleOnFail").toBool());
    ui->checkBoxWarnRequesters->setChecked(mySettings.value("MISC/NoWarnRequester").toBool());
}

void PrefsDialog::on_checkBoxSimpleStatusbar_clicked()
{
    if(ui->checkBoxSimpleStatusbar->isChecked())
    {
        ui->checkBoxNoCompileButton->setChecked(true);
        ui->checkBoxNoCompileButton->setDisabled(true);
        ui->checkBoxNoLCD->setChecked(true);
        ui->checkBoxNoLCD->setDisabled(true);
    }
    else
    {
        ui->checkBoxNoCompileButton->setChecked(false);
        ui->checkBoxNoCompileButton->setDisabled(false);
        ui->checkBoxNoLCD->setChecked(false);
        ui->checkBoxNoLCD->setDisabled(false);
    }
}

//
// if both statusbar comfort options are disabled:
// Select simpleStatusbar as default!
//
void PrefsDialog::simpleStatusbar()
{
    if((ui->checkBoxNoCompileButton->isChecked()) && (ui->checkBoxNoLCD->isChecked()))
    {
        ui->checkBoxNoCompileButton->setChecked(true);
        ui->checkBoxNoCompileButton->setDisabled(true);
        ui->checkBoxNoLCD->setChecked(true);
        ui->checkBoxNoLCD->setDisabled(true);
        ui->checkBoxSimpleStatusbar->setChecked(true);
    }
}

void PrefsDialog::on_checkBoxNoLCD_clicked()
{
    simpleStatusbar();
}

void PrefsDialog::on_checkBoxNoCompileButton_clicked()
{
    simpleStatusbar();
}

void PrefsDialog::on_btn_getDefaultIcon_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Path Amiga icon file"), QDir::homePath(),
            tr("Amiga Icon Files (*.info);;All Files (*)"));
    ui->lineEdit_getDefaultIcon->setText(fileName);
}

