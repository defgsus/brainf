#-------------------------------------------------
#
# Project created by QtCreator 2015-03-21T23:05:57
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = brainf
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    gene.cpp \
    genepool.cpp \
    brainfgene.cpp

HEADERS  += mainwindow.h \
    brainf.h \
    gene.h \
    genepool.h \
    brainfgene.h

OTHER_FILES += \
    appstyle.css

RESOURCES += \
    resources.qrc
