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

#include <Qsci/qsciscintilla.h>
#include <Qsci/qsciscintillabase.h>
#include <Qsci/qscilexercpp.h>
#include <Qsci/qscilexermakefile.h>
#include <Qsci/qscilexerbatch.h>
#include <Qsci/qscilexerpascal.h>
#include <Qsci/qscilexerfortran.h>
#include <Qsci/qscilexerfortran77.h>
#include <Qsci/qsciprinter.h>

// allways get your defaults!
#include "mainwindow.h"
#include "compilerdialog.h"

// Processes to start:
static QProcess myProcess;      // the process for running the compiler
static QProcess myEmulator;     // the process for running the emulator

// Open MainWindow with given filename...
MainWindow::MainWindow(QString cmdFileName)
{
    textEdit = new QsciScintilla;
    setCentralWidget(textEdit);

    initializeGUI();    // most initializations are done within tis method!

    // restore last saved position and size of the editor window
    readSettings();

    // react if document was modified
    connect(textEdit, SIGNAL(textChanged()), this, SLOT(documentWasModified()));

    // notify if cursor position changed
    connect(textEdit, SIGNAL(cursorPositionChanged(int, int)), this, SLOT(showCurrendCursorPosition()));

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
    if (maybeSave()) {
        writeSettings();
        event->accept();    // OK: Quit the app!
    } else {
        event->ignore();    // CANCEL: just stay where we are... ;)
    }
}

//
// jump to line #1
//
//void MainWindow::mousePressEvent(QMouseEvent *event)
//{
//    popNotImplemented();
//}

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
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this,
                "Open source file",
                QDir::currentPath(),    // look up for files in PROGDIR first!
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
        return saveAs();        // ...call saveAs dialog!
    } else
    {
        return saveFile(curFile);
    }
}

