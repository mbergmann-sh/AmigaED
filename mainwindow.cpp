/****************************************************************************
**
** Copyright (C) 2004-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** Licensees holding a valid Qt License Agreement may use this file in
** accordance with the rights, responsibilities and obligations
** contained therein.  Please consult your licensing agreement or
** contact sales@trolltech.com if any conditions of this licensing
** agreement are not clear to you.
**
** Further information about Qt licensing is available at:
** http://www.trolltech.com/products/qt/licensing.html or by
** contacting info@trolltech.com.
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

#include <QDebug>

#include "mainwindow.h"

MainWindow::MainWindow()
{
    textEdit = new QsciScintilla;
    setCentralWidget(textEdit);
    // give an icon and a name to the app:
    this->setWindowIcon(QIcon(":/images/lemming.png"));
    this->setWindowTitle("AmigaED");
    // give some blackish style to MainWindow
    this->setStyleSheet(QString::fromUtf8("background-color: rgb(175, 175, 175);"));
    textEdit->setStyleSheet(QString::fromUtf8("background-color: rgb(175, 175, 175);"));

    // make editor as Amiga-compatible as possible:
    textEdit->setEolMode(QsciScintilla::EolUnix);
    textEdit->setTabWidth(4);
    // auto indent source codes:
    textEdit->setAutoIndent(true);
    textEdit->setIndentationGuides(true);
    textEdit->setTabIndents(true);
    textEdit->setBraceMatching(QsciScintilla::SloppyBraceMatch);
    textEdit->setEolVisibility(false);
    textEdit->SendScintilla(textEdit->QsciScintilla::SCI_STYLESETCHARACTERSET, 1, QsciScintilla::SC_CHARSET_8859_15);
    // set minium window size
    textEdit->setMinimumSize(600, 450);
    // staRT WITH ALL FUNCTIONS FOLDED
    textEdit->foldAll(true);

    // set a readable default font for Linux and Windows:
    #if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    QFont font("Courier New", 10);
    qDebug() << "running on Windows...";
    #else
    QFont font("Source Code Pro", 10);
    qDebug() << "running on Linux...";
    #endif
    QFont myfont = font;
    font.setFixedPitch(true);
    myfont.setFixedPitch(true);
    textEdit->setFont(font);

    // set margins and line numbering:
    QFontMetrics fontmetrics = QFontMetrics(textEdit->font());
    textEdit->setMarginsFont(textEdit->font());
    textEdit->setMarginWidth(0, fontmetrics.width(QString::number(textEdit->lines())) + 10);
    textEdit->setMarginLineNumbers(0, true);
    textEdit->setMarginsBackgroundColor(QColor("#cccccc"));
    textEdit->setMarginsForegroundColor(QColor("#ff0000ff"));

    // set lexer to C/C++ mode:
    QsciLexerCPP *lexer = new QsciLexerCPP();
    lexer->setDefaultFont(textEdit->font());
    lexer->setFoldComments(true);
    textEdit->setLexer(lexer);
   // textEdit->SendScintilla(textEdit->QsciScintilla::SCI_STYLESETCHARACTERSET, 1, QsciScintilla::SC_CHARSET_8859_15);



    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();

    connect(textEdit, SIGNAL(textChanged()), this, SLOT(documentWasModified()));

    setCurrentFile("");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFile("");
    }
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
    //textEdit->foldAll(false);
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this);
    if (fileName.isEmpty())
        return false;

    return saveFile(fileName);
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About AmigaED"),
            tr("This <b>Application</b> example demonstrates how to "
               "write modern GUI applications using Qt and QScintilla, with a menu bar, "
               "toolbars, and a status bar.<br>"
               "<br>Source: QScintilla example app</br>"
               "<br>Modifications and Enhancements: Michael Bergmann</br>"));
}

void MainWindow::documentWasModified()
{
    setWindowModified(textEdit->isModified());
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAct->setShortcut(tr("Ctrl+N"));
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcut(tr("Ctrl+O"));
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcut(tr("Ctrl+S"));
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

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

    gotoLineAct = new QAction(QIcon(":/images/gotoline.png"), tr("&Goto Line..."), this);
    gotoLineAct->setShortcut(tr("Ctrl+G"));
    gotoLineAct->setStatusTip(tr("Goto line X..."));
    connect(gotoLineAct, SIGNAL(triggered()), this, SLOT(actionGoto_Line()));

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

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    menuBar()->addSeparator();

    buildMenue = menuBar()->addMenu(tr("&Build"));
    buildMenue->addAction(compileAct);

    menuBar()->addSeparator();

    navigationMenue = menuBar()->addMenu(tr("&Navigation"));
    navigationMenue->addAction(gotoLineAct);
    navigationMenue->addAction(gotoMatchingBraceAct);

    menuBar()->addSeparator();

    viewMenue = menuBar()->addMenu(tr("&View"));

    menuBar()->addSeparator();

    miscMenue = menuBar()->addMenu(tr("&Misc"));
    miscMenue->addAction(emulatorAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

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

    miscToolBar = addToolBar(tr("Navigation"));
    miscToolBar->addAction(exitAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings("MB-SoftWorX", "QScintilla AmigaED Example");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("MB-SoftWorX", "QScintilla AmigaED Example");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

bool MainWindow::maybeSave()
{
    if (textEdit->isModified()) {
        int ret = QMessageBox::warning(this, tr("C/C++ AmigaED"),
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

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QMessageBox::warning(this, tr("C/C++ AmigaED"),
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

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QMessageBox::warning(this, tr("C/C++ AmigaED"),
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

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    textEdit->setModified(false);
    setWindowModified(false);

    QString shownName;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    else
        shownName = strippedName(curFile);

    setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("C/C++ AmigaED")));
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

// Jump to line No. X...
void MainWindow::actionGoto_Line()
{
    int max = textEdit->lines(); // max. count of lines in code window
    bool ok;
    int i = QInputDialog::getInt(this, tr("Goto line"),
                                 tr("Line number:"), 1, 0, max, 1, &ok);
    if (ok)
    {
        textEdit->setCursorPosition(i-1,0);
        textEdit->setCaretLineVisible(true);
        textEdit->foldLine(i-1);
    }
}

void MainWindow::actionGoto_matching_brace()
{
    textEdit->moveToMatchingBrace();
}

// compile current file
void MainWindow::actionCompile()
{
    bool ok;
          QString text = QInputDialog::getText(this, tr("Compile!"),
                                               tr("Compiler Options:"), QLineEdit::Normal,
                                               "vc +aos68k -v -lmiee -lamiga -lauto -O3 -size -cpu=68020 ", &ok);
          if (ok && !text.isEmpty())
              qDebug() << text;
}

// start uae emulation
void MainWindow::actionEmulator()
{
    bool ok;
          QString text = QInputDialog::getText(this, tr("Start Emulator"),
                                               tr("UAE Options:"), QLineEdit::Normal,
                                               "C:/Program Files/WinUAE/winuae64.exe -f E:/AmiKit/WinUAE/Configurations/GCC-AmigaOS_HD.uae", &ok);
          if (ok && !text.isEmpty())
              qDebug() << text;
}
