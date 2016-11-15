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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.	 If not, see <http://www.gnu.org/licenses/>.
 */

#include "qdiscordwscomponent.hpp"

QDiscordWsComponent::QDiscordWsComponent(QObject* parent) : QObject(parent)
{
	connect(&_socket, &QWebSocket::connected,
			this, &QDiscordWsComponent::connected_);
	connect(&_socket, &QWebSocket::disconnected,
			this, &QDiscordWsComponent::disconnected_);
	connect(&_socket, static_cast<void (QWebSocket::*)(QAbstractSocket::SocketError)>(&QWebSocket::error),
			this, &QDiscordWsComponent::error_);
	connect(&_socket, &QWebSocket::textMessageReceived,
			this, &QDiscordWsComponent::textMessageReceived);
	connect(&_heartbeatTimer, &QTimer::timeout,
			this, &QDiscordWsComponent::heartbeat);
	_reconnectTimer.setSingleShot(true);
	connect(&_reconnectTimer, &QTimer::timeout,
			this, &QDiscordWsComponent::reconnect);
	_tryReconnecting = false;
	_useDumpfile = false;
	_reconnectAttempts = 0;
	_maxReconnectAttempts = -1;
	initDispatchTable();

	if(QDiscordUtilities::debugMode)
		qDebug()<<this<<"constructed";
}

void QDiscordWsComponent::connectToEndpoint(const QString& endpoint,
											const QString& token)
{
	if(_reconnectTimer.isActive())
		_reconnectTimer.stop();
	_gateway = endpoint;
	_token = token;
	_socket.open(endpoint);

	if(QDiscordUtilities::debugMode)
		qDebug()<<this<<"connecting to"<<endpoint;
}

void QDiscordWsComponent::close()
{
	if(_reconnectTimer.isActive())
		_reconnectTimer.stop();
	_tryReconnecting = false;
	_gateway = "";
	_token = "";
	_reconnectAttempts = 0;
	_socket.close();
}

void QDiscordWsComponent::setStatus(bool idle, QDiscordGame game)
{
	if(_token == "")
		return;
	if(_gateway == "")
		return;
	QJsonDocument document;
	QJsonObject object;
	object["op"] = 3;
	QJsonObject presenceObject;
	presenceObject["idle_since"] = idle ?
				QDateTime::currentMSecsSinceEpoch() : QJsonValue();
	if(game.name() != "")
	{
		QJsonObject gameObject;
		gameObject["name"] = game.name();
		gameObject["url"] = game.url()==""?QJsonValue():game.url();
		gameObject["type"] = game.type();
		presenceObject["game"] = gameObject;
	}
	else
		presenceObject["game"] = QJsonValue();
	object["d"] = presenceObject;
	document.setObject(object);
	_socket.sendTextMessage(document.toJson(QJsonDocument::Compact));
}

void QDiscordWsComponent::login(const QString& token)
{
	if(_reconnectTimer.isActive())
		_reconnectTimer.stop();
	QJsonDocument document;
	QJsonObject mainObject;
	mainObject["op"] = 2;
	QJsonObject dataObject;
	dataObject["token"] = token;
	dataObject["v"] = 5;
	dataObject["properties"] =
			QJsonObject({
							{"$os", QSysInfo::kernelType()},
							{"$browser", QDiscordUtilities::libName},
							{"$device", QDiscordUtilities::libName},
							{"$referrer", "https://discordapp.com/@me"},
							{"$referring_domain", "discordapp.com"}
						});
	dataObject["large_threshold"] = 100;
	dataObject["compress"] = false;
	mainObject["d"] = dataObject;
	document.setObject(mainObject);
	_socket.sendTextMessage(document.toJson(QJsonDocument::Compact));
}

void QDiscordWsComponent::reconnect()
{
	if(_reconnectTimer.isActive())
		_reconnectTimer.stop();
	if(_token == "")
		return;
	if(_gateway == "")
		return;

	if (QDiscordUtilities::debugMode)
		qDebug()<<this<<"reconnecting";

	if(_reconnectAttempts > _maxReconnectAttempts &&
			_maxReconnectAttempts != -1)
	{
		if(QDiscordUtilities::debugMode)
			qDebug()<<"maximum reconnect attempts reached, stopping";

		_reconnectAttempts = 0;
		close();
		emit reconnectImpossible();
	}
	else
	{
		if(_maxReconnectAttempts != -1)
			_reconnectAttempts++;
		emit attemptingReconnect();
		connectToEndpoint(_gateway, _token);
	}
}

void QDiscordWsComponent::connected_()
{
	if(_reconnectTimer.isActive())
		_reconnectTimer.stop();
	emit connected();
	login(_token);

	if(QDiscordUtilities::debugMode)
		qDebug()<<this<<"connected, logging in";
}

void QDiscordWsComponent::disconnected_()
{
	emit disconnected(_socket.closeReason(), _socket.closeCode());

	if(QDiscordUtilities::debugMode)
	{
		qDebug()<<this<<"disconnected: \""<<
				  _socket.closeReason()<<"\":"<<_socket.closeCode();
	}

	_heartbeatTimer.stop();
	if(_tryReconnecting)
		_reconnectTimer.start(_reconnectTime);
	else
	{
		_token = "";
		_gateway = "";
	}
}

void QDiscordWsComponent::error_(QAbstractSocket::SocketError err)
{
	emit error(err);
	if(_tryReconnecting)
	{
		QTimer::singleShot(_reconnectTime,
						   this, &QDiscordWsComponent::reconnect);
	}
	else
	{
		_token = "";
		_gateway = "";
		emit loginFailed();

		if(QDiscordUtilities::debugMode)
			qDebug()<<this<<"login failed: "<<err;
	}
}

