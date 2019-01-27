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

#include <QtGui>
#include <QMainWindow>
#include <QSettings>
#include <QMessageBox>
#include <QString>
#include <QStyle>
#include <QStyleFactory>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QStatusBar>
#include <QLabel>
#include <QLCDNumber>
#include <QComboBox>
#include <QStatusTipEvent>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QPrinter>
#include <QPrintDialog>
#include <QPrintEngine>
#include <QAction>
#include <QActionGroup>
#include <QActionEvent>
#include <QPoint>
#include <QProcess>
#include <QSplitter>
#include <QListView>
#include <QTextBrowser>
#include <QTextEdit>
#include <QPlainTextEdit>
#include <QTextCursor>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QShortcut>
#include <QTimer>
#include <QElapsedTimer>
#include <QTime>
#include <QGridLayout>
#include <QFormLayout>
#include <QCheckBox>
#include <QLineEdit>
#include <QLabel>
#include <QSpacerItem>

#include <QDebug>

class QAction;
class QActionGroup;
class QMenu;
class QLabel;
class QComboBox;
class QsciScintilla;
class PrefsDialog;
class aboutDialog;
class QTimer;
class QTime;
class QElapsedTimer;
class QtGui;

#define CHECKTIME(x)  \
    QElapsedTimer CONCAT(sb_, __LINE__); \
    CONCAT(sb_, __LINE__).start(); \
    x \
    qDebug() << __FUNCTION__ << ":" << __LINE__ << " Elapsed time: " <<  CONCAT(sb_, __LINE__).elapsed() << " ms.";

class MainWindow : public QMainWindow
{
    // give the meta compiler a chance!
    Q_OBJECT

public:
    explicit MainWindow(QString cmdFileName);
    bool fileExists(QString path);
    bool allready_selected = false;
    int last_selected_line = -1;
    #define MY_MARKER_ID 0

    // Layout and content for searchGroup
    QGridLayout *gridLayout_2;
    QLabel *label_find;
    QLineEdit *lineEdit_find;
    QGridLayout *gridLayout;
    QPushButton *btn_previous;
    QPushButton *btn_next;
    QLabel *label_replace;
    QLineEdit *lineEdit_replace;
    QFormLayout *formLayout_2;
    QPushButton *btn_replace;
    QPushButton *btn_replace_all;
    QPushButton *btn_hide;
    QFormLayout *formLayout;
    QCheckBox *checkBox_CaseSensitive;
    QCheckBox *checkBox_WholeWords;
    QCheckBox *checkBox_SearchForwards;
    QSpacerItem *horizontalSpacer_2;
    QWidget *centerSearchForm;

    QString line;
    QStringList fields;
    QTime timerCompile;         // Timer for compilation time
    int nMilliseconds;          // keeping compile time
    QString successMessage;     // build success message with compile time
    int line_nr = 0, column_nr = 0, error_nr = 0;   // keeps line and column numbers
    QString errortype;                              // keeps error type ('error' or 'warning')
    QString debugfilename;                          // keeps filename or unixpath


