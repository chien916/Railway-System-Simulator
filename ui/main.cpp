#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtCore>
#include <QtGui>
#include <QtQml>

#include "t3database.hpp"
#include "t3unittest.hpp"
int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
	QGuiApplication app(argc, argv);



	bool testMode = false;

	if(testMode) {
		T3UnitTest unitTests;
		unitTests.testAll();
	} else {
		QQmlApplicationEngine engine;
		const QUrl url(QStringLiteral("qrc:/main.qml"));
		QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
		&app, [url](QObject * obj, const QUrl & objUrl) {
			if (!obj && url == objUrl)
				QCoreApplication::exit(-1);
		}, Qt::QueuedConnection);

		QScopedPointer<T3Database> t3database(new T3Database());
		//	QScopedPointer<T3UnitTest> t3unittest(new T3UnitTest(t3database.data()));
		//T3Database t3database;
		t3database->startTimer(1000);//100 msecond timer
		engine.rootContext()->setContextProperty("t3databaseQml", t3database.data());

		engine.load(url);

		//	testTrainController.startTimer(100);

		QFontDatabase::addApplicationFont(":/T3InterFont.ttf");
		QFontDatabase::addApplicationFont(":/T3SegFont.ttf");

		return app.exec();
	}
}
