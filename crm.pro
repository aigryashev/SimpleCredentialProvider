TEMPLATE = subdirs
CONFIG += ordered
win32 {
SUBDIRS = \
        ./src/win/SimpleCredentialProvider \
        ./src/win/SimpleCredentialGamerUI
}

