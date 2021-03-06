/****************************************************************************
**
** Copyright (C) 2018/2019 Michael Bergmann. Placed in the public domain
** with some restrictions!
**
** This file is part of my enhanced AmigaED editor example, using classes of
** the Qt and QScintilla toolkits.
**
** You may use or enhance this piece of software anyway you want to - as long
** as you don't violate laws or copyright issues.
** I hereby explicitely prohibit the usage of my work for people who believe
** in racism, fascism and any kind of attitude against democratic lifestyle.
** It is self-explanatory that this prohibits the usage of my work to any
** member or fan of the german AfD party.
**
** Further information about Qt licensing is available at:
** http://www.trolltech.com/products/qt/licensing.html or by
** contacting info@trolltech.com.
**
** Further information about QScintilla licensing is available at:
** https://www.riverbankcomputing.com/software/qscintilla/license or by
** contacting sales@riverbankcomputing.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QMainWindow>
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QFont>
#include <QIcon>
#include <QWindow>
#include <QInputDialog>
#include <QTextLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPoint>
#include <QSettings>
#include <QSize>
#include <QStatusBar>
#include <QTextStream>
#include <QToolBar>
#include <QSplitter>
#include <QListView>
#include <QStyleFactory>

#include <Qsci/qsciscintilla.h>
#include <Qsci/qsciscintillabase.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexermakefile.h>
#include <Qsci/qscilexerbatch.h>
#include <Qsci/qscilexerpascal.h>
#include <Qsci/qscilexerfortran.h>
#include <Qsci/qscilexerfortran77.h>
#include <Qsci/qscilexerbash.h>
#include <Qsci/qsciprinter.h>

#include <search.h>

// allways get your defaults...
#include "mainwindow.h"
#include "prefsdialog.h"
#include "aboutdialog.h"

// Processes to start:
static QProcess myProcess;      // the process for using as spare for further extensions
static QProcess cmd;            // the process for running the compiler
static QProcess myEmulator;     // the process for running the emulator

// Open MainWindow with given filename...
MainWindow::MainWindow(QString cmdFileName)
{
    // load preferences
    // restores last saved position and size of the editor window, load other defaults
    readPosSettings();
    readSettings();
    debugVars();

    QApplication::setStyle(p_default_style);

    //QsciLexerBash *lexerbash = new QsciLexerBash;
    //lexer->setFoldComments(true);

    // Build a vertical Splitter
    QList<int> sizes;
    sizes << 320 <<150 << 200;
    splitter = new QSplitter(this);
    btnCloseOutput = new QPushButton(tr("Hide compiler output"), this);
    btnCloseOutput->setGeometry(50, 40, 75, 30);

    // Prepare Splitter components
    textEdit = new QsciScintilla;
    lview = new QListView;
    outputGroup = new QGroupBox(tr("Compiler output"));
    searchGroup = new QGroupBox(tr("Search and replace"));
    searchGroup->setFixedHeight(120);   // avoid auto sizeing - don't let our serch & replace group grow to high...
    output = new QPlainTextEdit;    
    output->setReadOnly(true);
    centerSearchForm = new QWidget;

    // Fill search form with life...
    if (centerSearchForm->objectName().isEmpty())
        centerSearchForm->setObjectName(QString::fromUtf8("centerSearchForm"));

    centerSearchForm->resize(573, 93);
    centerSearchForm->setMaximumSize(QSize(16777215, 16777215));
    gridLayout_2 = new QGridLayout(centerSearchForm);
    gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
    label_find = new QLabel(centerSearchForm);
    label_find->setObjectName(QString::fromUtf8("label_find"));

    gridLayout_2->addWidget(label_find, 0, 0, 1, 1);

    lineEdit_find = new QLineEdit(centerSearchForm);
    lineEdit_find->setObjectName(QString::fromUtf8("lineEdit_find"));
    lineEdit_find->setClearButtonEnabled(true);

    gridLayout_2->addWidget(lineEdit_find, 0, 1, 1, 2);

    gridLayout = new QGridLayout();
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
    btn_previous = new QPushButton(centerSearchForm);
    btn_previous->setObjectName(QString::fromUtf8("btn_previous"));
    btn_previous->setMaximumSize(QSize(18, 18));
    QIcon icon;
    icon.addFile(QString::fromUtf8(":/images/btn_prev.png"), QSize(), QIcon::Normal, QIcon::Off);
    btn_previous->setIcon(icon);

    gridLayout->addWidget(btn_previous, 0, 0, 1, 1);

    btn_next = new QPushButton(centerSearchForm);
    btn_next->setObjectName(QString::fromUtf8("btn_next"));
    btn_next->setMaximumSize(QSize(18, 18));
    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/images/btn_next.png"), QSize(), QIcon::Normal, QIcon::Off);
    btn_next->setIcon(icon1);

    gridLayout->addWidget(btn_next, 0, 1, 1, 1);


    gridLayout_2->addLayout(gridLayout, 0, 3, 1, 1);

    label_replace = new QLabel(centerSearchForm);
    label_replace->setObjectName(QString::fromUtf8("label_replace"));

    gridLayout_2->addWidget(label_replace, 1, 0, 1, 1);

    lineEdit_replace = new QLineEdit(centerSearchForm);
    lineEdit_replace->setObjectName(QString::fromUtf8("lineEdit_replace"));
    lineEdit_replace->setFrame(true);
    lineEdit_replace->setClearButtonEnabled(true);

    gridLayout_2->addWidget(lineEdit_replace, 1, 1, 1, 2);

    formLayout_2 = new QFormLayout();
    formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
    btn_replace = new QPushButton(centerSearchForm);
    btn_replace->setObjectName(QString::fromUtf8("btn_replace"));
    btn_replace->setMaximumSize(QSize(18, 18));
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/images/find-and-replace.png"), QSize(), QIcon::Normal, QIcon::Off);
    btn_replace->setIcon(icon2);

    formLayout_2->setWidget(0, QFormLayout::LabelRole, btn_replace);

    btn_replace_all = new QPushButton(centerSearchForm);
    btn_replace_all->setObjectName(QString::fromUtf8("btn_replace_all"));
    btn_replace_all->setMaximumSize(QSize(18, 18));
    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/images/ico_out.png"), QSize(), QIcon::Normal, QIcon::Off);
    btn_replace_all->setIcon(icon3);

    formLayout_2->setWidget(0, QFormLayout::FieldRole, btn_replace_all);


    gridLayout_2->addLayout(formLayout_2, 1, 3, 1, 1);

    btn_hide = new QPushButton(centerSearchForm);
    btn_hide->setObjectName(QString::fromUtf8("btn_hide"));
    btn_hide->setMaximumSize(QSize(18, 18));
    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/images/fileexit.png"), QSize(), QIcon::Normal, QIcon::Off);
    btn_hide->setIcon(icon4);

    gridLayout_2->addWidget(btn_hide, 1, 4, 1, 1);

    gridLayout = new QGridLayout();
    gridLayout->setSpacing(6);
    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));

    checkBox_CaseSensitive = new QCheckBox(centerSearchForm);
    checkBox_CaseSensitive->setObjectName(QString::fromUtf8("checkBox_CaseSensitive"));

    gridLayout->addWidget(checkBox_CaseSensitive, 0, 0, 1, 1);

    checkBox_WholeWords = new QCheckBox(centerSearchForm);
    checkBox_WholeWords->setObjectName(QString::fromUtf8("checkBox_WholeWords"));

    gridLayout->addWidget(checkBox_WholeWords, 0, 1, 1, 1);

    checkBox_SearchForwards = new QCheckBox(centerSearchForm);
    checkBox_SearchForwards->setObjectName(QString::fromUtf8("checkBox_SearchForwards"));
    checkBox_SearchForwards->setChecked(true);

    gridLayout->addWidget(checkBox_SearchForwards, 0, 2, 1, 1);

    gridLayout_2->addLayout(gridLayout, 2, 1, 1, 1);

    horizontalSpacer_2 = new QSpacerItem(204, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    gridLayout_2->addItem(horizontalSpacer_2, 2, 2, 1, 1);

#ifndef QT_NO_SHORTCUT
        label_find->setBuddy(lineEdit_find);
        label_replace->setBuddy(lineEdit_replace);
#endif // QT_NO_SHORTCUT

        centerSearchForm->setWindowTitle(QString());
        label_find->setText(QApplication::translate("centerSearchForm", "Find:", nullptr));
#ifndef QT_NO_TOOLTIP
        btn_previous->setToolTip(QApplication::translate("centerSearchForm", "goto previous occourance", nullptr));
#endif // QT_NO_TOOLTIP
        btn_previous->setText(QString());
#ifndef QT_NO_TOOLTIP
        btn_next->setToolTip(QApplication::translate("centerSearchForm", "goto next occourance", nullptr));
#endif // QT_NO_TOOLTIP
        btn_next->setText(QString());
        label_replace->setText(QApplication::translate("centerSearchForm", "Replace:", nullptr));
#ifndef QT_NO_TOOLTIP
        btn_replace->setToolTip(QApplication::translate("centerSearchForm", "Replace current occurance", nullptr));
#endif // QT_NO_TOOLTIP
        btn_replace->setText(QString());
#ifndef QT_NO_TOOLTIP
        btn_replace_all->setToolTip(QApplication::translate("centerSearchForm", "Replace ALL occurances", nullptr));
#endif // QT_NO_TOOLTIP
        btn_replace_all->setText(QString());
#ifndef QT_NO_TOOLTIP
        btn_hide->setToolTip(QApplication::translate("centerSearchForm", "Hide Search and Replace...", nullptr));
#endif // QT_NO_TOOLTIP
        btn_hide->setText(QString());
        checkBox_CaseSensitive->setText(QApplication::translate("centerSearchForm", "Case sensitive search", nullptr));
        checkBox_WholeWords->setText(QApplication::translate("centerSearchForm", "Whole words", nullptr));
        checkBox_SearchForwards->setText(QApplication::translate("MainWindow", "Search forwards", nullptr));



    QVBoxLayout *vsearch = new QVBoxLayout;
    vsearch->addWidget(centerSearchForm);
    searchGroup->setLayout(gridLayout_2);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(output);
    vbox->addWidget(btnCloseOutput);
    outputGroup->setLayout(vbox);

    splitter->setOrientation(Qt::Vertical);
    splitter->setHandleWidth(4);
    splitter->insertWidget(0,textEdit);
    splitter->insertWidget(1,outputGroup);
    splitter->insertWidget(1,searchGroup);
    splitter->setSizes(sizes);

    outputGroup->hide();
    searchGroup->hide();
    setCentralWidget(splitter);

    initializeGUI();    // most initializations are done within tis method!
    activateGUIdefaultSettings();

    setEmulatorMenu();  // disable emulator menu entries if no config was set

    // disable Emulator kill menu entry by default
    killEmulatorAct->setDisabled(true);

    // react on buttons of searchGroup:
    connect(btn_hide, SIGNAL(clicked(bool)), this, SLOT(on_btn_hide()));
    connect(btn_next, SIGNAL(clicked(bool)), this, SLOT(on_btn_next()));
    connect(btn_previous, SIGNAL(clicked(bool)), this, SLOT(on_btn_previous()));
    connect(btn_replace, SIGNAL(clicked(bool)), this, SLOT(on_btn_replace()));
    connect(btn_replace_all, SIGNAL(clicked(bool)), this, SLOT(on_btn_replace_all()));
    connect(lineEdit_find, SIGNAL(returnPressed()), this, SLOT(call_do_search_and_replace()));
    connect(lineEdit_find, SIGNAL(textChanged(const QString &)), this, SLOT(clearMarkers())); // Reset marked text items

    // react if document was modified
    connect(textEdit, SIGNAL(textChanged()), this, SLOT(documentWasModified()));

    // notify if cursor position changed
    connect(textEdit, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(showCurrendCursorPosition()));
    connect(output, SIGNAL(cursorPositionChanged()), this, SLOT(on_output_cursorPositionChanged()));


    cmd = new QProcess(this);
    cmd->setProcessChannelMode(QProcess::MergedChannels);

    ////////////////////////////////////////////////////////////////
    // Process handling for output to QTextBrowser or QTextEdit ///
    //////////////////////////////////////////////////////////////
    // process has some data to read
    connect(cmd, SIGNAL (readyRead()), this, SLOT (readCommand()));
    //process finished
    //connect(cmd, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT (stopCommand(int, QProcess::ExitStatus)));
    QObject::connect(cmd, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished), this, &MainWindow::stopCommand);

    ////////////////////////////////////////////
    /// command line handling for file input //
    //////////////////////////////////////////
    setCurrentFile("");

    // Load a file if specified on command line...
    if(!(cmdFileName.isEmpty()))
    {
        // Let's use our method for checking if a file allready
        // exists and create a new one if wanted...
        int response = loadNonExistantFile(cmdFileName);

        if(p_mydebug)
        {
            qDebug() << "loadNonExistantFile() returned " << response;
            // return values:
            //  0 - new file created
            // -1 - file creation failed
            //  1 - existing file loaded
            // 10 - quit app due to failed file creation
        }

        if (response == 10)
        {
            // if file creation went wrong
            // and MessageBox was answered 'Cancel':
            exit(-1);   // kill the App!
        }


    }
    else
    {
        if(p_mydebug)
        {
            qDebug() << "AmigaED started with empty file.";
        }

    }
}

//
// catch close() event and react
// on changes in current text document
//
void MainWindow::closeEvent(QCloseEvent *event)
{
    actionKillEmulator();
    if (maybeSave()) {
        writeSettings();
        event->accept();    // OK: Quit the app!
    } else {
        event->ignore();    // CANCEL: just stay where we are... ;)
    }
}


//
// create a new, empty file
//
void MainWindow::newFile()
{
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFile("");
    }
    // Let's asume our new file is C/C++
    initializeLexerCPP();
    p_main_set = false;
    p_versionstring_set = false;
}

//
// open file from disk
//
void MainWindow::open()
{
    if(curFile.isEmpty())
        curFile = p_projectsRootDir;
    else
        if(curFile.isEmpty())
            curFile = QDir::currentPath();

    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this,
                "Open source file",
                curFile,    // look up for files in PROGDIR first!
                "C/C++ files (*.c *.cpp *.h *.hpp) ;; "
                "ASM files (*.a *.asm *.s *.S *.m) ;; Makefiles (Make*.* *.mak) ;; "
                "AmigaE files (*.e *.m) ;; Pascal files (*.p *.pas) ;; "
                "Text files (*.txt *.md) ;; All files (*.*)");

        if (!fileName.isEmpty())
            loadFile(fileName);
        p_main_set = false;
        p_versionstring_set = false;
    }
}

//
// save file to disk
//
bool MainWindow::save()
{
    // if no file name has been given until now..
    if (curFile.isEmpty())
    {
        qDebug() << "curFile: " << curFile;
        return saveAs();        // ...call saveAs dialog!
    }
    else
    {
        return saveFile(curFile);
    }
}

//
// save file, let user choose a filename
//
bool MainWindow::saveAs()
{
    if(curFile.isEmpty())
        curFile = p_projectsRootDir;

    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Save source file",
                                                    curFile,    // save files into last used folder
                                                    "C/C++ files (*.c *.cpp *.h *.hpp) ;; "
                                                    "ASM files (*.a *.asm *.s *.S *.m) ;; Makefiles (Make*.* *.mak) ;; "
                                                    "AmigaE files (*.e *.m) ;; Pascal files (*.p *.pas) ;; "
                                                    "Text files (*.txt *.md) ;; All files (*.*)");

    if (fileName.isEmpty())
        return false;

    if(p_mydebug)
    {
        qDebug() << " File to save: " << fileName;
    }

    return saveFile(fileName);
}

//
// show information about this app...
//
void MainWindow::about()
{
   aboutDialog *aceAbout = new aboutDialog(this);
   aceAbout->exec();
}

//
// react on SIGNAL textChanged() if text was modified
//
void MainWindow::documentWasModified()
{
    setWindowModified(textEdit->isModified());  // put asterix (*) into filename!
}

//
// Helper to set approbiate Lexer according to a file's .ext
//
void MainWindow::SetLexerAtFileExtension(QString)
{
    if(p_mydebug)
    {
        qDebug() << "Lexer changed!";
    }
}

//
// you'll need some fucking actions first if you
// want to create menues!
//
void MainWindow::createActions()
{
    // Menue actions consist of:
    /* --- File ---------------------------------------------------*/
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);  // this is the action itsself, equipped with an image
                                                                        // and a shortcut (<alt> + <n>) for opening the menue

    newAct->setShortcut(tr("Ctrl+N"));                                  // this is the instant shortcut for calling the action
                                                                        // without opening the menue first

    newAct->setStatusTip(tr("Create a new file"));                      // Display a help message in app's status bar
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));        // connect the action to a Qt SIGNAL for calling
                                                                        // the approbiate class method to handle the request.
                                                                        // Here: MainWindow::newFile()
                                                                        // see "mainwindow.h" for details!

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(QIcon(":/images/filesaveas.png"),tr("Save &As..."), this);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    prefsAct = new QAction(QIcon(":/images/prefs.png"),tr("Global prefs..."), this);
    prefsAct->setShortcut(tr("F12"));
    prefsAct->setStatusTip(tr("Open global preferences..."));
    connect(prefsAct, SIGNAL(triggered()), this, SLOT(startPrefs()));

    prefsReloadAct = new QAction(tr("Reload settings"), this);
    prefsReloadAct->setShortcut(tr("Shift+F12"));
    prefsReloadAct->setStatusTip(tr("Reload global settings..."));
    connect(prefsReloadAct, SIGNAL(triggered()), this, SLOT(readSettings()));

    printAct = new QAction(QIcon(":/images/printer.png"),tr("&Print file..."), this);
    printAct->setShortcut(tr("Ctrl+p"));
    printAct->setStatusTip(tr("Prepare for printing..."));
    connect(printAct, SIGNAL(triggered()), this, SLOT(printFile()));

    exitAct = new QAction(QIcon(":/images/fileexit.png"), tr("&Exit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    /* --- Edit -----------------------------------------------------------------------*/
    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcut(tr("Ctrl+X"));
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcut(tr("Ctrl+C"));
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), textEdit, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcut(tr("Ctrl+V"));
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));

    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(textEdit, SIGNAL(copyAvailable(bool)), cutAct, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)), copyAct, SLOT(setEnabled(bool)));  

    searchAct = new QAction(QIcon(":/images/search.png"), tr("Sea&rch..."), this);
    searchAct->setShortcut(tr("Ctrl+f"));
    searchAct->setStatusTip(tr("Search text in document"));
    connect(searchAct, SIGNAL(triggered()), this, SLOT(actionSearch()));

    /* --- Navigation -------------------------------------------------------------------*/
    gotoTopAct = new QAction( tr("&Goto top..."), this);
    gotoTopAct->setShortcut(tr("Ctrl+Home"));
    gotoTopAct->setStatusTip(tr("Goto top of file..."));
    connect(gotoTopAct, SIGNAL(triggered()), this, SLOT(actionGotoTop()));

    gotoBottomAct = new QAction( tr("&Goto bottom..."), this);
    gotoBottomAct->setShortcut(tr("Ctrl+End"));
    gotoBottomAct->setStatusTip(tr("Goto bottom of file..."));
    connect(gotoBottomAct, SIGNAL(triggered()), this, SLOT(actionGotoBottom()));

    gotoLineAct = new QAction(QIcon(":/images/gotoline.png"), tr("&Goto Line..."), this);
    gotoLineAct->setShortcut(tr("Ctrl+G"));
    gotoLineAct->setStatusTip(tr("Goto line X..."));
    connect(gotoLineAct, SIGNAL(triggered()), this, SLOT(actionGoto_Line()));

    gotoMatchingBraceAct = new QAction(QIcon(":/images/brackets.png"), tr("Goto &matching bracket {} ... [] ... ()..."), this);
    gotoMatchingBraceAct->setShortcut(tr("Ctrl+b"));
    gotoMatchingBraceAct->setStatusTip(tr("Goto matching bracket..."));
    connect(gotoMatchingBraceAct, SIGNAL(triggered()), this, SLOT(actionGoto_matching_brace()));

    /* --- View -----------------------------------------------------------------------*/
    toggleFoldAct = new QAction(tr("&Fold all..."), this);
    toggleFoldAct->setShortcut(tr("Ctrl+Alt+f"));
    toggleFoldAct->setCheckable(true);
    toggleFoldAct->setChecked(false);
    toggleFoldAct->setStatusTip(tr("Toggle folding for whole document"));
    connect(toggleFoldAct, SIGNAL(triggered()), this, SLOT(initializeFolding()));

    showLineNumbersAct = new QAction(tr("Show line numbers..."), this);
    showLineNumbersAct->setCheckable(true);
    showLineNumbersAct->setChecked(true);
    showLineNumbersAct->setStatusTip(tr("Show or hide line numbers"));
    connect(showLineNumbersAct, SIGNAL(triggered()), this, SLOT(actionShowLineNumbers()));

    showCaretLineAct = new QAction(tr("Show caret line..."), this);
    showCaretLineAct->setShortcut(tr("Ctrl+#"));
    showCaretLineAct->setCheckable(true);
    showCaretLineAct->setChecked(false);
    showCaretLineAct->setStatusTip(tr("Show or hide caret line"));
    connect(showCaretLineAct, SIGNAL(triggered()), this, SLOT(actionShowCaretLine()));

    showDebugInfoAct = new QAction(tr("Show debug output"), this);
    showDebugInfoAct->setCheckable(true);
    showDebugInfoAct->setChecked(p_mydebug);
    showDebugInfoAct->setStatusTip(tr("Toggle debug output visibility"));
    connect(showDebugInfoAct, SIGNAL(triggered()), this, SLOT(actionShowDebug()));

    showEOLAct = new QAction(tr("Show EOL character"), this);
    showEOLAct->setCheckable(true);
    showEOLAct->setChecked(false);
    showEOLAct->setStatusTip(tr("Toggle EOL visibility"));
    connect(showEOLAct, SIGNAL(triggered()), this, SLOT(actionShowEOL()));

    showUnprintableAct = new QAction(tr("Show unprintable characters"), this);
    showUnprintableAct->setCheckable(true);
    showUnprintableAct->setChecked(false);
    showUnprintableAct->setStatusTip(tr("Toggle unprintable characters visibility"));
    connect(showUnprintableAct, SIGNAL(triggered()), this, SLOT(actionShowUnprintable()));

    showIndentationGuidesAct = new QAction(tr("Show indentation guides"), this);
    showIndentationGuidesAct->setCheckable(true);
    showIndentationGuidesAct->setChecked(p_show_indentation);
    showIndentationGuidesAct->setStatusTip(tr("Toggle indentation guides visibility"));
    connect(showIndentationGuidesAct, SIGNAL(triggered()), this, SLOT(actionShowIndentationGuides()));

    /* --- Build -----------------------------------------------------------------------*/
    selectCompilerVBCCAct = new QAction(QIcon(":/images/c-logo.png"), tr("VBCC vc (C mode only)..."), this);
    selectCompilerVBCCAct->setShortcut(tr("Shift+Ctrl+v"));
    selectCompilerVBCCAct->setStatusTip(tr("Set Compiler to VBCC (C mode only)..."));
    selectCompilerVBCCAct->setCheckable(true);
    selectCompilerVBCCAct->setChecked(false);
    connect(selectCompilerVBCCAct, SIGNAL(triggered()), this, SLOT(actionSelectCompilerVBCC()));

    selectCompilerGCCAct = new QAction(QIcon(":/images/c-logo.png"), tr("GNU gcc (C mode)..."), this);
    selectCompilerGCCAct->setShortcut(tr("Shift+Ctrl+g"));
    selectCompilerGCCAct->setStatusTip(tr("Set Compiler to GNU gcc (C mode)..."));
    selectCompilerGCCAct->setCheckable(true);
    selectCompilerGCCAct->setChecked(false);
    connect(selectCompilerGCCAct, SIGNAL(triggered()), this, SLOT(actionSelectCompilerGCC()));

    selectCompilerGPPAct = new QAction(QIcon(":/images/cpp-logo.png"), tr("GNU g++ (C++ mode)..."), this);
    selectCompilerGPPAct->setShortcut(tr("Shift+Ctrl+c"));
    selectCompilerGPPAct->setStatusTip(tr("Set Compiler to GNU g++ (C++ mode)..."));
    selectCompilerGPPAct->setCheckable(true);
    selectCompilerGPPAct->setChecked(false);
    connect(selectCompilerGPPAct, SIGNAL(triggered()), this, SLOT(actionSelectCompilerGPP()));

    toggleGccDefaultOptsAct = new QAction(tr("Show gcc/g++ options dialog..."), this);
    toggleGccDefaultOptsAct->setCheckable(true);
    toggleGccDefaultOptsAct->setChecked(p_show_gcc_opts);
    toggleGccDefaultOptsAct->setStatusTip(tr("Show or hide gcc/g++ options dialog"));
    connect(toggleGccDefaultOptsAct, SIGNAL(triggered()), this, SLOT(actionToggleGccDefaultOptsDialog()));

    toggleVbccDefaultOptsAct = new QAction(tr("Show vbcc options dialog..."), this);
    toggleVbccDefaultOptsAct->setCheckable(true);
    toggleVbccDefaultOptsAct->setChecked(p_show_vbcc_opts);
    toggleVbccDefaultOptsAct->setStatusTip(tr("Show or hide vbcc options dialog"));
    connect(toggleVbccDefaultOptsAct, SIGNAL(triggered()), this, SLOT(actionToggleVbccDefaultOptsDialog()));

    // this will put the compilers in our menue into a mutual exclusive
    // group for automatically checking/unchecking each other:
    compilerGroup = new QActionGroup(this);
    compilerGroup->addAction(selectCompilerVBCCAct);
    compilerGroup->addAction(selectCompilerGCCAct);
    compilerGroup->addAction(selectCompilerGPPAct);

    compileAct = new QAction(QIcon(":/images/dice.png"), tr("Comp&ile..."), this);
    compileAct->setShortcut(tr("F6"));
    compileAct->setStatusTip(tr("Compile current file..."));
    connect(compileAct, SIGNAL(triggered()), this, SLOT(actionCompile()));

    showOutputAct = new QAction(tr("Show output pane..."), this);
    showOutputAct->setShortcut(tr("F9"));
    showOutputAct->setStatusTip(tr("Show Compiler output..."));
    connect(showOutputAct, SIGNAL(triggered()), this, SLOT(actionShowOutputConsole()));

    hideOutputAct = new QAction(tr("Hide output pane..."), this);
    hideOutputAct->setShortcut(tr("Shift+F9"));
    hideOutputAct->setStatusTip(tr("Hide Compiler output..."));
    connect(hideOutputAct, SIGNAL(triggered()), this, SLOT(actionCloseOutputConsole()));

    /* --- Tools -----------------------------------------------------------------------*/
    emulatorAct = new QAction(QIcon(":/images/start-emu.png"), tr("Start default Workbench in UA&E..."), this);
    emulatorAct->setShortcut(tr("Alt+e"));
    emulatorAct->setStatusTip(tr("Start Amiga Emulation..."));
    connect(emulatorAct, SIGNAL(triggered()), this, SLOT(actionEmulator()));

    emulator13Act = new QAction(QIcon(":/images/start-emu.png"), tr("Start Workbench 1.3 in UAE..."), this);
    emulator13Act->setShortcut(tr("Alt+f"));
    emulator13Act->setStatusTip(tr("Start Amiga Emulation..."));
    connect(emulator13Act, SIGNAL(triggered()), this, SLOT(actionEmuOS13()));

    emulator20Act = new QAction(QIcon(":/images/start-emu.png"), tr("Start Workbench 2.1 in UAE..."), this);
    emulator20Act->setShortcut(tr("Alt+g"));
    emulator20Act->setStatusTip(tr("Start Amiga Emulation..."));
    connect(emulator20Act, SIGNAL(triggered()), this, SLOT(actionEmuOS20()));

    emulator30Act = new QAction(QIcon(":/images/start-emu.png"), tr("Start Workbench 3.x in UAE..."), this);
    emulator30Act->setShortcut(tr("Alt+h"));
    emulator30Act->setStatusTip(tr("Start Amiga Emulation..."));
    connect(emulator30Act, SIGNAL(triggered()), this, SLOT(actionEmuOS30()));

    emulator40Act = new QAction(QIcon(":/images/start-emu.png"), tr("Start Workbench 4.1 in UAE..."), this);
    emulator40Act->setShortcut(tr("Alt+i"));
    emulator40Act->setStatusTip(tr("Start Amiga Emulation..."));
    connect(emulator40Act, SIGNAL(triggered()), this, SLOT(actionEmuOS40()));

    killEmulatorAct = new QAction(QIcon(":/images/stop-emu.png"), tr("Stop running Emulation..."), this);
    killEmulatorAct->setShortcut(tr("Alt+s"));
    killEmulatorAct->setStatusTip(tr("Stop Amiga Emulation..."));
    connect(killEmulatorAct, SIGNAL(triggered()), this, SLOT(actionKillEmulator()));

    /* --- Syntax -----------------------------------------------------------------------*/
    lexCPPAct = new QAction(tr("C/C++..."), this);
    lexCPPAct->setStatusTip(tr("highlight C/C++ syntax"));
    lexCPPAct->setCheckable(true);
    lexCPPAct->setChecked(true);
    connect(lexCPPAct, SIGNAL(triggered()), this, SLOT(initializeLexerCPP()));

    lexMakefileAct = new QAction(tr("Makefile..."), this);
    lexMakefileAct->setStatusTip(tr("highlight Makefile syntax"));
    lexMakefileAct->setCheckable(true);
    lexMakefileAct->setChecked(false);
    connect(lexMakefileAct, SIGNAL(triggered()), this, SLOT(initializeLexerMakefile()));

    lexBatchAct = new QAction(tr("C/C++..."), this);
    lexBatchAct->setStatusTip(tr("highlight AmigaShell syntax"));
    lexBatchAct->setCheckable(true);
    lexBatchAct->setChecked(false);
    connect(lexBatchAct, SIGNAL(triggered()), this, SLOT(initializeLexerBatch()));

    lexFortranAct = new QAction(tr("Amiga installer..."), this);
    lexFortranAct->setStatusTip(tr("highlight Amiga Installer syntax"));
    lexFortranAct->setCheckable(true);
    lexFortranAct->setChecked(false);
    connect(lexFortranAct, SIGNAL(triggered()), this, SLOT(initializeLexerFortran()));

    lexPascalAct = new QAction(tr("Pascal..."), this);
    lexPascalAct->setStatusTip(tr("highlight Pascal syntax"));
    lexPascalAct->setCheckable(true);
    lexPascalAct->setChecked(false);
    connect(lexPascalAct, SIGNAL(triggered()), this, SLOT(initializeLexerPascal()));

    lexPlainTextAct = new QAction(tr("Plain Text..."), this);
    lexPlainTextAct->setStatusTip(tr("show Plain Text only"));
    lexPlainTextAct->setCheckable(true);
    lexPlainTextAct->setChecked(false);
    connect(lexPlainTextAct, SIGNAL(triggered()), this, SLOT(initializeLexerNone()));

    // this will put the lexers in our menue into a mutual exclusive
    // group for automatically checking/unchecking each other:
    syntaxGroup = new QActionGroup(this);
    syntaxGroup->addAction(lexCPPAct);
    syntaxGroup->addAction(lexMakefileAct);
    syntaxGroup->addAction(lexFortranAct);
    syntaxGroup->addAction(lexPascalAct);
    syntaxGroup->addAction(lexPlainTextAct);

    /*
     * insertMenue actions:
     */
    shellAppAct = new QAction(tr("Shell application template"), this); // inserts into insertMenue => preprocessorMenue
    shellAppAct->setShortcut(tr("Ctrl+Alt+n"));
    shellAppAct->setStatusTip(tr("Create a new file containing a complete AmigaShell app template"));
    connect(shellAppAct, SIGNAL(triggered()), this, SLOT(actionInsertShellAppSkeletton()));

    stdCAppAct = new QAction(tr("ANSI C application template"), this); // inserts into insertMenue => preprocessorMenue
    stdCAppAct->setShortcut(tr("Ctrl+Alt+a"));
    stdCAppAct->setStatusTip(tr("Create a new file containing a complete ANSI C app template"));
    connect(stdCAppAct, SIGNAL(triggered()), this, SLOT(actionInsertCAppSkeletton()));

    stdCppAppAct = new QAction(tr("C++ application template"), this); // inserts into insertMenue => preprocessorMenue
    stdCppAppAct->setShortcut(tr("Ctrl+Alt+p"));
    stdCppAppAct->setStatusTip(tr("Create a new file containing a complete C++ app template"));
    connect(stdCppAppAct, SIGNAL(triggered()), this, SLOT(actionInsertCppAppSkeletton()));

    includeAct = new QAction(tr("#include"), this); // inserts into insertMenue => preprocessorMenue
    includeAct->setShortcut(tr("Ctrl+i"));
    includeAct->setStatusTip(tr("insert #include <file>..."));
    connect(includeAct, SIGNAL(triggered()), this, SLOT(actionInsertInclude()));

    amigaIncludesAct = new QAction(tr("Amiga #include files"), this); // inserts into insertMenue => preprocessorMenue
    amigaIncludesAct->setStatusTip(tr("insert most commonly used Amiga #include files..."));
    connect(amigaIncludesAct, SIGNAL(triggered()), this, SLOT(actionInsertAmigaIncludes()));

    defineAct = new QAction(tr("#define"), this); // inserts into insertMenue => preprocessorMenue
    defineAct->setShortcut(tr("Alt+d"));
    defineAct->setStatusTip(tr("insert #define SOME_VALUE..."));
    connect(defineAct, SIGNAL(triggered()), this, SLOT(actionInsertDefine()));

    ifdefAct = new QAction(tr("#ifdef"), this); // inserts into insertMenue => preprocessorMenue
    ifdefAct->setStatusTip(tr("insert #ifdef ... #endif..."));
    connect(ifdefAct, SIGNAL(triggered()), this, SLOT(actionInsertIfdef()));

    ifdefinedAct = new QAction(tr("#if defined(...)"), this); // inserts into insertMenue => preprocessorMenue
    ifdefinedAct->setStatusTip(tr("insert #if defined(...) ... #endif..."));
    connect(ifdefinedAct, SIGNAL(triggered()), this, SLOT(actionInsertIfdefined()));

    ifdefinedCompilerAct = new QAction(tr("Identify Amiga compiler"), this); // inserts into insertMenue => preprocessorMenue
    ifdefinedCompilerAct->setStatusTip(tr("identify compiler in use..."));
    connect(ifdefinedCompilerAct, SIGNAL(triggered()), this, SLOT(actionInsertIfdefinedCompiler()));

    ifndefAct = new QAction(tr("#ifndef"), this); // inserts into insertMenue => preprocessorMenue
    ifndefAct->setStatusTip(tr("insert #ifndef ... #endif..."));
    connect(ifndefAct, SIGNAL(triggered()), this, SLOT(actionInsertIfndef()));

    OpenLibraryAct = new QAction(tr("OpenLibrary()"), this); // inserts into insertMenue => libraryMenue
    OpenLibraryAct->setStatusTip(tr("insert OpenLibrary()"));
    connect(OpenLibraryAct, SIGNAL(triggered()), this, SLOT(actionInsertOpenLibrary()));

    CloseLibraryAct = new QAction(tr("CloseLibrary()"), this); // inserts into insertMenue => libraryMenue
    CloseLibraryAct->setStatusTip(tr("insert CloseLibrary()"));
    connect(CloseLibraryAct, SIGNAL(triggered()), this, SLOT(actionInsertCloseLibrary()));

    ifAct = new QAction(tr("if(..) {...}"), this); // inserts into insertMenue => conditionsMenue
    ifAct->setShortcut(tr("Ctrl+Alt+i"));
    ifAct->setStatusTip(tr("insert if(..) {...}"));
    connect(ifAct, SIGNAL(triggered()), this, SLOT(actionInsertIf()));

    if_elseAct = new QAction(tr("if(..) {...} else {...}"), this); // inserts into insertMenue => conditionsMenue
    if_elseAct->setStatusTip(tr("insert if(..) {...} else {...}"));
    connect(if_elseAct, SIGNAL(triggered()), this, SLOT(actionInsertIfElse()));

    whileAct = new QAction(tr("while(...) {...}"), this); // inserts into insertMenue => loopsMenue
    whileAct->setStatusTip(tr("insert while(...) {...}"));
    connect(whileAct, SIGNAL(triggered()), this, SLOT(actionInsertWhile()));

    forAct = new QAction(tr("for(...) {...}"), this); // inserts into insertMenue => loopsMenue
    forAct->setStatusTip(tr("insert for(...) {...}"));
    connect(forAct, SIGNAL(triggered()), this, SLOT(actionInsertForLoop()));

    do_whileAct = new QAction(tr("do...{...}while(...)"), this); // inserts into insertMenue => loopsMenue
    do_whileAct->setStatusTip(tr("insert do...{...}while(...)"));
    connect(do_whileAct, SIGNAL(triggered()), this, SLOT(actionInsertDoWhile()));

    switchAct = new QAction(tr("switch(...)"), this); // inserts into insertMenue => loopsMenue
    switchAct->setStatusTip(tr("insert switch(...)"));
    connect(switchAct, SIGNAL(triggered()), this, SLOT(actionInsertSwitch()));

    mainAct = new QAction(tr("int main {...}"), this); // inserts into insertMenue
    mainAct->setStatusTip(tr("insert main() {...}"));
    connect(mainAct, SIGNAL(triggered()), this, SLOT(actionInsertMain()));

    functionAct = new QAction(tr("int function {...}"), this); // inserts into insertMenue
    functionAct->setStatusTip(tr("insert C function definition"));
    connect(functionAct, SIGNAL(triggered()), this, SLOT(actionInsertFunction()));

    enumAct = new QAction(tr("enum {...}"), this); // inserts into insertMenue
    enumAct->setStatusTip(tr("insert enum {...}"));
    connect(enumAct, SIGNAL(triggered()), this, SLOT(actionInsertEnum()));

    structAct = new QAction(tr("struct name {...}"), this); // inserts into insertMenue
    structAct->setStatusTip(tr("insert struct name {...}"));
    connect(structAct, SIGNAL(triggered()), this, SLOT(actionInsertStruct()));

    c_classAct = new QAction(tr("C-style class..."), this); // inserts into insertMenue => classMenue
    c_classAct->setStatusTip(tr("insert C-style class"));
    connect(c_classAct, SIGNAL(triggered()), this, SLOT(actionInsertCClass()));

    cpp_classAct = new QAction(tr("C++ style class..."), this); // inserts into insertMenue => classMenue
    cpp_classAct->setStatusTip(tr("insert C++ style class"));
    connect(cpp_classAct, SIGNAL(triggered()), this, SLOT(actionInsertCppClass()));

    fileheaderAct = new QAction(tr("Fileheader comment..."), this); // inserts into insertMenue => commentsMenue
    fileheaderAct->setStatusTip(tr("insert Fileheader comment"));
    connect(fileheaderAct, SIGNAL(triggered()), this, SLOT(actionInsertFileheaderComment()));

    versionStringAct = new QAction(tr("Amiga C version string"), this); // inserts into insertMenue => preprocessorMenue
    versionStringAct->setStatusTip(tr("insert $VER: programname version.revision (dd.mm.yyyy)"));
    connect(versionStringAct, SIGNAL(triggered()), this, SLOT(actionInsertAmigaVersionString()));

    c_singleAct = new QAction(tr("C-style single line comment..."), this); // inserts into insertMenue => commentsMenue
    c_singleAct->setStatusTip(tr("insert C-style single line comment"));
    connect(c_singleAct, SIGNAL(triggered()), this, SLOT(actionInsertCSingleComment()));

    c_multiAct = new QAction(tr("C-style multi line comment..."), this); // inserts into insertMenue => commentsMenue
    c_multiAct->setStatusTip(tr("insert C-style multi line comment"));
    connect(c_multiAct, SIGNAL(triggered()), this, SLOT(actionInsertCMultiComment()));

    cpp_singleAct = new QAction(tr("C++ style single line comment..."), this); // inserts into insertMenue => commentsMenue
    cpp_singleAct->setStatusTip(tr("insert C++ style single line comment"));
    connect(cpp_singleAct, SIGNAL(triggered()), this, SLOT(actionInsertCppSingleComment()));

    lineDevideCommentAct = new QAction(tr("C-style single line code dividing comment..."), this); // inserts into insertMenue => commentsMenue
    lineDevideCommentAct->setStatusTip(tr("insert code dividing comment: /* --- COMMENT -------*/"));
    connect(lineDevideCommentAct, SIGNAL(triggered()), this, SLOT(actionInsertCLineDevideComment()));

    snippet1Act = new QAction(tr("Snippet #1..."), this); // inserts into insertMenue => snippetsMenue
    snippet1Act->setStatusTip(tr("insert Snippet #1"));
    connect(snippet1Act, SIGNAL(triggered()), this, SLOT(actionInsertSnippet1()));

    snippet2Act = new QAction(tr("Snippet #2..."), this); // inserts into insertMenue => snippetsMenue
    snippet2Act->setStatusTip(tr("insert Snippet #2"));
    connect(snippet2Act, SIGNAL(triggered()), this, SLOT(actionInsertSnippet2()));

    snippet3Act = new QAction(tr("Snippet #3..."), this); // inserts into insertMenue => snippetsMenue
    snippet3Act->setStatusTip(tr("insert Snippet #3"));
    connect(snippet3Act, SIGNAL(triggered()), this, SLOT(actionInsertSnippet3()));

    snippet4Act = new QAction(tr("Snippet #4..."), this); // inserts into insertMenue => snippetsMenue
    snippet4Act->setStatusTip(tr("insert Snippet #4"));
    connect(snippet4Act, SIGNAL(triggered()), this, SLOT(actionInsertSnippet4()));

    /* --- Help -------------------------------------------------------------------------*/
    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

