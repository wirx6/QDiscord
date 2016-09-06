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

#ifndef QDISCORDGUILD_HPP
#define QDISCORDGUILD_HPP

#include <QDateTime>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
#include "qdiscordmember.hpp"
#include "qdiscordchannel.hpp"
#include "qdiscordutilities.hpp"

///\brief Represents a guild in the Discord API.
class QDiscordGuild
{
public:
	/*!
	 * \brief Creates an instance from the provided parameters.
	 * \param object A JSON object of a Discord guild.
	 */
	QDiscordGuild(const QJsonObject& object);
	///\brief Deep copies the provided object.
	QDiscordGuild(QDiscordGuild& other);
	~QDiscordGuild();
	///\brief Returns the guild's ID.
	QString id() {return _id;}
	///\brief Returns the guild's name.
	QString name() {return _name;}
	/*!
	 * \brief Returns whether the guild is unavailable.
	 *
	 * If this is true, most members will not be set to anything.
	 */
	bool unavailable() {return _unavailable;}
	///\brief Returns the guild's verification level.
	int verificationLevel() {return _verificationLevel;}
	///\brief Returns the guild's AFK time needed to move a user to the AFK channel.
	int afkTimeout() {return _afkTimeout;}
	///\brief Returns the guild's member count.
	int memberCount() {return _memberCount;}
	///\brief Returns the date the current user joined this guild.
	QDateTime joinedAt() {return _joinedAt;}
	///\brief Returns a map of pointers to the guild's channels and their IDs.
	QMap<QString, QDiscordChannel*> channels() {return _channels;}
	/*!
	 * \brief Returns a pointer to a guild channel that has the provided ID.
	 * May return `nullptr` if the channel was not found.
	 */
	QDiscordChannel* channel(const QString& id) {return _channels.value(id, nullptr);}
	/*!
	 * \brief Returns a pointer to a guild member that has the provided ID.
	 * May return `nullptr` if the member was not found.
	 */
	QDiscordMember* member(const QString& id) {return _members.value(id, nullptr);}
	/*!
	 * \brief Adds the provided channel to the guild.
	 *
	 * The guild will handle destroying the pointed channel when the guild gets destroyed.
	 */
	void addChannel(QDiscordChannel* channel);
	/*!
	 * \brief Removes the provided channel from the guild.
	 *
	 * If the guild contains the provided channel, the pointed channel will be destroyed.
	 * \returns `true` if the channel was successfully removed. `false` if `nullptr` was
	 * passed or the channel was not found.
	 */
	bool removeChannel(QDiscordChannel* channel);
	/*!
	 * \brief Adds the provided member to the guild.
	 *
	 * The guild will handle destroying the pointed member when the guild gets destroyed.
	 * If the member already exists, the previous member will be destroyed.
	 */
	void addMember(QDiscordMember* member);
	/*!
	 * \brief Removes the provided member from the guild.
	 *
	 * If the guild contains the provided member, the pointed member will be destroyed.
	 * \returns `true` if the member was successfully removed. `false` if `nullptr` was
	 * passed or the member was not found.
	 */
	bool removeMember(QDiscordMember* member);
private:
	QString _id;
	QString _name;
	bool _unavailable;
	int _verificationLevel;
	int _afkTimeout;
	int _memberCount;
	QDateTime _joinedAt;
	QMap<QString, QDiscordMember*> _members;
	QMap<QString, QDiscordChannel*> _channels;
};

#endif // QDISCORDGUILD_HPP