    // vars for controlling file header comments
    QString p_purpose;
    QString p_author;
    QString p_email;
    QString p_website;
    QString p_version = "1.0";
    QString p_revision = "0";
    QString p_description;
    QString p_default_icon;
    QString p_compiler ;                     // C-Compiler to call...
    QString p_compiler_call;
    QString p_compiler_gcc;             // Path to gcc executable
    QString p_compiler_gpp;             // Path to g++ executable
    QString p_compiler_vc;              // Path to vc executable
    QString p_compiler_vasm;            // Path to vasm executable (unused ATM)
    QString p_compiler_gcc_call;                // Arguments for compilation with gcc (C-Mode)..
    QString p_compiler_gpp_call;                // Arguments for compilation with g++ (C++ Mode)..
    QString p_compiler_vc13_call;               // Arguments for vbcc targetting OS 1.3
    QString p_compiler_vc30_call;               // Arguments for vbcc targetting OS 2.x up to OS 3.x
    QString p_compiler_vc40_call;               // Arguments for vbcc targetting OS 4.1
    int  p_compiler_vc_default_target;          // holds combobox value of selected vbcc default target (OSses 1.3 up to 4.1)
    QString p_selected_compiler;                // holds value of compiler to use for recent compilation
    QString p_selected_compiler_args;           // holds value for default compiler args to use (constructed in method 'startCompiler()' )
    QString p_vbcc_config_dir;                  // Path to vbcc config dir, unused ATM
    QString p_make;                             // Path to make, unused ATM
    QString p_strip;                            // Path to strip, unused ATM
    QString p_compiledFile;                     // keep the currently compiled file for file checking
    QString p_compiledFileSuffix;               // keep filename suffix for compiled output
    QFileInfo p_stripped_name;                  // stripped filename for constructing exe file name and icon
    QString p_emulator;             // path to emulator to start, used as 'command'
    QString p_os13_config;          // path to OS 1.3 emulator config-file, used as 'argument'
    QString p_os20_config;          // path to OS 2.1 emulator config-file, used as 'argument'
    QString p_os30_config;          // path to OS 3.x emulator config-file, used as 'argument'
    QString p_os40_config;          // path to OS 4.1 emulator config-file, used as 'argument'
    int p_defaultEmulator;          // default emulator to start, setted by prefs > emulator tab combobox
    QString p_emulator_to_start;    // Argument for default OS to start in UAE, depends on p_defaultEmulator
    QString p_projectsRootDir;      // Path to default folder to store projects in (use that path as a hd mount in UAE in order to test compiled app!)
    QStringList p_Compilers = {"VBCC - C", "GNU - C", "GNU - C++"};    // used for building compiler preselection combobox entries
    QStringList p_targetOS = {"OS 1.3", "OS 3.x", "OS 4.x"};
    int p_defaultCompiler;          // set from prefs file
    QString p_default_style;        // set from prefs file
    bool p_blackish;                // use blackish stylesheet?
    bool p_show_indentation;        // show indentation guidelines by default?
    bool p_mydebug = false;         // show or hide debugging informations
    bool p_no_lcd_statusbar;        // use normal text instead of LCD for cursor position view
    bool p_no_compilerbuttons;      // hide compiler selector and compile button from statusbar
    bool p_simple_statusbar;        // show basic statusbar only
    bool p_create_icon;             // create icon for compiled program?
    bool p_console_on_fail;         // open console only if compilation fails
    bool p_no_warn_requesters;      // reduce messegebox show-up
    bool p_show_gcc_opts;           // show options requester every time compilation with gcc/g++ is triggered?
    bool p_show_vbcc_opts;          // show options requester every time compilation with vbcc is triggered?

    // Setter for prefs vars
    void setCompilerGCC(QString compiler);


public slots:
    // Custom context menue:
    void showCustomContextMenue(const QPoint &pos); // implements custom context menu for QScintilla
    // methods for launching a compiler
    void error(QProcess::ProcessError error);
    void stateChanged(QProcess::ProcessState state);

    void readyReadStandardError();
    void readyReadStandardOutput();
    void started();
    void emu_finished(int exitCode, QProcess::ExitStatus exitStatus);
    void emu_readyReadStandardOutput();
    void compiler_readyReadStandardOutput();
    void SelectCompiler(int index);
    void debugVars();
    void runCommand(QString command, QStringList arguments);
    void readCommand();
    int stopCommand(int exitCode, QProcess::ExitStatus exitStatus);
    void actionKillEmulator();
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void readPosSettings();
    void readSettings();                                // read app settings
    void jumpCompilerWarnings();                        // jump to error or warning, load file of occurance if not opened (unfinisched yet!)
    void setVbccTargetOS(int default_os);               // change VBCC default target OS at runtime
    void on_output_cursorPositionChanged();             // react on user click in order to jump to error/warning in editor window
    void checkVBCC(QString str_to_search);              // RegEX VBCC messages : jump to line x
    void checkGCC(QString str_to_search);               // RegEX GCC/G++ messages : jump to line x
    void jumpToError(int error_line, int error_column); // set cursor to error line and column
    void testGCCregEx(QString str_to_search);    // to be deleted ASAP!

private slots:
    void call_do_search_and_replace();
    void clearMarkers();                            // clear marked occourances in case of new search
    int startEmulator();                            // starts a process (f.e. Emulator)
    int startCompiler();                            // starts a process (f.e. Compiler)
    void popNotImplemented();                       // shows "not implemented" MessageBox
    void newFile();                                 // sets editor into new file mode
    void open();                                    // loads a file
    bool save();                                    // saves current file
    bool saveAs();                                  // saves current file as...
    void about();                                   // pops up "about" MessageBox
    void startPrefs();                              // Workaround to start prefsDialog with a parameter
    void setEmulatorMenu();                         // disable emulator menu entries if no config was specified

