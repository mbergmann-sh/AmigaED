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
#include <QTextStream>
#include <QStatusBar>
#include <QLabel>
#include <QLCDNumber>
#include <QStatusTipEvent>

#include <QDebug>

class QAction;
class QMenu;
class QsciScintilla;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QString cmdFileName);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void popNotImplemented();
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void initializeFolding();
    void showCurrendCursorPosition();
    void documentWasModified();
    void actionGoto_Line();
    void actionGoto_matching_brace();
    void actionCompile();
    void actionEmulator();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);

    QsciScintilla *textEdit;
    QString curFile;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *navigationMenue;
    QMenu *viewMenue;
    QMenu *buildMenue;
    QMenu *helpMenu;
    QMenu *miscMenue;
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QToolBar *navigationToolBar;
    QToolBar *buildToolBar;
    QToolBar *miscToolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *aboutAct;
    QAction *aboutQtAct;
    QAction *gotoLineAct;
    QAction *gotoMatchingBraceAct;
    QAction *compileAct;
    QAction *emulatorAct;

    QLabel *statusLabelX;
    QLabel *statusLabelY;
    QLCDNumber  *statusLCD_X;
    QLCDNumber  *statusLCD_Y;

};

#endif
