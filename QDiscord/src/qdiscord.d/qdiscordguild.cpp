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

#include "qdiscordchannel.hpp"
#include "qdiscordguild.hpp"

QDiscordGuild::QDiscordGuild(const QJsonObject& object)
{
	_id = object["id"].toString("");
	_unavailable = object["unavailable"].toBool(false);
	_name = object["name"].toString("");
	_verificationLevel = object["verification_level"].toInt(0);
	_afkTimeout = object["afk_timeout"].toInt(0);
	_memberCount = object["member_count"].toInt(1);
	_joinedAt = QDateTime::fromString(object["joined_at"].toString(""),
			Qt::ISODate);
	for(QJsonValue item : object["members"].toArray())
	{
		QDiscordMember* member = new QDiscordMember(item.toObject(), this);
		if(_members.keys().contains(member->user()->id()))
		{
			delete _members.value(member->user()->id());
			_members.insert(member->user()->id(), member);
		}
		else
			_members.insert(member->user()->id(), member);
	}
	for(QJsonValue item : object["channels"].toArray())
	{
		QDiscordChannel* channel = new QDiscordChannel(item.toObject(), this);
		if(_channels.keys().contains(channel->id()))
		{
			delete _channels.value(channel->id());
			_channels.insert(channel->id(), channel);
		}
		else
			_channels.insert(channel->id(), channel);
	}

	if(QDiscordUtilities::debugMode)
		qDebug()<<"QDiscordGuild("<<this<<") constructed";
}

QDiscordGuild::QDiscordGuild(const QDiscordGuild& other)
{
	_id = other.id();
	_unavailable = other.unavailable();
	_name = other.name();
	_verificationLevel = other.verificationLevel();
	_afkTimeout = other.afkTimeout();
	_memberCount = other.memberCount();
	_joinedAt = other.joinedAt();
	for(QDiscordChannel* item : other.channels())
	{
		QDiscordChannel* newChannel = new QDiscordChannel(*item);
		newChannel->setGuild(this);
		_channels.insert(other.channels().key(item), newChannel);
	}
}

QDiscordGuild::QDiscordGuild()
{
	_id = "";
	_unavailable = false;
	_name = "";
	_verificationLevel = 0;
	_afkTimeout = 0;
	_memberCount = 0;
	_joinedAt = QDateTime();

	if(QDiscordUtilities::debugMode)
		qDebug()<<"QDiscordGuild("<<this<<") constructed";
}

QDiscordGuild::~QDiscordGuild()
{
	qDeleteAll(_channels);
	qDeleteAll(_members);
}

void QDiscordGuild::addChannel(QDiscordChannel* channel)
{
	if(!channel)
		return;
	if(_channels.keys().contains(channel->id()))
		delete _channels.value(channel->id());
	 _channels.insert(channel->id(), channel);
}

bool QDiscordGuild::removeChannel(QDiscordChannel* channel)
{
	if(!channel)
		return false;
	if(!_channels.keys().contains(channel->id()))
		return false;
	_channels.remove(channel->id());
	delete channel;
	return true;
}

void QDiscordGuild::addMember(QDiscordMember* member)
{
	if(!member)
		return;
	if(_members.keys().contains(member->user()->id()))
		delete _members.value(member->user()->id());
	_members.insert(member->user()->id(), member);
}

bool QDiscordGuild::removeMember(QDiscordMember* member)
{
	if(!member)
		return false;
	if(!_members.keys().contains(member->user()->id()))
		return false;
	_members.remove(member->user()->id());
	delete member;
	return true;
}
