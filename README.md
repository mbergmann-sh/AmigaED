# AmigaED
AmigaED is an example for writing an advanced coder's editor using the Qt5 and QScintilla frameworks in C++, containing a menu bar, toolbar, menus, an enhanced statusbar and some sophisticated editing actions. The source is based on Riverbank's original editor example, as provided with the QScintilla sources. This project was started as an entry point for learning how to use QScintilla. Though somewhat of a working editor, this project is meant for studies and as a base for own enhancements.

# LICENSE
Copyright (C) 2018/2019 by Michael Bergmann. Placed in the public domain with some restrictions!

These files are part of my advanced AmigaED C/C++ editor example, using classes of the Qt and QScintilla toolkits.

You may use or enhance this piece of software anyway you want to - as long as you don't violate laws or copyright issues. I hereby explicitely prohibit the usage of my work to people who believe in racism, fascism and any kind of attitude against democratic lifestyle. It is self-explanatory that this prohibits the usage of my work to any member or fan of the german AfD party.

Further information about Qt licensing is available at: http://www.trolltech.com/products/qt/licensing.html or by contacting info@trolltech.com.

Further information about QScintilla licensing is available at: https://www.riverbankcomputing.com/software/qscintilla/license or by contacting sales@riverbankcomputing.com.

This program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

# FEATURES that work so far - a work in progress...
- load file from GUI or command line
- create new file with initial C comment header in working directory if called with command line argument
- save file to storage medium
- recognize changes in text
- code folding
- fold/unfold all (toggle folding)
- goto line number X
- goto top/bottom
- jump to matching bracket {...} | [...] | (...)
- C/C++ | Pascal | Makefile  syntax highlighting
- display plain text without syntax highlighting and folding margin
- clipboard support
- display current cursor position
- print current file if page not empty (could be done better - e.g. take parameters from default printer...)

more to come...

# Compiling the app...
...should work fine under Linux, Windows, MacOS X and probably under BSD, too (didn't test that!).
You'll need a computer with either MSVC, GCC or MinGW C/C++ compiler setup with Qt5 and QScintilla installed and working!

1. Create a folder to store the app
2. CD to this folder
3. git clone https://github.com/mbergmann-sh/AmigaED.git
4. If on Windows, open a Qt command prompt correspondending to the compiler in use. ANY system: CD to <your_folder>/AmigaED
5. qmake application.pro (creates neccessary descriptions for compiling and linking)
6. qmake (creates Makefiles for Debug and Release versions)
7. MSVC: nmake, GCC or MinGW: make -j4
8. Wait for compiler to finish

The easy way: Simply import the .pro file into Qt Creator, configure your toolchain and build the app... ;)

# Making the app run standalone on Windows
1. Open a Qt command prompt correspondending to the compiler in use 
2. CD to folder where the release version was stored
3. windeployqt --angle --compiler-runtime --release AmigaED.exe (this will create some additional folders and copy most of the needed DLLs to the folder containing your executable). Note that this step becomes neccessary whenever you have changed a feature and recompiled the sources!
4. Manually copy appropriate Qt5PrintSupport.dll and qscintilla2_qt5.dll to that folder (...if not been done by windeployqt allready!)
5. Add an entry containing the path to that folder to your system's $PATH variable if you want to call the app from command line!

# Making the app run standalone on Linux or MacOS X
1. CD to the folder where your compiled executable resides
2. execute sudo cp -l -f AmigaED /usr/local/bin/amigaed

you might also add this folder to your search path instead!

# Roadmap - wanted features (at least hopefully planned...)
- make the editor drag'n'drop-aware
- implement inserts and snippets [partially done]
- add a search/replace dialog
- improve printing [partially done - prints text and line numbers on DIN A4 paper]
- implement direct cross compiling to Amiga target for vbcc and gcc cross compiler toolchain
- startup Amiga Emulator
- implement a prefs dialog
- implement project manager
- implement makefile generator
- implement compiler settings for vbcc, gcc, SAS/C
- locale support (at least for german language)
- implement custom lexers for Amiga C and libraries, AmigaE
- implement custom code beautifier
- implement various wizards for code generation
- implement GUI builders for BGUI, ReAction and probably MUI (YES - they have to be written for a PC/Mac environment! But that's another project...)
- implement project time management, including per-project wordcount, charcount, linecount, work hours statistics
- MAYBE implement code creation and cross compiling for AmigaOS 4.1 PPC target... Somebody willing to donate a A1222 machine? ;)
- MAYBE implement code creation and cross compiling for AROS native i386 target... if I can get hands on a working cross compiler running on PC

# Credits
- Matic Kucovec and Kristof Mulier for their great tutorial and website on how to use QScintilla in Python
- Keith F. Kelly for pointing me out on how to kill an existing QsciLexer

Thanks for your helping hands when I got stuck, folks. You rock!

# If you like my work...
Since there aren't too many working examples on how to write an advanced editor in C++ using QScintilla I thought it to be a good idea to put the sources into public for free.
Developing and enhancing this example code is kind of time intensive, though a whole lot of fun. If you like my work and want to support the developent of AmigaED with some beer, books or Espresso, feel free to donate to mb@mbergmann-sh.de via PayPal. Thanks for your support!

# Usefull links for Qt5/QScintilla coders
- Qt5 open source edition download site: https://www.qt.io/download-qt-installer?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5
- QScintilla download site: https://www.riverbankcomputing.com/software/qscintilla/download
- Qt5 documentation: http://doc.qt.io/qt-5/
- QScintilla API documentation: http://pyqt.sourceforge.net/Docs/QScintilla2/annotated.html
- A site showing the usage of QScintilla from within Python in order to build an editor: https://qscintilla.com/ (...you might want to use this one as a starting point for understanding how QScintilla works if you are able to read and understand Python code!)