    // GUI creation...
    void initializeGUI();
    void initializeLexerCPP();
    void initializeLexerMakefile();
    void initializeLexerPascal();
    void initializeLexerBatch();
    void initializeLexerFortran();
    void initializeLexerNone();
    void initializeFolding();
    void initializeMargin();
    void initializeCaretLine();
    void initializeFont();
    // misc. methods for GUI manipulation
    void showCurrendCursorPosition();       // shows current cursor position in statusbar
    void documentWasModified();             // marks document as modified if text was changed
    void fitMarginLines();                  // adjusts margin sizes to fit line numbers
    // Menu actions...
    void actionPrefsDialog(int tabindex);   // open Preferences at given TAB
    void printFile();                   // print current file
    void actionGotoTop();               // jump to line #1
    void actionGotoBottom();            // jump to last line in text
    void actionGoto_Line();             // jump to line X
    void actionGoto_matching_brace();   // jumps to matching brace
    int actionCompile();                // calls compilation of current file
    // Emulator
    bool actionEmulator();              // starts default UAE
    void actionEmuOS13();               // sets UAE default to Workbench 1.3 and calls actionEmulator()
    void actionEmuOS20();               // sets UAE default to Workbench 2.1 and calls actionEmulator()
    void actionEmuOS30();               // sets UAE default to Workbench 3.x and calls actionEmulator()
    void actionEmuOS40();               // sets UAE default to Workbench 4.1 and calls actionEmulator()

    // editMenu
    void actionSearch();                  // opens a search dialog
    // viewMenue and submenue actions
    void actionShowLineNumbers();         // show or hide line numbers
    void actionShowCaretLine();           // show or hide caret line
    void actionShowDebug();               // sets showing or hideing for debugging informations
    void actionShowEOL();                 // show or hide EOL character
    void actionShowUnprintable();         // show or hide unprintable characters
    void actionShowIndentationGuides();   // show or hide indentation guides
    // insertMenue and submenue actions:
    void actionInsertShellAppSkeletton();
    void actionInsertCAppSkeletton();
    void actionInsertCppAppSkeletton();
    void actionInsertInclude();
    void actionInsertAmigaIncludes();
    void actionInsertDefine();
    void actionInsertIfdef();
    void actionInsertIfdefined();
    void actionInsertIfdefinedCompiler();
    void actionInsertIfndef();
    void actionInsertOpenLibrary();
    void actionInsertCloseLibrary();
    void actionInsertIf();
    void actionInsertIfElse();
    void actionInsertWhile();
    void actionInsertForLoop();
    void actionInsertDoWhile();
    void actionInsertSwitch();
    void actionInsertMain();
    void actionInsertEnum();
    void actionInsertStruct();
    void actionInsertFunction();
    void actionInsertAmigaVersionString();
    void actionInsertCClass();
    void actionInsertCppClass();
    void actionInsertFileheaderComment();
    void actionInsertCSingleComment();
    void actionInsertCMultiComment();
    void actionInsertCppSingleComment();
    void actionInsertCLineDevideComment();
    void actionInsertSnippet1();
    void actionInsertSnippet2();
    void actionInsertSnippet3();
    void actionInsertSnippet4();
    void actionSelectCompilerVBCC();
    void actionSelectCompilerGCC();
    void actionSelectCompilerGPP();
    void actionToggleGccDefaultOptsDialog();
    void actionToggleVbccDefaultOptsDialog();
    // Splitter
    void actionCloseOutputConsole();
    void actionShowOutputConsole();

