/*
 * QDiscord - An unofficial C++ and Qt wrapper for the Discord API.
 * Copyright (C) 2016 george99g
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef QDISCORD_HPP
#define QDISCORD_HPP

/*!
 * \mainpage %QDiscord
 * \section qdiscord QDiscord
 * %QDiscord is a [Qt](https://www.qt.io/) wrapper for the
 * [Discord API](https://discordapp.com/developers/docs) written
 * in C++. It was written to easily allow the creation of Discord bots.
 * Currently, it is in an incomplete state and will be under constant development,
 * so expect things to not work in future versions.
 * \section discord Discord
 * [Discord](https://discordapp.com) is a proprietary Voice over IP application
 * designed for gaming communities and is developed by
 * [Hammer & Chisel](https://discordapp.com/company).
 * \section github GitHub repository
 * The GitHub repository for this project can be found
 * [here](https://github.com/george99g/QDiscord).
 * \section installing Installing
 * \subsection step1 Step 1: Compile
 * The quickest way to do this is to open the project file with QtCreator
 * and hit the build (not run!) button. Alternatively, you may build it via
 * the command line like this (assuming you already cloned the project
 * and are in the main directory):
 * \code
 * mkdir build
 * cd build
 * qmake -qt=qt5 ../QDiscord/QDiscord.pro
 * make mocables all
 * \endcode
 * \subsection step2 Step 2: Link
 * I'll start by showing an example of what I have to include in the
 * .pro file on my machine to get the link working.
 * \code
 * CONFIG += C++11
 * INCLUDEPATH += "/home/user/git/QDiscord/QDiscord"
 * LIBS += "/home/user/git/QDiscord/build-QDiscord-Desktop_Qt_5_5_1_GCC_64bit-Debug/libQDiscord.a"
 * \endcode
 * The `CONFIG` part tells the compiler to use the C++11 standard.
 * The `INCLUDEPATH` part specifies where the header files for the
 * library can be found. The `LIBS` part specifies where your compiled
 * library is.
 * \section example Example
 * Here's a short example application that will send a message to a specified server
 * once it connects.
 * \subsection main main.cpp
 * \code
 * #include <QCoreApplication>
 * #include "application.hpp"
 *
 * int main(int argc, char* argv[])
 * {
 *     QCoreApplication a(argc, argv);
 *     Application app;
 *     return a.exec();
 * }
 * \endcode
 * \subsection header application.hpp
 * \code
 * #ifndef APPLICATION_HPP
 * #define APPLICATION_HPP
 *
 * #include <QDiscord>
 *
 * class Application : public QObject
 * {
 *     Q_OBJECT
 * public:
 *     explicit Application(QObject *parent = 0);
 * private:
 *     void guildCreated(QDiscordGuild* guild);
 *     void logout();
 *     QString _token;
 *     QDiscord _discord;
 * };
 *
 * #endif
 * \endcode
 * \subsection source application.cpp
 * \code
 * #include "application.hpp"
 *
 * Application::Application(QObject *parent) : QObject(parent)
 * {
 *     connect(_discord.state(), &QDiscordStateComponent::guildCreated, this, &Application::guildCreated);
 *     connect(_discord.rest(), &QDiscordRestComponent::messageSent, this, &Application::logout);
 *     _discord.login("<TOKEN_HERE>");
 * }
 *
 * void Application::guildCreated(QDiscordGuild* guild)
 * {
 *     if(guild->name() == "Test Server")
 *         _discord.rest()->sendMessage("Test", guild->channels().values()[0]);
 * }
 *
 * void Application::logout()
 * {
 *     _discord.logout();
 * }
 * \endcode
 * \section relatedProjects Related projects
 * A list of Discord API libraries can be found
 * [here](https://discordapi.com/unofficial/libs.html).
 */

#include <QObject>
#include <QDebug>
#include "qdiscordrestcomponent.hpp"
#include "qdiscordwscomponent.hpp"
#include "qdiscordstatecomponent.hpp"

/*!
 * \brief This class represents a single connection to the Discord API.
 *
 * It has helper methods for interfacing with the contained components.\n
 * It also handles communication between the components, allowing each
 * component to not be coupled to any other component.
 */
class QDiscord : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Standard QObject constructor.
     * \param parent
     */
    explicit QDiscord(QObject* parent = 0);
    /*!
     * \brief Automatically calls all component functions required to handle a standard Discord login.
     *
     * This might cause issues if you are manually interfacing with the components, in which case
     * it is recommended to not call this and instead log in manually.
     * \param email The Discord account to be used's e-mail.
     * \param password The password to the supplied e-mail.
     * \deprecated This method is deprecated for bot usage.\n
     * See https://discordapp.com/developers/docs/topics/oauth2#bot-vs-user-accounts
     */
    void login(const QString& email, const QString& password);
    /*!
     * \brief Automatically calls all component functions required to handle a standard Discord login.
     *
     * This might cause issues if you are manually interfacing with the components, in which case
     * it is recommended to not call this and instead log in manually.
     * \param token The token to use.
     */
    void login(const QString& token);
    /*!
     * \brief Handles logging out from the Discord API and destroying any state information in memory.
     */
    void logout();
    /*!
     * \brief Returns a pointer to the REST component.
     */
    QDiscordRestComponent* rest() {return &_rest;}
    /*!
     * \brief Returns a pointer to the WebSocket component.
     */
    QDiscordWsComponent* ws() {return &_ws;}
    /*!
     * \brief Returns a pointer to the state component.
     */
    QDiscordStateComponent* state() {return &_state;}
signals:
    /*!
     * \brief Emitted when logging in has failed.
     *
     * Connect to component signals in order to get more information about the error that caused this.
     */
    void loginFailed();
    /*!
     * \brief Emitted when logging in is completed and successful.
     */
    void loginSuccess();
    /*!
     * \brief Emitted when logging out has finished.
     */
    void loggedOut();
    /*!
     * \brief Emitted when the WebSocket has disconnected.
     *
     * It should automatically reconnect, so error-handling code is not required here.
     */
    void disconnected();
private:
    void tokenVerfified(const QString& token);
    void endpointAcquired(const QString& endpoint);
    void connectComponents();
    void connectDiscordSignals();
    void disconnectDiscordSignals();
    void logoutFinished();
    QString _token;
    QDiscordRestComponent _rest;
    QDiscordWsComponent _ws;
    QDiscordStateComponent _state;
    bool _signalsConnected;
};

#endif // QDISCORD_HPP
