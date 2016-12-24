QT       += network websockets
QT       -= gui

TARGET = QDiscord
TEMPLATE = lib
CONFIG += c++11

DEFINES += QDISCORD_LIBRARY
CONFIG(staticlib) {
    DEFINES += QDISCORD_STATIC
}

isEmpty(PREFIX) {
    PREFIX=/usr
} else {
    equals(PREFIX, "/") {
        PREFIX=""
    }
}

include(depends.pri)

unix | mingw {
    target.path = $$PREFIX/lib
    topheaders.files = $$TOPHEADERS
    topheaders.path = $$PREFIX/include
    dirheaders.files = $$DIRHEADERS
    dirheaders.path = $$PREFIX/include/qdiscord.d
    INSTALLS += target topheaders dirheaders
}
