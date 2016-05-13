#include "qdiscordmessage.hpp"

QDiscordMessage::QDiscordMessage(const QJsonObject& object, QDiscordChannel* channel)
{
    _id = object["id"].toString("");
    _mentionEveryone = object["mention_everyone"].toBool(false);
    _content = object["content"].toString("");
    _author = nullptr;
    _channel = channel;
    _channelId = object["channel_id"].toString("");
    _author = new QDiscordUser(object["author"].toObject());
    _tts = object["tts"].toBool(false);
    _timestamp = QDateTime::fromString(object["timestamp"].toString(""), Qt::ISODate);
    _editedTimestamp = QDateTime::fromString(object["edited_timestamp"].toString(""), Qt::ISODate);
    qDebug()<<"QDiscordMessage("<<this<<") constructed";
}

QDiscordMessage::QDiscordMessage(QDiscordMessage& other)
{
    _id = other.id();
    _mentionEveryone = other.mentionEveryone();
    _content = other.content();
    _author = new QDiscordUser(*other.author());
    _channel = other.channel();
    _channelId = other.channelId();
    _tts = other.tts();
    _timestamp = other.timestamp();
    _editedTimestamp = other.editedTimestamp();
}

QDiscordMessage::~QDiscordMessage()
{
    if(_author)
        delete _author;
}

QDiscordGuild*QDiscordMessage::guild()
{
    if(_channel)
        return _channel->guild();
    else
        return nullptr;
}
