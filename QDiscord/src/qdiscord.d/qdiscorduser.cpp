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

#include "qdiscorduser.hpp"
#include "qdiscordutilities.hpp"

QDiscordUser::QDiscordUser(const QJsonObject& object)
{
	_id = object["id"].toString("");
	_bot = object["bot"].toBool(false);
	_discriminator = object["discriminator"].toString("");
	_email = object["email"].toString("");
	_username = object["username"].toString("");
	_verified = object["verified"].toBool(false);
	_avatar = object["avatar"].toString("");

	if(QDiscordUtilities::debugMode)
		qDebug()<<"QDiscordUser("<<this<<") constructed";
}

QDiscordUser::QDiscordUser()
{
	_id = "";
	_bot = false;
	_discriminator = "";
	_email = "";
	_username = "";
	_verified = false;
	_avatar = "";

	if(QDiscordUtilities::debugMode)
		qDebug()<<"QDiscordUser("<<this<<") constructed";
}

void QDiscordUser::update(const QJsonObject& object)
{
	if(object.contains("id"))
		_id = object["id"].toString("");
	if(object.contains("bot"))
		_bot = object["bot"].toBool(false);
	if(object.contains("discriminator"))
		_discriminator = object["discriminator"].toString("");
	if(object.contains("email"))
		_email = object["email"].toString("");
	if(object.contains("username"))
		_username = object["username"].toString("");
	if(object.contains("verified"))
		_verified = object["verified"].toBool(false);
	if(object.contains("avatar"))
		_avatar = object["avatar"].toString("");

	if(QDiscordUtilities::debugMode)
		qDebug()<<"QDiscordUser("<<this<<") updated";
}

bool QDiscordUser::operator ==(const QDiscordUser& other) const
{
	return other.id() == _id;
}

bool QDiscordUser::operator !=(const QDiscordUser& other) const
{
	return other.id() != _id;
}