    // search and replace connections:
    void do_search_and_replace(QString action_str);  // search and replace for matching word
    void on_btn_next();                         // search for next occurance of matching word
    void on_btn_previous();                 // search for previous occurance ofmatching word
    void on_btn_replace();                  // replace current occurance of matching word
    void on_btn_replace_all();              // replace all occurances of matching word
    void on_btn_hide();                     // hide search & replace, set p_search_allready_open to false

private:
    QProcess *cmd;
    QProcess proc;
    QProcess myProcess;                                                 // we need a QProcess to run a compiler...
    QProcess myEmulator;

    // GUI creation...
    void createActions();                                               // defines actions for menues and toolbars
    void createMenus();                                                 // creates menues from actions
    void createToolBars();                                              // creates toolbars from actions
    void createStatusBarMessage(QString statusmessage, int timeout);    // sets up the statusbar with a custom message
    // GUI methods...
    void SetLexerAtFileExtension(QString fileName);     // Helper to set approbiate Lexer according to a file's .ext
    void actionSelectCompiler(int index);               // Helper for selcting a compiler to use
    void writeSettings();                               // write app settings
    bool maybeSave();                                   // will be called if user quits while text has changed
    int loadNonExistantFile(const QString &fileName);   // ask for creation if a file does NOT exist (used for command line loading)
    void loadFile(const QString &fileName);             // open an existing file
    bool saveFile(const QString &fileName);             // save current file
    void setCurrentFile(const QString &fileName);       // will be called to store current filename and put it into window title
    QString strippedName(const QString &fullFileName);  // gives back current filename without path
    void activateGUIdefaultSettings();
    void delay();


    // Qscintila Editor widget instance
    QsciScintilla *textEdit;
    // Instances for Splitter
    QSplitter *splitter;
    QListView *lview;
    QPlainTextEdit *output;
    QGroupBox *outputGroup;
    QGroupBox *searchGroup;
    QPushButton *btnCloseOutput;

    // stores name of the file currently in use
    QString curFile;

    // Menues
    QMenu *fileMenue;           // holds file manipulating actions
    QMenu *editMenue;           // holds copy, paste and some more...
    QMenu *insertMenue;         // holds snippet insertions
    QMenu *buildMenue;          // holds compiler / build actions
    QMenu *navigationMenue;     // holds actions to move around in text
    QMenu *viewMenue;           // holds actions to change editors view
    QMenu *tabwidthMenue;       // Submenu of viewMenu, holds different values for tab width
    QMenu *syntaxMenue;         // holds actions to change syntax lexers
    QMenu *toolsMenue;          // holds misc actions
    QMenu *emulatorMenue;       // Submenue of toolsMenue, holds startups for different Amiga emulation models
    QMenu *helpMenue;           // holds help topics
    QMenu *preprocessorMenue;   // Submenue of insertMenue, holds preprocessor inserts
    QMenu *libraryMenue;        // Submenue of insertMenue, holds library inserts
    QMenu *conditionsMenue;     // Submenue of insertMenue, holds C condition inserts
    QMenu *loopsMenue;          // Submenue of insertMenue, holds C/++ loops inserts
    QMenu *commentsMenue;       // Submenue of insertMenue, holds comment inserts
    QMenu *classMenue;          // Submenue of insertMenue, holds C/++ class inserts
    QMenu *snippetsMenue;       // Submenue of insertMenue, holds inserts for user-defined code snippets
    QMenu *templatesMenue;      // Submenue of insertMenue, holds inserts for application templates
    QMenu *compilerMenue;       // Submenue of buildMenue, holds entries for selecting a certain compiler
    QMenu *charMenue;           // Submenue of viewMenue, holds entries for EOL and unprintable characters

    // Toolbars
    QToolBar *fileToolBar;          // holds file manipulating actions
    QToolBar *editToolBar;          // holds copy, paste and some more...
    QToolBar *navigationToolBar;    // holds actions to move around in text
    QToolBar *buildToolBar;         // holds compiler / build actions
    QToolBar *toolsToolBar;         // holds misc actions
    QToolBar *searchToolBar;        // holds search/replace

