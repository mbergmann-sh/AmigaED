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
#include <Qsci/qscilexercpp.h>

// allways get your defaults!
#include "mainwindow.h"

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
        loadFile(cmdFileName);
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
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    popNotImplemented();
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
                "C/C++ source files (*.c *.cpp) ;; C/C++ header files (*.h *.hpp) ;; "
                "ASM source files (*.a *.asm) ;; Makefiles (*.mak) ;; "
                "AmigaE source files (*.e) ;; PASCAL source files (*.p *.pas) ;; All files (*.*)");

        if (!fileName.isEmpty())
            loadFile(fileName);
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
                                                    nullptr,    // save files into last used folder
                                                    "C/C++ source files (*.c *.cpp) ;; C/C++ header files (*.h *.hpp) ;; "
                                                    "ASM source files (*.a *.asm) ;; Makefiles (*.mak) ;; "
                                                    "AmigaE source files (*.e) ;; PASCAL source files (*.p *.pas) ;; "
                                                    "All files (*.*)");

    if (fileName.isEmpty())
        return false;

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
// you'll need some fucking actions first if you
// want to create menues!
//
void MainWindow::createActions()
{
    // Menue actions consist of:
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
    printAct->setStatusTip(tr("Print current file"));
    connect(printAct, SIGNAL(triggered()), this, SLOT(printFile()));

    exitAct = new QAction(QIcon(":/images/fileexit.png"), tr("&Exit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

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

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(textEdit, SIGNAL(copyAvailable(bool)), cutAct, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)), copyAct, SLOT(setEnabled(bool)));

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

    toggleFoldAct = new QAction(QIcon(":/images/gotoline.png"), tr("Toggle &folding..."), this);
    toggleFoldAct->setShortcut(tr("Ctrl+f"));
    connect(toggleFoldAct, SIGNAL(triggered()), this, SLOT(initializeFolding()));

    gotoMatchingBraceAct = new QAction(QIcon(":/images/brackets.png"), tr("Goto &matching bracket {} ... [] ... ()..."), this);
    gotoMatchingBraceAct->setShortcut(tr("Ctrl+b"));
    gotoMatchingBraceAct->setStatusTip(tr("Goto matching bracket..."));
    connect(gotoMatchingBraceAct, SIGNAL(triggered()), this, SLOT(actionGoto_matching_brace()));

    compileAct = new QAction(QIcon(":/images/dice.png"), tr("Comp&ile..."), this);
    compileAct->setShortcut(tr("Ctrl+r"));
    compileAct->setStatusTip(tr("Compile current file..."));
    connect(compileAct, SIGNAL(triggered()), this, SLOT(actionCompile()));

    emulatorAct = new QAction(QIcon(":/images/workbench.png"), tr("Start &UAE..."), this);
    emulatorAct->setShortcut(tr("Ctrl+e"));
    emulatorAct->setStatusTip(tr("Start Amiga Emulation..."));
    connect(emulatorAct, SIGNAL(triggered()), this, SLOT(actionEmulator()));
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

    menuBar()->addSeparator();

    // Inserts menue
    insertMenue = menuBar()->addMenu(tr("&Inserts"));

    menuBar()->addSeparator();

    // Build menue
    buildMenue = menuBar()->addMenu(tr("&Build"));
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
    viewMenue->addAction(toggleFoldAct);

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
// Show initial message in the app's status bar
//
void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
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
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("C/C++ Amiga Cross Editor"),
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
        qDebug() << state;
        // if folded: unfold first!!
        if (state > 0)
        {
            textEdit->foldAll(false);
        }
        textEdit->setCursorPosition(i-1,0);
        textEdit->setCaretLineVisible(true);
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
// Compile current file
// CHANGE THIS according to your compiler and opts!
//
void MainWindow::actionCompile()
{
    popNotImplemented();
    bool ok;
          QString text = QInputDialog::getText(this, tr("Compile!"),
                                               tr("Compiler Options:"), QLineEdit::Normal,
                                               "vc +aos68k -v -lmiee -lamiga -lauto -O3 -size -cpu=68020 ", &ok);
          if (ok && !text.isEmpty())
              qDebug() << text;
}

//
// Start UAE emulation
// CHANGE THIS according to your installation path and UAE flavour!
//
void MainWindow::actionEmulator()
{
    popNotImplemented();

    bool ok;
          QString text = QInputDialog::getText(this, tr("Start Emulator"),
                                               tr("UAE Options:"), QLineEdit::Normal,
                                               "C:/Program Files/WinUAE/winuae64.exe -f E:/AmiKit/WinUAE/Configurations/GCC-AmigaOS_HD.uae", &ok);
          if (ok && !text.isEmpty())
              qDebug() << text;
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
    #else
    QFont font("Source Code Pro", 10);
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
    textEdit->setMarginsBackgroundColor(QColor("#cccccc"));
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
// initialize C/C++ lexer bei default
//
void MainWindow::initializeLexer()
{
    QsciLexerCPP *lexer = new QsciLexerCPP();
    lexer->setDefaultFont(textEdit->font());
    lexer->setFoldComments(true);
   // textEdit->SendScintilla(textEdit->QsciScintilla::SCI_STYLESETCHARACTERSET, 1, QsciScintilla::SC_CHARSET_8859_15);
    textEdit->setLexer(lexer);
    textEdit->SendScintilla(textEdit->QsciScintilla::SCI_STYLESETCHARACTERSET, 1, QsciScintilla::SC_CHARSET_8859_15);
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
        qDebug() << "running on some kind of Mac...";
    #endif

    // give an icon and a name to the app:
    this->setWindowIcon(QIcon(":/images/lemming.png"));
    this->setWindowTitle("Amiga Cross Editor");

    // set a decent start value for app's size
    this->setMinimumSize(600, 450);

    // prepare and initialize statusbar items:
    this->statusLabelX = new QLabel(this);

    this->statusLCD_X = new QLCDNumber(this);
    this->statusLCD_X->display(0);

    this->statusLabelY = new QLabel(this);

    this->statusLCD_Y = new QLCDNumber(this);
    this->statusLCD_Y->display(0);

    // permanently add the controls to the status bar
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

    // initialize textEdit's most needed attributes:
    initializeFont();
    initializeMargin();
    initializeCaretLine();
    initializeLexer();
    initializeFolding();

    // make editor as Amiga-compatible as possible:
    textEdit->setEolMode(QsciScintilla::EolUnix);
    textEdit->setTabWidth(4);
    textEdit->setAutoIndent(true);
    textEdit->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    textEdit->SendScintilla(textEdit->QsciScintilla::SCI_STYLESETCHARACTERSET, 1, QsciScintilla::SC_CHARSET_8859_15);

    // create actions, menues, toolbars and status bar:
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
}

//
// print current file
//
void MainWindow::printFile()
{
    popNotImplemented();
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