void QDiscordWsComponent::textMessageReceived(const QString& message)
{
	QJsonDocument document = QJsonDocument::fromJson(message.toUtf8());
	QJsonObject object = document.object();
	if(_useDumpfile)
	{
		QFile file("DUMPFILE.txt");
		file.open(QFile::WriteOnly|QFile::Append);
		file.write(document.toJson(QJsonDocument::Indented));
		file.flush();
		file.close();
	}

	if(QDiscordUtilities::debugMode)
		qDebug()<<this<<"op:"<<object["op"].toInt()<<" t:"<<object["t"].toString();

	switch(object["op"].toInt(-1))
	{
	case 0:
		if(_eventDispatchTable.keys().contains(object["t"].toString()))
			_eventDispatchTable[object["t"].toString()](object["d"].toObject());
		else if(QDiscordUtilities::debugMode)
			qDebug()<<this<<"encountered an event not in the dispatch table";
		break;
	case -1:
		if(QDiscordUtilities::debugMode)
			qDebug()<<this<<"error while parsing operation code";
		break;
	default:
		if(QDiscordUtilities::debugMode)
			qDebug()<<this<<"encountered an unhandled operation";
	}
}

void QDiscordWsComponent::heartbeat()
{
	QJsonDocument document;
	QJsonObject object;
	object["op"] = 1;
	object["d"] = QString::number(QDateTime::currentMSecsSinceEpoch());
	document.setObject(object);
	_socket.sendTextMessage(document.toJson(QJsonDocument::Compact));

	if(QDiscordUtilities::debugMode)
		qDebug()<<this<<"heartbeat sent";
}

void QDiscordWsComponent::initDispatchTable()
{
	_eventDispatchTable = {
		{"READY", [&](const QJsonObject& dataObject){
			 _heartbeatTimer.start(dataObject["heartbeat_interval"].toInt());

			 if (QDiscordUtilities::debugMode)
				 qDebug()<<this<<"beating every "<<
			 _heartbeatTimer.interval()/1000.<<" seconds";

			 _tryReconnecting = true;
			 _reconnectAttempts = 0;
			 emit loginSuccess();
			 emit readyReceived(dataObject);
		 }},
		{"GUILD_CREATE", [this](const QJsonObject& dataObject){
			 emit guildCreateReceived(dataObject);
		 }},
		{"MESSAGE_CREATE", [this](const QJsonObject& dataObject){
			 emit messageCreateReceived(dataObject);
		 }},
		{"MESSAGE_DELETE", [this](const QJsonObject& dataObject){
			 emit messageDeleteReceived(dataObject);
		 }},
		{"MESSAGE_UPDATE", [this](const QJsonObject& dataObject){
			 emit messageUpdateReceived(dataObject);
		 }},
		{"TYPING_START", [this](const QJsonObject& dataObject){
			 emit typingStartReceived(dataObject);
		 }},
		{"VOICE_STATE_UPDATE", [this](const QJsonObject& dataObject){
			 emit voiceStateUpdateReceived(dataObject);
		 }},
		{"CHANNEL_CREATE", [this](const QJsonObject& dataObject){
			 emit channelCreateReceived(dataObject);
		 }},
		{"CHANNEL_DELETE", [this](const QJsonObject& dataObject){
			 emit channelDeleteReceived(dataObject);
		 }},
		{"CHANNEL_UPDATE", [this](const QJsonObject& dataObject){
			 emit channelUpdateReceived(dataObject);
		 }},
		{"GUILD_DELETE", [this](const QJsonObject& dataObject){
			 emit guildDeleteReceived(dataObject);
		 }},
		{"GUILD_BAN_ADD", [this](const QJsonObject& dataObject){
			 emit guildBanAddReceived(dataObject);
		 }},
		{"GUILD_BAN_REMOVE", [this](const QJsonObject& dataObject){
			 emit guildBanRemoveReceived(dataObject);
		 }},
		{"GUILD_INTEGRATIONS_UPDATE", [this](const QJsonObject& dataObject){
			 emit guildIntegrationsUpdateRecevied(dataObject);
		 }},
		{"GUILD_MEMBER_ADD", [this](const QJsonObject& dataObject){
			 emit guildMemberAddReceived(dataObject);
		 }},
		{"GUILD_MEMBER_REMOVE", [this](const QJsonObject& dataObject){
			 emit guildMemberRemoveReceived(dataObject);
		 }},
		{"GUILD_MEMBER_UPDATE", [this](const QJsonObject& dataObject){
			 emit guildMemberUpdateReceived(dataObject);
		 }},
		{"GUILD_ROLE_CREATE", [this](const QJsonObject& dataObject){
			 emit guildRoleCreateReceived(dataObject);
		 }},
		{"GUILD_ROLE_DELETE", [this](const QJsonObject& dataObject){
			 emit guildRoleDeleteReceived(dataObject);
		 }},
		{"GUILD_ROLE_UPDATE", [this](const QJsonObject& dataObject){
			 emit guildRoleUpdateReceived(dataObject);
		 }},
		{"GUILD_UPDATE", [this](const QJsonObject& dataObject){
			 emit guildUpdateReceived(dataObject);
		 }},
		{"PRESENCE_UPDATE", [this](const QJsonObject& dataObject){
			 emit presenceUpdateReceived(dataObject);
		 }},
		{"USER_SETTINGS_UPDATE", [this](const QJsonObject& dataObject){
			 emit userSettingsUpdateReceived(dataObject);
		 }}
	};
}