    // synatxMenue/tabwithMenue mutual exclude ActionGroups
    QActionGroup *syntaxGroup;      // holds different Lexers for mutual exclusion in menue
    QActionGroup *tabwidthGroup;     // holds different values for tab with
    QActionGroup *compilerGroup;     // holds different values for compiler to use: 0 = vc, 1 = gcc, 2 = g++

    //Actions for fileMenue
    QAction *newAct;                // create new empty window
    QAction *openAct;               // open file
    QAction *saveAct;               // save file
    QAction *saveAsAct;             // save file as...
    QAction *prefsAct;              // open prefs dialog
    QAction *prefsReloadAct;        // reload prefs
    QAction *printAct;              // print current file
    QAction *exitAct;               // quit the app
    // Actions for editMenue
    QAction *cutAct;                // copy marked text into clipboard and delete original
    QAction *copyAct;               // copy marked text into clipboard
    QAction *pasteAct;              // paste clipboard
    QAction *searchAct;             // search for text
    // Actions for helpMenue
    QAction *aboutAct;              // show about message
    QAction *aboutQtAct;            // show about-Qt message
    // Actions for navigationMenue
    QAction *gotoTopAct;            // jump to line #1...
    QAction *gotoBottomAct;         // jump to line #1...
    QAction *gotoLineAct;           // jump to line X...    
    QAction *gotoMatchingBraceAct;  // jump to matching brace
    // Actions for viewMenue
    QAction *showLineNumbersAct;        // toggle visibility of Line numbers
    QAction *showCaretLineAct;          // toggle caret line
    QAction *toggleFoldAct;             // toggle text folding
    QAction *showIndentationGuidesAct;  // toggle visibility of indentation lines
    QAction *showDebugInfoAct;          // show or hide debugging informations
    QAction *showEOLAct;                // toggle visbility of EOL
    QAction *showUnprintableAct;        // toggle visibility of unprintable characters
    QAction *toggleAutoIndentAct;       // toggle automatic indentation
    QAction *toggleIndentUsesTabAct;    // use TAB or whitespace for indentation
    QAction *tabWith2Act;               // to be used in submenu tabwithMenue, sets tab with to 2
    QAction *tabWith4Act;               // to be used in submenu tabwithMenue, sets tab with to 4 (default)
    QAction *tabWith6Act;               // to be used in submenu tabwithMenue, sets tab with to 6
    QAction *tabWith8Act;               // to be used in submenu tabwithMenue, sets tab with to 8
    // Actions for buildMenue
    QAction *selectCompilerAct;          // select the compiler to use (vbcc, gcc, g++)
    QAction *selectCompilerVBCCAct;      // select the compiler to use (vbcc, gcc, g++)
    QAction *selectCompilerGCCAct;       // select the compiler to use (vbcc, gcc, g++)
    QAction *selectCompilerGPPAct;       // select the compiler to use (vbcc, gcc, g++)
    QAction *compileAct;                 // calls compilation of current file
    QAction *showOutputAct;              // pops up compiler output pane
    QAction *hideOutputAct;              // pops up compiler output pane
    QAction *toggleGccDefaultOptsAct;    // show or hide gcc/g++ default options dialog
    QAction *toggleVbccDefaultOptsAct;   // show or hide vc default options dialog
    // Actions for toolsMenue
    QAction *emulatorAct;             // start default UAE
    QAction *emulator13Act;           // start UAE with Workbench 1.3
    QAction *emulator20Act;           // start UAE with Workbench 2.1
    QAction *emulator30Act;           // start UAE with Workbench 3.x
    QAction *emulator40Act;           // start UAE with Workbench 4.1
    QAction *killEmulatorAct;         // attempt to kil a running Emulation
    // Actions for syntaxMenue
    QAction *lexCPPAct;             // switch lexer to C++ syntax
    QAction *lexBatchAct;           // switch lexer to Batch / Shell syntax
    QAction *lexMakefileAct;        // switch lexer to Makefile syntax
    QAction *lexFortranAct;         // switch lexer to Amiga Installer (e.g. more like LISP) syntax
    QAction *lexPascalAct;          // switch lexer to Pascal syntax
    QAction *lexPlainTextAct;       // switch lexer to no syntax highlighting
    // Actions for insertMenue
    // Shell App skelleton
    QAction *shellAppAct;           // abandons all the previous work and inserts a brand new file with complete AmigaShell app skelleton
    QAction *stdCAppAct;            // abandons all the previous work and inserts a brand new file with complete std. C app skelleton
    QAction *stdCppAppAct;          // abandons all the previous work and inserts a brand new file with complete std. C++app skelleton
    // Preprocessor
    QAction *includeAct;            // inserts #include <file>
    QAction *amigaIncludesAct;      // inserts the most common Amiga #include files
    QAction *defineAct;             // inserts #define SOME_VALUE
    QAction *ifdefAct;              // inserts #ifdef ... #endif
    QAction *ifdefinedAct;          // inserts #if defined(SOMETHING) ... #elif defined(SOME_OTHER_THING) ... #endif
    QAction *ifdefinedCompilerAct;  // inserts compiler identification via macro
    QAction *ifndefAct;             // inserts #ifndef ... #endif
    // Library
    QAction *OpenLibraryAct;        // inserts OpenLibrary(some.library", 0L);
    QAction *CloseLibraryAct;       // inserts CloseLibrary(some.library);
    // Conditions
    QAction *ifAct;                 // inserts if(condition){..} statement
    QAction *if_elseAct;            // inserts if(condition){...} else {...} statement
    // Loops
    QAction *whileAct;              // inserts while(condition) {...} loop
    QAction *forAct;                // inserts while(condition) {...}do loop
    QAction *do_whileAct;           // inserts do{...}while(condition) loop
    QAction *switchAct;             // inserts switch(condition) select case statements
    // Class
    QAction *c_classAct;            // inserts a C-style class skelleton
    QAction *cpp_classAct;          // inserts a C++-style class skelleton
    // Comments
    QAction *fileheaderAct;         // inserts a fileheader comment
    QAction *c_singleAct;           // inserts a C-style single line comment
    QAction *c_multiAct;            // inserts a C-style multi line comment
    QAction *cpp_singleAct;         // inserts a C++-style single line comment
    QAction *lineDevideCommentAct;  // inserts a C-style comment line devider
    // Snippets
    QAction *snippet1Act;           // inserts Snippet #1 from snippetfile1.snip
    QAction *snippet2Act;           // inserts Snippet #2 from snippetfile2.snip
    QAction *snippet3Act;           // inserts Snippet #3 from snippetfile3.snip
    QAction *snippet4Act;           // inserts Snippet #4 from snippetfile4.snip
    // main(), Function, Enum,
    // Struct, Amiga version string...
    QAction *mainAct;
    QAction *functionAct;            // inserts C function skeletton
    QAction *enumAct;                // inserts C enumeration skeletton
    QAction *structAct;              // inserts C struct skeletton
    QAction *versionStringAct;       // inserts Amiga C version string