//
// ...now that we've created actions, let's create some menues for our app!
//
void MainWindow::createMenus()
{
    // File menu
    fileMenue = menuBar()->addMenu(tr("&File")); // this is a main menue entry as shown in the menue bar of the app window (File)
    fileMenue->addAction(newAct);                // this is menue entry (File/New)
    fileMenue->addAction(openAct);
    fileMenue->addAction(saveAct);
    fileMenue->addAction(saveAsAct);
    fileMenue->addSeparator();
    fileMenue->addAction(printAct);
    fileMenue->addSeparator();
    fileMenue->addAction(prefsAct);
    fileMenue->addAction(prefsReloadAct);
    fileMenue->addSeparator();
    fileMenue->addAction(exitAct);

    // Edit menue
    editMenue = menuBar()->addMenu(tr("&Edit"));
    editMenue->addAction(cutAct);
    editMenue->addAction(copyAct);
    editMenue->addAction(pasteAct);
    editMenue->addSeparator();
    editMenue->addAction(searchAct);

    menuBar()->addSeparator();

    // Inserts menue
    insertMenue = menuBar()->addMenu(tr("&Inserts"));
    templatesMenue = insertMenue->addMenu(tr("Application templates..."));
    templatesMenue->addAction(stdCAppAct);
    templatesMenue->addSeparator();
    templatesMenue->addAction(stdCppAppAct);
    templatesMenue->addSeparator();
    templatesMenue->addAction(shellAppAct);
    insertMenue->addSeparator();
    preprocessorMenue = insertMenue->addMenu(tr("Preprocessor..."));
    preprocessorMenue->addAction(includeAct);
    preprocessorMenue->addAction(defineAct);
    preprocessorMenue->addAction(ifdefAct);
    preprocessorMenue->addAction(ifndefAct);
    preprocessorMenue->addAction(ifdefinedAct);
    preprocessorMenue->addSeparator();
    preprocessorMenue->addAction(amigaIncludesAct);
    preprocessorMenue->addSeparator();
    preprocessorMenue->addAction(ifdefinedCompilerAct);
    insertMenue->addSeparator();
    libraryMenue = insertMenue->addMenu(tr("Libraries..."));
    libraryMenue->addAction(OpenLibraryAct);
    libraryMenue->addAction(CloseLibraryAct);
    insertMenue->addSeparator();
    conditionsMenue = insertMenue->addMenu(tr("Condition..."));
    conditionsMenue->addAction(ifAct);
    conditionsMenue->addAction(if_elseAct);
    insertMenue->addSeparator();
    loopsMenue = insertMenue->addMenu(tr("Loops..."));
    loopsMenue->addAction(whileAct);
    loopsMenue->addAction(forAct);
    loopsMenue->addAction(do_whileAct);
    loopsMenue->addAction(switchAct);
    insertMenue->addSeparator();
    insertMenue->addAction(mainAct);
    insertMenue->addAction(functionAct);
    insertMenue->addAction(enumAct);
    insertMenue->addAction(structAct);
    insertMenue->addSeparator();
    classMenue = insertMenue->addMenu(tr("Class..."));
    classMenue->addAction(c_classAct);
    classMenue->addSeparator();
    classMenue->addAction(cpp_classAct);
    insertMenue->addSeparator();
    commentsMenue = insertMenue->addMenu(tr("Comments..."));
    commentsMenue->addAction(fileheaderAct);
    commentsMenue->addSeparator();
    commentsMenue->addAction(c_singleAct);
    commentsMenue->addAction(c_multiAct);
    commentsMenue->addSeparator();
    commentsMenue->addAction(cpp_singleAct);
    insertMenue->addSeparator();
    snippetsMenue = insertMenue->addMenu(tr("Snippets..."));
    snippetsMenue->addAction(snippet1Act);
    snippetsMenue->addAction(snippet2Act);
    snippetsMenue->addAction(snippet3Act);
    snippetsMenue->addAction(snippet4Act);

    menuBar()->addSeparator();

    // Build menue
    buildMenue = menuBar()->addMenu(tr("&Build"));
    compilerMenue = buildMenue->addMenu(tr("Select Compiler..."));
    compilerMenue->addAction(selectCompilerVBCCAct);
    compilerMenue->addSeparator();
    compilerMenue->addAction(selectCompilerGCCAct);
    compilerMenue->addSeparator();
    compilerMenue->addAction(selectCompilerGPPAct);
    buildMenue->addSeparator();
    buildMenue->addAction(compileAct);
    buildMenue->addSeparator();
    buildMenue->addAction(showOutputAct);
    buildMenue->addAction(hideOutputAct);
    buildMenue->addSeparator();
    buildMenue->addAction(toggleGccDefaultOptsAct);
    buildMenue->addAction(toggleVbccDefaultOptsAct);


    menuBar()->addSeparator();

    // Navigation menue
    navigationMenue = menuBar()->addMenu(tr("&Navigation"));
    navigationMenue->addAction(gotoTopAct);
    navigationMenue->addAction(gotoBottomAct);
    navigationMenue->addAction(gotoLineAct);
    navigationMenue->addSeparator();
    navigationMenue->addAction(gotoMatchingBraceAct);

    menuBar()->addSeparator();

    // View menue
    viewMenue = menuBar()->addMenu(tr("&View"));
    viewMenue->addAction(showLineNumbersAct);
    viewMenue->addSeparator();
    viewMenue->addAction(showCaretLineAct);
    viewMenue->addSeparator();
    viewMenue->addAction(showIndentationGuidesAct);
    viewMenue->addSeparator();
    viewMenue->addAction(toggleFoldAct);
    viewMenue->addSeparator();
    charMenue = viewMenue->addMenu(tr("Special characters..."));
    charMenue->addAction(showEOLAct);
    charMenue->addAction(showUnprintableAct);

    viewMenue->addAction(showDebugInfoAct);

    menuBar()->addSeparator();

    // Syntax menue
    syntaxMenue = menuBar()->addMenu(tr("Synta&x"));
    syntaxMenue->addAction(lexCPPAct);
    syntaxMenue->addSeparator();
    syntaxMenue->addAction(lexMakefileAct);
    syntaxMenue->addSeparator();
    syntaxMenue->addAction(lexFortranAct);
    syntaxMenue->addSeparator();
    syntaxMenue->addAction(lexPascalAct);
    syntaxMenue->addSeparator();
    syntaxMenue->addAction(lexPlainTextAct);

    menuBar()->addSeparator();

    // Tools menue
    toolsMenue = menuBar()->addMenu(tr("&Tools"));
    emulatorMenue = toolsMenue->addMenu(tr("Emulator..."));
    emulatorMenue->addAction(emulator13Act);
    emulatorMenue->addAction(emulator20Act);
    emulatorMenue->addAction(emulator30Act);
    emulatorMenue->addAction(emulator40Act);
    emulatorMenue->addSeparator();
    emulatorMenue->addAction(emulatorAct);
    toolsMenue->addSeparator();
    toolsMenue->addAction(killEmulatorAct);

    // Help menue
    helpMenue = menuBar()->addMenu(tr("&Help"));
    helpMenue->addAction(aboutAct);
    helpMenue->addAction(aboutQtAct);
}

