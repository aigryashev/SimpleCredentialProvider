win32 {

TEMPLATE = app
TARGET = SimpleCredentialProviderTest
CONFIG += console

INCLUDEPATH += \
    . 

SOURCES += \
    main.cpp

Release:DESTDIR = ../../../release
Release:OBJECTS_DIR = ../../../release/.obj
Release:MOC_DIR = ../../../release/.moc
Release:RCC_DIR = ../../../release/.rcc
Release:UI_DIR = ../../../release/.ui

Debug:DESTDIR = ../../../debug
Debug:OBJECTS_DIR = ../../../debug/.obj
Debug:MOC_DIR = ../../../debug/.moc
Debug:RCC_DIR = ../../../debug/.rcc
Debug:UI_DIR = ../../../debug/.ui

Release:DLLDESTDIR = ../../../release
Debug:DLLDESTDIR = ../../../debug
QMAKE_POST_LINK = $$(QTDIR)/bin/windeployqt.exe --compiler-runtime $${DLLDESTDIR}

} else {
error(This project should be used on windows only)
}
