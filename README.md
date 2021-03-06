# AmigaED
AmigaED is an example for writing an advanced coder's editor using the Qt5 and QScintilla frameworks in C++, containing a menu bar, toolbars, menus, a custom context menue, an enhanced statusbar and some sophisticated editing actions. The source is based on Riverbank's original editor example, as provided with the QScintilla sources. This project was started as an entry point for learning how to use QScintilla. Though somewhat of a working editor, this project is meant for studies and as a base for own enhancements.

# ANNOUNCEMENT / Beta-Testers
Public beta testing phase starts on Tuesday, January 8th 2019.
Beta-testers please send bug reports, wishlists, suggestions, errata to amigaed@mbergmann-sh.de
Send beer cans, pizza vouchers and other usefull stuff that coders need directly to my postal adress. If you can't find it and want to support my work, you might consider to make a donation of your likes to my paypal account: mb@mbergmann-sh.de
Thanks and have fun with Amiga Cross Editor!

# Known bugs
- Compiler fails if path to source contains whitespace (detected under Windows first) - no intention to fix this ATM, please use folders without whitespace for your pathes!!! Needs complete rewrite of the ways QStringList commands is constructed.


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
- jump-to-error
- C/C++ | Pascal | Makefile  syntax highlighting
- display plain text without syntax highlighting and folding margin
- clipboard support
- display current cursor position
- print current file if page not empty (could be done better - e.g. take parameters from default printer...)
- custom QScintilla context menue to do insertions by right-clicking the mouse
- code insertion via menue, shortcuts and custom context menue [partially]
- app template creation [AmigaShell, ANSI C, C++ app ATM]
- select cross compiler to use: vbcc, gcc or g++
- Preferences dialog for most important stuff, like compiler settings, et cetera..
- startup Amiga Emulator, different models and Workbenches selectable

more to come...

# Prerequisites
- Qt5 development environment installed and working
- QScintilla installed and working
- GIT client installed (if you want to pull updates!)

Optional, but usefull:
- Amiga Cross Compiler Toolchain installed and working. I suggest Bebbo's repo at https://github.com/bebbo/amiga-gcc. Windows users might want to download a precompiled, installable version. You can find it here: https://franke.ms/download/setup-amiga-gcc.exe
- Some UAE flavour installed and set up to boot a virtual Amiga environment
- Folder that is mounted as a HD in your virtual Amiga to test your cross compilates. Select this as your project default folder in Amiga Cross Editor's preferences!

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
- make error console output clickable, jump to error line [DONE]
- write a _better_ error parser with text line markers!!
- make the editor drag'n'drop-aware
- implement inserts and snippets [mostly done]
- add a search/replace dialog   [partially done]
- improve printing [partially done - prints text and line numbers on DIN A4 paper]
- implement direct cross compiling to Amiga target for vbcc and gcc cross compiler toolchain [working, so far...]
- startup Amiga Emulator    [DONE]
- implement a prefs dialog  [DONE]
- implement project manager
- implement makefile generator
- implement compiler settings for vbcc, gcc, SAS/C [can allready be done via textinput by now]
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


...and last, but not least all those not personally mentioned people out on stackoverflow.com for beeing patiently helpfull.
Thanks for your helping hands when I got stuck, folks. You rock!

# If you like my work...
Since there aren't too many working examples on how to write an advanced editor in C++ using QScintilla I thought it to be a good idea to put the sources into public for free.
Developing and enhancing this example code is kind of time intensive, though a whole lot of fun. If you like my work and want to support the developent of AmigaED with some beer, books or Espresso, feel free to donate to mb@mbergmann-sh.de via PayPal. Thanks for your support!

# Screenshots
![aced_testing_iconcreation](https://user-images.githubusercontent.com/23148422/50824174-21ef3f80-1336-11e9-955d-eda2ca12bb67.png)
![aced_compile](https://user-images.githubusercontent.com/23148422/50733066-a9964c00-1186-11e9-8a7b-5eb29e18adaa.png)
![aced_prefs](https://user-images.githubusercontent.com/23148422/50733067-a9964c00-1186-11e9-8dc6-0ab818d6f368.png)
![aced_prefs_uae](https://user-images.githubusercontent.com/23148422/50733068-aa2ee280-1186-11e9-8df9-b134e33c1dce.png)
![acrossedit_linux](https://user-images.githubusercontent.com/23148422/50418994-bdf55300-082e-11e9-988d-c91130b71946.png)
![acrossedit_osx](https://user-images.githubusercontent.com/23148422/50419007-ca79ab80-082e-11e9-964e-945d7b04968e.png)
![acrossedit_w10_insertsmenu](https://user-images.githubusercontent.com/23148422/50419014-d6fe0400-082e-11e9-8386-0a12e1908cb3.png)
![acrossedit_w10_contextmenu](https://user-images.githubusercontent.com/23148422/50419018-db2a2180-082e-11e9-9c10-5f5ff82f0733.png)
![amiged_shell-tmplate](https://user-images.githubusercontent.com/23148422/50439937-2bb78280-08f4-11e9-9fc0-c0201770fabd.png)
![ace](https://user-images.githubusercontent.com/23148422/50571363-762f7800-0da8-11e9-8da6-1bea5fc5f000.png)
![screenshot_20190105_181721](https://user-images.githubusercontent.com/23148422/50727162-24278300-1117-11e9-9254-dfadd392de17.png)

# Usefull links for Qt5/QScintilla coders
- Qt5 open source edition download site: https://www.qt.io/download-qt-installer?hsCtaTracking=9f6a2170-a938-42df-a8e2-a9f0b1d6cdce%7C6cb0de4f-9bb5-4778-ab02-bfb62735f3e5
- QScintilla download site: https://www.riverbankcomputing.com/software/qscintilla/download
- Qt5 documentation: http://doc.qt.io/qt-5/
- QScintilla API documentation: http://pyqt.sourceforge.net/Docs/QScintilla2/annotated.html
- A site showing the usage of QScintilla from within Python in order to build an editor: https://qscintilla.com/ (...you might want to use this one as a starting point for understanding how QScintilla works if you are able to read and understand Python code!)
