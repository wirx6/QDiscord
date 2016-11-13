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

	if(object["op"].toInt() == 0 && object["t"].toString() == "READY")
	{
		QJsonObject dataObject = object["d"].toObject();
		_heartbeatTimer.start(dataObject["heartbeat_interval"].toInt());

		if (QDiscordUtilities::debugMode)
			qDebug()<<this<<"beating every "<<_heartbeatTimer.interval()/1000.<<" seconds";

		_tryReconnecting = true;
		_reconnectAttempts = 0;
		emit loginSuccess();
		emit readyReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "GUILD_CREATE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit guildCreateReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "MESSAGE_CREATE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit messageCreateReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "MESSAGE_DELETE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit messageDeleteReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "MESSAGE_UPDATE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit messageUpdateReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "TYPING_START")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit typingStartReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "VOICE_STATE_UPDATE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit voiceStateUpdateReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "CHANNEL_CREATE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit channelCreateReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "CHANNEL_DELETE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit channelDeleteReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "CHANNEL_UPDATE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit channelUpdateReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "GUILD_DELETE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit guildDeleteReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "GUILD_BAN_ADD")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit guildBanAddReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "GUILD_BAN_REMOVE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit guildBanRemoveReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "GUILD_INTEGRATIONS_UPDATE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit guildIntegrationsUpdateRecevied(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "GUILD_MEMBER_ADD")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit guildMemberAddReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "GUILD_MEMBER_REMOVE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit guildMemberRemoveReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "GUILD_MEMBER_UPDATE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit guildMemberUpdateReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "GUILD_ROLE_CREATE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit guildRoleCreateReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "GUILD_ROLE_DELETE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit guildRoleDeleteReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "GUILD_ROLE_UPDATE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit guildRoleUpdateReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "GUILD_UPDATE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit guildUpdateReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "PRESENCE_UPDATE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit presenceUpdateReceived(dataObject);
	}
	else if(object["op"].toInt() == 0 && object["t"].toString() == "USER_SETTINGS_UPDATE")
	{
		QJsonObject dataObject = object["d"].toObject();
		emit userSettingsUpdateReceived(dataObject);
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
