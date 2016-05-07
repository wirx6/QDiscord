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

#ifndef QDISCORDWSCOMPONENT_HPP
#define QDISCORDWSCOMPONENT_HPP

#include <QObject>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
//#include <QFile>
#include "qdiscordutilities.hpp"

/*!
 * \brief The WebSocket component of QDiscord.
 *
 * This class handles all WebSocket-related operations to the Discord API.
 */
class QDiscordWsComponent : public QObject
{
    Q_OBJECT
public:
    /*!
     * \brief Standard QObject constructor.
     */
    explicit QDiscordWsComponent(QObject* parent = 0);
    /*!
     * \brief Makes the WebSocket connect to the specified endpoint.
     * \param endpoint The URL to the endpoint the WebSocket should connect to.
     * \param token The token the WebSocket should use to authenticate itself once it connects.
     */
    void connectToEndpoint(const QString& endpoint, const QString& token);
    /*!
     * \brief Makes the WebSocket disconnect from the endpoint.
     */
    void close();
signals:
    /*!
     * \brief Emitted when the WebSocket has successfully logged in.
     */
    void loginSuccess();
    /*!
     * \brief Emitted when the WebSocket fails to log in with the provided token.
     *
     * This should never happen unless the WebSocket component is manually called by external code.
     */
    void loginFailed();
    /*!
     * \brief Emitted when the WebSocket successfully connects to the endpoint.
     *
     * Useful when an application needs to track the WebSocket's current state.
     */
    void connected();
    /*!
     * \brief Emitted when the WebSocket has been disconnected from the endpoint.
     */
    void disconnected();
    /*!
     * \brief Emitted when all reconnect attempts have failed and the WebSocket will stop retrying.
     */
    void reconnectImpossible();
    /*!
     * \brief Emitted when the WebSocket encounters an error.
     * \param error A `QAbstractSocket::SocketError` enum providing more information about the encountered error.
     */
    void error(QAbstractSocket::SocketError error);
    void readyReceived(const QJsonObject& object);
    void guildCreateReceived(const QJsonObject& object);
    void guildDeleteReceived(const QJsonObject& object);
    void guildBanAddReceived(const QJsonObject& object);
    void guildBanRemoveReceived(const QJsonObject& object);
    void guildIntegrationsUpdateRecevied(const QJsonObject& object);
    void guildMemberAddReceived(const QJsonObject& object);
    void guildMemberRemoveReceived(const QJsonObject& object);
    void guildMemberUpdateReceived(const QJsonObject& object);
    void guildRoleCreateReceived(const QJsonObject& object);
    void guildRoleDeleteReceived(const QJsonObject& object);
    void guildRoleUpdateReceived(const QJsonObject& object);
    void guildUpdateReceived(const QJsonObject& object);
    void messageCreateReceived(const QJsonObject& object);
    void messageDeleteReceived(const QJsonObject& object);
    void messageUpdateReceived(const QJsonObject& object);
    void presenceUpdateReceived(const QJsonObject& object);
    void typingStartReceived(const QJsonObject& object);
    void userSettingsUpdateReceived(const QJsonObject& object);
    void voiceStateUpdateReceived(const QJsonObject& object);
    void channelCreateReceived(const QJsonObject& object);
    void channelDeleteReceived(const QJsonObject& object);
    void channelUpdateReceived(const QJsonObject& object);
private:
    void login(const QString& token);
    void reconnect();
    void connected_();
    void disconnected_();
    void error_(QAbstractSocket::SocketError error);
    void textMessageReceived(const QString& message);
    void heartbeat();
    bool _tryReconnecting;
    static const int _reconnectTime = 20*1000;
    int _reconnectAttempts;
    static const int _maxReconnectAttempts = 5;
    QTimer _heartbeatTimer;
    QString _gateway;
    QString _token;
    QWebSocket _socket;
};

#endif // QDISCORDWSCOMPONENT_HPP