//
// Let's put some of our actions into the app's toolbar!
// You can put any action into any toolbar section - as
// long as you have created that section before!
//
void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));   // this is a section, correspondending to main menue "File"
    fileToolBar->addAction(newAct);         // this is a section entry, correspondending to menue entry "File/New"
    fileToolBar->addSeparator();
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
    fileToolBar->addAction(saveAsAct);
    fileToolBar->addSeparator();
    fileToolBar->addAction(printAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);

    searchToolBar = addToolBar(tr("File"));
    searchToolBar->addAction(searchAct);

    navigationToolBar = addToolBar(tr("Navigation"));
    navigationToolBar->addAction(gotoLineAct);
    navigationToolBar->addAction(gotoMatchingBraceAct);

    buildToolBar = addToolBar(tr("Build"));
    buildToolBar->addAction(compileAct);
    buildToolBar->addSeparator();
    buildToolBar->addAction(emulatorAct);
    buildToolBar->addAction(killEmulatorAct);

    toolsToolBar = addToolBar(tr("Navigation"));
    toolsToolBar->addAction(exitAct);
}

//
// Show initial message in the app's status bar or change it on demand
//
void MainWindow::createStatusBarMessage(QString statusmessage, int timeout)
{
    statusBar()->showMessage(statusmessage, timeout);
}

