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

#include "qdiscordutilities.hpp"

//--------------------------------------------------------------------------------------
//-----------------------LIBRARY CONFIGURATION------------------------------------------
//--------------------------------------------------------------------------------------
QString QDiscordUtilities::botName = "QDiscordBot";
const QString QDiscordUtilities::libName = "QDiscord";
const QString QDiscordUtilities::libLink = "https://github.com/george99g/QDiscord";
const QString QDiscordUtilities::libMajor = "0";
const QString QDiscordUtilities::libMinor = "6";
//const QString QDiscordUtilities::userAgent = "DiscordBot (" + QDiscordUtilities::libLink +
//                                              ", v" + QDiscordUtilities::libMajor + ":" + QDiscordUtilities::libMinor + ")" +
//                                              "; " + QDiscordUtilities::botName;
//--------------------------------------------------------------------------------------

const struct QDiscordUtilities::EndPoints QDiscordUtilities::endPoints =
{
    "https://discordapp.com",
    "https://discordapp.com/api",
    "https://discordapp.com/api/gateway?encoding=json&v=4",
    "https://discordapp.com/api/users",
    "https://discordapp.com/api/users/@me",
    "https://discordapp.com/api/auth/register",
    "https://discordapp.com/api/auth/login",
    "https://discordapp.com/api/auth/logout",
    "https://discordapp.com/api/guilds",
    "https://discordapp.com/api/channels"
};

QString QDiscordUtilities::networkErrorToString(QNetworkReply::NetworkError error)
{
    switch((int)error)
    {
    case 200:
        return "200 (OK): The response completed sucessfully.";
    case 201:
        return "200 (CREATED): The entity was created successfully.";
    case 304:
        return "304 (NOT MODIFIED): The entity was not modified (no action was taken).";
    case 400:
        return "400 (BAD REQUEST): The request was improperly formatted, or the server couldn't understand it.";
    case 401:
        return "401 (UNAUTHORIZED): The Authorization header was missing or invalid.";
    case 403:
        return "403 (FORBIDDEN): The Authorization token you passed did not have permission to the resource";
    case 404:
        return "404 (NOT FOUND): The resource at the location specified doesn't exist.";
    case 405:
        return "405 (METHOD NOT ALLOWED): The HTTP method used is not valid for the location specified.";
    case 429:
        return "429 (TOO MANY REQUESTS): You've made too many requests.";
    case 502:
        return "502 (GATEWAY UNAVAILABLE): There was not a gateway available to process your request. Wait a bit and retry.";
    default:
        if((int)error > 499 && (int)error < 600)
            return "5xx (SERVER ERROR): The server had an error processing your request.";
        else return "xxx (UNKNOWN): Unknown error.";
    }
}