    // statusbar widgets
    QLabel *statusLabelX;
    QLabel *statusLabelY;
    QLabel *compilerLabel;
    QLCDNumber  *statusLCD_X;       // shows cursor's line coordinate
    QLCDNumber  *statusLCD_Y;       // shows cursor's column coordinate
    QLabel *statusContainer_X;      // alternative view for cursor position
    QLabel *statusContainer_Y;      // alternative view for cursor position
    QComboBox *compilerCombo;       // puts a Compobox for compiler selection into statusbar
    QComboBox *osCombo;             // puts a Compobox for AmigaOS target selection into statusbar
    QPushButton *compilerButton;    // puts a dice button for compiler start into statusbar

    // Font in use
    QFont myfont;

    // is search allready opened?
    bool p_search_is_open = false;

    // is document text folded?
    bool foldall;
    bool p_show_compilerbutton = true;  // enable or disable Button in statusbar via prefs

    // check if there is allready a main() function in a file
    bool p_main_set = false;
    bool p_versionstring_set = false;
    int p_proc_is_started = 0;
    int p_index = 0;
    Q_PID proc_pid;


protected:
    void closeEvent(QCloseEvent *event);        // catch close() event
    //void mousePressEvent(QMouseEvent *event);   // catch mouse press event
};

#endif
