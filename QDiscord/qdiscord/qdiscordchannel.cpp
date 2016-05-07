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

#include "qdiscordchannel.hpp"
#include "qdiscordguild.hpp"

QDiscordChannel::QDiscordChannel(const QJsonObject& object, QDiscordGuild* guild)
{
    _id = object["id"].toString("");
    _isPrivate = object["is_private"].toBool(false);
    _lastMessageId = object["last_message_id"].toString("");
    _name = object["name"].toString("");
    _position = object["position"].toInt(0);
    _topic = object["topic"].toString("");
    QString type = object["type"].toString("text");
    if(type == "text")
        _type = ChannelType::Text;
    else if(type == "voice")
        _type = ChannelType::Voice;
    else
        _type = ChannelType::UnknownType;
    _guild = guild;
    QJsonObject recipientObject = object["recipient"].toObject();
    _recipient = nullptr;
    if(_isPrivate)
        _recipient = new QDiscordUser(recipientObject);
    qDebug()<<"QDiscordChannel("<<this<<") constructed";
}

QDiscordChannel::QDiscordChannel(QDiscordChannel& other)
{
    _id = other.id();
    _isPrivate = other.isPrivate();
    _lastMessageId = other.lastMessageId();
    _name = other.name();
    _position = other.position();
    _topic = other.topic();
    _type = other.type();
    _guild = other.guild();
    if(other.recipient())
        _recipient = new QDiscordUser(*other.recipient());
    else
        _recipient = nullptr;
}

QDiscordChannel::~QDiscordChannel()
{
    if(_recipient)
        delete _recipient;
}
