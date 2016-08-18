# QDiscord

[![Discord](https://discordapp.com/api/guilds/215902295303520256/widget.png)](https://discord.gg/MGdYDqC)<br>
QDiscord is a [Qt](https://www.qt.io/) wrapper for the [Discord API](https://discordapp.com/developers/docs) written in C++.
It was written to easily allow the creation of Discord bots.
Currently it is in an incomplete state and will constantly be under development, so expect things to either not work or not be available.

Feel free to submit an issue if you need specific functionality to be working before anything else.

# Discord

[Discord](https://discordapp.com) is a proprietary Voice over IP application designed for gaming communities and is developed by [Hammer & Chisel](https://discordapp.com/company).

# Documentation

All documentation can be found [here](http://george99g.github.io/QDiscord).

# Installing

### Step 1: Compile

The quickest way to do this is to open the project file with QtCreator and hit the build button.
Alternatively, you may build it via the command line like this (assuming you already cloned the project, are in the main directory and have the Qt packages installed on your system):
```
mkdir build
cd build
qmake -qt=qt5 ../QDiscord/QDiscord.pro
make mocables all
```

### Step 2: Link

This is what I have to do in my `.pro` file to get everything working on my machine.
```
INCLUDEPATH += "/home/user/git/QDiscord/QDiscord"
LIBS += "/home/user/git/QDiscord/build-QDiscord-Desktop_Qt_5_7_0_GCC_64bit-Debug/libQDiscord.a"
```

`INCLUDEPATH` specifies where the header files for the library can be found.
`LIBS` specifies where your compiled library is.

Alternatively, you may include this project as a Git submodule and use QtCreator's wizards to link it as a library to your own project.
This is the preferred method as Qt will manage building and linking the library, however it is out of the scope of this readme.
For more information, see [the official Qt documentation](http://doc.qt.io/qtcreator/creator-project-qmake-libraries.html).

# Related projects

A list of Discord API libraries in other languages can be found [here](https://discordapi.com/unofficial/libs.html).
