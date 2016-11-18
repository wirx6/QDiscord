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

#include "qdiscordstatecomponent.hpp"

QDiscordStateComponent::QDiscordStateComponent(QObject* parent)
	: QObject(parent)
{
	_self = nullptr;

	if(QDiscordUtilities::debugMode)
		qDebug()<<this<<"constructed";
}

QDiscordStateComponent::~QDiscordStateComponent()
{
	clear();
}

QDiscordChannel* QDiscordStateComponent::channel(const QString& id)
{
	if(_privateChannels.contains(id))
		return _privateChannels.value(id);
	for(QDiscordGuild* item : _guilds)
	{
		if(item->channels().keys().contains(id))
			return item->channels().value(id);
	}
	return nullptr;
}

void QDiscordStateComponent::clear()
{
	delete _self;
	_self = nullptr;
	qDeleteAll(_guilds.values());
	_guilds.clear();
	qDeleteAll(_privateChannels.values());
	_privateChannels.clear();
}

void QDiscordStateComponent::readyReceived(const QJsonObject& object)
{
	delete _self;
	_self = new QDiscordUser(object["user"].toObject());
	emit selfCreated(*_self);
	for(QJsonValue item : object["guilds"].toArray())
		guildCreateReceived(item.toObject());
	for(QJsonValue item : object["private_channels"].toArray())
		channelCreateReceived(item.toObject());
}

void QDiscordStateComponent::guildCreateReceived(const QJsonObject& object)
{
	QDiscordGuild* guild = new QDiscordGuild(object);
	if(_guilds.keys().contains(guild->id()))
	{
		delete _guilds.value(guild->id());
		_guilds.insert(guild->id(), guild);
	}
	else
		_guilds.insert(guild->id(), guild);
	emit guildCreated(guild);
	if(!guild->unavailable())
		emit guildAvailable(guild);
}

void QDiscordStateComponent::guildDeleteReceived(const QJsonObject& object)
{
	QDiscordGuild guild(object);
	if(_guilds.keys().contains(guild.id()))
		delete _guilds.value(guild.id());
	_guilds.remove(guild.id());
	emit guildDeleted(guild);
}

void QDiscordStateComponent::guildBanAddReceived(const QJsonObject& object)
{
	//TODO Implement
	Q_UNUSED(object);
}

void QDiscordStateComponent::guildBanRemoveReceived(const QJsonObject& object)
{
	//TODO Implement
	Q_UNUSED(object);
}

void QDiscordStateComponent::guildIntegrationsUpdateRecevied(const QJsonObject& object)
{
	//TODO Implement
	Q_UNUSED(object);
}

void QDiscordStateComponent::guildMemberAddReceived(const QJsonObject& object)
{
	QDiscordGuild* guildPtr = guild(object["guild_id"].toString(""));
	QDiscordMember* member = new QDiscordMember(object, guildPtr);
	if(guildPtr)
		guildPtr->addMember(member);
	emit guildMemberAdded(member);

}

void QDiscordStateComponent::guildMemberRemoveReceived(const QJsonObject& object)
{
	QDiscordGuild* guildPtr = guild(object["guild_id"].toString(""));
	QDiscordMember* member;
	if(guildPtr)
	{
		QDiscordMember* tmpMember =
				guildPtr->member(object["user"].toObject()["id"].toString(""));
		if(tmpMember)
		{
			member = new QDiscordMember(*tmpMember);
			guildPtr->removeMember(tmpMember);
		}
		else
			member = new QDiscordMember(object, nullptr);
	}
	else
		member = new QDiscordMember(object, nullptr);
	QDiscordMember newMember(*member);
	emit guildMemberRemoved(newMember);
	delete member;
}

void QDiscordStateComponent::guildMemberUpdateReceived(const QJsonObject& object)
{
	QDiscordGuild* guildPtr = guild(object["guild_id"].toString(""));
	if(guildPtr)
	{
		QDiscordMember* memberPtr =
				guildPtr->member(object["user"].toObject()["id"].toString(""));
		if(memberPtr)
		{
			memberPtr->update(object, guildPtr);
			emit guildMemberUpdated(memberPtr);
		}
		else
			if(QDiscordUtilities::debugMode)
			{
				qDebug()<<this<<
				"DESYNC: Member update received but member is not stored in guild.\n"
				"Member ID: "+object["user"].toObject()["id"].toString("")+"\n"
				"Guild ID: "+guildPtr->id();
			}
	}
	else
		if(QDiscordUtilities::debugMode)
			qDebug()<<this<<
			"DESYNC: Member update received but guild is not stored in state.\n"
			"Guild ID: "+object["guild_id"].toString("");
}

