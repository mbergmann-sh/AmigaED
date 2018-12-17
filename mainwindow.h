    void popNotImplemented();       // shows "not implemented" MessageBox
    void newFile();                 // sets editor into new file mode
    void open();                    // loads a file
    bool save();                    // saves current file
    bool saveAs();                  // saves current file as...
    void about();                   // pops up "about" MessageBox
    // GUI creation...
    // misc. methods for GUI manipulation
    void showCurrendCursorPosition();       // shows current cursor position in statusbar
    void documentWasModified();             // marks document as modified if text was changed
    void fitMarginLines();                  // adjusts margin sizes to fit line numbers
    // Menu actions...
    void actionGoto_matching_brace();   // jumps to matching brace
    void actionCompile();               // calls compilation of current file
    void actionEmulator();              // starts UAE
    // GUI creation...
    void createActions();                               // defines actions for menues and toolbars
    void createMenus();                                 // creates menues from actions
    void createToolBars();                              // creates toolbars from actions
    void createStatusBar();                             // sets up the statusbar
    // GUI methods...
    void readSettings();                                // read app settings
    void writeSettings();                               // write app settings
    bool maybeSave();                                   // will be called if user quits while text has changed
    void loadFile(const QString &fileName);             // open an existing file
    bool saveFile(const QString &fileName);             // save current file
    void setCurrentFile(const QString &fileName);       // will be called to store current filename and put it into window title
    QString strippedName(const QString &fullFileName);  // gives back current filename without path
    // Qscintila Editor widget instance

    // stores name of the file currently in use
    // Menues
    QMenu *fileMenue;           // holds file manipulating actions
    QMenu *editMenue;           // holds copy, paste and some more...
    QMenu *navigationMenue;     // holds actions to move around in text
    QMenu *viewMenue;           // holds actions to change editors view
    QMenu *buildMenue;          // holds compiler / build actions
    QMenu *helpMenue;           // holds help topics
    QMenu *miscMenue;           // holds misc actions
    QMenu *insertMenu;          // holds snippet insertions

    // Toolbars
    QToolBar *fileToolBar;          // holds file manipulating actions
    QToolBar *editToolBar;          // holds copy, paste and some more...
    QToolBar *navigationToolBar;    // holds actions to move around in text
    QToolBar *buildToolBar;         // holds compiler / build actions
    QToolBar *miscToolBar;          // holds misc actions
    //Actions
    QAction *newAct;                // create new empty window
    QAction *openAct;               // open file
    QAction *saveAct;               // save file
    QAction *saveAsAct;             // save file as...
    QAction *exitAct;               // quit the app
    QAction *cutAct;                // copy marked text into clipboard and delete original
    QAction *copyAct;               // copy marked text into clipboard
    QAction *pasteAct;              // paste clipboard
    QAction *aboutAct;              // show about message
    QAction *aboutQtAct;            // show about-Qt message
    QAction *gotoLineAct;           // jump to line X...
    QAction *toggleFoldAct;         // toggle text folding
    QAction *gotoMatchingBraceAct;  // jump to matching brace
    QAction *compileAct;            // calls compilation of current file
    QAction *emulatorAct;           // start UAE

    // statusbar widgets
    QLCDNumber  *statusLCD_X;       // shows cursor's line coordinate
    QLCDNumber  *statusLCD_Y;       // shows cursor's column coordinate
    // Font in use
    // is document text folded?
    bool foldall;
