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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QString>
#include <QStyle>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QStatusBar>
#include <QLabel>
#include <QLCDNumber>
#include <QStatusTipEvent>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintEngine>

#include <QDebug>

class QAction;
class QMenu;
class QsciScintilla;

class MainWindow : public QMainWindow
{
    // give the meta compiler a chance!
    Q_OBJECT

public:
    explicit MainWindow(QString cmdFileName);

protected:
    void closeEvent(QCloseEvent *event);        // catch close() event
    void mousePressEvent(QMouseEvent *event);   // catch mouse press event

private slots:
    void popNotImplemented();       // shows "not implemented" MessageBox
    void newFile();                 // sets editor into new file mode
    void open();                    // loads a file
    bool save();                    // saves current file
    bool saveAs();                  // saves current file as...
    void about();                   // pops up "about" MessageBox
    // GUI creation...
    void initializeGUI();
    void initializeLexerCPP();
    void initializeLexerMakefile();
    void initializeLexerPascal();
    void initializeLexerBatch();
    void initializeLexerFortran();
    void initializeFolding();
    void initializeMargin();
    void initializeCaretLine();
    void initializeFont();
    // misc. methods for GUI manipulation
    void showCurrendCursorPosition();       // shows current cursor position in statusbar
    void documentWasModified();             // marks document as modified if text was changed
    void fitMarginLines();                  // adjusts margin sizes to fit line numbers
    // Menu actions...
    void printFile();                   // print current file
    void actionGotoTop();               // jump to line #1
    void actionGotoBottom();            // jump to last line in text
    void actionGoto_Line();             // jump to line X
    void actionGoto_matching_brace();   // jumps to matching brace
    void actionCompile();               // calls compilation of current file
    void actionEmulator();              // starts UAE

private:
    // GUI creation...
    void createActions();                                               // defines actions for menues and toolbars
    void createMenus();                                                 // creates menues from actions
    void createToolBars();                                              // creates toolbars from actions
    void createStatusBarMessage(QString statusmessage, int timeout);    // sets up the statusbar with a custom message
    // GUI methods...
    void SetLexerAtFileExtension(QString fileName);     // Helper to set approbiate Lexer according to a file's .ext
    void readSettings();                                // read app settings
    void writeSettings();                               // write app settings
    bool maybeSave();                                   // will be called if user quits while text has changed
    int loadNonExistantFile(const QString &fileName);   // ask for creation if a file does NOT exist (used for command line loading)
    void loadFile(const QString &fileName);             // open an existing file
    bool saveFile(const QString &fileName);             // save current file
    void setCurrentFile(const QString &fileName);       // will be called to store current filename and put it into window title
    QString strippedName(const QString &fullFileName);  // gives back current filename without path

    // Qscintila Editor widget instance
    QsciScintilla *textEdit;

    // stores name of the file currently in use
    QString curFile;

    // Menues
    QMenu *fileMenue;           // holds file manipulating actions
    QMenu *editMenue;           // holds copy, paste and some more...
    QMenu *insertMenue;         // holds snippet insertions
    QMenu *buildMenue;          // holds compiler / build actions
    QMenu *navigationMenue;     // holds actions to move around in text
    QMenu *viewMenue;           // holds actions to change editors view
    QMenu *syntaxMenue;         // holds actions to change syntax lexers
    QMenu *toolsMenue;          // holds misc actions
    QMenu *helpMenue;           // holds help topics



    // Toolbars
    QToolBar *fileToolBar;          // holds file manipulating actions
    QToolBar *editToolBar;          // holds copy, paste and some more...
    QToolBar *navigationToolBar;    // holds actions to move around in text
    QToolBar *buildToolBar;         // holds compiler / build actions
    QToolBar *toolsToolBar;          // holds misc actions

    //Actions
    QAction *newAct;                // create new empty window
    QAction *openAct;               // open file
    QAction *saveAct;               // save file
    QAction *saveAsAct;             // save file as...
    QAction *printAct;              // print current file
    QAction *exitAct;               // quit the app
    QAction *cutAct;                // copy marked text into clipboard and delete original
    QAction *copyAct;               // copy marked text into clipboard
    QAction *pasteAct;              // paste clipboard
    QAction *aboutAct;              // show about message
    QAction *aboutQtAct;            // show about-Qt message
    QAction *gotoTopAct;            // jump to line #1...
    QAction *gotoBottomAct;         // jump to line #1...
    QAction *gotoLineAct;           // jump to line X...
    QAction *toggleFoldAct;         // toggle text folding
    QAction *gotoMatchingBraceAct;  // jump to matching brace
    QAction *compileAct;            // calls compilation of current file
    QAction *emulatorAct;           // start UAE
    QAction *lexCPPAct;             // switch lexer to C++ syntax
    QAction *lexBatchAct;           // switch lexer to Batch / Shell syntax
    QAction *lexMakefileAct;        // switch lexer to Makefile syntax
    QAction *lexFortranAct;         // switch lexer to Amiga Installer (e.g. more like LISP) syntax
    QAction *lexPascalAct;          // switch lexer to Pascal syntax

    // statusbar widgets
    QLabel *statusLabelX;
    QLabel *statusLabelY;
    QLCDNumber  *statusLCD_X;       // shows cursor's line coordinate
    QLCDNumber  *statusLCD_Y;       // shows cursor's column coordinate

    // Font in use
    QFont myfont;

    // is document text folded?
    bool foldall;

    // vars for controlling file header comments
    const QString p_purpouse = "CHANGE_ME";
    const QString p_author = "Michael Bergmann";
    const QString p_email = "mb@mbergmann-sh.de";
    const QString p_version = "1.0";
    const QString p_revision = "0";
    const QString p_description = "CHANGE_ME";

};

#endif