void QDiscordStateComponent::guildRoleCreateReceived(const QJsonObject& object)
{
	//TODO Implement
	Q_UNUSED(object);
}

void QDiscordStateComponent::guildRoleDeleteReceived(const QJsonObject& object)
{
	//TODO Implement
	Q_UNUSED(object);
}

void QDiscordStateComponent::guildRoleUpdateReceived(const QJsonObject& object)
{
	//TODO Implement
	Q_UNUSED(object);
}

void QDiscordStateComponent::guildUpdateReceived(const QJsonObject& object)
{
	//TODO Implement
	Q_UNUSED(object);
}

void QDiscordStateComponent::messageCreateReceived(const QJsonObject& object)
{
	QDiscordMessage message(object, channel(object["channel_id"].toString("")));
	emit messageCreated(message);
}

void QDiscordStateComponent::messageDeleteReceived(const QJsonObject& object)
{
	QDiscordMessage message(object, channel(object["channel_id"].toString("")));
	emit messageDeleted(message);
}

void QDiscordStateComponent::messageUpdateReceived(const QJsonObject& object)
{
	QDiscordMessage message(object, channel(object["channel_id"].toString("")));
	emit messageUpdated(message,
						QDateTime::fromString(
							object["edited_timestamp"].toString(),
							Qt::ISODate)
			);
}

void QDiscordStateComponent::presenceUpdateReceived(const QJsonObject& object)
{
	//TODO Implement
	Q_UNUSED(object);
}

void QDiscordStateComponent::typingStartReceived(const QJsonObject& object)
{
	//TODO Implement
	Q_UNUSED(object);
}

void QDiscordStateComponent::userSettingsUpdateReceived(const QJsonObject& object)
{
	//TODO Implement
	Q_UNUSED(object);
}

void QDiscordStateComponent::voiceStateUpdateReceived(const QJsonObject& object)
{
	//TODO Implement
	Q_UNUSED(object);
}

void QDiscordStateComponent::channelCreateReceived(const QJsonObject& object)
{
	QDiscordChannel* channel =
			new QDiscordChannel(object, guild(object["guild_id"].toString("")));
	if(channel->isPrivate())
	{
		if(_privateChannels.keys().contains(channel->id()))
		{
			delete _privateChannels.value(channel->id());
			_privateChannels.insert(channel->id(), channel);
		}
		else
			_privateChannels.insert(channel->id(), channel);
		emit privateChannelCreated(channel);
	}
	else
	{
		if(!channel->guild())
		{
			delete channel;
			return;
		}
		channel->guild()->addChannel(channel);
		emit channelCreated(channel);
	}
}

void QDiscordStateComponent::channelDeleteReceived(const QJsonObject& object)
{
	QDiscordChannel channel(object, guild(object["guild_id"].toString("")));
	if(channel.isPrivate())
	{
		if(_privateChannels.keys().contains(channel.id()))
			delete _privateChannels.value(channel.id());
		_privateChannels.remove(channel.id());
		emit privateChannelDeleted(channel);
	}
	else
	{
		if(!channel.guild())
			return;
		channel.guild()->removeChannel(channel.guild()->channel(channel.id()));
		emit channelDeleted(channel);
	}
}

void QDiscordStateComponent::channelUpdateReceived(const QJsonObject& object)
{
	QDiscordChannel* channel =
			new QDiscordChannel(object, guild(object["guild_id"].toString("")));
	if(channel->isPrivate())
	{
		if(_privateChannels.keys().contains(channel->id()))
		{
			delete _privateChannels.value(channel->id());
			_privateChannels.insert(channel->id(), channel);
		}
		else
			_privateChannels.insert(channel->id(), channel);
		emit privateChannelUpdated(channel);
	}
	else
	{
		if(!channel->guild())
		{
			delete channel;
			return;
		}
		channel->guild()->addChannel(channel);
		emit channelUpdated(channel);
	}
}