//
// save file, let user choose a filename
//
bool MainWindow::saveAs()
{
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
   QMessageBox::about(this, tr("About AmigaED"),
            tr("This <b>Application</b> example demonstrates how to "
               "write modern GUI applications using Qt and QScintilla, with a menu bar, "
               "toolbars, and a status bar.<br>"
               "<br>Source: Riverbank QScintilla example app</br>"
               "<br>Modifications and Enhancements: Michael Bergmann 2018/2019</br>"));
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
    searchAct->setShortcut(tr("Alt+r"));
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
    toggleFoldAct->setShortcut(tr("Ctrl+f"));
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
    showIndentationGuidesAct->setChecked(false);
    showIndentationGuidesAct->setStatusTip(tr("Toggle indentation guides visibility"));
    connect(showIndentationGuidesAct, SIGNAL(triggered()), this, SLOT(actionShowIndentationGuides()));

    /* --- Build -----------------------------------------------------------------------*/
    selectCompilerVBCCAct = new QAction(tr("VBCC vc (C mode only)..."), this);
    selectCompilerVBCCAct->setStatusTip(tr("Set Compiler to VBCC (C mode only)..."));
    selectCompilerVBCCAct->setCheckable(true);
    selectCompilerVBCCAct->setChecked(false);
    connect(selectCompilerVBCCAct, SIGNAL(triggered()), this, SLOT(actionSelectCompilerVBCC()));

    selectCompilerGCCAct = new QAction(tr("GNU gcc (C mode)..."), this);
    selectCompilerGCCAct->setStatusTip(tr("Set Compiler to GNU gcc (C mode)..."));
    selectCompilerGCCAct->setCheckable(true);
    selectCompilerGCCAct->setChecked(false);
    connect(selectCompilerGCCAct, SIGNAL(triggered()), this, SLOT(actionSelectCompilerGCC()));

    selectCompilerGPPAct = new QAction(tr("GNU g++ (C++ mode)..."), this);
    selectCompilerGPPAct->setStatusTip(tr("Set Compiler to GNU g++ (C++ mode)..."));
    selectCompilerGPPAct->setCheckable(true);
    selectCompilerGPPAct->setChecked(false);
    connect(selectCompilerGPPAct, SIGNAL(triggered()), this, SLOT(actionSelectCompilerGPP()));

    // this will put the compilers in our menue into a mutual exclusive
    // group for automatically checking/unchecking each other:
    compilerGroup = new QActionGroup(this);
    compilerGroup->addAction(selectCompilerVBCCAct);
    compilerGroup->addAction(selectCompilerGCCAct);
    compilerGroup->addAction(selectCompilerGPPAct);

    compileAct = new QAction(QIcon(":/images/dice.png"), tr("Comp&ile..."), this);
    compileAct->setShortcut(tr("Ctrl+r"));
    compileAct->setStatusTip(tr("Compile current file..."));
    connect(compileAct, SIGNAL(triggered()), this, SLOT(actionCompile()));

    /* --- Tools -----------------------------------------------------------------------*/
    emulatorAct = new QAction(QIcon(":/images/workbench.png"), tr("Start UA&E..."), this);
    emulatorAct->setShortcut(tr("Ctrl+e"));
    emulatorAct->setStatusTip(tr("Start Amiga Emulation..."));
    connect(emulatorAct, SIGNAL(triggered()), this, SLOT(actionEmulator()));

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
    toolsMenue->addAction(emulatorAct);

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
    buildToolBar->addAction(emulatorAct);

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

//
// read app's last saved position and sizes
//
void MainWindow::readSettings()
{
    QSettings settings("MB-SoftWorX", "QScintilla Amiga Cross Editor Example");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

//
// Store app's last known position and sizes
//
void MainWindow::writeSettings()
{
    QSettings settings("MB-SoftWorX", "QScintilla Amiga Cross Editor Example");
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
// user defined file creationloading for source files
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
    //textEdit->setCaretLineVisible(true);
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
        textEdit->setCaretLineVisible(true);
        this->showCaretLineAct->setChecked(true);
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
    compilerCombo->setCurrentIndex(0);
    SelectCompiler(0);
}

//
// select a compiler to use: gcc
//
void MainWindow::actionSelectCompilerGCC()
{
    qDebug() << "GCC selection called.";
    compilerCombo->setCurrentIndex(1);
    SelectCompiler(1);
}

//
// select a compiler to use: VBCC
//
void MainWindow::actionSelectCompilerGPP()
{
    qDebug() << "g++ selection called.";
    compilerCombo->setCurrentIndex(2);
    SelectCompiler(2);
}

//
// select a compiler to use (vbcc, gcc, g++)
//
void MainWindow::SelectCompiler(int index)
{
    qDebug() << "Compiler selection called.";
    qDebug() << "Value: " << index;
    switch(index)
    {
    case 0:
        p_selected_compiler = p_compiler_vc;
        p_selected_compiler_args = p_compiler_vc_call;
        p_compiledFileSuffix = "_vc";
        // check selected menu item, uncheck others
        selectCompilerVBCCAct->setChecked(true);
        break;
    case 1:
        p_selected_compiler = p_compiler_gcc;
        p_selected_compiler_args = p_compiler_gcc_call;
        p_compiledFileSuffix = "_gcc";
        // check selected menu item, uncheck others
        selectCompilerGCCAct->setChecked(true);
        break;
    case 2:
        p_selected_compiler = p_compiler_gpp;
        p_selected_compiler_args = p_compiler_gcc_call;
        p_compiledFileSuffix = "_gpp";
        // check selected menu item, uncheck others
        selectCompilerGPPAct->setChecked(true);
        break;
    }
}

//
// Compile current file
// CHANGE THIS according to your compiler and opts!
//
void MainWindow::actionCompile()
{
    QString temp_compiler_call, mbox_title;

    // set approbriate title for QInputDialog:
    if(selectCompilerVBCCAct->isChecked())
        mbox_title = "vbcc";
    if(selectCompilerGCCAct->isChecked())
        mbox_title = "m68k-amigaos-gcc";
    if(selectCompilerGPPAct->isChecked())
        mbox_title = "m68k-amigaos-g++";


    QString text = p_selected_compiler_args;
    bool ok;
    qDebug() << "START: Proc started " << p_proc_is_started << " times.";

    text = QInputDialog::getText(this, mbox_title,
                                       tr("Compiler Options:"), QLineEdit::Normal,
                                       p_selected_compiler_args, &ok);
    if (ok && !text.isEmpty())
    {
        save();
        QString outName = QFileInfo(curFile).baseName();
        QString outPath = QFileInfo(curFile).absolutePath();

        // construct path and name of compiled file for file checking:
        p_compiledFile = outPath + QDir::separator() + outName + p_compiledFileSuffix;
        qDebug() << "compiled file: " << p_compiledFile;

        temp_compiler_call = p_selected_compiler_args;                                                       // store compiler parameters temporarily
        text.append(curFile + " -o " + outPath + QDir::separator() + outName + p_compiledFileSuffix);        // add output file name
        qDebug() << "Text not empty: " << text;
        p_selected_compiler_args = text;

        //
        // put REAL compiler start HERE!
        // (uses p_compiler and p_compiler_call in mainwindow.h as default options)
        //
        startCompiler();

        // afterwards, reset everything to its defaults!
        text.clear();
        p_selected_compiler_args.clear();
        p_selected_compiler_args = temp_compiler_call;
    }
    else
    {
      text.append(curFile);
      p_compiler_call = text;

      qDebug() << "Text: " << text;
      qDebug() << "Compiler call: " << p_compiler_call;
      text.clear();
    }
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
    textEdit->insertAt("\tprintf(\"\\nCompiler in use: StormC3.\\n\");\n", ++line, 0);
    textEdit->insertAt("#elif defined(__STORMGCC__)\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler is StormGCC4 */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"\\nCompiler in use: GNU gcc, StormC4 flavour.\\n\");\n", ++line, 0);
    textEdit->insertAt("#elif defined(__MAXON__)\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler is Maxon/HiSoft C++ */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"\\nCompiler in use: Maxon/HiSoft C++.\\n\");\n", ++line, 0);
    textEdit->insertAt("#elif defined(__GNUC__)\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler is gcc */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"\\nCompiler in use: GNU gcc v%d.%d Patchlevel %d.\\n\", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);\n", ++line, 0);
    textEdit->insertAt("#elif defined(__VBCC__)\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler is vbcc */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"\\nCompiler in use: vbcc.\\n\");\n", ++line, 0);
    textEdit->insertAt("#elif defined(__SASC)\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler is SAS/C */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"\\nCompiler in use: SAS/C.\\n\");\n", ++line, 0);
    textEdit->insertAt("#elif defined(LATTICE)\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler is Lattice C */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"\\nCompiler in use: Lattice C.\\n\");\n", ++line, 0);
    textEdit->insertAt("#elif defined(AZTEC_C)\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler is Aztec C */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"\\nCompiler in use: Manx Aztec C.\\n\");\n", ++line, 0);
    textEdit->insertAt("#elif defined(_DCC)\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler is dice */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"\\nCompiler in use: dice.\\n\");\n", ++line, 0);
    textEdit->insertAt("#else\n", ++line, 0);
    textEdit->insertAt("\t/* Compiler not identified */\n", ++line, 0);
    textEdit->insertAt("\tprintf(\"\\nCompiler was not identified.\\n\");\n", ++line, 0);
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

        // Now let's build a fileheader containing some information
        int line, index;
        textEdit->getCursorPosition(&line, &index); // get the position...

        // ...now insert the first line of text!
        textEdit->insert("/*\n");
        // next, we need to continue printing at a certain location:
        textEdit->insertAt(" *\tFile:\t\t " + strippedName(curFile)+ "\n", ++line, 0);
        textEdit->insertAt(" *\tVersion:\t\t1.0\n", ++line, 0);
        textEdit->insertAt(" *\tRevision:\t\t0\n", ++line, 0);
        textEdit->insertAt(" *\n", ++line, 0);
        textEdit->insertAt(" *\tDescription:\t\tCHANGE_ME\n", ++line, 0);
        textEdit->insertAt(" *\tPurpose:\t\tCHANGE_ME\n", ++line, 0);
        textEdit->insertAt(" *\n", ++line, 0);
        textEdit->insertAt(" *\tAuthor:\t\t" + p_author + "\n", ++line, 0);
        textEdit->insertAt(" *\tEmail:\t\t" + p_email + "\n", ++line, 0);
        textEdit->insertAt(" */\n", ++line, 0);
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!

        // Let's do some versioning...
        textEdit->insertAt("/* ------------- PROGNAME & VERSION ---------------------------------- */\n", ++line, 0);
        textEdit->insertAt("#define PROGRAMNAME\t\t\"my_app\"\n", ++line, 0);
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
        textEdit->insertAt("#include\t<proto/asl.h>\n", ++line, 0);
        textEdit->insertAt("#include\t<proto/dos.h>\n", ++line, 0);
        textEdit->insertAt("#include\t<proto/exec.h>\n", ++line, 0);
        textEdit->insertAt("#include\t<proto/intuition.h>\n", ++line, 0);
        textEdit->insertAt("#include\t<proto/locale.h>\n", ++line, 0);
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!
        textEdit->insertAt("/* --- console output --- */\n", ++line, 0);
        textEdit->insertAt("#include\t<stdio.h>\n", ++line, 0);


        // it's good style to provide function prototypes...
        textEdit->insertAt("\n", ++line, 0);  // insert empty line!
        textEdit->insertAt("/* ------------- FUNCTION PROTOS -------------------------------------- */\n", ++line, 0);
        textEdit->insertAt("extern int main(int argc, char* argv[]);\n", ++line, 0);

        // ok - let's build an Amiga-style version tag:
        QString sas_versionstring = "\tconst UBYTE VersionTag[] = \"$VER: \" PROGRAMNAME \" \" VERSIONSTRING \" \" __AMIGADATE__ \"\\n\\0\";";
        QString dice_versionstring = "\tconst UBYTE VersionTag[] = \"$VER: \" PROGRAMNAME \" \" VERSIONSTRING \" (\" __COMMODORE_DATE__ \")\\n\\0\";";
        QString other_versionstring = "\tconst UBYTE VersionTag[] = \"$VER: \" PROGRAMNAME \" \" VERSIONSTRING \" (\" __DATE__ \")\\n\\0\";";

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

        // Now let's build a fileheader containing some information
        int line, index;
        textEdit->getCursorPosition(&line, &index); // get the position...

        // ...now insert the first line of text!
        textEdit->insert("/*\n");
        // next, we need to continue printing at a certain location:
        textEdit->insertAt(" *\tFile:\t\t " + strippedName(curFile)+ "\n", ++line, 0);
        textEdit->insertAt(" *\tVersion:\t\t1.0\n", ++line, 0);
        textEdit->insertAt(" *\tRevision:\t\t0\n", ++line, 0);
        textEdit->insertAt(" *\n", ++line, 0);
        textEdit->insertAt(" *\tDescription:\t\tCHANGE_ME\n", ++line, 0);
        textEdit->insertAt(" *\tPurpose:\t\tCHANGE_ME\n", ++line, 0);
        textEdit->insertAt(" *\n", ++line, 0);
        textEdit->insertAt(" *\tAuthor:\t\t" + p_author + "\n", ++line, 0);
        textEdit->insertAt(" *\tEmail:\t\t" + p_email + "\n", ++line, 0);
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

        // Now let's build a fileheader containing some information
        int line, index;
        textEdit->getCursorPosition(&line, &index); // get the position...

        // ...now insert the first line of text!
        textEdit->insert("/*\n");
        // next, we need to continue printing at a certain location:
        textEdit->insertAt(" *\tFile:\t\t " + strippedName(curFile)+ "\n", ++line, 0);
        textEdit->insertAt(" *\tVersion:\t\t1.0\n", ++line, 0);
        textEdit->insertAt(" *\tRevision:\t\t0\n", ++line, 0);
        textEdit->insertAt(" *\n", ++line, 0);
        textEdit->insertAt(" *\tDescription:\t\tCHANGE_ME\n", ++line, 0);
        textEdit->insertAt(" *\tPurpose:\t\tCHANGE_ME\n", ++line, 0);
        textEdit->insertAt(" *\n", ++line, 0);
        textEdit->insertAt(" *\tAuthor:\t\t" + p_author + "\n", ++line, 0);
        textEdit->insertAt(" *\tEmail:\t\t" + p_email + "\n", ++line, 0);
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
    textEdit->insertAt(" *\tDescription:\t\tCHANGE_ME\n", ++line, 0);
    textEdit->insertAt(" *\tPurpose:\t\tCHANGE_ME\n", ++line, 0);
    textEdit->insertAt(" *\n", ++line, 0);
    textEdit->insertAt(" *\tAuthor:\t\t" + p_author + "\n", ++line, 0);
    textEdit->insertAt(" *\tEmail:\t\t" + p_email + "\n", ++line, 0);
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
// CHANGE THIS according to your installation path and UAE flavour!
//
void MainWindow::actionEmulator()
{
    qDebug() << "in Emulator";
    popNotImplemented();

    bool ok;
          QString text = QInputDialog::getText(this, tr("Start Emulator"),
                                               tr("UAE Options:"), QLineEdit::Normal,
                                               "C:/Program Files/WinUAE/winuae64.exe -f E:/AmiKit/WinUAE/Configurations/GCC-AmigaOS_HD.uae", &ok);
          if (ok && !text.isEmpty())
          {
              ;
          }
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
// Toggle visibility of caret line
//
void MainWindow::actionSearch()
{
    bool ok;
    qDebug() << "in search";
    QInputDialog *searchDialog = new QInputDialog(this);


    QString text = searchDialog->getText(this, tr("Amiga Cross Editor - Search"),
                                       tr("Search text:"), QLineEdit::Normal,
                                       nullptr, &ok);
    searchDialog->setOkButtonText("Search!");

    if (ok && !text.isEmpty())
    {
        qDebug() << text;
        textEdit->SendScintilla(QsciScintillaBase::SCI_INDICSETSTYLE,0, 7);

        QString docText = textEdit->text();
        int end = docText.lastIndexOf(text);
        int cur = -1;

        if(end != -1)
        {
            while(cur != end)
            {
                cur = docText.indexOf(text,cur+1);
                textEdit->SendScintilla(QsciScintillaBase::SCI_INDICATORFILLRANGE,cur,
                    text.length());
            }
        }

    }
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
        qDebug() << "Running on Mac. Font is SF Mono Regular now!";
    #elif defined(__unix__)
        QFont font("Source Code Pro", 9);
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
    statusLCD_X->display(line + 1);
    statusLCD_Y->display(index +1);
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
    this->setWindowIcon(QIcon(":/images/lemming.png"));
    this->setWindowTitle("Amiga Cross Editor");

    // set a decent start value for app's size
    this->setMinimumSize(600, 450);

    // prepare and initialize statusbar items:
    this->compilerLabel = new QLabel(this);
    this->compilerCombo = new QComboBox(this);
    this->statusLabelX = new QLabel(this);

    this->statusLCD_X = new QLCDNumber(this);
    this->statusLCD_X->display(0);

    this->statusLabelY = new QLabel(this);

    this->statusLCD_Y = new QLCDNumber(this);
    this->statusLCD_Y->display(0);

    // permanently add the controls to the status bar
    statusBar()->addPermanentWidget(compilerLabel);
    compilerLabel->setText("Compiler:");
    statusBar()->addPermanentWidget(compilerCombo);
    compilerCombo->addItems(p_Compilers);
    compilerCombo->setCurrentIndex(1);
    statusBar()->addPermanentWidget(statusLabelX);
    statusLabelX->setText(tr("Line:"));
    statusBar()->addPermanentWidget(statusLCD_X);
    statusLCD_X->display(1);
    statusBar()->addPermanentWidget(statusLabelY);
    statusLabelY->setText(tr("Column:"));
    statusBar()->addPermanentWidget(statusLCD_Y);
    statusLCD_Y->display(1);

    // give some blackish style to MainWindow
    // ...surely looks nasty on OS X, so we will skip that for Mac!
    #if !defined(__APPLE__)
        this->setStyleSheet(QString::fromUtf8("background-color: rgb(175, 175, 175);"));
    #endif

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

    // Set default Compiler
    p_selected_compiler = p_compiler_gcc;
    p_selected_compiler_args = p_compiler_gcc_call;
    compilerCombo->setCurrentIndex(1);
    selectCompilerGCCAct->setChecked(true);
    p_compiledFileSuffix = "_gcc";

    connect(compilerCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(SelectCompiler(int)));

    //connect(&proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(ProcBeendet(int, QProcess::ExitStatus)));
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
        qDebug() << "compiled file: " << p_compiledFile << " exists.";
        return true;
    }
    else
    {
        qDebug() << "compiled file: " << p_compiledFile << " does not exist!";
        return false;
    }
}

/**************************************
 * Stuff for launching a Compiler... **
 **************************************/
int MainWindow::startProc(QString infile, QString outfile)
{
    qDebug() << "startProc() called.";
     //debugVars();
    QString command = p_selected_compiler;
    QStringList arguments;
    arguments << p_selected_compiler_args.split(" ");

    createStatusBarMessage(tr("Compiler run has been started."),0);

    myProcess.start(command, arguments);

    //QObject::connect(&myProcess, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
   // QObject::connect(&myProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(compiler_readyReadStandardOutput()));
    QObject::connect(&myProcess, SIGNAL(started()), this, SLOT(started()));

    //qDebug() << myProcess.command;
    qDebug() << myProcess.arguments();

    return 0;
}

int MainWindow::startCompiler()
{
    //debugVars();
    QString command = p_selected_compiler;
    QStringList arguments;
    // IMPORTANT! 'arguments' must be a QStringList, NOT a QString, else the compiler call will not work!
    // Therefore, we'll have to put each argument separately.
    // Each argument in our string is separated by a whitespace.
    // We use the split() function to isolate them, giving each argument separately to our QStringList.
    arguments << p_selected_compiler_args.split(" ");

    createStatusBarMessage(tr("Compiler run has been started."),0);

    myProcess.start(command, arguments);

    qDebug() << "\nCommand: " << command;
    qDebug() << "Argumets: " << myProcess.arguments();

    return 0;
}

void MainWindow::error(QProcess::ProcessError error)
{
  qDebug() << "Error: " << error;
}

void MainWindow::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitCode == 0)
    {
//        ui->actionStart_im_Emulator->setEnabled(true);
//        ui->btnStart->setEnabled(true);
//        ui->actionCompilieren->setDisabled(true);
//        ui->btnCompile->setDisabled(true);
//        ui->leSource->clear();
//        ui->leTarget->clear();
    }
    else
    {
//        ui->actionStart_im_Emulator->setDisabled(true);
//        ui->btnStart->setDisabled(true);
//        ui->actionCompilieren->setDisabled(true);
//        ui->btnCompile->setDisabled(true);

    }
  qDebug() << "Finished: " << exitCode;
  //qApp->exit();

  if (exitStatus==QProcess::CrashExit || exitCode!=0)
  {      
      createStatusBarMessage("Compiler error!", 0);
      (void)QMessageBox::critical(this, tr("Amiga Cross Editor"),
      tr("Build error!\n"
      "Please check source for errors and recompile."),
      QMessageBox::Ok);
  }
  else
  {
      // Let's check if the compiler produced an executable file:
      if(fileExists(p_compiledFile))
      {
          createStatusBarMessage("Compiler run finished.", 0);
          //ui->actionStart_im_Emulator->setEnabled(true);

          (void)QMessageBox::information(this, tr("Amiga Cross Editor"),
          tr("Successfully compiled.\n"
          "You may now want to test it in UAE."),
          QMessageBox::Ok);

          createStatusBarMessage("Compiler run finished successfully.", 0);
      }
      else
      {
          //ui->actionStart_im_Emulator->setEnabled(true);

          (void)QMessageBox::information(this, tr("Amiga Cross Editor"),
          tr("No success in building your executable file!.\n"
          "Please check for Errors and recompile."),
          QMessageBox::Ok);

          createStatusBarMessage("Compiler run finished unsuccessfully.", 0);
      }

  }
}

void MainWindow::readyReadStandardError()
{
  qDebug() << "ReadyError";
}

void MainWindow::readyReadStandardOutput()
{
  qDebug() << "readyOut";
  QProcess *myProcess = (QProcess *)sender();
  QByteArray buf = myProcess->readAllStandardOutput();

  QFile data(s_projectdir + QDir::separator() + "compiler_out.txt");
  qDebug() << "logfile: " << s_projectdir + QDir::separator() + "compiler_out.txt";
  if (data.open(QFile::WriteOnly | QFile::Truncate))
  {
      //ui->textBrowser->clear();
      QTextStream out(&data);
      out << buf;
      //ui->textBrowser->append(buf);
  }
}

void MainWindow::started()
{
  qDebug() << "START: Proc Started";
}

void MainWindow::emu_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitCode == 0)
    {
        qDebug() << "Emulator terminated regularily.";
    }
    else
    {
        qDebug() << "Emulator NOT terminated regularily.";
    }
  qDebug() << "Finished: " << exitCode;

  if (exitStatus==QProcess::CrashExit || exitCode!=0)
  {
     createStatusBarMessage("CrashExit - Problem mit dem Emulator!!", 0);
  }
  else
  {
      createStatusBarMessage("Emulator normal beendet.", 0);
  }
}

void MainWindow::emu_readyReadStandardOutput()
{
  qDebug() << "readyOut";
  QProcess *myEmulator = (QProcess *)sender();
  QByteArray buf = myEmulator->readAllStandardOutput();

  qDebug() << buf;
}

void MainWindow::compiler_readyReadStandardOutput()
{
  qDebug() << "readyOut";
  QProcess *myProcess = (QProcess *)sender();
  QByteArray buf = myProcess->readAllStandardOutput();

  qDebug() << buf;
}



