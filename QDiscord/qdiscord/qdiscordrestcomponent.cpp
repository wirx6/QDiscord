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

#include "qdiscordrestcomponent.hpp"

QDiscordRestComponent::QDiscordRestComponent(QObject* parent) : QObject(parent)
{
    _authentication = "";
    _self = nullptr;
    qDebug()<<this<<"constructed";
}

QDiscordRestComponent::~QDiscordRestComponent()
{
    if(_self)
        delete _self;
}

void QDiscordRestComponent::login(const QString& email, const QString& password)
{
    QJsonObject object;
    object["email"] = email;
    object["password"] = password;
    post(object, QDiscordUtilities::endPoints.login, &QDiscordRestComponent::loginFinished);
}

void QDiscordRestComponent::login(const QString& token)
{
    _authentication = token;
    get(QDiscordUtilities::endPoints.me, &QDiscordRestComponent::tokenLoginFinished);
}

void QDiscordRestComponent::sendMessage(const QString& content, QDiscordChannel* channel, bool tts)
{
    if(_authentication == "")
        return;
    if(!channel)
        return;
    QString id = channel->id();
    QJsonObject object;
    object["content"] = content;
    int seed = QDateTime::currentDateTime().toTime_t();
    std::minstd_rand0 generator(seed);
    int nonce;
    while(true)
    {
        nonce = (int)generator();
        for(int i = 0; i < _messageSendQueue.length(); i++)
            if(_messageSendQueue[i].nonce == nonce)
                continue;
        break;
    }
    object["nonce"] = nonce;
    if(tts)
        object["tts"] = true;
    //MessageParams params = {nonce, content, channel, tts};
    //_messageSendQueue.append(params);
    post(object, QUrl(QString(QDiscordUtilities::endPoints.channels + "/" + id + "/messages")),
         &QDiscordRestComponent::sendMessageFinished);
}

void QDiscordRestComponent::logout()
{
    if(_authentication == "")
        return;
    if(_self)
        delete _self;
    _self = nullptr;
    QJsonObject object;
    object["token"] = _authentication;
    _authentication = "";
    post(object, QDiscordUtilities::endPoints.logout, &QDiscordRestComponent::logoutFinished);
}

void QDiscordRestComponent::getEndpoint()
{
    if(_authentication == "")
        return;
    get(QDiscordUtilities::endPoints.gateway, &QDiscordRestComponent::getEndpointFinished);
}

void QDiscordRestComponent::selfCreated(const QDiscordUser& self)
{
    if(_self)
        delete _self;
    _self = new QDiscordUser(self);
}

void QDiscordRestComponent::getEndpointFinished()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    if(!reply)
        return;
    if(reply->error() != QNetworkReply::NoError)
        emit endpointAcquireFailed(reply->error());
    else
        emit endpointAcquired(QJsonDocument::fromJson(reply->readAll()).object().value("url").toString());
    reply->deleteLater();
}

void QDiscordRestComponent::loginFinished()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    if(!reply)
        return;
    if(reply->error() != QNetworkReply::NoError)
        emit loginFailed(reply->error());
    else
    {
        _authentication = QJsonDocument::fromJson(reply->readAll()).object().value("token").toString();
        emit tokenVerified(_authentication);
    }
    reply->deleteLater();
}

void QDiscordRestComponent::tokenLoginFinished()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    if(!reply)
        return;
    if(reply->error() != QNetworkReply::NoError)
    {
        _authentication = "";
        emit loginFailed(reply->error());
    }
    else
        emit tokenVerified(_authentication);
    reply->deleteLater();
}

void QDiscordRestComponent::logoutFinished()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    if(!reply)
        return;
    emit loggedOut();
    reply->deleteLater();
}

void QDiscordRestComponent::sendMessageFinished()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    if(!reply)
        return;
    if(reply->error() != QNetworkReply::NoError)
        emit messageSendFailed(reply->error());
    else
    {
        QJsonDocument document = QJsonDocument::fromJson(reply->readAll());
        QJsonObject object = document.object();
        QDiscordMessage message(object, nullptr);
        emit messageSent(message);
    }
    reply->deleteLater();
}

void QDiscordRestComponent::post(const QJsonObject& object, const QUrl& url, void (QDiscordRestComponent::*function)())
{
    QJsonDocument document;
    document.setObject(object);
    QNetworkRequest request(url);
    if(_authentication != "")
        request.setRawHeader("Authorization", _authentication.toUtf8());
    request.setRawHeader("User-Agent", QDiscordUtilities::userAgent.toUtf8());
    request.setRawHeader("content-type", "application/json");
    QNetworkReply* reply = _manager.post(request, document.toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, function);
    qDebug()<<this<<"posted: "<<object<<" to "<<url;
}

void QDiscordRestComponent::post(const QJsonArray& array, const QUrl& url, void (QDiscordRestComponent::*function)())
{
    QJsonDocument document;
    document.setArray(array);
    QNetworkRequest request(url);
    if(_authentication != "")
        request.setRawHeader("Authorization", _authentication.toUtf8());
    request.setRawHeader("User-Agent", QDiscordUtilities::userAgent.toUtf8());
    request.setRawHeader("content-type", "application/json");
    QNetworkReply* reply = _manager.post(request, document.toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, function);
    qDebug()<<this<<"posted: "<<array<<" to "<<url;
}

void QDiscordRestComponent::get(const QUrl& url, void (QDiscordRestComponent::*function)())
{
    QNetworkRequest request(url);
    if(_authentication != "")
        request.setRawHeader("Authorization", _authentication.toUtf8());
    request.setRawHeader("User-Agent", QDiscordUtilities::userAgent.toUtf8());
    QNetworkReply* reply = _manager.get(request);
    connect(reply, &QNetworkReply::finished, this, function);
    qDebug()<<this<<"GET to "<<url;
}
