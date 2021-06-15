win32 {

Release: {
    SUFFIX_DIR = release
    SUFFIX_DIRLIB = Release
}
Debug: {
    SUFFIX_DIR = debug
    SUFFIX_DIRLIB = Debug
}

QT += widgets
TEMPLATE = app
CONFIG += create_prl link_prl
TARGET = SimpleCredentialGamerUI
DEFINES += _CRT_SECURE_NO_WARNINGS
INCLUDEPATH += ../../../lib/CRM_DLL/include \
    $$_PRO_FILE_PWD_/../SimpleCredentialProvider
DEPENDPATH += ../../../lib/CRM_DLL/include \
    ../$$_PRO_FILE_PWD_/SimpleCredentialProvider
FORMS += ui/GamerUIWidget.ui
HEADERS += ui/GamerUIWidget.h
SOURCES += main.cpp \
    ui/GamerUIWidget.cpp
RESOURCES += resources/ui_resources.qrc
LIBS += -L"$$_PRO_FILE_PWD_/../../../../build-crm-Desktop_x86_windows_msvc2019_pe_64bit-$$SUFFIX_DIRLIB/$$SUFFIX_DIR" \
    -L"$$_PRO_FILE_PWD_/../../../lib/CRM_DLL/release/x64" -lCRM_DLL -lUser32 SimpleCredentialProvider.lib

DESTDIR = ../../../$$SUFFIX_DIR
OBJECTS_DIR = ../../../$$SUFFIX_DIR/.obj
MOC_DIR = ../../../$$SUFFIX_DIR/.moc
RCC_DIR = ../../../$$SUFFIX_DIR/.rcc
UI_DIR = ../../../$$SUFFIX_DIR/.ui
DLLDESTDIR = ../../../$$SUFFIX_DIR
QMAKE_POST_LINK = $$(QTDIR)/bin/windeployqt.exe --compiler-runtime $${DLLDESTDIR}
} else {
error(This project should be used on windows only)
}


