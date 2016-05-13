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

#ifndef QDISCORDRESTCOMPONENT_HPP
#define QDISCORDRESTCOMPONENT_HPP

#include <QObject>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include "qdiscordmessage.hpp"
#include "qdiscordutilities.hpp"
#include "qdiscordchannel.hpp"
#include "qdiscorduser.hpp"

class QDiscord;

/*!
 * \brief The REST component of QDiscord.
 *
 * This class handles all REST operations to the Discord API.
 */
class QDiscordRestComponent : public QObject
{
    friend class QDiscord;
    Q_OBJECT
public:
    /*!
     * \brief Standard QObject constructor.
     */
    explicit QDiscordRestComponent(QObject* parent = 0);
    ~QDiscordRestComponent();
    /*!
     * \brief Acquires a token for use in all other methods using a standard Discord account.
     * \param email An e-mail for the Discord account to log in with.
     * \param password The password to the provided Discord account.
     * \deprecated This method has been deprecated for bot usage by Discord.\n
     * See https://discordapp.com/developers/docs/topics/oauth2#bot-vs-user-accounts
     */
    void login(const QString& email, const QString& password);
    /*!
     * \brief Sets the token for use in all other methods.
     * \param token The token to use.
     */
    void login(const QString& token);
    /*!
     * \brief Sends a message to the specified channel.
     * \param content The message's contents.
     * \param channel The channel to send the message in. This may be a private or a guild channel.
     * \param tts Whether to use text to speech when sending the message.
     */
    void sendMessage(const QString& content, QDiscordChannel* channel, bool tts = false);
    /*!
     * \brief Logs out using the stored token.
     */
    void logout();
    /*!
     * \brief Sends a request to receive an endpoint for connecting using a WebSocket.
     */
    void getEndpoint();
signals:
    /*!
     * \brief Emitted when a WebSocket endpoint has successfully been acquired.
     * \param endpoint The URL to the acquired endpoint.
     */
    void endpointAcquired(const QString& endpoint);
    /*!
     * \brief Emitted when acquiring a WebSocket endpoint has failed.
     * \param error A QNetworkReply::NetworkError enum containing more
     * information about the reason why this request failed.
     */
    void endpointAcquireFailed(QNetworkReply::NetworkError error);
    /*!
     * \brief Emitted when a logout has successfully completed.
     */
    void loggedOut();
    /*!
     * \brief Emitted when the token acquired when logging in has been verified to be correct.
     */
    void tokenVerified(const QString& token);
    /*!
     * \brief Emitted when logging in has failed. The most common cause for this is invalid authentication.
     * \param error A QNetworkReply::NetworkError enum containing more
     * information about the reason why this request failed.
     */
    void loginFailed(QNetworkReply::NetworkError error);
    /*!
     * \brief Emitted when a message has been successfully sent.
     */
    void messageSent(const QDiscordMessage& message);
    /*!
     * \brief Emitted when sending a message has failed.
     * \param error A QNetworkReply::NetworkError enum containing more
     * information about the reason why this request failed.
     */
    void messageSendFailed(QNetworkReply::NetworkError error);
private:
    struct MessageParams
    {
        int nonce;
        QString content;
        QDiscordChannel* channel;
        bool tts;
    };
    void selfCreated(const QDiscordUser& self);
    void getEndpointFinished();
    void loginFinished();
    void tokenLoginFinished();
    void logoutFinished();
    void sendMessageFinished();
    void post(const QJsonObject& object, const QUrl& url, void (QDiscordRestComponent::*function)(void));
    void post(const QJsonArray& array, const QUrl& url, void (QDiscordRestComponent::*function)(void));
    void get(const QUrl& url, void (QDiscordRestComponent::*function)(void));
    QDiscordUser* _self;
    QString _authentication;
    QNetworkAccessManager _manager;
    QList<MessageParams> _messageSendQueue;
};

#endif // QDISCORDRESTCOMPONENT_HPP