void MainWindow::readPosSettings()
{
    QSettings settings("MB-SoftWorX", "Amiga Cross Editor");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

//
// read app's last saved prefs
//
void MainWindow::readSettings()
{
    QSettings settings("MB-SoftWorX", "Amiga Cross Editor");
    // TAB: Project
    p_author = (settings.value("Project/Author").toString());
    p_email = (settings.value("Project/Email").toString());
    p_website = (settings.value("Project/Website").toString());
    p_description = (settings.value("Project/Description").toString());
    p_purpose = (settings.value("Project/Purpose").toString());
    p_projectsRootDir = (settings.value("Project/ProjectRootDir").toString());
    p_default_icon = (settings.value("Project/DefaultIcon").toString());

    // TAB: GCC
    p_compiler_gcc = (settings.value("GCC/GccPath").toString());
    p_compiler_gpp = (settings.value("GCC/GppPath").toString());
    p_make = (settings.value("GCC/MakePath").toString());
    p_strip = (settings.value("GCC/StripPath").toString());
    p_compiler_gcc_call = (settings.value("GCC/GccDefaultOpts").toString());
    p_compiler_gpp_call = (settings.value("GCC/GppDefaultOpts").toString());
    p_show_gcc_opts = (settings.value("GCC/ShowGccDefaultOpts").toBool());

    // TAB: VBCC
    p_compiler_vc = (settings.value("VBCC/VcPath").toString());
    p_compiler_vasm = (settings.value("VBCC/VasmPath").toString());
    p_vbcc_config_dir = (settings.value("VBCC/VcConfigPath").toString());
    p_compiler_vc13_call = (settings.value("VBCC/VcDefaultOpts13").toString());
    p_compiler_vc30_call = (settings.value("VBCC/VcDefaultOpts30").toString());
    p_compiler_vc40_call = (settings.value("VBCC/VcDefaultOpts40").toString());
    p_compiler_vc_default_target = (settings.value("VBCC/VcDefaultTarget").toInt());
    p_show_vbcc_opts = (settings.value("VBCC/ShowVbccDefaultOpts").toBool());

    // TAB: Emulator
    p_emulator = (settings.value("UAE/UaePath").toString());
    p_os13_config = (settings.value("UAE/Os13ConfigPath").toString());
    p_os20_config = (settings.value("UAE/Os20ConfigPath").toString());
    p_os30_config = (settings.value("UAE/Os30ConfigPath").toString());
    p_os40_config = (settings.value("UAE/Os40ConfigPath").toString());
    p_defaultEmulator = (settings.value("UAE/DefaultConfig").toInt());

    // TAB: Misc
    p_default_style = (settings.value("MISC/DefaultStyle").toString());
    p_blackish = (settings.value("MISC/UseBlackishStyle").toBool());
    p_show_indentation = (settings.value("MISC/ShowIndentGuide").toBool());
    p_mydebug = (settings.value("MISC/ShowDebugOutput").toBool());
    p_no_lcd_statusbar = (settings.value("MISC/NoLCDstatusbar").toBool());
    p_defaultCompiler = (settings.value("MISC/DefaultCrossCompiler").toInt());
    p_no_compilerbuttons = (settings.value("MISC/NoCompileButton").toBool());
    p_simple_statusbar = (settings.value("MISC/SimpleStatusbar").toBool());
    p_create_icon = (settings.value("MISC/CreateIcon").toBool());
    p_console_on_fail = (settings.value("MISC/OpenConsoleOnFail").toBool());
    p_no_warn_requesters = (settings.value("MISC/NoWarnRequester").toBool());
}

//
// Store app's last known position and sizes
//
void MainWindow::writeSettings()
{
    QSettings settings("MB-SoftWorX", "Amiga Cross Editor");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

//
// react on changed document text, allow the user to
// decide if he wants to keep or abandon his work
//
bool MainWindow::maybeSave()
{
    if (textEdit->isModified()) {
        int ret = QMessageBox::warning(this, tr("C/C++ Amiga Cross Editor"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Yes | QMessageBox::Default,
                     QMessageBox::No,
                     QMessageBox::Cancel | QMessageBox::Escape);
        if (ret == QMessageBox::Yes)
            return save();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

//
// user defined loading for source files
//
void MainWindow::loadFile(const QString &fileName)
{
    if(p_mydebug)
    {
        qDebug() << "loadFile() called with parameter: " << fileName;
    }
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
    {
        QMessageBox::warning(this, tr("Amiga Cross Editor"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    textEdit->setText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

//
// user defined file creation loading for source files
// used for command line file loading
//
int MainWindow::loadNonExistantFile(const QString &fileName)
{
    int success = 1;

    // We need a qualified path in order to create and load the new file,
    // so let's build it:
    QString fileToOpen = QDir::currentPath();   // Path part of the filename
    fileToOpen.append(QDir::separator());       // add a unique separator after filename
    fileToOpen.append(fileName);                // add the fileName that we've requested on command line

    if(p_mydebug)
    {
        qDebug() << "created Filepath and -name: " << fileToOpen;
    }

    // Now let's create a QFile instance to open our file and maybe write to it!
    QFile file(fileToOpen);
    QFileInfo fileInfo(file);
    curFile = file.fileName();
    if(p_mydebug)
    {
        qDebug() << "Filepath given to the app: " << file.fileName();
    }

    // does our requested file allready exist?
    if (!file.open(QFile::ReadOnly))            // NO! File does NOT exist until now!
    {
        // Let's ask if it should be created!
        int ret = QMessageBox::question(this, tr("Amiga Cross Editor"),
                     tr("File does not exist: %1<br>"
                        "<br>Do you want me to create it?")
                                            .arg(fileName),
                     QMessageBox::Yes | QMessageBox::Default,
                     QMessageBox::Cancel | QMessageBox::Escape);

        // YES - we want the file to be created!
        if (ret == QMessageBox::Yes)
        {
            if(p_mydebug)
            {
                qDebug() << "Now creating requested file!";
            }

            if(file.open(QIODevice::WriteOnly))
            {
                if(p_mydebug)
                {
                    qDebug() << "File opened successfull for streaming...";
                }

                QTextStream stream(&file);                      // instanciate a stream to write to...
                stream << "/*\n * File:\t\t" << fileName;       // stream some comments into file..
                stream << "\n * Version:\t\t" << p_version;
                stream << "\n * Revision:\t\t" << p_revision;
                stream << "\n *";
                stream << "\n * Description:\t" << p_description;
                stream << "\n * Purpose:\t" << p_purpose;
                stream << "\n *";
                stream << "\n * Author:\t" << p_author;
                stream << "\n * Email:\t" << p_email;
                stream << "\n * Web:\t" << p_website;
                stream << "\n *";
                stream << "\n*/" << endl;

                // ...close the freshly created file so we will
                // be able to load it into the editor window!
                file.close();

                if(p_mydebug)
                {
                    qDebug() << "Status: " << stream.status();
                    qDebug() << curFile;
                }
                loadFile(curFile);     //...and finally open that file in editor window

                // jump to last line in file
                actionGotoBottom();

                return 0;
            }
            else
            {
                if(p_mydebug)
                {
                    qDebug() << "ERROR: Could not create file!";
                    qDebug() << curFile;
                }

                int ret = QMessageBox::warning(this, tr("Amiga Cross Editor"),
                             tr("<b>Something went terribly wrong!</b>"
                                "<br>File could <b>not</b> been created: %1<br>"
                                "<br>You will have to <b><i>save as...</i></b> an empty file,"
                                "<br>providing the requested file name.<br>"
                                "<br><b>Hint:</b> Selecting CANCEL quits AmigaED!")
                                                    .arg(fileName),
                                                    QMessageBox::Ok | QMessageBox::Cancel );
                if (ret == QMessageBox::Cancel )
                {
                    return 10;
                }
                else
                {
                    return -1;
                }

            }
        }
        // NO - abandon file creation and start with a new, empty C file
        else if (ret == QMessageBox::Cancel)
        {
            if(p_mydebug)
            {
                qDebug() << "File creation abandoned!";
            }
            newFile();
        }
    }

    if(p_mydebug)
    {
        qDebug() << "DEBUG: Now trying to load file into editor!";
    }
    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    textEdit->setText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 6000);

    // jump to last line in file
    actionGotoBottom();

    return success;
}

//
// save current file
//
bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("C/C++ Amiga Cross Editor"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << textEdit->text();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

//
// set current file name, placing name and status
// in app's window title
//
void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    textEdit->setModified(false);
    setWindowModified(false);

    QString shownName;
    if (curFile.isEmpty())
        shownName = "untitled.c";
    else
        shownName = strippedName(curFile);

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("C/C++ Amiga Cross Editor")));
}

//
// Strip path from current file name
//
QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

//
// react on toogle vbcc show options changed...
//
void MainWindow::actionToggleVbccDefaultOptsDialog()
{
    if(toggleVbccDefaultOptsAct->isChecked())
        p_show_vbcc_opts = true;
    else
        p_show_vbcc_opts = false;
}


//
// react on toogle gcc/g++ show options changed...
//
void MainWindow::actionToggleGccDefaultOptsDialog()
{
    if(toggleGccDefaultOptsAct->isChecked())
        p_show_gcc_opts = true;
    else
        p_show_gcc_opts = false;
}


//
// jump to line #1
//
// BUG: Shortcut not responding! To be fixed...
//
void MainWindow::actionGotoTop()
{
    const int i = 1;

    // check if text is folded!
    QsciScintilla::FoldStyle state = static_cast<QsciScintilla::FoldStyle>((!textEdit->folding()) * 5);
    // if folded: unfold first!!
    if (state > 0)
    {
        textEdit->foldAll(false);
    }
    textEdit->setCursorPosition(i-1,0);
    textEdit->setCaretLineVisible(true);
}

//
// jump to last line in text
//
// BUG: Shortcut not responding! To be fixed...
//
void MainWindow::actionGotoBottom()
{
    const int max_lines = textEdit->lines();    // number of lines in text!
    // check if text is folded!
    QsciScintilla::FoldStyle state = static_cast<QsciScintilla::FoldStyle>((!textEdit->folding()) * 5);
    // if folded: unfold first!!
    if (state > 0)
    {
        textEdit->foldAll(false);
    }
    textEdit->setCursorPosition(max_lines, 0);
}

//
// Jump to line No. X...
//
void MainWindow::actionGoto_Line()
{
    int max = textEdit->lines(); // max. count of lines in code window
    bool ok;
    int i = QInputDialog::getInt(this, tr("Goto line"),
                                 tr("Line number:"), 1, 0, max, 1, &ok);
    if (ok)
    {
        // check if text is folded!
        QsciScintilla::FoldStyle state = static_cast<QsciScintilla::FoldStyle>((!textEdit->folding()) * 5);

        // if folded: unfold first!!
        if (state > 0)
        {
            textEdit->foldAll(false);
        }
        textEdit->setCursorPosition(i-1,0);
    }
}

//
// jump to matching brace: {...}, [...], (...)
//
void MainWindow::actionGoto_matching_brace()
{
    textEdit->moveToMatchingBrace();
}

//
// select a compiler to use: VBCC
//
void MainWindow::actionSelectCompilerVBCC()
{
    qDebug() << "VBCC selection called.";
    if(!(p_no_compilerbuttons))    // react on user prefs: show or hide compiler combo and -button
    {
        compilerCombo->setCurrentIndex(0);
    }
    p_defaultCompiler = 0;
    SelectCompiler(0);
}

//
// select a compiler to use: gcc
//
void MainWindow::actionSelectCompilerGCC()
{
    qDebug() << "GCC selection called.";
    if(!(p_no_compilerbuttons))    // react on user prefs: show or hide compiler combo and -button
    {
        compilerCombo->setCurrentIndex(1);
    }
    p_defaultCompiler = 1;
    SelectCompiler(1);
}

//
// select a compiler to use: VBCC
//
void MainWindow::actionSelectCompilerGPP()
{
    qDebug() << "g++ selection called.";
    if(!(p_no_compilerbuttons))    // react on user prefs: show or hide compiler combo and -button
    {
        compilerCombo->setCurrentIndex(2);
    }
    p_defaultCompiler = 2;
    SelectCompiler(2);
}

//
// select a compiler to use (vbcc, gcc, g++)
//
void MainWindow::SelectCompiler(int index)
{
    if(p_mydebug)
    {
        qDebug() << "in SelectCompiler(index)";
        qDebug() << "index: " << index;
        qDebug() << "p_selected_compiler_args: " << p_selected_compiler_args;
        qDebug() << "\nCommand: " << p_defaultCompiler;
        qDebug() << "Arguments: " << p_compiler_call;

    }


    // Toggle statusbar combobox:
    if(!(p_no_compilerbuttons))    // react on user prefs: show or hide compiler combo and -button
    {
        this->compilerCombo->setCurrentIndex(index);
        switch(index)
        {
            case 0: // VBCC
                osCombo->setEnabled(true);
                p_defaultCompiler = 0;
                break;
            case 1: // GCC
                osCombo->setEnabled(false);
                p_defaultCompiler = 1;
                break;
            case 2: // G++
                osCombo->setEnabled(false);
                p_defaultCompiler = 2;
                break;
            }
    }

    switch(index)
    {
        // VBCC
        case 0:
            switch(p_compiler_vc_default_target)
            {
                // OS 1.3
                case 0:
                    p_selected_compiler_args = p_compiler_vc13_call;
                    break;
                // OS 3.x
                case 1:
                    p_selected_compiler_args = p_compiler_vc30_call;
                    break;
                // OS 4.1
                case 2:
                    p_selected_compiler_args = p_compiler_vc40_call;
                    break;
            }
            p_selected_compiler = p_compiler_vc;
            p_compiledFileSuffix = "_vc";
            // check selected menu item, uncheck others
            selectCompilerVBCCAct->setChecked(true);
            break;
         // GCC
        case 1:
            p_selected_compiler = p_compiler_gcc;
            p_selected_compiler_args = p_compiler_gcc_call;
            p_compiledFileSuffix = "_gcc";
            // check selected menu item, uncheck others
            selectCompilerGCCAct->setChecked(true);           
            break;
        // G++
        case 2:
            p_selected_compiler = p_compiler_gpp;
            p_selected_compiler_args = p_compiler_gpp_call;
            p_compiledFileSuffix = "_gpp";
            // check selected menu item, uncheck others
            selectCompilerGPPAct->setChecked(true);
            break;
    }

    if(p_mydebug)
    {
        qDebug() << "in SelectCompiler(index) AFTER index selection";
        qDebug() << "index: " << index;
        qDebug() << "p_selected_compiler_args: " << p_selected_compiler_args;
        qDebug() << "\nCommand: " << p_defaultCompiler;
        qDebug() << "Arguments: " << p_compiler_call;

    }
}

//
// Close Output console in Splitter...
//
void MainWindow::actionCloseOutputConsole()
{
    outputGroup->hide();
}

//
// OpenOutput console in Splitter...
//
void MainWindow::actionShowOutputConsole()
{
    outputGroup->show();
    jumpCompilerWarnings();
}

//
// Compile current file
// CHANGE THIS according to your compiler and opts!
//
int MainWindow::actionCompile()
{
    QString new_compiler_args;

    // check if we have a valid compiler to call:
    if(p_selected_compiler.isEmpty())
    {
        if(p_mydebug)
            qDebug() << "in actionCompile:\nNo compiler selected in prefs!\n";

        (void)QMessageBox::critical(this, tr("Amiga Cross Editor"),
        tr("There is a problem with your compiler presets!\n"
        "Please set preferences according to your compiler locations and default options.\n\n"
           "You need to restart Amiga Cross Editor afterwards!"),
        QMessageBox::Ok);
        if(selectCompilerGCCAct->isChecked() || selectCompilerGPPAct->isChecked())
            actionPrefsDialog(1);
        else
            actionPrefsDialog(2);

        return 1;
    }

    // check if we have valid compiler arguments in prefs:
    if(p_compiler_gcc_call.isEmpty() || p_compiler_gpp_call.isEmpty() || p_compiler_vc13_call.isEmpty() || p_compiler_vc30_call.isEmpty() || p_compiler_vc40_call.isEmpty())
    {
        if(p_mydebug)
            qDebug() << "in actionCompile:\nNo valid arguments selected in prefs!\n";

        (void)QMessageBox::critical(this, tr("Amiga Cross Editor"),
        tr("There is a problem with your compiler presets!\n"
        "Please set preferences according to your compiler locations and default options.\n\n"
           "You need to restart Amiga Cross Editor afterwards!"),
        QMessageBox::Ok);
        if(selectCompilerGCCAct->isChecked() || selectCompilerGPPAct->isChecked())
            actionPrefsDialog(1);
        else
            actionPrefsDialog(2);

        return 1;
    }

    // Don't start compiler on an empty source!
    if(!(textEdit->text().isEmpty()))
    {
        QString temp_compiler_call, mbox_title;
        QFileInfo file(curFile);

        // set approbriate title for QInputDialog:
        if(selectCompilerVBCCAct->isChecked())
        {
            if(p_mydebug)
                qDebug() << "Extension: " << file.suffix();

            if((file.suffix() == "cpp") || (file.suffix() == "CPP"))
            {
                if(p_mydebug)
                    qDebug() << "vbcc ERROR C++";

                selectCompilerVBCCAct->setChecked(false);
                selectCompilerGPPAct->setChecked(true);
                if(!(p_no_compilerbuttons))    // react on user prefs: show or hide compiler combo and -button
                {
                    compilerCombo->setCurrentIndex(2);
                }
                else
                {
                   if(selectCompilerGPPAct->isChecked())
                       SelectCompiler(2);
                }

                // give a user warning
                (void)QMessageBox::warning(this,
                               "Amiga Cross Editor", "VBCC does <i><b>NOT</b> permit</i> to compile <b><i>C++ sources!</i></b><br> "
                                "Compiler was set to <b>GNU g++</b> instead.<br>"
                                "<br>This usually makes more sense, ya know?!",
                                QMessageBox::Ok);

            } else {mbox_title = "vbcc";}
        }

        if(selectCompilerGCCAct->isChecked())
        {
            if((file.suffix() == "cpp") || (file.suffix() == "CPP"))
            {
                if(p_mydebug)
                    qDebug() << "gcc ERROR C++";

                selectCompilerGCCAct->setChecked(false);
                selectCompilerGPPAct->setChecked(true);
                if(!(p_no_compilerbuttons))    // react on user prefs: show or hide compiler combo and -button
                {
                    compilerCombo->setCurrentIndex(2);
                }
                else
                {
                   if(selectCompilerGPPAct->isChecked())
                       SelectCompiler(2);
                }


                // give a user warning
                (void)QMessageBox::warning(this,
                               "Amiga Cross Editor", "GCC does <i><b>NOT</b> permit</i> to compile <b><i>C++ sources!</i></b><br> "
                                "Compiler was set to <b>GNU g++</b> instead.<br>"
                                "<br>This usually makes more sense, ya know?!",
                                QMessageBox::Ok);

            }
            else
            {
                mbox_title = "m68k-amigaos-gcc";
            }
        }


        if(selectCompilerGPPAct->isChecked())
            mbox_title = "m68k-amigaos-g++";


        // 'text' holds selected compiler args, 'ok' determines if to use them
        QString text = p_selected_compiler_args;
        bool ok;

        if(p_mydebug)
            qDebug() << "START: Proc started " << p_proc_is_started << " times.";

        // Now let's check if user wants to see options dialog(s) for gcc, g++ and vc...
        if(selectCompilerGCCAct->isChecked() || selectCompilerGPPAct->isChecked())          // Are gcc or g++ selected default compiler...?
        {
            if(p_show_gcc_opts)
            {
                text = QInputDialog::getText(this, mbox_title,
                                                   tr("Compiler Options:"), QLineEdit::Normal,
                                                   p_selected_compiler_args, &ok);
            }
            else
            {
                ok = true;
                text = p_selected_compiler_args;
            }
        }
        else    // ...NO? So vc must be the selected default compiler!
        {
            if(p_show_vbcc_opts)
            {
                text = QInputDialog::getText(this, mbox_title,
                                                   tr("Compiler Options:"), QLineEdit::Normal,
                                                   p_selected_compiler_args, &ok);
            }
            else
            {
                ok = true;
                text = p_selected_compiler_args;
            }
        }   // END checking for show/hide default compiler opts dialog

        if (ok && !text.isEmpty())
        {
            new_compiler_args = text;   // store args if changed!
            qDebug() << "text in line 1850: " << text;
            save();
            QString outName = QFileInfo(curFile).baseName();
            QString outPath = QFileInfo(curFile).absolutePath();

            // construct path and name of compiled file for file checking:
            p_compiledFile = outPath + QDir::separator() + outName + p_compiledFileSuffix;

            if(p_mydebug)
                qDebug() << "compiled file: " << p_compiledFile;

            temp_compiler_call = p_selected_compiler_args;  // store compiler parameters temporarily
            text.append(" ");   // add one space to separate arguments!!
            text.append(curFile + " -o " + outPath + QDir::separator() + outName + p_compiledFileSuffix);        // add output file name

            if(p_mydebug)
                qDebug() << "Text not empty: " << text;

            p_selected_compiler_args = text;

            if(!(p_console_on_fail))
            {
                // make output window visible:
                actionShowOutputConsole();
            }

            if(p_mydebug)
            {
                qDebug() << "in actionCompile()";

                qDebug() << "p_selected_compiler_args: " << p_selected_compiler_args;
                qDebug() << "text: " << text;

            }

            //
            // put REAL compiler start HERE!
            // (uses p_compiler and p_compiler_call in mainwindow.h as default options)
            //
            timerCompile.start();
            startCompiler();
            nMilliseconds = timerCompile.elapsed();

            if(p_mydebug)
                qDebug() << "Compilation took " << nMilliseconds << "Milliseconds";


            // afterwards, reset everything to its defaults!
            text.clear();
            p_selected_compiler_args.clear();
            p_selected_compiler_args = temp_compiler_call;
        }
        else
        {
            text.append(curFile);
            p_compiler_call = text;

            if(p_mydebug)
            {
                qDebug() << "Text: " << text;
                qDebug() << "Compiler call: " << p_compiler_call;
            }
        }

        text.clear();
        }
    else
    {
        // give a user warning
        (void)QMessageBox::critical(this,
                       "Amiga Cross Editor", "It makes <i><b>no sense</b></i> to compile <b><i>empty source files!</i></b><br> "
                        "Compilation was <b>terminated</b> instead.<br>"
                        "<br>This usually makes more sense, ya know?!",
                        QMessageBox::Ok);
    }

    // keep changes if any...
    p_selected_compiler_args = new_compiler_args;

    return 0;
}

//
// Show or hide debugging output
// Value is stored in p_mydebug
//
void MainWindow::actionShowDebug()
{
    if(showDebugInfoAct->isChecked())
    {
        p_mydebug = true;
    }
    else
        p_mydebug = false;

    if(p_mydebug)
        qDebug() << "p_mydebug = " << p_mydebug;
}

//
// Show or hide indentaation guides
//
void MainWindow::actionShowIndentationGuides()
{
    if(showIndentationGuidesAct->isChecked())
    {
        textEdit->setIndentationGuides(true);
    }
    else
    {
        textEdit->setIndentationGuides(false);
    }
}

/* ------------ Begin insertMenue Actions -----------------*/
//
// Insert #include <file.h>
//
void MainWindow::actionInsertInclude()
{
    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // ...now insert our text:
    textEdit->insert("#include    <some_header.h>");
    // finally, move caret to next line.
    textEdit->setCursorPosition(line, index + 13);
}

//
// this will insert some of the #includes mostly used for Amiga app development
//
void MainWindow::actionInsertAmigaIncludes()
{
    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // we need some #includes, so let's insert them:
    textEdit->insertAt("\n", ++line, 0);  // insert empty line!
    textEdit->insertAt("/* ------------- OS 3.x  INCLUDE FILES ---------------------------------- */\n", ++line, 0);
    textEdit->insertAt("/* --- typical standard headers --- */\n", ++line, 0);
    textEdit->insertAt("#include\t<stdio.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<stdlib.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<string.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<ctype.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<stdarg.h>\n", ++line, 0);
    textEdit->insertAt("\n", ++line, 0);  // insert empty line!
    textEdit->insertAt("/* --- Amiga stuff ---------------- */\n", ++line, 0);
    textEdit->insertAt("#include\t<exec/exec.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<dos/dos.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<dos/dostags.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<dos/dosextens.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<dos/datetime.h>\n", ++line, 0);
    textEdit->insertAt("\n", ++line, 0);  // insert empty line!
    textEdit->insertAt("#include\t<graphics/gfx.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<graphics/gfxmacros.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<graphics/layers.h>\n", ++line, 0);
    textEdit->insertAt("\n", ++line, 0);  // insert empty line!
    textEdit->insertAt("#include\t<intuition/intuition.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<intuition/intuitionbase.h>\n", ++line, 0);
    textEdit->insertAt("\n", ++line, 0);  // insert empty line!
    textEdit->insertAt("#include\t<workbench/workbench.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<workbench/startup.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<workbench/icon.h>\n", ++line, 0);
    textEdit->insertAt("\n", ++line, 0);  // insert empty line!
    textEdit->insertAt("#include\t<datatypes/pictureclass.h>\n", ++line, 0);
    textEdit->insertAt("\n", ++line, 0);  // insert empty line!
    textEdit->insertAt("#include\t<libraries/asl.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<libraries/commodities.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<libraries/gadtools.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<libraries/iffparse.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<libraries/locale.h>\n", ++line, 0);
    textEdit->insertAt("\n", ++line, 0);  // insert empty line!
    textEdit->insertAt("#include\t<rexx/rxslib.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<rexx/storage.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<rexx/errors.h>\n", ++line, 0);
    textEdit->insertAt("\n", ++line, 0);  // insert empty line!
    textEdit->insertAt("#include\t<utility/hooks.h>\n", ++line, 0);
    textEdit->insertAt("\n", ++line, 0);  // insert empty line!
    textEdit->insertAt("/* --- protos ----------------- */\n", ++line, 0);
    textEdit->insertAt("#if !defined(__MAXON__)\n", ++line, 0);
    textEdit->insertAt("#include\t<proto/asl.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<proto/commodities.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<proto/datatypes.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<proto/diskfont.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<proto/dos.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<proto/exec.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<proto/gadtools.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<proto/graphics.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<proto/icon.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<proto/iffparse.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<proto/intuition.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<proto/layers.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<proto/locale.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<proto/rexxsyslib.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<proto/utility.h>\n", ++line, 0);
    textEdit->insertAt("#include\t<proto/wb.h>\n", ++line, 0);
    textEdit->insertAt("#endif\n", ++line, 0);

    // finally, move caret to next line.
    textEdit->setCursorPosition(line, index);
}

//
// Insert #define SOME_VALUE 0
//
void MainWindow::actionInsertDefine()
{
    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // ...now insert our text:
    textEdit->insert("#define\tSOME_VALUE\t0\n");
    // finally, move caret to next line.
    textEdit->setCursorPosition(line + 1, index);
    qDebug() << "leaving actionInsertDefine now...";
}

//
// Insert #ifdef ... #endif
//
void MainWindow::actionInsertIfdef()
{
    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // ...now insert the first line of text!
    textEdit->insert("\n");
    // next, we need to continue printing at a certain location:
    textEdit->insertAt("#ifdef __SOME_DEFINITION__\n", ++line, 0);
    textEdit->insertAt("\t/* some_action */\n", ++line, 0);
    textEdit->insertAt("\t\n", ++line, 0);
    textEdit->insertAt("#endif\n", ++line, 0);

    // finally, we set our caret to the next following empty line!
    textEdit->setCursorPosition(line + 1, index);
}

//
// Insert #if defined(..) ... #elif defined(...) ... #endif
//
void MainWindow::actionInsertIfdefined()
{
    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // ...now insert the first line of text!
    textEdit->insert("\n");
    // next, we need to continue printing at a certain location:
    textEdit->insertAt("#if defined(__SOME_DEFINITION__)\n", ++line, 0);
    textEdit->insertAt("\t/* some_action */\n", ++line, 0);
    textEdit->insertAt("#elif defined(__SOME_OTHER_DEFINITION__)\n", ++line, 0);
    textEdit->insertAt("\t/* some_other_action */\n", ++line, 0);
    textEdit->insertAt("#else\n", ++line, 0);
    textEdit->insertAt("\t/* nothing matched... */\n", ++line, 0);
    textEdit->insertAt("\t\n", ++line, 0);
    textEdit->insertAt("#endif\n", ++line, 0);

    // finally, we set our caret to the next following empty line!
    textEdit->setCursorPosition(line + 1, index);
}

//
// Inserts a compiler check via #if defined(..) ... #elif defined(...) ...#else(...) ... #endif
//
void MainWindow::actionInsertIfdefinedCompiler()
{
    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // ...now insert the first line of text!
    textEdit->insert("\n");
    // next, we need to continue printing at a certain location:
    textEdit->insertAt("#if defined(__STORM__)\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler is StormC3 */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"compiled with StormC3.\\n\\n\");\n", ++line, 0);
    textEdit->insertAt("#elif defined(__STORMGCC__)\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler is StormGCC4 */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"compiled with GNU gcc, StormC4 flavour.\\n\\n\");\n", ++line, 0);
    textEdit->insertAt("#elif defined(__MAXON__)\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler is Maxon/HiSoft C++ */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"compiled with Maxon/HiSoft C++.\\n\\n\");\n", ++line, 0);
    textEdit->insertAt("#elif defined(__GNUC__)\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler is gcc */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"compiled with GNU gcc v%d.%d Patchlevel %d.\\n\\n\", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);\n", ++line, 0);
    textEdit->insertAt("#elif defined(__VBCC__)\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler is vbcc */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"compiled with vbcc.\\n\\n\");\n", ++line, 0);
    textEdit->insertAt("#elif defined(__SASC)\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler is SAS/C */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"compiled with SAS/C.\\n\\n\");\n", ++line, 0);
    textEdit->insertAt("#elif defined(LATTICE)\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler is Lattice C */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"compiled with Lattice C.\\n\\n\");\n", ++line, 0);
    textEdit->insertAt("#elif defined(AZTEC_C)\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler is Aztec C */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"compiled with Manx Aztec C.\\n\\n\");\n", ++line, 0);
    textEdit->insertAt("#elif defined(_DCC)\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler is dice */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"compiled with dice.\\n\\n\");\n", ++line, 0);
    textEdit->insertAt("#else\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler not identified */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"Compiler was not identified.\\n\\n\");\n", ++line, 0);
    textEdit->insertAt("#endif\n", ++line, 0);

    // finally, we set our caret to the next following empty line!
    textEdit->setCursorPosition(line + 1, index);
}

//
// Insert #ifndef ... #endif
//
void MainWindow::actionInsertIfndef()
{
    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // ...now insert the first line of text!
    textEdit->insert("\n");
    // next, we need to continue printing at a certain location:
    textEdit->insertAt("#ifndef __SOME_DEFINITION__\n", ++line, 0);
    textEdit->insertAt("\t/* some_action */\n", ++line, 0);
    textEdit->insertAt("\t\n", ++line, 0);
    textEdit->insertAt("#endif\n", ++line, 0);

    // finally, we set our caret to the next following empty line!
    textEdit->setCursorPosition(line + 1, index);
}

//
// Insert OpenLibrary("some.library", 0L)
//
void MainWindow::actionInsertOpenLibrary()
{
    qDebug() << "in OpenLibrary";
    popNotImplemented();
}

//
// Insert CloseLibrary("some.library")
//
void MainWindow::actionInsertCloseLibrary()
{
    qDebug() << "in CloseLibrary";
    popNotImplemented();
}

//
// Insert if(...){...}
//
void MainWindow::actionInsertIf()
{
    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // ...now insert the first line of text!
    textEdit->insert("\n");
    // next, we need to continue printing at a certain location:
    textEdit->insertAt("if( condition )\n", ++line, 0);
    textEdit->insertAt("{\n", ++line, 0);
    textEdit->insertAt("\t/* some_action */\n", ++line, 0);
    textEdit->insertAt("\t\n", ++line, 0);
    textEdit->insertAt("}\n", ++line, 0);

    // finally, we set our caret to the next following empty line!
    textEdit->setCursorPosition(line + 1, index);
}

//
// Insert if(...){...} else{..}
//
void MainWindow::actionInsertIfElse()
{
    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // ...now insert the first line of text!
    textEdit->insert("\n");
    // next, we need to continue printing at a certain location:
    textEdit->insertAt("if( condition )\n", ++line, 0);
    textEdit->insertAt("{\n", ++line, 0);
    textEdit->insertAt("\t/* some_action */\n", ++line, 0);
    textEdit->insertAt("\t\n", ++line, 0);
    textEdit->insertAt("}\n", ++line, 0);
    textEdit->insertAt("else\n", ++line, 0);
    textEdit->insertAt("{\n", ++line, 0);
    textEdit->insertAt("\t/* some_other_action */\n", ++line, 0);
    textEdit->insertAt("\t\n", ++line, 0);
    textEdit->insertAt("}\n", ++line, 0);

    // finally, we set our caret to the next following empty line!
    textEdit->setCursorPosition(line + 1, index);
}

//
// Insert while(...){...}
//
void MainWindow::actionInsertWhile()
{
    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // ...now insert the first line of text!
    textEdit->insert("\n");
    // next, we need to continue printing at a certain location:
    textEdit->insertAt("int var = 0;\n", ++line, 0);
    textEdit->insertAt("while( condition )\n", ++line, 0);
    textEdit->insertAt("{\n", ++line, 0);
    textEdit->insertAt("\t/* some_action */\n", ++line, 0);
    textEdit->insertAt("\t\n", ++line, 0);
    textEdit->insertAt("}\n", ++line, 0);

    // finally, we set our caret to the next following empty line!
    textEdit->setCursorPosition(line + 1, index);
}

//
// Insert for(...){...} loop
//
void MainWindow::actionInsertForLoop()
{
    qDebug() << "in while do";
    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // ...now insert the first line of text!
    textEdit->insert("\n");
    // next, we need to continue printing at a certain location:
    textEdit->insertAt("int var = 0;\n", ++line, 0);
    textEdit->insertAt("for( var = 0; var < 5; var++ )\n", ++line, 0);
    textEdit->insertAt("{\n", ++line, 0);
    textEdit->insertAt("\t/* some_action */\n", ++line, 0);
    textEdit->insertAt("\t\n", ++line, 0);
    textEdit->insertAt("}\n", ++line, 0);

    // finally, we set our caret to the next following empty line!
    textEdit->setCursorPosition(line + 1, index);
}

//
// Insert do{...}while(...)
//
void MainWindow::actionInsertDoWhile()
{
    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // ...now insert the first line of text!
    textEdit->insertAt("\n", line, index);
    // next, we need to continue printing at a certain location:
    textEdit->insertAt("int var = 0;\r", ++line, index);
    textEdit->insertAt("do\r", ++line, index);
    textEdit->insertAt("{\r", ++line, index);
    textEdit->insertAt("\t/* some_action */\n", ++line, index);
    textEdit->insertAt("\t\n", ++line, index);
    textEdit->insertAt("} while( condition );\n", ++line, index);

    // finally, we set our caret to the next following empty line!
    textEdit->setCursorPosition(line + 1, index);
}

//
// Insert switch(...){select case...break}
//
void MainWindow::actionInsertSwitch()
{
    qDebug() << "in switch";
    popNotImplemented();
}

//
// Insert int main(int argc, char *argv[]){...}
//
void MainWindow::actionInsertMain()
{
    // is there allready a main() function in this file?
    if(!(p_main_set))
    {
    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // ...now insert the first line of text!
    textEdit->insert("\n");
    // next, we need to continue printing at a certain location:
    textEdit->insertAt("/*************************\n", ++line, 0);
    textEdit->insertAt(" **	main() function    **\n", ++line, 0);
    textEdit->insertAt(" ************************/\n", ++line, 0);
    textEdit->insertAt("int main(int argc, char* argv[])\n", ++line, 0);
    textEdit->insertAt("{\n", ++line, 0);
    textEdit->insertAt("\t/* TODO: Write your code! */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"Now produce something usefull!\\n\");\n", ++line, 0);
    textEdit->insertAt("\n\n\treturn(0);\n}\n", ++line, 0);

    // finally, we set our caret to the place where editing might start
    textEdit->setCursorPosition(line - 1, index + 2);
    p_main_set = true;
    }
    else
    {
        (void)QMessageBox::information(this,
                       "Amiga Cross Editor", "It seems there is allready a <i><b>main() </b>function</i> in this document!<br> "
                        "It makes absolutely <b>no sense</b> to add another one."
                        "<br>Insertion will be cancelled, ya know?!",
                        QMessageBox::Ok);

    }

}

//
// Insert a partially functional App skeletton
// with Fileheader, #includes, versionstring and main() function
//
void MainWindow::actionInsertShellAppSkeletton()
{
    int ret = QMessageBox::warning(this, tr("Amiga Cross Editor"),
                 tr("<b>WARNING:</b> This will abandon your current document and "
                    "create a new file containing an <i>AmigaShell app template!</i>\n"
                    "<br>You will be asked to save your file under a new name first.</br>\n"
                    "<br><br>Do you want to continue?</br></br>"),
                 QMessageBox::Yes | QMessageBox::Default,
                 QMessageBox::No | QMessageBox::Escape);
    if (ret == QMessageBox::Yes)
    {
        qDebug() << "text in editor: " << textEdit->text().isEmpty();

        // if document allready contains text...
        if(!(textEdit->text().isEmpty()))
        {
            qDebug() << "Text not empty!";
            // ...empty it!
            textEdit->setText("");
        }

        // Let's save our template with a new name first!
        saveAs();
        qDebug() << "Filename after creation: " << curFile;
        QFileInfo file(curFile);
        qDebug() << "File: " << file.baseName();

        // Now let's build a fileheader containing some information
        int line, index;
        textEdit->getCursorPosition(&line, &index); // get the position...

        // ...now insert the first line of text!
        textEdit->insert("/*\n");
        // next, we need to continue printing at a certain location:
        textEdit->insertAt(" *\tFile:\t\t " + strippedName(curFile) + "\n", ++line, 0);
        textEdit->insertAt(" *\tProgram:\t\t " + file.baseName() + "\n", ++line, 0);
        textEdit->insertAt(" *\tVersion:\t\t1.0\n", ++line, 0);
        textEdit->insertAt(" *\tRevision:\t\t0\n", ++line, 0);
        textEdit->insertAt(" *\n", ++line, 0);
        textEdit->insertAt(" *\tDescription:\t\t" + p_description + "\n", ++line, 0);
        textEdit->insertAt(" *\tPurpose:\t\t\t" + p_purpose + "\n", ++line, 0);
        textEdit->insertAt(" *\n", ++line, 0);
        textEdit->insertAt(" *\tAuthor:\t\t" + p_author + "\n", ++line, 0);
        textEdit->insertAt(" *\tEmail:\t\t" + p_email + "\n", ++line, 0);
        textEdit->insertAt(" *\tWeb:\t\t" + p_website + "\n", ++line, 0);
        textEdit->insertAt(" */\n", ++line, 0);
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!

        // Let's do some versioning...
        textEdit->insertAt("/* ------------- AUTHOR, PROGNAME & VERSION ----------------------------------- */\n", ++line, 0);
        textEdit->insertAt("#define AUTHOR\t\t\"by " + p_author + "\""  + "\n", ++line, 0);
        textEdit->insertAt("#define PROGRAMNAME\t\t""\"" + file.baseName() + "\"" + "\n", ++line, 0);
        textEdit->insertAt("#define VERSION\t\t\t\t1\n", ++line, 0);
        textEdit->insertAt("#define REVISION\t\t\t\t0\n", ++line, 0);
        textEdit->insertAt("#define VERSIONSTRING\t\t\"1.0\"\n", ++line, 0);


        textEdit->insertAt("\n", ++line, 0);  // insert empty line!

        // we need some #includes, though...
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!
        textEdit->insertAt("/* ------------- INCLUDE FILES ----------------------------------------------- */\n", ++line, 0);
        textEdit->insertAt("#include\t<exec/exec.h>\n", ++line, 0);
        textEdit->insertAt("#include\t<dos/dos.h>\n", ++line, 0);
        textEdit->insertAt("#include\t<dos/dostags.h>\n", ++line, 0);
        textEdit->insertAt("#include\t<dos/datetime.h>\n", ++line, 0);
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!
        textEdit->insertAt("#include\t<intuition/intuition.h>\n", ++line, 0);
        textEdit->insertAt("#include\t<intuition/intuitionbase.h>\n", ++line, 0);
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!
        textEdit->insertAt("#include\t<libraries/asl.h>\n", ++line, 0);
        textEdit->insertAt("#include\t<libraries/locale.h>\n", ++line, 0);
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!
        textEdit->insertAt("/* --- protos ----------------- */\n", ++line, 0);
        textEdit->insertAt("#if !defined(__MAXON__)\n", ++line, 0);
        textEdit->insertAt("#include\t<proto/asl.h>\n", ++line, 0);
        textEdit->insertAt("#include\t<proto/dos.h>\n", ++line, 0);
        textEdit->insertAt("#include\t<proto/exec.h>\n", ++line, 0);
        textEdit->insertAt("#include\t<proto/intuition.h>\n", ++line, 0);
        textEdit->insertAt("#include\t<proto/locale.h>\n", ++line, 0);
        textEdit->insertAt("#endif\n", ++line, 0);
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!
        textEdit->insertAt("/* --- console output --- */\n", ++line, 0);
        textEdit->insertAt("#include\t<stdio.h>\n", ++line, 0);


        // it's good style to provide function prototypes...
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!
        textEdit->insertAt("/* ------------- FUNCTION PROTOS -------------------------------------- */\n", ++line, 0);
        textEdit->insertAt("extern int main(int argc, char* argv[]);\n", ++line, 0);

        // ok - let's build an Amiga-style version tag:
        QString sas_versionstring = "\tconst UBYTE VersionTag[] = \"$VER: \" PROGRAMNAME \" \" VERSIONSTRING \" \" AUTHOR \" \" __AMIGADATE__ \"\\n\\0\";";
        QString dice_versionstring = "\tconst UBYTE VersionTag[] = \"$VER: \" PROGRAMNAME \" \" VERSIONSTRING \" \" AUTHOR \" (\" __COMMODORE_DATE__ \")\\n\\0\";";
        QString other_versionstring = "\tconst UBYTE VersionTag[] = \"$VER: \" PROGRAMNAME \" \" VERSIONSTRING \" \" AUTHOR \" (\" __DATE__ \")\\n\\0\";";

        textEdit->insertAt("\n", ++line, 0);  // insert empty line!
        textEdit->insertAt("/* ------------- VERSION TAG -------------------------------------- */\n", ++line, 0);
        textEdit->insertAt("#if defined(__SASC)\n", ++line, 0);
        textEdit->insertAt(sas_versionstring + "\n", ++line, 0);
        textEdit->insertAt("#elif defined(_DCC)\n", ++line, 0);
        textEdit->insertAt(dice_versionstring + "\n", ++line, 0);
        textEdit->insertAt("#else\n", ++line, 0);
        textEdit->insertAt(other_versionstring + "\n", ++line, 0);
        textEdit->insertAt("#endif\n", ++line, 0);

        // ...finally, we need a main() function!
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!
        textEdit->insertAt("/*************************\n", ++line, 0);
        textEdit->insertAt(" **	main() function    **\n", ++line, 0);
        textEdit->insertAt(" ************************/\n", ++line, 0);
        textEdit->insertAt("int main(int argc, char* argv[])\n", ++line, 0);
        textEdit->insertAt("{\n", ++line, 0);
        textEdit->insertAt("\t/* TODO: Write your code! */\n", ++line, 0);
        textEdit->insertAt("\tprintf(\"\\nI am an AmigaShell program!\\n\");\n", ++line, 0);
        textEdit->insertAt("\tprintf(\"%s\\n\", VersionTag);\n", ++line, 0);
        textEdit->insertAt("\n\n\treturn(0);\n}\n", ++line, 0);

        // we're gonna save our freshly created file with its automated changes now...
        saveFile(curFile);

        // disable insertion of main() and versionstring
        p_main_set = true;
        p_versionstring_set = true;

        // we're gona give the user some information on how to edit the template
        (void)QMessageBox::information(this,
                           "Amiga Cross Editor", "You have successfully created a <i><b>AmigaShell </b>app template!</i><br> "
                            "Now you may edit the file according to your needs."
                            "<br>A good starting point would be to change your application's PROGRAMNAME, VERSION, REVISION and VERSIONSTRING definitions!",
                            QMessageBox::Ok);

        // now let's hump to the top of the document, so the user can revise and edit his template.
        actionGotoTop();


    }
    else if (ret == QMessageBox::Cancel)
    {
        //return 1;
    }
}

//
// Insert a partially functional C App skeletton
// with Fileheader, #includes and main() function
//
void MainWindow::actionInsertCAppSkeletton()
{
    int ret = QMessageBox::warning(this, tr("Amiga Cross Editor"),
                 tr("<b>WARNING:</b> This will abandon your current document and "
                    "create a new file containing an <i>standard console ANSI C app template!</i>\n"
                    "<br>You will be asked to save your file under a new name first.</br>\n"
                    "<br><br>Do you want to continue?</br></br>"),
                 QMessageBox::Yes | QMessageBox::Default,
                 QMessageBox::No | QMessageBox::Escape);
    if (ret == QMessageBox::Yes)
    {
        qDebug() << "text in editor: " << textEdit->text().isEmpty();

        // if document allready contains text...
        if(!(textEdit->text().isEmpty()))
        {
            qDebug() << "Text not empty!";
            // ...empty it!
            textEdit->setText("");
        }

        // Let's save our template with a new name first!
        saveAs();
        qDebug() << "Filename after creation: " << curFile;
        QFileInfo file(curFile);

        // Now let's build a fileheader containing some information
        int line, index;
        textEdit->getCursorPosition(&line, &index); // get the position...

        // ...now insert the first line of text!
        textEdit->insert("/*\n");
        // next, we need to continue printing at a certain location:
        textEdit->insertAt(" *\tFile:\t\t " + strippedName(curFile) + "\n", ++line, 0);
        textEdit->insertAt(" *\tProgram:\t\t " + file.baseName() + "\n", ++line, 0);
        textEdit->insertAt(" *\tVersion:\t\t1.0\n", ++line, 0);
        textEdit->insertAt(" *\tRevision:\t\t0\n", ++line, 0);
        textEdit->insertAt(" *\n", ++line, 0);
        textEdit->insertAt(" *\tDescription:\t\t" + p_description + "\n", ++line, 0);
        textEdit->insertAt(" *\tPurpose:\t\t" + p_purpose + "\n", ++line, 0);
        textEdit->insertAt(" *\n", ++line, 0);
        textEdit->insertAt(" *\tAuthor:\t\t" + p_author + "\n", ++line, 0);
        textEdit->insertAt(" *\tEmail:\t\t" + p_email + "\n", ++line, 0);
        textEdit->insertAt(" *\tWeb:\t\t" + p_website + "\n", ++line, 0);
        textEdit->insertAt(" */\n", ++line, 0);
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!

        // we need some #includes, though...
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!
        textEdit->insertAt("/* ------------- INCLUDE FILES ----------------------------------------------- */\n", ++line, 0);
        textEdit->insertAt("#include\t<stdio.h>\n", ++line, 0);


        // it's good style to provide function prototypes...
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!
        textEdit->insertAt("/* ------------- FUNCTION PROTOS -------------------------------------- */\n", ++line, 0);
        textEdit->insertAt("extern int main(int argc, char* argv[]);\n", ++line, 0);

        // ...finally, we need a main() function!
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!
        textEdit->insertAt("/*************************\n", ++line, 0);
        textEdit->insertAt(" **	main() function    **\n", ++line, 0);
        textEdit->insertAt(" ************************/\n", ++line, 0);
        textEdit->insertAt("int main(int argc, char* argv[])\n", ++line, 0);
        textEdit->insertAt("{\n", ++line, 0);
        textEdit->insertAt("\t/* TODO: Write your code! */\n", ++line, 0);
        textEdit->insertAt("\tprintf(\"\\nI am an ANSI C console program!\\n\");\n", ++line, 0);
        textEdit->insertAt("\n\treturn(0);\n}\n", ++line, 0);

        // we're gonna save our freshly created file with its automated changes now...
        saveFile(curFile);

        // we're gona give the user some information on how to edit the template
        (void)QMessageBox::information(this,
                           "Amiga Cross Editor", "You have successfully created a <i><b>ANSI C </b>app template!</i><br> "
                            "Now you may edit the file according to your needs.",
                            QMessageBox::Ok);

        // now let's hump to the top of the document, so the user can revise and edit his template.
        actionGotoTop();


    }
    else if (ret == QMessageBox::Cancel)
    {
        //return 1;
    }
}

//
// Insert a partially functional C++ App skeletton
// with Fileheader, #includes and main() function
//
void MainWindow::actionInsertCppAppSkeletton()
{
    int ret = QMessageBox::warning(this, tr("Amiga Cross Editor"),
                 tr("<b>WARNING:</b> This will abandon your current document and "
                    "create a new file containing an <i>standard console C++ app template!</i>\n"
                    "<br>You will be asked to save your file under a new name first.</br>\n"
                    "<br><br>Do you want to continue?</br></br>"),
                 QMessageBox::Yes | QMessageBox::Default,
                 QMessageBox::No | QMessageBox::Escape);
    if (ret == QMessageBox::Yes)
    {
        qDebug() << "text in editor: " << textEdit->text().isEmpty();

        // if document allready contains text...
        if(!(textEdit->text().isEmpty()))
        {
            qDebug() << "Text not empty!";
            // ...empty it!
            textEdit->setText("");
        }

        // Let's save our template with a new name first!
        saveAs();
        qDebug() << "Filename after creation: " << curFile;
        QFileInfo file(curFile);

        // Now let's build a fileheader containing some information
        int line, index;
        textEdit->getCursorPosition(&line, &index); // get the position...

        // ...now insert the first line of text!
        textEdit->insert("/*\n");
        // next, we need to continue printing at a certain location:
        textEdit->insertAt(" *\tFile:\t\t " + strippedName(curFile) + "\n", ++line, 0);
        textEdit->insertAt(" *\tProgram:\t\t " + file.baseName() + "\n", ++line, 0);
        textEdit->insertAt(" *\tVersion:\t\t1.0\n", ++line, 0);
        textEdit->insertAt(" *\tRevision:\t\t0\n", ++line, 0);
        textEdit->insertAt(" *\n", ++line, 0);
        textEdit->insertAt(" *\tDescription:\t\t" + p_description + "\n", ++line, 0);
        textEdit->insertAt(" *\tPurpose:\t\t" + p_purpose + "\n", ++line, 0);
        textEdit->insertAt(" *\n", ++line, 0);
        textEdit->insertAt(" *\tAuthor:\t\t" + p_author + "\n", ++line, 0);
        textEdit->insertAt(" *\tEmail:\t\t" + p_email + "\n", ++line, 0);
        textEdit->insertAt(" *\tWeb:\t\t" + p_website + "\n", ++line, 0);
        textEdit->insertAt(" */\n", ++line, 0);
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!

        // we need some #includes, though...
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!
        textEdit->insertAt("/* ------------- INCLUDE FILES ----------------------------------------------- */\n", ++line, 0);
        textEdit->insertAt("#include\t<iostream>\n", ++line, 0);

        // we are in namespace <std>
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!
        textEdit->insertAt("using namespace std;\n", ++line, 0);

        // ...finally, we need a main() function!
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!
        textEdit->insertAt("/*************************\n", ++line, 0);
        textEdit->insertAt(" **	main() function    **\n", ++line, 0);
        textEdit->insertAt(" ************************/\n", ++line, 0);
        textEdit->insertAt("int main(int argc, char* argv[])\n", ++line, 0);
        textEdit->insertAt("{\n", ++line, 0);
        textEdit->insertAt("\t/* TODO: Write your code! */\n", ++line, 0);
        textEdit->insertAt("\tcout << \"\\nI am an standard C++ console program!\\n\" << endl;\n", ++line, 0);
        textEdit->insertAt("\n\treturn(0);\n}\n", ++line, 0);

        // we're gonna save our freshly created file with its automated changes now...
        saveFile(curFile);

        // we're gona give the user some information on how to edit the template
        (void)QMessageBox::information(this,
                           "Amiga Cross Editor", "You have successfully created a <i><b>C++ </b>app template!</i><br> "
                            "Now you may edit the file according to your needs.",
                            QMessageBox::Ok);


        // now let's hump to the top of the document, so the user can revise and edit his template.
        actionGotoTop();

    }
    else if (ret == QMessageBox::Cancel)
    {
        //return 1;
    }
}

//
// Insert C function skelleton
//
void MainWindow::actionInsertFunction()
{
    qDebug() << "in Function";

    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // ...now insert the first line of text!
    textEdit->insert("\n");
    // next, we need to continue printing at a certain location:

    // first, let's define a function prototype:
    textEdit->insertAt("/*\n", ++line, 0);
    textEdit->insertAt(" * -- Function prototype --\n", ++line, 0);
    textEdit->insertAt(" * EDIT, then cut & paste BEFORE main() !\n", ++line, 0);
    textEdit->insertAt(" */\n", ++line, 0);
    textEdit->insertAt("int some_function(int arg1, int arg2);\n", ++line, 0);
    textEdit->insertAt("\n", ++line, 0);
    // now let's create the function...
    textEdit->insertAt("/*\n", ++line, 0);
    textEdit->insertAt(" *\tName:\tint some_function(int arg1, int arg2)\n", ++line, 0);
    textEdit->insertAt(" *\tPurpose:\tdo something usefull...\n", ++line, 0);
    textEdit->insertAt(" */\n", ++line, 0);
    textEdit->insertAt("int some_function(int arg1, int arg2)\n", ++line, 0);
    textEdit->insertAt("{\n", ++line, 0);
    textEdit->insertAt("\t/* TODO: Write your code! */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"Now let your function do some work...\\n\");\n", ++line, 0);
    textEdit->insertAt("\n\treturn(0);\n}\n", ++line, 0);

    // finally, we set our caret to the place where editing might start
    textEdit->setCursorPosition(line - 1, index + 1);
}

//
// Insert C enumeration
//
void MainWindow::actionInsertEnum()
{
    qDebug() << "in Enum";
    popNotImplemented();
}

//
// Insert C custom data structure
//
void MainWindow::actionInsertStruct()
{
    qDebug() << "in Struct";
    popNotImplemented();
}

//
// Insert C pseudo class: typedef struct{...}class_something
//
void MainWindow::actionInsertCClass()
{
    qDebug() << "in CClass";
    popNotImplemented();
}

//
// Insert C++ class
//
void MainWindow::actionInsertCppClass()
{
    qDebug() << "in CppClass";
    popNotImplemented();
}

//
// Insert fileheader comment...
//
void MainWindow::actionInsertFileheaderComment()
{
    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // ...now insert the first line of text!
    textEdit->insert("/*\n");
    // next, we need to continue printing at a certain location:
    textEdit->insertAt(" *\tFile:\t\t " + strippedName(curFile)+ "\n", ++line, 0);
    textEdit->insertAt(" *\tVersion:\t\t1.0\n", ++line, 0);
    textEdit->insertAt(" *\tRevision:\t\t0\n", ++line, 0);
    textEdit->insertAt(" *\n", ++line, 0);
    textEdit->insertAt(" *\tDescription:\t\t" + p_description + "\n", ++line, 0);
    textEdit->insertAt(" *\tPurpose:\t\t" + p_purpose + "\n", ++line, 0);
    textEdit->insertAt(" *\n", ++line, 0);
    textEdit->insertAt(" *\tAuthor:\t\t" + p_author + "\n", ++line, 0);
    textEdit->insertAt(" *\tEmail:\t\t" + p_email + "\n", ++line, 0);
    textEdit->insertAt(" *\tWeb:\t\t" + p_website + "\n", ++line, 0);
    textEdit->insertAt(" */\n", ++line, 0);

    // finally, we set our caret to the next following empty line!
    textEdit->setCursorPosition(line + 1, index);
}

//
// Insert C-style single line comment
//
void MainWindow::actionInsertCSingleComment()
{
    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // ...now insert our text:
    textEdit->insert("\t/* NOTE: */");
    // finally, move caret to (N)OTE:
    textEdit->setCursorPosition(line, index + 4);
}

//
// Insert C-style multi line comment
//
void MainWindow::actionInsertCMultiComment()
{
    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // ...now insert the first line of text!
    textEdit->insert("/*\n");
    // next, we need to continue printing at a certain location:
    textEdit->insertAt(" *\tComment:\n", ++line, 0);
    textEdit->insertAt(" */", ++line, 0);

    // finally, we set our caret to the next following empty line!
    textEdit->setCursorPosition(line - 1, index + 3);
}

//
// Insert C++style single line comment
//
void MainWindow::actionInsertCppSingleComment()
{
    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // ...now insert our text:
    textEdit->insert("\t// NOTE:");
    // finally, move caret to (N)OTE:
    textEdit->setCursorPosition(line, index + 4);
}

//
// Insert C++style single line comment
//
void MainWindow::actionInsertCLineDevideComment()
{
    // we need the caret's ("cursor") recent position stored as a starting point for insertion!
    int line, index;
    textEdit->getCursorPosition(&line, &index); // get the position...

    // ...now insert our text:
    textEdit->insert("/* ------------- COMMENT --------------------------------------------- */\n");
    // finally, move caret to next line.
    textEdit->setCursorPosition(line + 1, index);
}

//
// Insert snippet...
//
void MainWindow::actionInsertSnippet1()
{
    qDebug() << "in Snippet1";
    popNotImplemented();
}

//
// Insert Amiga Version String...
//
void MainWindow::actionInsertAmigaVersionString()
{
    if(!(p_versionstring_set))
    {
        QString my_versionstring = "const char *ver = \"\\0$VER: my_program 1.0 (31.12.2019)\";";
        // we need the caret's ("cursor") recent position stored as a starting point for insertion!
        int line, index;
        textEdit->getCursorPosition(&line, &index); // get the position...

        // ...now insert the first line of text!
        textEdit->insert("\n");
        // next, we need to continue printing at a certain location:
        textEdit->insertAt(my_versionstring + "\n", ++line, 0);

        // finally, we set our caret to the next following empty line!
        textEdit->setCursorPosition(line + 1, index);

        p_versionstring_set = true;
    }
    else
    {
        (void)QMessageBox::information(this,
                       "Amiga Cross Editor", "It seems there is allready a <i><b>version string</b></i> in this document!"
                        "How many of them do you want?"
                        "<br>Insertion will be cancelled, ya know?!",
                        QMessageBox::Ok);

    }
}

//
// Insert snippet...
//
void MainWindow::actionInsertSnippet2()
{
    qDebug() << "in Snippet2";
    popNotImplemented();
}

//
// Insert snippet...
//
void MainWindow::actionInsertSnippet3()
{
    qDebug() << "in Snippet3";
    popNotImplemented();
}

//
// Insert snippet...
//
void MainWindow::actionInsertSnippet4()
{
    qDebug() << "in Snippet4";
    popNotImplemented();
}
/* ------------ End insertMenue Actions -------------------*/

//
// Start UAE emulation
// CHANGE Programm Prefs according to your installation path and UAE flavour!
//
bool MainWindow::actionEmulator()
{
    QString command = p_emulator;
    QStringList arguments;

    switch(p_defaultEmulator)
    {
        case 0:
            p_emulator_to_start = p_os13_config;
            break;

        case 1:
            p_emulator_to_start = p_os20_config;
            break;

        case 2:
            p_emulator_to_start = p_os30_config;
            break;

        case 3:
            p_emulator_to_start = p_os40_config;
            break;

        default:
            p_emulator_to_start = "--help";
    }

    if(p_emulator_to_start.isEmpty())
    {
        // give a user warning
        (void)QMessageBox::critical(this,
                       "Amiga Cross Editor", "There seems to be <i><b>NO config file</b></i> for your requested <b><i>Emulation startup!</i></b><br> "
                        "Please revisit the Prefs editor and name a configuration.<br>"
                        "<br>This helps, ya know?!",
                        QMessageBox::Ok);

        actionPrefsDialog(3);

        return (false);
    }


    arguments << p_emulator_to_start;

    createStatusBarMessage(tr("Attempting to start UAE..."), 0);

    myEmulator.start(command, arguments);
    if(p_mydebug)
        qDebug() << "process pid: " << myEmulator.pid();

    // save process pid for termination checking!
    proc_pid = myEmulator.pid();

    QObject::connect(&myEmulator, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(emu_finished(int,QProcess::ExitStatus)));
    QObject::connect(&myEmulator, SIGNAL(readyReadStandardOutput()), this, SLOT(emu_readyReadStandardOutput()));
    QObject::connect(&myEmulator, SIGNAL(started()), this, SLOT(started()));


    return (true);
}

//
// start Workbench 1.3 emulation
//
void MainWindow::actionEmuOS13()
{
    p_defaultEmulator = 0;
    if(!(actionEmulator()))
        actionPrefsDialog(3);
}

//
// start Workbench 2.1 emulation
//
void MainWindow::actionEmuOS20()
{
    p_defaultEmulator = 1;
    if(!(actionEmulator()))
        actionPrefsDialog(3);
}

//
// start Workbench 3.x emulation
//
void MainWindow::actionEmuOS30()
{
    p_defaultEmulator = 2;
    if(!(actionEmulator()))
        actionPrefsDialog(3);;
}

//
// Kill a running Emulator
//
void MainWindow::actionKillEmulator()
{
    disconnect(&myEmulator, 0, 0, 0);
    myEmulator.terminate();

    if(p_mydebug)
        qDebug() <<"PID after kill: " << myEmulator.pid();

    // wait ome time, so OS has time to delete PID!
    delay();

    if(myEmulator.pid() != proc_pid)
    {
        emulatorAct->setEnabled(true);
        emulatorMenue->setEnabled(true);
        killEmulatorAct->setDisabled(true);
    }
}

//
// start Workbench 4.1 emulation
//
void MainWindow::actionEmuOS40()
{
    p_defaultEmulator = 3;
    if(!(actionEmulator()))
        actionPrefsDialog(3);
}

//
// Toggle visibility of caret line
//
void MainWindow::actionShowCaretLine()
{
    qDebug() << "in carret line";
    //popNotImplemented();
    if(showCaretLineAct->isChecked())
    {
        // show caret line
        textEdit->setCaretLineVisible(true);
    }
    else
    {
        // don't show caret line
        textEdit->setCaretLineVisible(false);
    }
}

//
// Toggle visibility of line numbers
//
void MainWindow::actionShowLineNumbers()
{
    QFontMetrics fontmetrics = QFontMetrics(textEdit->font());
    qDebug() << "in line numbers";
    // popNotImplemented();
    if(showLineNumbersAct->isChecked())
    {
        // show line numbers
        textEdit->setMarginLineNumbers(0, true);
        textEdit->setMarginWidth(0, fontmetrics.width(QString::number(textEdit->lines())) + 10);
    }
    else
    {
        // don't show line numbers
        textEdit->setMarginLineNumbers(0, false);
        textEdit->setMarginWidth(0, "");
    }
}

//
//  TOGGLE: Show or hide unprintable characters
//
void MainWindow::actionShowUnprintable()
{
    if(showUnprintableAct->isChecked())
    {
        // show unprintable characters
        textEdit->setEolVisibility(true);
        textEdit->setIndentationGuides(true);
        textEdit->setWhitespaceVisibility(QsciScintilla::WsVisible);
        this->showEOLAct->setChecked(true);
        this->showEOLAct->setEnabled(false);
        this->showIndentationGuidesAct->setChecked(true);
        this->showIndentationGuidesAct->setEnabled(false);
    }
    else
    {
        // don't show unprintable characters
        textEdit->setEolVisibility(false);
        textEdit->setIndentationGuides(false);
        textEdit->setWhitespaceVisibility(QsciScintilla::WsInvisible);
        this->showEOLAct->setChecked(false);
        this->showEOLAct->setEnabled(true);
        this->showIndentationGuidesAct->setChecked(false);
        this->showIndentationGuidesAct->setEnabled(true);
    }
}

//
//  TOGGLE: Show or hide EOL character
//
void MainWindow::actionShowEOL()
{
    if(showEOLAct->isChecked())
    {
        // show EOL character
        textEdit->setEolVisibility(true);
    }
    else
    {
        // don't show EOL character
        textEdit->setEolVisibility(true);
    }
}

//
//  Open global preferences dialog
//
void MainWindow::actionPrefsDialog(int tabindex = 0)
{
    PrefsDialog *acePrefs = new PrefsDialog(this, tabindex);
    acePrefs->exec();

    // afterwards insert new prefs into MainWindow variables for instant use!
    readSettings();
}

//
// search for given argument...
//
void MainWindow::actionSearch()
{
    if(!(p_search_is_open)) // allready opened?
    {
        searchGroup->show();
        lineEdit_find->setFocus();
        p_search_is_open = true;
        qDebug() << "in actionSearch()";
    }
}


//
// find strings "error", "Warning" and "file" in output pane - make their line information jumpable
//
void MainWindow::jumpCompilerWarnings()
{
    ;
}

//
// set default fonts, depending on OS
//
void MainWindow::initializeFont()
{
    // set a readable default font for Linux and Windows:
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
        QFont font("Courier New", 10);
        #elif defined(__APPLE__)
        QFont font("SF Mono Regular", 11);
        if(p_mydebug)
            qDebug() << "Running on Mac. Font is SF Mono Regular now!";
    #elif defined(__unix__)
        QFont font("Source Code Pro", 9);
        if(p_mydebug)
            qDebug() << "Linux detected. Setting font to Source Code Pro";
    #endif

    myfont = font;
    myfont.setFixedPitch(true);
    textEdit->setFont(myfont);
}

//
// initialize editor's margins with decent values according to text sizes in use
//
void MainWindow::initializeMargin()
{
    QFontMetrics fontmetrics = QFontMetrics(textEdit->font());
    textEdit->setMarginsFont(textEdit->font());
    textEdit->setMarginWidth(0, fontmetrics.width(QString::number(textEdit->lines())) + 10);
    textEdit->setMarginLineNumbers(0, true);

    // Make background the same color than the applied stylesheet for MainWindow
    // rgb: (175, 175, 175), hex: (#afafaf)
#if !defined(__APPLE__)
    textEdit->setMarginsBackgroundColor(QColor("#afafaf"));
#else
    textEdit->setMarginsBackgroundColor(QColor("#cccccccc"));
#endif
    textEdit->setMarginsForegroundColor(QColor("#ff0000ff"));

    // resize line numbers margin if needed!
    connect(textEdit, SIGNAL(textChanged()), this, SLOT(fitMarginLines()));
}

//
// resize line numbers margin
//
void MainWindow::fitMarginLines()
{
    QFontMetrics fontmetrics = textEdit->fontMetrics();
    textEdit->setMarginWidth(0, fontmetrics.width(QString::number(textEdit->lines())) + 10);
}

//
// initialize plain text "lexer" (NO syntax highlighting!)
// e.g. KILL existing lexer...
//
void MainWindow::initializeLexerNone()
{
    // We want to get rid of the Lexer that is allready initialized!
    textEdit->setLexer(nullptr);
    // Now that it's gone - let's take care that everything looks like before...
    textEdit->SendScintilla(textEdit->QsciScintilla::SCI_STYLESETCHARACTERSET, 1, QsciScintilla::SC_CHARSET_8859_15);
    initializeMargin();
    // We don't want to have the fold margin visible, since it's plain text we're displaying...
    textEdit->setFolding(QsciScintilla::NoFoldStyle);
    // Make sure everything is unfolded!
    QsciScintilla::FoldStyle state = static_cast<QsciScintilla::FoldStyle>((!textEdit->folding()) * 5);
    if (!state)
    {
        textEdit->foldAll(false);
    }

    qDebug() << "Faltstatus: " << state;
    createStatusBarMessage(tr("Syntax changed to PlainText"), 0);
}

//
// initialize C/C++ lexer bei default
//
void MainWindow::initializeLexerCPP()
{
    QsciLexerCPP *lexer = new QsciLexerCPP();
    textEdit->setLexer(lexer);
    textEdit->SendScintilla(textEdit->QsciScintilla::SCI_STYLESETCHARACTERSET, 1, QsciScintilla::SC_CHARSET_8859_15);
    textEdit->setFolding(QsciScintilla::BoxedTreeFoldStyle);
    createStatusBarMessage(tr("Syntax changed to C/C++"), 0);
}

//
// initialize lexer for Makefiles
//
void MainWindow::initializeLexerMakefile()
{
    QsciLexerMakefile *lexer = new QsciLexerMakefile;
    //lexer->setDefaultFont(textEdit->font());
    //lexer->setFoldComments(true);
    textEdit->setLexer(lexer);
    textEdit->SendScintilla(textEdit->QsciScintilla::SCI_STYLESETCHARACTERSET, 1, QsciScintilla::SC_CHARSET_8859_15);

    textEdit->setFolding(QsciScintilla::BoxedTreeFoldStyle);
    initializeMargin();
    createStatusBarMessage(tr("Syntax changed to Makefiles"), 0);
}

//
// initialize lexer for Batch files
//
void MainWindow::initializeLexerBatch()
{
    QsciLexerBatch *lexer = new QsciLexerBatch;
    //lexer->setFoldComments(true);
    textEdit->setLexer(lexer);
    textEdit->SendScintilla(textEdit->QsciScintilla::SCI_STYLESETCHARACTERSET, 1, QsciScintilla::SC_CHARSET_8859_15);

    textEdit->setFolding(QsciScintilla::BoxedTreeFoldStyle);
    initializeMargin();
    createStatusBarMessage(tr("Syntax changed to Shell"), 0);
}

//
// initialize lexer for Amiga Installer
//
void MainWindow::initializeLexerFortran()
{
    QsciLexerFortran *lexer = new QsciLexerFortran;
    //lexer->setFoldComments(true);
    textEdit->setLexer(lexer);
    textEdit->SendScintilla(textEdit->QsciScintilla::SCI_STYLESETCHARACTERSET, 1, QsciScintilla::SC_CHARSET_8859_15);

    textEdit->setFolding(QsciScintilla::BoxedTreeFoldStyle);
    initializeMargin();
    createStatusBarMessage(tr("Syntax changed to Amiga installer"), 0);
}

//
// initialize lexer for Pascal
//
void MainWindow::initializeLexerPascal()
{
    QsciLexerPascal *lexer = new QsciLexerPascal;
    //lexer->setFoldComments(true);
    textEdit->setLexer(lexer);
    textEdit->SendScintilla(textEdit->QsciScintilla::SCI_STYLESETCHARACTERSET, 1, QsciScintilla::SC_CHARSET_8859_15);

    textEdit->setFolding(QsciScintilla::BoxedTreeFoldStyle);
    initializeMargin();
    createStatusBarMessage(tr("Syntax changed to Pascal"), 0);
}

//
// give decent values to carret line
//
void MainWindow::initializeCaretLine()
{
    // Current line visible with special background color
    textEdit->setCaretLineVisible(false);
    textEdit->setCaretLineBackgroundColor(QColor("#a7edfe"));
}

//
// toggle code folding if called from action
//
void MainWindow::initializeFolding()
{
    QsciScintilla::FoldStyle state = static_cast<QsciScintilla::FoldStyle>((!textEdit->folding()) * 5);
    if (!state)
    {
        textEdit->foldAll(false);
    }
    else
    {
        textEdit->foldAll(true);
    }

    textEdit->setFolding(state);
    statusBar()->showMessage(tr("Folding toggled"), 2000);
}

//
// show current cursor position and display
// line and row in app's status bar's LCD widgets
//
void MainWindow::showCurrendCursorPosition()
{
    int line, index;
    textEdit->getCursorPosition(&line, &index);
    // did we want LCD display?
    if(!(p_no_lcd_statusbar))
    {
        statusLCD_X->display(line + 1);
        statusLCD_Y->display(index +1);
    }
    // ...or did we want plain text display?
    else
    {
        statusContainer_X->setText(QString::number(line + 1));
        statusContainer_Y->setText(QString::number(index + 1));
    }
}

//
// this method initializes the whole GUI!
//
void MainWindow::initializeGUI()
{
    // if on Mac, immitate its GUI behaviour:
    #if defined(__APPLE__)
        this->setUnifiedTitleAndToolBarOnMac(true);
        if(p_mydebug)
        {
            qDebug() << "running on some kind of Mac...";
        }
    #endif

    // give an icon and a name to the app:
    this->setWindowIcon(QIcon(":/images/amiga_classic.png"));
    this->setWindowTitle("Amiga Cross Editor");

    // set a decent start value for app's size
    this->setMinimumSize(600, 450);

    // prepare and initialize statusbar items:
    if(!(p_no_compilerbuttons))    // react on user prefs: show or hide compiler combo and -button
    {
        this->compilerLabel = new QLabel(this);
        this->compilerCombo = new QComboBox(this);
        this->osCombo = new QComboBox(this);
    }

    this->statusLabelX = new QLabel(this);

    if(!(p_no_lcd_statusbar))
    {
        this->statusLCD_X = new QLCDNumber(this);
        this->statusLCD_X->display(0);
    }
    else
    {
        this->statusContainer_X = new QLabel(this);
        statusContainer_X->setFrameShape(QFrame::Panel);
        statusContainer_X->setFrameShadow(QFrame::Sunken);
        statusContainer_X->setMinimumWidth(42);
        statusContainer_X->setAlignment(Qt::AlignRight);
        statusContainer_X->setText("0");
    }

    this->statusLabelY = new QLabel(this);

    if(!(p_no_lcd_statusbar))
    {
        this->statusLCD_Y = new QLCDNumber(this);
        this->statusLCD_Y->display(0);
    }
    else
    {
        this->statusContainer_Y = new QLabel(this);
        statusContainer_Y->setFrameShape(QFrame::Panel);
        statusContainer_Y->setFrameShadow(QFrame::Sunken);
        statusContainer_Y->setMinimumWidth(42);
        statusContainer_Y->setAlignment(Qt::AlignRight);
        statusContainer_Y->setText("0");

    }

    if(!(p_no_compilerbuttons))    // react on user prefs: show or hide compiler combo and -button
    {
        this->compilerButton = new QPushButton(NULL,this);
        compilerButton->setIcon(QIcon(":/images/dice.png"));
        compilerButton->setIconSize(QSize(18,18));
    }

    // permanently add the controls to the status bar
    if(!(p_no_compilerbuttons))    // react on user prefs: show or hide compiler combo and -button
    {
        statusBar()->addPermanentWidget(compilerLabel);
        compilerLabel->setText("Compiler:");
        statusBar()->addPermanentWidget(compilerCombo);
        compilerCombo->addItems(p_Compilers);
        compilerCombo->setItemIcon(0, QIcon(":/images/c-logo.png"));
        compilerCombo->setItemIcon(1, QIcon(":/images/c-logo.png"));
        compilerCombo->setItemIcon(2, QIcon(":/images/cpp-logo.png"));
        compilerCombo->setCurrentIndex(p_defaultCompiler);
        compilerCombo->setStatusTip(tr("Select compiler to use for this file"));

        osCombo->addItems(p_targetOS);
        osCombo->setItemIcon(0, QIcon(":/images/workbench.png"));
        osCombo->setItemIcon(1, QIcon(":/images/amiga_classic.png"));
        osCombo->setItemIcon(2, QIcon(":/images/amiga_boing.png"));
        osCombo->setCurrentIndex(p_compiler_vc_default_target);
        osCombo->setMinimumWidth(90);
        osCombo->setDisabled(true);
        osCombo->setStatusTip(tr("Change VBCC default target OS"));
        statusBar()->addPermanentWidget(osCombo);
        statusBar()->addPermanentWidget(compilerButton);

        compilerButton->setStatusTip(tr("Compile current file..."));
    }

    statusBar()->addPermanentWidget(statusLabelX);
    statusLabelX->setText(tr("Line:"));
    if(!(p_no_lcd_statusbar))
    {
        statusBar()->addPermanentWidget(statusLCD_X);
        statusLCD_X->display(1);
    }
    else
    {
        statusBar()->addPermanentWidget(statusContainer_X);
        statusContainer_X->setText("1");
    }
    statusBar()->addPermanentWidget(statusLabelY);
    statusLabelY->setText(tr("Column:"));
    if(!(p_no_lcd_statusbar))
    {
        statusBar()->addPermanentWidget(statusLCD_Y);
        statusLCD_Y->display(1);
    }
    else
    {
        statusBar()->addPermanentWidget(statusContainer_Y);
        statusContainer_Y->setText("1");
    }

    // give some blackish style to MainWindow (if the user wants so...)
    if(p_blackish)
    {
        // ...surely looks nasty on OS X, so we will skip that for Mac!
        #if !defined(__APPLE__)
            this->setStyleSheet(QString::fromUtf8("background-color: rgb(175, 175, 175);"));
        #endif
    }

    // implement a custom context menue for textEdit:
    textEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(textEdit, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showCustomContextMenue(const QPoint &)));

    // initialize textEdit's most needed attributes:
    initializeFont();
    initializeMargin();
    initializeCaretLine();
    initializeLexerCPP();
    //initializeFolding();

    // make editor as Amiga-compatible as possible:
    textEdit->setEolMode(QsciScintilla::EolUnix);
    textEdit->setIndentationsUseTabs(true);
    textEdit->setIndentationWidth(4);
    textEdit->setTabWidth(4);
    textEdit->setAutoIndent(true);
    textEdit->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    textEdit->SendScintilla(textEdit->QsciScintilla::SCI_STYLESETCHARACTERSET, 1, QsciScintilla::SC_CHARSET_8859_15);

    // create actions, menues, toolbars and status bar:
    createActions();
    createMenus();
    createToolBars();
    createStatusBarMessage(tr("Ready"), 0);


    if(!(p_no_compilerbuttons))    // react on user prefs: show or hide compiler combo and -button
    {
        connect(compilerCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(SelectCompiler(int)));
        connect(osCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setVbccTargetOS(int)));
        connect(compilerButton, SIGNAL(clicked(bool)), this, SLOT(actionCompile()));

        if(p_defaultCompiler == 0)
            osCombo->setEnabled(true);
    }

    connect(btnCloseOutput, SIGNAL(clicked(bool)), this, SLOT(actionCloseOutputConsole()));

    QObject::connect(&myProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
    QObject::connect(&myProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(compiler_readyReadStandardOutput()));
    QObject::connect(&myProcess, SIGNAL(started()), this, SLOT(started()));
}

//
// print current file
//
// TODO:    make printing printer prefs-aware!
//          print printing header ("file %f printed on dd.mm.yyyy")
//          ...use QPainter for that?
//
void MainWindow::printFile()
{
    // Check if editor window contains some text...
    int textsize = textEdit->text().size();  // if max > 0 then there must be some text in that editor window!
    if (textsize > 0)                        // more than one character? So there is some text available. Let's check for a printer!
    {
        this->createStatusBarMessage(tr("Calling printer dialog......"), 6000);

        // create a high resolution QScintilla printer instance...
        QsciPrinter printer(QPrinter::HighResolution);
        // ...now we need to know wich printer to use! Call standard system printer dialog:
        QPrintDialog printDialog(&printer, this);

        if (printDialog.exec() == QDialog::Accepted)    // if printer dialog was told to print...
        {
            this->createStatusBarMessage(tr("Printing started!"), 6000);
            // set paper size to DIN A4
            printer.setPageSize(QsciPrinter::A4);

            // print ...
            // printer.printRange(textEdit, 1, max); REFUSES TO PRINT! Use this instead:
            printer.printRange(textEdit);

            this->createStatusBarMessage(tr("File was send to printer and should be printed soon..."), 6000);
        }
        else    // ...if printer dialog was canceled
        {
            this->createStatusBarMessage(tr("Printing canceled."), 6000);
        }
    }
    // textEdit->text().size() == 0. No text available. We will not print anything!
    else
    {
        this->createStatusBarMessage(tr("Printing canceled tue to wasting!"), 6000);
        (void)QMessageBox::information(this,
                       "Printing - Amiga Cross Editor", "It seems there is <i><b>no text</b></i> in this editor window!<br> Printing is cancelled due to waste of paper."
                                                        "<br>There's allways a unicorn dying if you waste things, ya know?!",
                        QMessageBox::Ok);
    }
}

//
// inform user about unimplemented feature
//
void MainWindow::popNotImplemented()
{
    (void)QMessageBox::information(this,
                   "Not implemented - Amiga Cross Editor", "You have requested a feature that is <i><b>not</b></i> fully<br>implemented yet! The requested feature might not work as expected...",
                    QMessageBox::Ok);

}

//
// Let's talk about mouse press events...
//
//void MainWindow::mousePressEvent(QMouseEvent *event)

//{
//    if (event->button() == Qt::RightButton)
//    {
//        qDebug() << "Right mousebutton pressed!";
//        this->menuWidget()->setContextMenuPolicy(Qt::CustomContextMenu);
//    }
//    if (event->button() == Qt::LeftButton)
//        qDebug() << "Left mousebutton pressed!";
//    if (event->button() == Qt::MiddleButton)
//        qDebug() << "Middle mousebutton pressed!";

//}

//
// Let's implement a custom right-click context menu for QScintilla textEdit
//
void MainWindow::showCustomContextMenue(const QPoint &pos)
{
    // name the context menue
    QMenu contextMenu(tr("Inserts"), this);

    // define a pseudo action to show some kind of menue title
    QAction pseudo_action(tr("What to insert?"), this);
    pseudo_action.setStatusTip(tr("Select an item to be inserted into current text!"));
    //connect(&pseudo_action, &QAction::triggered, this, []{qDebug() << "action 1 click!";});

    // add all those predefined actions that we want to show...
    contextMenu.addAction(&pseudo_action);
    contextMenu.addSeparator();
    contextMenu.addAction(shellAppAct);
    contextMenu.addSeparator();
    contextMenu.addAction(fileheaderAct);
    contextMenu.addSeparator();
    contextMenu.addAction(ifdefinedAct);
    contextMenu.addSeparator();
    contextMenu.addAction(ifdefinedCompilerAct);
    contextMenu.addSeparator();
    contextMenu.addAction(includeAct);
    contextMenu.addSeparator();
    contextMenu.addAction(versionStringAct);
    contextMenu.addSeparator();
    contextMenu.addAction(mainAct);
    contextMenu.addSeparator();
    contextMenu.addAction(functionAct);
    contextMenu.addSeparator();
    contextMenu.addAction(structAct);
    contextMenu.addSeparator();
    contextMenu.addAction(OpenLibraryAct);
    contextMenu.addSeparator();
    contextMenu.addAction(CloseLibraryAct);
    contextMenu.addSeparator();
    contextMenu.addAction(ifAct);
    contextMenu.addSeparator();
    contextMenu.addAction(if_elseAct);
    contextMenu.addSeparator();
    contextMenu.addAction(whileAct);
    contextMenu.addSeparator();
    contextMenu.addAction(do_whileAct);
    contextMenu.addSeparator();
    contextMenu.addAction(forAct);
    contextMenu.addSeparator();
    contextMenu.addAction(c_singleAct);
    contextMenu.addSeparator();
    contextMenu.addAction(c_multiAct);
    contextMenu.addSeparator();
    contextMenu.addAction(lineDevideCommentAct);

    contextMenu.exec(mapToGlobal(pos));

}

bool MainWindow::fileExists(QString path)
{
    QFileInfo check_file(path);

    // check if exists and if yes: Is it really a file?
    if(check_file.exists() && check_file.isFile())
    {
        if(p_mydebug)
        {
            qDebug() << "compiled file: " << p_compiledFile << " exists.";
        }
        return true;
    }
    else
    {
        if(p_mydebug)
        {
            qDebug() << "compiled file: " << p_compiledFile << " does not exist!";
        }
        return false;
    }
}

/**************************************
 * Stuff for launching a Compiler... **
 **************************************/
int MainWindow::startEmulator()
{
    if(p_mydebug)
        qDebug() << "startEmulator() called.";

    QString command = p_emulator;
    QStringList arguments;
    arguments << p_selected_compiler_args.split(" ");

    createStatusBarMessage(tr("Compiler run has been started."),0);

    myProcess.start(command, arguments);

    connect(&myProcess, SIGNAL (readyRead()), this, SLOT (readCommand()));
    QObject::connect(&myProcess, SIGNAL(started()), this, SLOT(started()));

    return 0;
}

int MainWindow::startCompiler()
{
    myProcess.setProcessChannelMode(QProcess::MergedChannels);
    if(p_mydebug)
    {
        qDebug() << "startCompiler() called.";
    }
    //debugVars();

    QString command = p_selected_compiler;
    QStringList arguments;
    // IMPORTANT! 'arguments' must be a QStringList, NOT a QString, else the compiler call will not work!
    // Therefore, we'll have to put each argument separately.
    // Each argument in our string is separated by a whitespace.
    // We use the split() function to isolate them, giving each argument separately to our QStringList.
    arguments << p_selected_compiler_args.split(" ");

    createStatusBarMessage(tr("Compiler run has been started."),0);

    if(p_mydebug)
    {
        qDebug() << "in startCompiler()";

        qDebug() << "p_selected_compiler_args: " << p_selected_compiler_args;
        qDebug() << "\nCommand: " << command;
        qDebug() << "Arguments: " << arguments;

    }

    // Fire up compiler!
    runCommand(command, arguments);    

    return 0;
}


void MainWindow::readyReadStandardError()
{
    if(p_mydebug)
      qDebug() << "ReadyError";
}

void MainWindow::readyReadStandardOutput()
{
    if(p_mydebug)
    {
        qDebug() << "readyOut";
    }
    QProcess *myProcess = (QProcess *)sender();
    QByteArray buf = myProcess->readAllStandardOutput();

    QFile data(p_projectsRootDir + QDir::separator() + "compiler_out.txt");

    if(p_mydebug)
    {
        qDebug() << "logfile: " << p_projectsRootDir + QDir::separator() + "compiler_out.txt";
    }
    if (data.open(QFile::WriteOnly | QFile::Truncate))
    {
      QTextStream out(&data);
      out << buf;
    }
}

void MainWindow::started()
{
    createStatusBarMessage("UAE successfully started.", 0);
    if(p_mydebug)
    {
        qDebug() << "START: Proc Started";
        qDebug() << "UAE seems to have started!";
    }

    emulatorAct->setDisabled(true);
    emulatorMenue->setDisabled(true);
    killEmulatorAct->setEnabled(true);
}

void MainWindow::emu_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitCode == 0)
    {
        if(p_mydebug)
        {
            qDebug() << "Emulator terminated regularily.";
        }
    }
    else
    {
        if(p_mydebug)
        {
            qDebug() << "Emulator NOT terminated regularily.";
        }
    }

    if(p_mydebug)
    {
        qDebug() << "Finished with ExitCode: " << exitCode;
    }

    if (exitStatus==QProcess::CrashExit || exitCode!=0)
    {
     createStatusBarMessage("CrashExit - UAE has a problem!!", 0);
    }
    else
    {
      createStatusBarMessage("UAE terminated regularly.", 0);
    }
    emulatorMenue->setEnabled(true);
    emulatorAct->setEnabled(true);
    killEmulatorAct->setDisabled(true);
}

void MainWindow::emu_readyReadStandardOutput()
{
    if(p_mydebug)
        qDebug() << "readyOut";

  QProcess *myEmulator = (QProcess *)sender();
  QByteArray buf = myEmulator->readAllStandardOutput();

  if(p_mydebug)
      qDebug() << buf;
}

void MainWindow::compiler_readyReadStandardOutput()
{
    if(p_mydebug)
        qDebug() << "readyReadStandardOut";

  QProcess *myProcess = (QProcess *)sender();
  QByteArray buf = myProcess->readAllStandardOutput();

    if(p_mydebug)
        qDebug() << buf;
}


//
// set GUI according to default prefs
//
void MainWindow::activateGUIdefaultSettings()
{
    // setup Compiler Environment
    SelectCompiler(p_defaultCompiler);
    actionShowIndentationGuides();
}

////////////////////////////
// Debug Helper           //
///////////////////////////
void MainWindow::debugVars()
{
    if(p_mydebug)
    {
        // TAB: Project
        qDebug() << "p_author: " << p_author;
        qDebug() << "p_email: " << p_email;
        qDebug() << "p_website: " << p_website;
        qDebug() << "p_description: " << p_description;
        qDebug() << "p_purpose: " << p_purpose;
        qDebug() << "p_projectsRootDir: " << p_projectsRootDir;
        // TAB: GCC
        qDebug() << "p_compiler_gcc: " << p_compiler_gcc;
        qDebug() << "p_compiler_gpp: " << p_compiler_gpp;
        qDebug() << "p_make: " << p_make;
        qDebug() << "p_strip: " << p_strip;
        qDebug() << "p_compiler_gcc_call: " << p_compiler_gcc_call;
        qDebug() << "p_compiler_gpp_call: " << p_compiler_gpp_call;
        // TAB: VBCC
        qDebug() << "p_compiler_vc: " << p_compiler_vc;
        qDebug() << "pp_compiler_vasm: " << p_compiler_vasm;
        qDebug() << "p_vbcc_config_dir: " << p_vbcc_config_dir;
        qDebug() << "p_compiler_vc13_call: " << p_compiler_vc13_call;
        // TAB: Emulator
        qDebug() << "p_emulator: " << p_emulator;
        qDebug() << "p_os13_config: " << p_os13_config;
        qDebug() << "p_os20_config: " << p_os20_config;
        qDebug() << "p_os30_config: " << p_os30_config;
        qDebug() << "p_os40_config: " << p_os40_config;
        qDebug() << "p_defaultEmulator: " << p_defaultEmulator;
        // TAB: Misc
        qDebug() << "p_default_style: " << p_default_style;
        qDebug() << "p_blackish: " << p_blackish;
        qDebug() << "p_show_indentation: " << p_show_indentation;
        qDebug() << "p_mydebug: " << p_mydebug;
        qDebug() << "p_defaultCompiler: " << p_defaultCompiler;
        qDebug() << "p_no_lcd_statusbar: " << p_no_lcd_statusbar;
        qDebug() << "p_no_compilerbuttons: " << p_no_compilerbuttons;
        qDebug() << "p_simple_statusbar: " << p_simple_statusbar;
        qDebug() << "p_create_icon: " << p_create_icon;
        qDebug() << "p_console_on_fail: " << p_console_on_fail;
        qDebug() << "p_no_warn_requesters: " << p_no_warn_requesters;
    }
}

/////////////////////////////////////////////////////////////////////////
/// new process handling methods for compiler output to QTextBrowser ///
///////////////////////////////////////////////////////////////////////
void MainWindow::runCommand(QString command, QStringList arguments)
{
    if(p_mydebug)
    {
        qDebug() << "in runCommand()";

        qDebug() << "p_selected_compiler_args: " << p_selected_compiler_args;
        qDebug() << "\nCommand: " << command;
        qDebug() << "Arguments: " << arguments;

    }
    cmd->setProcessChannelMode(QProcess::MergedChannels);

    // check if there's allready a compiled file with that name
    // if it's there - delete it. Just to be sure...
    bool fileExists = QFileInfo::exists(p_compiledFile) && QFileInfo(p_compiledFile).isFile();
    if(fileExists)
    {
        if(p_mydebug)
            qDebug() << "Executable allready exists. Now trying to delete!";
        if(!(QFile::remove(p_compiledFile)))
            // delete file first!
            if(p_mydebug)
                qDebug() << "Icon could not be deleted!";
    }

    // Empty output widget
    output->clear();
    // Give message about started process:
    //output->append("Run process...\nCompiler started:\n");

    // fire up our process:
    cmd->start(command, arguments);
}

void MainWindow::readCommand()
{
    QString Buffer = cmd->readAll();    // buffer compiler output!
    QStringList listToFilter;           // List that keeps output items
    QStringList filterList;             // filtered List without line feed, containing only errors and warnings

    // feed buffer list with buffered output, stripping line feed
    listToFilter = Buffer.split("\n");

    // Now, let's filter!
    foreach (QString item, listToFilter)
    {
        // We only need messages containing errors and warnings...
        if(!(item.isEmpty()) && (item.contains("error", Qt::CaseInsensitive) || item.contains("warning", Qt::CaseInsensitive)) && !(item.contains("error found!", Qt::CaseInsensitive)) )
        {
            // add filtered items to our second list...
            if(!(item.isEmpty()))
                filterList << item;
        }
    }

    // put filtered List together for usage as a QString, separated by "\n":
    QString joinedMessages = filterList.join("\n");

    // put it all into console output:
    output->appendPlainText(joinedMessages);    // 'output' is our QPlainTextBrowser in splitter output pane
}


//
// things to do when execution of startCompiler() has stopped
//
int MainWindow::stopCommand(int exitCode, QProcess::ExitStatus exitStatus)
{
    output->appendPlainText(cmd->readAll());

    if(p_mydebug)
        qDebug() << "In stopCommand()\nexitCode: " << QString::number(exitCode);

    if(QString::number(exitCode) == "0")
    {
        createStatusBarMessage(tr("File compiled."), 0);
    }
    else
    {
        createStatusBarMessage(tr("There where errors..."), 0);
    }

    if (exitStatus==QProcess::CrashExit || exitCode!=0)
    {
        createStatusBarMessage("Compiler error!", 0);
        textEdit->setCaretLineVisible(true);
        this->showCaretLineAct->setChecked(true);
        textEdit->setFocus();

        if(p_console_on_fail)
        {
            actionShowOutputConsole();
        }

        if(!(p_no_warn_requesters))
        {
            (void)QMessageBox::critical(this, tr("Amiga Cross Editor"),
            tr("Build error!\n"
            "Please check source for errors and recompile."),
            QMessageBox::Ok);
        }
    }
    else
    {
        // Let's check if the compiler produced an executable file:
        if(fileExists(p_compiledFile))
        {            
            successMessage = "Compiler run finished, took " + QString::number(nMilliseconds) + " mSecs";
            createStatusBarMessage(successMessage, 0);

            if(!(p_no_warn_requesters))
            {
                (void)QMessageBox::information(this, tr("Compilation finished - Amiga Cross Editor"),
                tr("Successfully compiled.\nCompilation took %1 milliseconds to finish.\n\n"
                "You may now want to test your program in UAE.").arg(nMilliseconds),
                QMessageBox::Ok);
            }

            successMessage = "Compiled successfully. Compile time: " + QString::number(nMilliseconds) + " mSecs";
            createStatusBarMessage(successMessage, 0);


            // create icon for this app?
            if(p_create_icon)
            {
                if(p_default_icon.isEmpty())    // if no icon file is set...
                {
                    (void)QMessageBox::warning(this, tr("Amiga Cross Editor"),
                    tr("Default Amiga icon file not defined!\n"
                    "You may now wset its location in preferences editor.\n\n"
                       "Don't forget to restart Amiga Cross Editor afterwards!"),
                QMessageBox::Ok);

                    actionPrefsDialog(0);   // ...call prefsDialog's first TAB and exit this method!
                    return 1;
                }
                else
                {
                    // get executable path and build icon filename
                    QString iconcopyfile = p_compiledFile + ".info";
                    bool fileExists = QFileInfo::exists(iconcopyfile) && QFileInfo(iconcopyfile).isFile();

                    // check if icon is allready there and delete it in that case!
                    if(fileExists)
                    {
                        if(p_mydebug)
                            qDebug() << "Icon allready exists. Now trying to delete!";
                        if(!(QFile::remove(iconcopyfile)))
                            // delete icon file first!
                            if(p_mydebug)
                                qDebug() << "Icon could not be deleted!";
                    }

                    // copy icon file to that path..
                    if(!(QFile::copy(p_default_icon, iconcopyfile)))
                    {
                        if(p_mydebug)
                            qDebug() << "Icon could not be copied!";

                        (void)QMessageBox::information(this, tr("Amiga Cross Editor"),
                        tr("Sorry - icon file could not bee created!\n"),
                        QMessageBox::Ok);
                    }
                    else
                    {
                        if(p_mydebug)
                            qDebug() << "Icon was successfully copied!";
                    }
                }
            }
        }
        else
        {
            (void)QMessageBox::information(this, tr("Amiga Cross Editor"),
            tr("No success in building your executable file!.\n"
            "Please check for Errors and recompile."),
            QMessageBox::Ok);

            successMessage = "Compiled successfully. Compile time: " + QString::number(nMilliseconds) + " mSecs";
            createStatusBarMessage(successMessage, 0);
        }

    }
    return 0;
}

void MainWindow::error(QProcess::ProcessError error)
{
       qDebug() << "Error" << error;

}

void MainWindow::stateChanged(QProcess::ProcessState state)
{
    qDebug() << "Process::stateChanged" << state;
}

//
// prefDialog start helper
//
void MainWindow::startPrefs()
{
    actionPrefsDialog(0);
}

//
// HELPER: wait some time without freezing the gui
//
void MainWindow::delay()
{
    QTime dieTime= QTime::currentTime().addSecs(1);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

//
// HELPER: disable Emulator menu entries if no configuration was specified
//
void MainWindow::setEmulatorMenu()
{
    if(p_os13_config.isEmpty())
        emulator13Act->setDisabled(true);
    else
        emulator13Act->setEnabled(true);

    if(p_os20_config.isEmpty())
        emulator20Act->setDisabled(true);
    else
        emulator20Act->setEnabled(true);

    if(p_os30_config.isEmpty())
        emulator30Act->setDisabled(true);
    else
        emulator30Act->setEnabled(true);

    if(p_os40_config.isEmpty())
        emulator40Act->setDisabled(true);
    else
        emulator40Act->setEnabled(true);
}

//
// change VBCC default target OS at runtime
//
void MainWindow::setVbccTargetOS(int default_os)
{
    switch(default_os)
    {
        // OS 1.3
        case 0:
            p_selected_compiler_args = p_compiler_vc13_call;
            break;
        // OS 3.x
        case 1:
            p_selected_compiler_args = p_compiler_vc30_call;
            break;
        // OS 4.1
        case 2:
            p_selected_compiler_args = p_compiler_vc40_call;
            break;
    }
}

//
// Convenience method - will never really be executed...
//
void MainWindow::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(p_mydebug)
        qDebug() << "We are in MainWindow::finished\n(...this place should have never been reached.)\nThis is the exitCode: " << exitCode;

    if(exitCode == 0)
    {
        if(p_mydebug)
            qDebug() << "Finished: " << exitCode;

    }
    else
    {
        if(p_mydebug)
            qDebug() << "ExitCode " << exitCode;

    }
    if(p_mydebug)
        qDebug() << "Finished: " << exitCode;

  if (exitStatus==QProcess::CrashExit || exitCode!=0)
  {
      createStatusBarMessage("Compiler error!", 0);
      if(!(p_no_warn_requesters))
      {
          (void)QMessageBox::critical(this, tr("Amiga Cross Editor"),
          tr("Build error!\n"
          "Please check source for errors and recompile."),
          QMessageBox::Ok);
      }
      //output->moveCursor(QTextCursor::Start);
  }
  else
  {
      // Let's check if the compiler produced an executable file:
      if(fileExists(p_compiledFile))
      {
          successMessage = "Compiled successfully. Compile time: " + QString::number(nMilliseconds) + " mSecs";
          createStatusBarMessage(successMessage, 0);

          (void)QMessageBox::information(this, tr("Compilation finished - Amiga Cross Editor"),
          tr("Successfully compiled.\nCompilation took %1 milliseconds to finish.\n\n"
          "You may now want to test your program in UAE.").arg(nMilliseconds),
          QMessageBox::Ok);

          successMessage = "Compiled successfully. Compile time: " + QString::number(nMilliseconds) + " mSecs";
          createStatusBarMessage(successMessage, 0);
      }
      else
      {
          (void)QMessageBox::information(this, tr("Amiga Cross Editor"),
          tr("No success in building your executable file!.\n"
          "Please check for Errors and recompile."),
          QMessageBox::Ok);

          createStatusBarMessage("Compiler run finished unsuccessfully.", 0);
      }

  }
}

//
// get line of text that was selected and store it in variable text_to_search
// connect to output!
//
void MainWindow::on_output_cursorPositionChanged()
{

    QString text_to_search;                         // keeps the line of text to parse
    text_to_search.clear();

    QTextCursor txtCursor = output->textCursor();
    QTextBlockFormat t_format;                      // for higlighting...

    QString data = output->toPlainText();
    QStringList strList = data.split(QRegExp("[\n]"),QString::SkipEmptyParts);

    if(txtCursor.blockNumber() <= (strList.count()) -1 )
    {
        QString content = strList[txtCursor.blockNumber()];
        if(!(content.isEmpty()))
        {
            if(p_mydebug)
            {
                qDebug() << "Line accepted. ";
                qDebug() << "now looking for warnings and errors...";
            }
            // set search string for regEX checkup:
            text_to_search = strList[txtCursor.blockNumber()];
        }
        else
        {
            text_to_search.clear();
        }
    }

    // Now let's do all the work for jumping to error/warning!
    if (!(text_to_search.isEmpty()))
    {
        switch(p_defaultCompiler)
        {
        case 0:
            if(p_mydebug)
                qDebug() << "Now checking for VBCC";

            checkVBCC(text_to_search);
            jumpToError(line_nr, 0);
            break;
        case 1:
        case 2:
            if(p_mydebug)
                qDebug() << "Now checking for gcc/g++";

            checkGCC(text_to_search);
            jumpToError(line_nr, column_nr - 1);
            break;
        }
    }

    if(p_mydebug)
        qDebug() << "//--- END on_outputCursorPositionChanged() DEBUG --------//";
}


//
// RegEx parse VBCC output
//
void MainWindow::checkVBCC(QString str_to_search)
{
    QString re1="((?:[a-z][a-z]+))";	// Word 1
    QString re2=".*?";	//# Non-greedy match on filler
    QString re3="(?:[a-z][a-z]+)";	//# Uninteresting: word
    QString re4=".*?";	//# Non-greedy match on filler
    QString re5="((?:[a-z][a-z]+))";	//# Word 2
    QString re6=".*?";	//# Non-greedy match on filler
    QString re7="(\\d+)";	// # Integer Number 1
    QString re8=".*?";	//# Non-greedy match on filler
    QString re9="((?:\\/[\\w\\.\\-]+)+)";	//# Unix Path 1
    QStringList strlist;
    strlist << re1 << re2 << re3 << re4 << re5 << re6 << re7 << re8 << re9;
    QString reg = re1 + re2 + re3 + re4 + re5 + re6 + re7 + re8 + re9;

    if(!(str_to_search.isEmpty()))
    {
        QRegularExpression rx_line("(\\d+)");                       // check for line (and column, if available)
        QRegularExpression rx_words("((?:\"[a-z][a-z]+\\.c)\")");   // check for ALL strings starting with \", containing .c, ending with \"
        QRegularExpression rx_file("((?:\\/[\\w\\.\\-]+)+)");       // unix path
        QRegularExpression rx_messagetype("(error)");               // check if error or warning


        QList<int> list;    // List to hold search results (integer values)
        QStringList list1;  // List to hold string-type results

        //
        // check if error or warning
        //
        QRegularExpressionMatchIterator i = rx_messagetype.globalMatch(str_to_search);
        while (i.hasNext())
        {
            QRegularExpressionMatch match = i.next();
            QString word = match.captured(1);
            list1 << word;
        }

        if(!(list1.isEmpty()))
        {
            if(list1[0] == "error")
            {
                createStatusBarMessage("Error while parsing jumplist", 0);

            }
            else
            {
                createStatusBarMessage("Error while parsing jumplist", 0);
            }

            qDebug() << "List entries: " << list1 << "List count: " << list1.count();

        }
        else
        {
            //ui->label_MessageType->setText("Warning");
            qDebug() << "No matches!";
        }

        //
        // check for simple filename
        //
        list1.clear();
        QRegularExpressionMatchIterator w = rx_words.globalMatch(str_to_search);
        while (w.hasNext())
        {
            QRegularExpressionMatch match = w.next();
            QString word = match.captured(1);
            list1 << word;
        }

        if(!(list1.isEmpty()))
        {
            qDebug() << "List entries: " << list1 << "List count: " << list1.count();
            //ui->lineEdit_Filepath->setText(list1[0]);
        }
        else
            qDebug() << "no simple filename";


        //
        // check for unix path
        //
        list1.clear();
        QRegularExpressionMatchIterator f = rx_file.globalMatch(str_to_search);
        while (f.hasNext())
        {
            QRegularExpressionMatch match = f.next();
            QString word = match.captured(1);
            list1 << word;
        }

        if(!(list1.isEmpty()))
        {
            qDebug() << "List entries: " << list1 << "List count: " << list1.count();
            //ui->lineEdit_Filepath->setText(list1[0]);
        }
        else
            qDebug() << "no unix path!";



        //
        // check for line and column
        //
        QRegularExpressionMatchIterator lc = rx_line.globalMatch(str_to_search);
        while (lc.hasNext())
        {
            QRegularExpressionMatch match = lc.next();
            QString word = match.captured(1);
            list << word.toInt();
        }

        if(!(list.isEmpty()))
        {
            int index = 0;
            qDebug() << "List entries: " << list << "List count: " << list.count();
            if(index <= list.count() -1)
                error_nr = list[index];

            ++index;

            if(index <= list.count() -1)
                //ui->lcdNumber_Line->display(QString::number(list[index]));
                qDebug() << "in checkVBCC()";
                qDebug() << "goto line:" << QString::number(list[index]);

            line_nr = list[index];
        }
        else
            qDebug() << "WTF!";

    } // END str_to_search.isEmpty()
    qDebug() << "|-----------------------------------------------------|";
    qDebug() << "error_nr: " << error_nr;
    qDebug() << "errortype: " << errortype;
    qDebug() << "line_nr: " << line_nr;
    qDebug() << "column_nr: " << column_nr;
    qDebug() << "debugfilename: " << debugfilename;
    qDebug() << "|-----------------------------------------------------|";
} // END checkVBCC()

//
// RegEx parse GCC/G++ output
//
void MainWindow::checkGCC(QString str_to_search)
{
    QList<int> list;    // List to hold search results (integer values)
    QStringList list1;  // List to hold string-type results

    // get name of file to debug:
    QRegularExpression rx_debugfile("(.*?):(\\d+):(\\d+:)? (.*)");
    QRegularExpressionMatchIterator i = rx_debugfile.globalMatch(str_to_search);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        QString word = match.captured(1);
        list1 << word;
    }
    // set name of file to debug:
    if(!(list1.isEmpty()))
    {
        debugfilename = list1[0];
    }

    //------ next regEX ----------------------------------------//

    // get line and column
    QRegularExpression rx_line("(\\d+)");

    // check for line and column
    QRegularExpressionMatchIterator lc = rx_line.globalMatch(str_to_search);
    while (lc.hasNext())
    {
        QRegularExpressionMatch match = lc.next();
        QString word = match.captured(1);
        list << word.toInt();
    }
    // set line and column:
    if(!(list.isEmpty()) && list.count() == 2)
    {
        line_nr = list[0];
        column_nr = list[1];
    }
    else
    {
        createStatusBarMessage("no valid line and column data!", 0);
    }

    if(p_mydebug)
    {
        qDebug() << "|-----------------------------------------------------|";
        qDebug() << "gcc debugfilename List entries: " << list1 << "List count: " << list1.count();
        qDebug() << "gcc line/column List entries: " << list << "List count: " << list.count();
        qDebug() << "error_nr: " << error_nr;
        qDebug() << "errortype: " << errortype;
        qDebug() << "line_nr: " << line_nr;
        qDebug() << "column_nr: " << column_nr;
        qDebug() << "debugfilename: " << debugfilename;
        qDebug() << "|-----------------------------------------------------|";
    }
}

//
// Jump to line No. X...
//
void MainWindow::jumpToError(int error_line, int error_column)
{
    bool ok = true;
    textEdit->setFocus();

    if (ok)
    {
        // check if text is folded!
        QsciScintilla::FoldStyle state = static_cast<QsciScintilla::FoldStyle>((!textEdit->folding()) * 5);

        // if folded: unfold first!!
        if (state > 0)
        {
            textEdit->foldAll(false);
        }
        textEdit->setCursorPosition(error_line-1, error_column);
    }
}


//
// gcc regEX testing area!
// to be deleted ASAP
//
void MainWindow::testGCCregEx(QString str_to_search)
{
    QList<int> list;    // List to hold search results (integer values)
    QStringList list1;  // List to hold string-type results

    // get name of file to debug:
    QRegularExpression rx_debugfile("(.*?):(\\d+):(\\d+:)? (.*)");
    QRegularExpressionMatchIterator i = rx_debugfile.globalMatch(str_to_search);
    while (i.hasNext())
    {
        QRegularExpressionMatch match = i.next();
        QString word = match.captured(1);
        list1 << word;
    }
    // set name of file to debug:
    if(!(list1.isEmpty()))
    {
        debugfilename = list1[0];
    }

    //------ next regEX ----------------------------------------//

    // get line and column
    QRegularExpression rx_line("(\\d+)");

    // check for line and column
    QRegularExpressionMatchIterator lc = rx_line.globalMatch(str_to_search);
    while (lc.hasNext())
    {
        QRegularExpressionMatch match = lc.next();
        QString word = match.captured(1);
        list << word.toInt();
    }
    // set line and column:
    if(!(list.isEmpty()) && list.count() == 2)
    {
        line_nr = list[0];
        column_nr = list[1];
    }
    else
    {
        createStatusBarMessage("no valid line and column data!", 0);
    }

    qDebug() << "|-----------------------------------------------------|";
    qDebug() << "gcc debugfilename List entries: " << list1 << "List count: " << list1.count();
    qDebug() << "gcc line/column List entries: " << list << "List count: " << list.count();
    qDebug() << "error_nr: " << error_nr;
    qDebug() << "errortype: " << errortype;
    qDebug() << "line_nr: " << line_nr;
    qDebug() << "column_nr: " << column_nr;
    qDebug() << "debugfilename: " << debugfilename;
    qDebug() << "|-----------------------------------------------------|";
}

//
// search and replace:
// Helper slot for compatible call of do_search_and_replace()
// from editingFinished()
// void call_do_search_and_replace()
//
void MainWindow::call_do_search_and_replace()
{
    do_search_and_replace("0");
}

//
// search & replace:
// do_search_and_replace() - search for matching word
//
void MainWindow::do_search_and_replace(QString action_str)
{
    //clearMarkers();
    int line, index;
    qDebug() <<  "do_search_and_replace()";
    // just to be sure...
    if(action_str.isEmpty())
        action_str == "0";

    int action_nr = action_str.toInt();    // convert argument to int, so we can switch() on it...
    QString text = lineEdit_find->text();
    QString docText = textEdit->text();
    qDebug() <<  "action_nr: " << action_nr;

    textEdit->getCursorPosition(&line, &index);
    textEdit->setCursorPosition(line - 1, index);
    clearMarkers();

    //
    // first part: Find first occurance of search term and select it
    //
    bool use_regular_expression, is_case_sensitive, match_whole_word_only, use_wrap, search_forward;
    use_regular_expression = false;
    is_case_sensitive = checkBox_CaseSensitive->isChecked();
    match_whole_word_only = checkBox_WholeWords->isChecked();
    use_wrap = true;
    search_forward = checkBox_SearchForwards->isChecked();

    textEdit->SendScintilla(QsciScintillaBase::SCI_INDICSETSTYLE, 0, QsciScintilla::INDIC_FULLBOX);
    textEdit->SendScintilla(QsciScintillaBase::SCI_INDICSETFORE,0, QColor(Qt::darkBlue));

    bool found = textEdit->findFirst(text, use_regular_expression, is_case_sensitive, match_whole_word_only, use_wrap, search_forward);
    qDebug() <<  "START: found = " << found;
    while(found)
    {
        textEdit->getCursorPosition(&line, &index);

        qDebug() << "line: " << line << " index: " << index;
        qDebug() << text;

        // pattern: found = findFirst(pattern, use_regular_expression, is_case_sensitive, match_whole_word_only, use_wrap, search_forward)
        //found = ui->textEdit->findFirst(text, use_regular_expression, is_case_sensitive, match_whole_word_only, use_wrap, search_forward);

        if(found && !text.isEmpty())
        {
            textEdit->SendScintilla(QsciScintillaBase::SCI_INDICATORFILLRANGE, line, text.length());
            int start = textEdit->positionFromLineIndex(line, index);
            int end = textEdit->positionFromLineIndex(line, index + text.length());
            qDebug() << "line: " << line << " start: " << start << " end: " << end;

//            found = ui->textEdit->findNext();
//            ui->textEdit->SendScintilla(QsciScintillaBase::SCI_INDICATORFILLRANGE, line, text.length());

        }

        found = false;
    }

    //
    // second part: Mark all occurances of search term
    //
    found = textEdit->findFirst(text, use_regular_expression, is_case_sensitive, match_whole_word_only, use_wrap, search_forward);
    if (!( text.isEmpty() ) && found == true)
    {
        qDebug() << text;
        qDebug() << "found in markALL: " << found;
        textEdit->SendScintilla(QsciScintillaBase::SCI_INDICSETSTYLE, 0, QsciScintilla::INDIC_FULLBOX);
        textEdit->SendScintilla(QsciScintillaBase::SCI_INDICSETFORE,0, QColor(Qt::darkBlue));


        int end = docText.lastIndexOf(text);
        int cur = -1;

        if(end != -1)
        {
            textEdit->getCursorPosition(&line, &index);
            qDebug() << "line: " << line << " index: " << index;
            while(cur != end)
            {
                cur = docText.indexOf(text,cur + 1);
                textEdit->SendScintilla(QsciScintillaBase::SCI_INDICATORFILLRANGE,cur, text.length());
            }
        }
    } // END text.isEmpty(), END mark ALL

}

//
// search & replace:
// on_btn_next() - search for next matching word
//
void MainWindow::on_btn_next()
{
    qDebug() <<  "on_btn_next()";
    do_search_and_replace("2");
}

//
// search & replace:
// on_btn_previous() - search for previous matching word
//
void MainWindow::on_btn_previous()
{
    qDebug() <<  "on_btn_previous()";
    do_search_and_replace("1");
}

//
// search & replace:
// on_btn_replace() - replace current occourance of matching word
//
void MainWindow::on_btn_replace()
{
    qDebug() <<  "on_btn_replace()";
    do_search_and_replace("3");
}

//
// search & replace:
// on_btn_replace_all() - replace all occourances of matching word
//
void MainWindow::on_btn_replace_all()
{
    qDebug() <<  "on_btn_replace_all()";
    do_search_and_replace("4");
}

//
// search & replace:
// on_btn_hide() - hide searchGroup
//
void MainWindow::on_btn_hide()
{
    qDebug() <<  "on_btn_hide()";
    searchGroup->hide();
    p_search_is_open = false;
}

//
// Reset search Markers
//
void MainWindow::clearMarkers()
{
    int lastLine = textEdit->lines() - 1;
    textEdit->clearIndicatorRange( 0, 0, lastLine, textEdit->text( lastLine ).length() - 1, MY_MARKER_ID );
}
