#include <QtTest>
#include "qdiscord.d/qdiscordmember.hpp"
#include "qdiscord.d/qdiscorduser.hpp"

class tst_QDiscordMember : public QObject
{
	Q_OBJECT
public:
	tst_QDiscordMember();
private slots:
	void testConstructor_data();
	void testConstructor();
	void testMentions_data();
	void testMentions();
private:
	QJsonObject _nullMember;
	QJsonObject _guildlessMember;
	QJsonObject _userlessMember;
	QJsonObject _testMember;
	QDiscordUser _testUser;
};

tst_QDiscordMember::tst_QDiscordMember():
	_nullMember({
			{"deaf", QJsonValue::Null},
			{"mute", QJsonValue::Null},
			{"nick", QJsonValue::Null},
			{"joined_at", QJsonValue::Null}
		}),
	_userlessMember({
			{"deaf", true},
			{"mute", true},
			{"nick", "testbot"},
			{"joined_at", "2016-07-22T18:15:12.448000+00:00"}
		}),
	_testMember({
			{"deaf", true},
			{"mute", true},
			{"nick", "testbot"},
			{"joined_at", "2016-07-22T18:15:12.448000+00:00"},
			{"user", QJsonObject({
									 {"id", "111264179623531612"}
									})}
		}),
	_testUser(
		QJsonObject({
			{"id", "111264179623531612"}
		})
		)
{

}

void tst_QDiscordMember::testConstructor_data()
{
	QTest::addColumn<QJsonObject>("input_object");
	QTest::addColumn<QDiscordUser>("output_user");
	QTest::addColumn<bool>("user_is_null");
	QTest::addColumn<bool>("output_deaf");
	QTest::addColumn<bool>("output_mute");
	QTest::addColumn<QDateTime>("output_joinedAt");
	QTest::addColumn<QString>("output_nickname");

	QTest::newRow("nullMember") << _nullMember <<
								   _testUser <<
								   true <<
								   false <<
								   false <<
								   QDateTime() <<
								   "";

	QTest::newRow("testMember") << _testMember <<
								   _testUser <<
								   false <<
								   true <<
								   true <<
								   QDateTime::fromString(
									   "2016-07-22T18:15:12.448000+00:00",
									   Qt::ISODate) <<
								   "testbot";
	QTest::newRow("userlessMember") << _userlessMember <<
								   _testUser <<
								   true <<
								   true <<
								   true <<
								   QDateTime::fromString(
									   "2016-07-22T18:15:12.448000+00:00",
									   Qt::ISODate) <<
								   "testbot";
}

void tst_QDiscordMember::testConstructor()
{
	QFETCH(QJsonObject, input_object);
	QFETCH(QDiscordUser, output_user);
	QFETCH(bool, user_is_null);
	QFETCH(bool, output_deaf);
	QFETCH(bool, output_mute);
	QFETCH(QDateTime, output_joinedAt);
	QFETCH(QString, output_nickname);

	QDiscordMember member(input_object, nullptr);

	if(!user_is_null)
		QVERIFY(*member.user() == output_user);
	else
		QVERIFY(member.user() == nullptr);
	QVERIFY(member.deaf() == output_deaf);
	QVERIFY(member.mute() == output_mute);
	QVERIFY(member.joinedAt() == output_joinedAt);
	QCOMPARE(member.nickname(), output_nickname);
}

void tst_QDiscordMember::testMentions_data()
{
	QTest::addColumn<QDiscordMember>("input_member");
	QTest::addColumn<QString>("output_nickname");
	QTest::addColumn<QString>("output_username");

	QTest::newRow("testMember") << QDiscordMember(_testMember, nullptr) <<
								   "<@!111264179623531612>" <<
								   "<@111264179623531612>";
	QTest::newRow("nullMember") << QDiscordMember(_nullMember, nullptr) <<
								   "<@!nullptr>" <<
								   "<@nullptr>";
}

void tst_QDiscordMember::testMentions()
{
	QFETCH(QDiscordMember, input_member);
	QFETCH(QString, output_nickname);
	QFETCH(QString, output_username);

	QCOMPARE(input_member.mentionNickname(), output_nickname);
	QCOMPARE(input_member.mentionUsername(), output_username);
}

QTEST_MAIN(tst_QDiscordMember)

#include "tst_qdiscordmember.moc"