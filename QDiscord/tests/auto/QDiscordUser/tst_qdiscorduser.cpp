#include <QtTest>
#include <QDiscord>

class tst_QDiscordUser: public QObject
{
	Q_OBJECT
public:
	tst_QDiscordUser();
private slots:
	void testConstructor_data();
	void testConstructor();
	void testUpdate_data();
	void testUpdate();
	void testOperatorEquals_data();
	void testOperatorEquals();
private:
	QJsonObject _testUser;
	QJsonObject _nullUser;
};



tst_QDiscordUser::tst_QDiscordUser():
	_testUser({
				{"avatar", "577444852b"},
				{"bot", true},
				{"discriminator", "8701"},
				{"email", "bot@example.org"},
				{"id", "111264179623531612"},
				{"mfa_enabled", false},
				{"username", "TestBot"},
				{"verified", true}
			}),
	_nullUser({
				{"avatar", QJsonValue::Null},
				{"bot", QJsonValue::Null},
				{"discriminator", QJsonValue::Null},
				{"email", QJsonValue::Null},
				{"id", QJsonValue::Null},
				{"mfa_enabled", QJsonValue::Null},
				{"username", QJsonValue::Null},
				{"verified", QJsonValue::Null}
			})
{

}

void tst_QDiscordUser::testConstructor_data()
{
	QTest::addColumn<QJsonObject>("input_object");
	QTest::addColumn<QString>("output_id");
	QTest::addColumn<QString>("output_avatar");
	QTest::addColumn<bool>("output_bot");
	QTest::addColumn<QString>("output_discriminator");
	QTest::addColumn<QString>("output_email");
	QTest::addColumn<QString>("output_username");
	QTest::addColumn<bool>("output_verified");

	QTest::newRow("testbot") << _testUser <<
								"111264179623531612" <<
								"577444852b" <<
								true <<
								"8701" <<
								"bot@example.org" <<
								"TestBot" <<
								true;
	QTest::newRow("nulluser") << _nullUser <<
								"" <<
								"" <<
								false <<
								"" <<
								"" <<
								"" <<
								false;
}

void tst_QDiscordUser::testConstructor()
{
	QFETCH(QJsonObject, input_object);
	QFETCH(QString, output_id);
	QFETCH(QString, output_avatar);
	QFETCH(bool, output_bot);
	QFETCH(QString, output_discriminator);
	QFETCH(QString, output_email);
	QFETCH(QString, output_username);
	QFETCH(bool, output_verified);

	QDiscordUser testBot(input_object);

	QCOMPARE(testBot.id(), output_id);
	QCOMPARE(testBot.avatar(), output_avatar);
	QVERIFY(testBot.bot() == output_bot);
	QCOMPARE(testBot.discriminator(), output_discriminator);
	QCOMPARE(testBot.email(), output_email);
	QCOMPARE(testBot.username(), output_username);
	QVERIFY(testBot.verified() == output_verified);
}

void tst_QDiscordUser::testUpdate_data()
{
	QTest::addColumn<QDiscordUser>("original");
	QTest::addColumn<QJsonObject>("update");
	QTest::addColumn<QString>("output_id");
	QTest::addColumn<QString>("output_avatar");
	QTest::addColumn<bool>("output_bot");
	QTest::addColumn<QString>("output_discriminator");
	QTest::addColumn<QString>("output_email");
	QTest::addColumn<QString>("output_username");
	QTest::addColumn<bool>("output_verified");

	QDiscordUser nullUser(_nullUser);

	QTest::newRow("test1") << nullUser <<
							  _testUser <<
							  "111264179623531612" <<
							  "577444852b" <<
							  true <<
							  "8701" <<
							  "bot@example.org" <<
							  "TestBot" <<
							  true;
}

void tst_QDiscordUser::testUpdate()
{
	QFETCH(QDiscordUser, original);
	QFETCH(QJsonObject, update);
	QFETCH(QString, output_id);
	QFETCH(QString, output_avatar);
	QFETCH(bool, output_bot);
	QFETCH(QString, output_discriminator);
	QFETCH(QString, output_email);
	QFETCH(QString, output_username);
	QFETCH(bool, output_verified);

	original.update(update);

	QCOMPARE(original.id(), output_id);
	QCOMPARE(original.avatar(), output_avatar);
	QVERIFY(original.bot() == output_bot);
	QCOMPARE(original.discriminator(), output_discriminator);
	QCOMPARE(original.email(), output_email);
	QCOMPARE(original.username(), output_username);
	QVERIFY(original.verified() == output_verified);
}

void tst_QDiscordUser::testOperatorEquals_data()
{
	QTest::addColumn<QDiscordUser>("nullUser");
	QTest::addColumn<QDiscordUser>("testUser");

	QDiscordUser nullUser(_nullUser);
	QDiscordUser testUser(_testUser);

	QTest::newRow("test1") << nullUser << testUser;
}

void tst_QDiscordUser::testOperatorEquals()
{
	QFETCH(QDiscordUser, nullUser);
	QFETCH(QDiscordUser, testUser);

	QVERIFY(nullUser != testUser);
	QVERIFY(!(nullUser == testUser));
}

QTEST_MAIN(tst_QDiscordUser)

#include "tst_qdiscorduser.moc"
