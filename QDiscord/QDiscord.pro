#-------------------------------------------------
#
# Project created by QtCreator 2016-05-01T17:55:58
#
#-------------------------------------------------

QT       += network
QT       +=  websockets
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

HEADERS += qdiscord/qdiscord.hpp \
    QDiscord \
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
unix {
    target.path = /usr/lib
    INSTALLS += target
}
