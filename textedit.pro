QT += dbus widgets
requires(qtConfig(filedialog))
qtHaveModule(printsupport): QT += printsupport

CONFIG += c++17

TEMPLATE        = app
TARGET          = text-edit-qdbus

HEADERS         = textedit.h \
    dbusserver.h
SOURCES         = textedit.cpp \
                  dbusserver.cpp \
                  main.cpp

RESOURCES += textedit.qrc
build_all:!build_pass {
    CONFIG -= build_all
    CONFIG += release
}

VERSION = 1.0.0.0
DEFINES += VERSION=\\\"$$VERSION\\\"
DEFINES += TARGET=\\\"$$TARGET\\\"


EXAMPLE_FILES = textedit.qdoc

# install
INSTALLS += target

