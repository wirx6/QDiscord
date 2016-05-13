#ifndef QDISCORDMESSAGE_HPP
#define QDISCORDMESSAGE_HPP

#include "qdiscordmember.hpp"
#include "qdiscordchannel.hpp"
#include "qdiscordguild.hpp"

/*!
 * \brief Represents a message in the Discord API.
 */
class QDiscordMessage
{
public:
    /*!
     * \brief Creates an instance from the provided parameters.
     * \param object A JSON object of a Discord message.
     * \param channel A pointer to the channel the message was sent to.
     */
    QDiscordMessage(const QJsonObject& object, QDiscordChannel* channel = nullptr);
    ///\brief Deep copies the provided object.
    QDiscordMessage(QDiscordMessage& other);
    ~QDiscordMessage();
    ///\brief Returns the message's ID.
    QString id() {return _id;}
    ///\brief Returns the message's contents.
    QString content() {return _content;}
    ///\brief Returns the date at which the message was edited. May be invalid if the message was not edited.
    QDateTime editedTimestamp() {return _editedTimestamp;}
    ///\brief Returns the date at which the message was created.
    QDateTime timestamp() {return _timestamp;}
    ///\brief Returns whether the message will use text to speech.
    bool tts() {return _tts;}
    ///\brief Returns whether the message successfully mentioned everyone.
    bool mentionEveryone() {return _mentionEveryone;}
    ///\brief Returns the ID of the channel this message was sent in.
    QString channelId() {return _channelId;}
    ///\brief Returns a pointer to the channel this message was sent in.
    QDiscordChannel* channel() {return _channel;}
    ///\brief Returns a pointer to the user that sent this message.
    QDiscordUser* author() {return _author;}
    /*!
     * \brief Returns a pointer to the guild this message was sent in using
     * the channel parameter provided in the class' constructor.
     *
     * Will return nullptr if the message was sent in a private channel and thus the channel has no guild.\n
     * Will also return nullptr if a channel was not provided in the class' constructor.
     */
    QDiscordGuild* guild();
private:
    QString _id;
    QString _content;
    QDateTime _editedTimestamp;
    QDateTime _timestamp;
    bool _tts;
    bool _mentionEveryone;
    QString _channelId;
    QDiscordChannel* _channel;
    QDiscordUser* _author;
};

#endif // QDISCORDMESSAGE_HPP
