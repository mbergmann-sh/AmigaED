QT       += core gui widgets printsupport
CONFIG += release qscintilla2 c++11

TARGET = AmigaED
TEMPLATE = app

macx {
    QMAKE_POST_LINK = install_name_tool -change libqscintilla2_qt$${QT_MAJOR_VERSION}.13.dylib $$[QT_INSTALL_LIBS]/libqscintilla2_qt$${QT_MAJOR_VERSION}.13.dylib $(TARGET)
}

HEADERS      = mainwindow.h
SOURCES      = main.cpp mainwindow.cpp
RESOURCES    += application.qrc


#RC_FILE = application.qrc

DISTFILES += \
    README.md
