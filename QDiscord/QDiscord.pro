#-------------------------------------------------
#
# Project created by QtCreator 2016-05-01T17:55:58
#
#-------------------------------------------------

QT       += network websockets
QT       -= gui

TARGET = QDiscord
TEMPLATE = lib
CONFIG += staticlib c++11

SOURCES += qdiscord/qdiscord.cpp \
    qdiscord/qdiscordrestcomponent.cpp \
    qdiscord/qdiscordutilities.cpp \
    qdiscord/qdiscordwscomponent.cpp \
    qdiscord/qdiscordstatecomponent.cpp \
    qdiscord/qdiscordguild.cpp \
    qdiscord/qdiscorduser.cpp \
    qdiscord/qdiscordchannel.cpp \
    qdiscord/qdiscordmember.cpp \
    qdiscord/qdiscordmessage.cpp \
    qdiscord/qdiscordgame.cpp
    
TOPHEADERS += QDiscord

DIRHEADERS += qdiscord/qdiscord.hpp \
    qdiscord/qdiscordrestcomponent.hpp \
    qdiscord/qdiscordutilities.hpp \
    qdiscord/qdiscordwscomponent.hpp \
    qdiscord/qdiscordstatecomponent.hpp \
    qdiscord/qdiscordguild.hpp \
    qdiscord/qdiscorduser.hpp \
    qdiscord/qdiscordchannel.hpp \
    qdiscord/qdiscordmember.hpp \
    qdiscord/qdiscordmessage.hpp \
    qdiscord/qdiscordgame.hpp
    
HEADERS += $$TOPHEADERS \
    $$DIRHEADERS

isEmpty(PREFIX) {
    PREFIX=/usr
} else {
    equals(PREFIX, "/") {
        PREFIX=""
    }
}

OTHER_FILES += Doxyfile
include(doc/doc.pri)

unix {
    target.path = $$PREFIX/lib
    topheaders.files = $$TOPHEADERS
    topheaders.path = $$PREFIX/include
    dirheaders.files = $$DIRHEADERS
    dirheaders.path = $$PREFIX/include/qdiscord
    INSTALLS += target topheaders dirheaders
}
