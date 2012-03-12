QT       += core gui webkit network

TARGET = Collev
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    editor.cpp \
    webapp.cpp \
    file.cpp \
    filetypemanager.cpp \
    client.cpp

HEADERS  += mainwindow.h \
    editor.h \
    webapp.h \
    file.h \
    filetypemanager.h \
    client.h

RESOURCES += \
    resources/collev.qrc


