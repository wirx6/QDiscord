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

    if (QDiscordUtilities::debugMode)
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
    post(object, QDiscordUtilities::endPoints.login,
    [=](){
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
    });
}

void QDiscordRestComponent::login(const QString& token)
{
	_authentication = "Bot "+token;
    get(QDiscordUtilities::endPoints.me,
    [=](){
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
    });
}

void QDiscordRestComponent::sendMessage(const QString& content, QDiscordChannel* channel, bool tts)
{
    if(_authentication.isEmpty())
        return;

    if(!channel)
        return;

	QString id = channel->id();
	QJsonObject object;
	object["content"] = content;

	if(tts)
		object["tts"] = true;

    post(object, QUrl(QString(QDiscordUtilities::endPoints.channels + "/" + id + "/messages")),[=]()
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
			QDiscordMessage message(object, channel);
			emit messageSent(message);
		}
		reply->deleteLater();
	});
}

void QDiscordRestComponent::sendMessage(const QString& content, const QString& channelId, bool tts)
{
	if(_authentication.isEmpty())
		return;

	QJsonObject object;
	object["content"] = content;

	if(tts)
		object["tts"] = true;

    post(object, QUrl(QString(QDiscordUtilities::endPoints.channels + "/" + channelId + "/messages")), [=]()
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
	});
}

void QDiscordRestComponent::deleteMessage(QDiscordMessage message)
{
	deleteMessage(message.id(), message.channelId());
}

void QDiscordRestComponent::deleteMessage(const QString& messageId, const QString& channelId)
{
	if(_authentication.isEmpty())
		return;
	deleteResource(QUrl(QString(QDiscordUtilities::endPoints.channels + "/" + channelId + "/messages/" + messageId)),
	[=](){
		QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
		if(!reply)
			return;
		if(reply->error() != QNetworkReply::NoError)
			emit messageDeleteFailed(reply->error());
		else
			emit messageDeleted(messageId);
		reply->deleteLater();
	});
}

void QDiscordRestComponent::logout()
{
    if(_authentication.isEmpty())
        return;
    if(_self)
        delete _self;
    _self = nullptr;
    QJsonObject object;
    object["token"] = _authentication;
    _authentication = "";
    post(object, QDiscordUtilities::endPoints.logout,
    [=](){
        QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
        if(!reply)
            return;
        emit loggedOut();
        reply->deleteLater();
    });
}

void QDiscordRestComponent::getEndpoint()
{
    if(_authentication.isEmpty())
        return;
    get(QDiscordUtilities::endPoints.gateway,
    [=](){
        QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
        if(!reply)
            return;
        if(reply->error() != QNetworkReply::NoError)
            emit endpointAcquireFailed(reply->error());
        else
            emit endpointAcquired(QJsonDocument::fromJson(reply->readAll()).object().value("url").toString());
        reply->deleteLater();
    });
}

void QDiscordRestComponent::selfCreated(const QDiscordUser& self)
{
    if(_self)
        delete _self;
	_self = new QDiscordUser(self);
}

void QDiscordRestComponent::deleteResource(const QUrl& url, std::function<void()> function)
{
	QString userAgent = "DiscordBot (" + QDiscordUtilities::libLink +
						", v" + QDiscordUtilities::libMajor + ":" + QDiscordUtilities::libMinor + ")" +
						"; " + QDiscordUtilities::botName;
	QNetworkRequest request(url);
	if(_authentication != "")
		request.setRawHeader("Authorization", _authentication.toUtf8());
	request.setRawHeader("User-Agent", userAgent.toUtf8());
	connect(_manager.deleteResource(request), &QNetworkReply::finished, this, function);
	qDebug()<<this<<"DELETE to "<<url;
}

void QDiscordRestComponent::post(const QJsonObject& object, const QUrl& url, std::function<void()> function)
{
    QString userAgent = "DiscordBot (" + QDiscordUtilities::libLink +
                        ", v" + QDiscordUtilities::libMajor + ":" + QDiscordUtilities::libMinor + ")" +
                        "; " + QDiscordUtilities::botName;
    QJsonDocument document;
    document.setObject(object);
    QNetworkRequest request(url);
    if(_authentication != "")
        request.setRawHeader("Authorization", _authentication.toUtf8());
    request.setRawHeader("User-Agent", userAgent.toUtf8());
    request.setRawHeader("content-type", "application/json");
    QNetworkReply* reply = _manager.post(request, document.toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, function);

    if (QDiscordUtilities::debugMode)
        qDebug()<<this<<"posted: "<<object<<" to "<<url;
}

void QDiscordRestComponent::post(const QJsonArray& array, const QUrl& url, std::function<void()> function)
{
    QString userAgent = "DiscordBot (" + QDiscordUtilities::libLink +
                        ", v" + QDiscordUtilities::libMajor + ":" + QDiscordUtilities::libMinor + ")" +
                        "; " + QDiscordUtilities::botName;
    QJsonDocument document;
    document.setArray(array);
    QNetworkRequest request(url);
    if(_authentication != "")
        request.setRawHeader("Authorization", _authentication.toUtf8());
    request.setRawHeader("User-Agent", userAgent.toUtf8());
    request.setRawHeader("content-type", "application/json");
    QNetworkReply* reply = _manager.post(request, document.toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, function);

    if (QDiscordUtilities::debugMode)
        qDebug()<<this<<"posted: "<<array<<" to "<<url;
}

void QDiscordRestComponent::get(const QUrl& url, std::function<void()> function)
{
    QString userAgent = "DiscordBot (" + QDiscordUtilities::libLink +
                        ", v" + QDiscordUtilities::libMajor + ":" + QDiscordUtilities::libMinor + ")" +
                        "; " + QDiscordUtilities::botName;
    QNetworkRequest request(url);
    if(_authentication != "")
        request.setRawHeader("Authorization", _authentication.toUtf8());
    request.setRawHeader("User-Agent", userAgent.toUtf8());
    QNetworkReply* reply = _manager.get(request);
    connect(reply, &QNetworkReply::finished, this, function);

    if (QDiscordUtilities::debugMode)
        qDebug()<<this<<"GET to "<<url;
}
