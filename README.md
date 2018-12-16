# AmigaED
qAmigaED is an enhanced example for writing a coder's editor using Qt5 and QScintilla, containig a menu bar, toolbar, menus and some sophisticated editing actions. The source is based on QScintilla's original editor example. This project was started as an entry point for learning how to use QScintilla. Though somewhat of a working editor, the project is meant for studies and as a base for own enhancements. 

# LICENSE
Copyright (C) 2018/2019 Michael Bergmann. Placed in the public domain with some restrictions!

These files are part of my enhanced AmigaED editor example, using classes of the Qt and QScintilla toolkits.

You may use or enhance this piece of software anyway you want to - as long as you don't violate laws or copyright issues. I hereby explicitely prohibit the usage of my work to people who believe in racism, fascism and any kind of attitude against democratic lifestyle. It is self-explanatory that this prohibits the usage of my work to any member or fan of the german AfD party.

Further information about Qt licensing is available at: http://www.trolltech.com/products/qt/licensing.html or by contacting info@trolltech.com.

Further information about QScintilla licensing is available at: https://www.riverbankcomputing.com/software/qscintilla/license or by contacting sales@riverbankcomputing.com.

This program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
# FEATURES - a work in progress...
- load file from GUI or command line
- save file to storage medium
- recognize changes in text
- code folding
- goto line number X
- jump to matching bracket {...} | [...] | (...)
- C/C++ syntax highlighting
- clipboard support
- display current cursor position

more to come...

# Compiling the app...
...should work fine under Linux, Windows, MacOS X and probably under BSD, too (didn't test that!).
You'll need a computer with either MSVC, GCC or MinGW C/C++ compiler setup with Qt5 and QScintilla installed and working!

1. Create a folder to store the app
2. CD to this folder
3. git clone https://github.com/mbergmann-sh/AmigaED.git
4. CD to <your_folder>/AmigaED
5. qmake application.pro
6. qmake (creates Makefiles for Debug and Release versions)
7. MSVC: nmake, GCC or MinGW: make -j4
8. wait for compiler to finish

# Making the app run standalone on Windows
1. Open a Qt command prompt correspondending to the compiler in use 
2. CD to folder where the release version was stored
3. windeployqt --angle --compiler-runtime --release qAmigaED.exe (this will create some additional folders and copy most of the needed DLLs to the folder containing your executable)
4. Manually copy appropriate Qt5PrintSupport.dll and qscintilla2_qt5.dll to that folder
5. Add a PATH entry to that folder to your system if you want to call the app from command line!

