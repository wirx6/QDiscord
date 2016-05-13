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

#include "qdiscordmember.hpp"
#include "qdiscordguild.hpp"

QDiscordMember::QDiscordMember(const QJsonObject& object, QDiscordGuild* guild)
{
    _deaf = object["deaf"].toBool(false);
    _mute = object["mute"].toBool(false);
    _joinedAt = QDateTime::fromString(object["joined_at"].toString(""), Qt::ISODate);
    _user = nullptr;
    _guild = guild;
    if(object["user"].isObject())
        _user = new QDiscordUser(object["user"].toObject());
    qDebug()<<"QDiscordMember("<<this<<") constructed";
}

QDiscordMember::QDiscordMember(QDiscordMember& other)
{
    _deaf = other.deaf();
    _mute = other.mute();
    _joinedAt = other.joinedAt();
    _user = nullptr;
    if(other.user())
        _user = new QDiscordUser(*other.user());
    _guild = other.guild();
}

QDiscordMember::~QDiscordMember()
{
    if(_user)
        delete _user;
}
