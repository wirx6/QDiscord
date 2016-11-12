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

#include "qdiscordmessage.hpp"

QDiscordMessage::QDiscordMessage(const QJsonObject& object, QDiscordChannel* channel)
{
	_id = object["id"].toString("");
	_mentionEveryone = object["mention_everyone"].toBool(false);
	_content = object["content"].toString("");
	_author = nullptr;
	_channel = channel;
	_channelId = object["channel_id"].toString("");
	if(object.contains("author"))
		_author = new QDiscordUser(object["author"].toObject());
	_tts = object["tts"].toBool(false);
	_timestamp = QDateTime::fromString(object["timestamp"].toString(""), Qt::ISODate);;
	QJsonArray mentionArray = object["mentions"].toArray();
	for(int i = 0; i < mentionArray.count(); i++)
	{
		if(guild())
		{
			QString id = mentionArray[i].toObject()["id"].toString("");
			QDiscordMember* member = guild()->member(id);
			if(member && member->user())
				_mentions.insert(member->user(), false);
			else
			{
				_mentions.insert(new QDiscordUser(mentionArray[i].toObject()),
								 true);
			}
		}
		else
		{
			_mentions.insert(new QDiscordUser(mentionArray[i].toObject()),
							 true);
		}
	}

	if(QDiscordUtilities::debugMode)
		qDebug()<<"QDiscordMessage("<<this<<") constructed";
}

QDiscordMessage::QDiscordMessage()
{
	_id = "";
	_mentionEveryone = false;
	_content = "";
	_author = nullptr;
	_channel = nullptr;
	_channelId = "";
	_tts = false;
	_timestamp = QDateTime();

	if(QDiscordUtilities::debugMode)
		qDebug()<<"QDiscordMessage("<<this<<") constructed";
}

QDiscordMessage::QDiscordMessage(const QDiscordMessage& other)
{
	_id = other.id();
	_mentionEveryone = other.mentionEveryone();
	_content = other.content();
	if(other.author())
		_author = new QDiscordUser(*other.author());
	else
		_author = nullptr;
	_channel = other.channel();
	_channelId = other.channelId();
	_tts = other.tts();
	_timestamp = other.timestamp();
	QMap<QDiscordUser*, bool> otherMentions = other.mentionsWithOwnership();
	for(int i = 0; i < otherMentions.keys().length(); i++)
	{
		if(otherMentions.values()[i])
		{
			QDiscordUser* newUser = new QDiscordUser(*otherMentions.keys()[i]);
			_mentions.insert(newUser, true);
		}
		else
		{
			_mentions.insert(otherMentions.keys()[i],
							 false);
		}
	}
}

QDiscordMessage::~QDiscordMessage()
{
	if(_author)
		delete _author;
	for(int i = 0; i < _mentions.keys().length(); i++)
		if(_mentions.values()[i])
			delete _mentions.keys()[i];
}

QDiscordGuild*QDiscordMessage::guild() const
{
	return _channel?_channel->guild():nullptr;
}
