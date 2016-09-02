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

#ifndef QDISCORDGAME_HPP
#define QDISCORDGAME_HPP

#include <QJsonObject>
#include <QDebug>

class QDiscordGame
{
public:
	enum GameType : int
	{
		None = 0,
		Streaming = 1,
		UnknownType = -1
	};
	QDiscordGame(QString name = "", QString url = "", GameType type = GameType::None);
	QDiscordGame(const QJsonObject& object);
	QString name() {return _name;}
	QString url() {return _url;}
	GameType type() {return _type;}
private:
	QString _name;
	QString _url;
	GameType _type;
};

#endif // QDISCORDGAME_HPP
