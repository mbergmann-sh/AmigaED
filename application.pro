QT       += core gui widgets printsupport
CONFIG += console qscintilla2 c++11

TARGET = AmigaED
TEMPLATE = app

macx {
    QMAKE_POST_LINK = install_name_tool -change libqscintilla2_qt$${QT_MAJOR_VERSION}.13.dylib $$[QT_INSTALL_LIBS]/libqscintilla2_qt$${QT_MAJOR_VERSION}.13.dylib $(TARGET)
}

HEADERS      = mainwindow.h \
    prefsdialog.h \
    aboutdialog.h
SOURCES      = main.cpp mainwindow.cpp \
    prefsdialog.cpp \
    aboutdialog.cpp
RESOURCES    += application.qrc


#RC_FILE = application.qrc

DISTFILES += \
    README.md \
    aslrequest.txt \
    install_linux.sh \
    syntax-examples/README.txt \
    syntax-examples/pascal_boublesort.pas \
    syntax-examples/fortran-example.f \
    syntax-examples/amiga_installer \
    syntax-examples/README.txt \
    syntax-examples/fortran-example.f \
    syntax-examples/Makefile_sample.mak \
    syntax-examples/pascal_boublesort.pas

FORMS += \
    prefsdialog.ui \
    aboutdialog.ui
