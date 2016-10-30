#-------------------------------------------------
#
# Project created by QtCreator 2016-05-01T17:55:58
#
#-------------------------------------------------

TEMPLATE = subdirs

SUBDIRS += src
CONFIG(debug, debug|release) {
    SUBDIRS += tests
}

include(doc/doc.pri)

OTHER_FILES += Doxyfile
