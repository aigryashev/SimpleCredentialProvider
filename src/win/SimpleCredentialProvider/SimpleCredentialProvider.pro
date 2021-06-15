win32 {

QT += core
TEMPLATE = lib
CONFIG += dll create_prl link_prl
TARGET = SimpleCredentialProvider
DEFINES += _CRT_SECURE_NO_WARNINGS NOMINMAX

DEF_FILE = SimpleCredentialProvider.def

INCLUDEPATH += \
    . \
    ../../../lib/CRM_DLL/include

HEADERS += \
    resources/resources.h \
    consts/CommonConsts.hpp \
    consts/GUID.hpp \
    utils/ReleaserForIUnknown.hpp \
    utils/utils.hpp \
    logging.hpp \
    dllmain.hpp \
    SimpleCredentialProviderFields.hpp \
    SimpleCredentialProviderCredentialBase.hpp \
    SimpleCredentialProviderCredentialGamer.hpp \
    SimpleCredentialProviderCredentialSystemUser.hpp \
    SimpleCredentialProvider.hpp \
    SimpleCredentialProviderSystemUser.hpp \
    SimpleCredentialProviderBuiltInUsers.hpp \
    ProcessWatchdog.hpp

SOURCES += \
    consts/CommonConsts.cpp \
    consts/GUID.cpp \
    utils/utils.cpp \
    logging.cpp \
    dllmain.cpp \
    SimpleCredentialProviderCredentialBase.cpp \
    SimpleCredentialProviderCredentialGamer.cpp \
    SimpleCredentialProviderCredentialSystemUser.cpp \
    SimpleCredentialProvider.cpp \
    SimpleCredentialProviderSystemUser.cpp \
    ProcessWatchdog.cpp

LIBS += \
    Shlwapi.lib \
    Ole32.lib \
    Credui.lib \
    user32.lib \
    Advapi32.lib \
    Shell32.lib \
    Secur32.lib \
    Propsys.lib \
    -L"$$_PRO_FILE_PWD_/../../../lib/CRM_DLL/release/x64" -lCRM_DLL

RESOURCES += \
    resources/ui_resources.qrc

RC_FILE = resources/resources.rc

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

} else {
error(This project should be used on windows only)
}

