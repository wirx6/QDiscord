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
	_channel = channel;
	_channelId = object["channel_id"].toString("");
	_author = object.contains("author") ? new QDiscordUser(object["author"].toObject()) : nullptr;
	_tts = object["tts"].toBool(false);
	_timestamp = QDateTime::fromString(object["timestamp"].toString(""), Qt::ISODate);;
	for(auto i : object["mentions"].toArray())
	{
		if(guild())
		{
			QDiscordMember* member = guild()->member(i.toObject()["id"].toString(""));
			if(member && member->user())
				_mentions.insert(member->user(), false);
			else
			{
				_mentions.insert(new QDiscordUser(i.toObject()), true);
			}
		}
		else
		{
			_mentions.insert(new QDiscordUser(i.toObject()), true);
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
	_author = other.author() ? new QDiscordUser(*other.author()) : nullptr;
	_channel = other.channel();
	_channelId = other.channelId();
	_tts = other.tts();
	_timestamp = other.timestamp();
	QMap<QDiscordUser*, bool> otherMentions = other.mentionsWithOwnership();
	for(auto& i : otherMentions.keys())
	{
		if(otherMentions.value(i))
		{
			QDiscordUser* newUser = new QDiscordUser(*i);
			_mentions.insert(newUser, true);
		}
		else
		{
			_mentions.insert(i, false);
		}
	}
}

QDiscordMessage::~QDiscordMessage()
{
	delete _author;
	for(auto& i : _mentions.keys())
		if(_mentions.value(i))
			delete i;
}

QDiscordGuild*QDiscordMessage::guild() const
{
	return _channel ? _channel->guild() : nullptr;
}
