# QDiscord

QDiscord is a [Qt](https://www.qt.io/) wrapper for the [Discord API](https://discordapp.com/developers/docs) written in C++.
It was written to easily allow the creation of Discord bots.
Currently it is in an incomplete state and will constantly be under development, so expect things to not work in future versions.

# Discord

[Discord](https://discordapp.com) is a proprietary Voice over IP application designed for gaming communities and is developed by [Hammer & Chisel](https://discordapp.com/company).

# Documentation

All documentation can be found [here](http://george99g.github.io/QDiscord).

# Installing

### Step 1: Compile

The quickest way to do this is to open the project file with QtCreator and hit the build (not run!) button.
Alternatively, you may build it via the command line like this (assuming you already cloned the project and are in the main directory):
```
mkdir build
cd build
qmake -qt=qt5 ../QDiscord/QDiscord.pro
make mocables all
```

### Step 2: Link

I'll start by showing an example of what I have to include in the .pro file on my machine to get the link working.
```
CONFIG += C++11
INCLUDEPATH += "/home/user/git/QDiscord/QDiscord"
LIBS += "/home/user/git/QDiscord/build-QDiscord-Desktop_Qt_5_5_1_GCC_64bit-Debug/libQDiscord.a"
```

The `CONFIG` part tells the compiler to use the C++11 standard.
The `INCLUDEPATH` part specifies where the header files for the library can be found.
The `LIBS` part specifies where your compiled library is.

# Example

Here's a short example application that will send a message to a specified server once it connects.
#### main.cpp
```cpp
#include <QCoreApplication>
#include "application.hpp"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    Application app;
    return a.exec();
}
```
#### application.hpp
```cpp
#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <QDiscord>

class Application : public QObject
{
    Q_OBJECT
public:
    explicit Application(QObject *parent = 0);
private:
    void guildCreated(QDiscordGuild* guild);
    void logout();
    QString _token;
    QDiscord _discord;
};

#endif
```
#### application.cpp
```cpp
#include "application.hpp"

Application::Application(QObject *parent) : QObject(parent)
{
    connect(_discord.state(), &QDiscordStateComponent::guildCreated, this, &Application::guildCreated);
    connect(_discord.rest(), &QDiscordRestComponent::messageSent, this, &Application::logout);
    _discord.login("<TOKEN_HERE>");
}

void Application::guildCreated(QDiscordGuild* guild)
{
    if(guild->name() == "Test Server")
        _discord.rest()->sendMessage("Test", guild->channels().values()[0]);
}

void Application::logout()
{
    _discord.logout();
}
```
# Related projects
A list of Discord API libraries in other languages can be found [here](https://discordapi.com/unofficial/libs.html).
