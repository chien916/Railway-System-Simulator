#ifndef T3UNITTEST_HPP
#define T3UNITTEST_HPP
#include "t3trackcontroller.hpp"
class T3UnitTest {

  public:
	T3UnitTest() {}
	bool testAll();
  private:
	bool km_plcScriptTest();
};

inline bool T3UnitTest::testAll() {
	return
		km_plcScriptTest();
}

inline bool T3UnitTest::km_plcScriptTest() {
	QJSEngine plcRuntime;
	QJSValue plcFunction;
	T3TrackController::addPlcScriptFromCsv(":/T3KCPlcScript.js", &plcRuntime, &plcFunction);
	QString CTCPLCIO_输入 =
		QString("00000000") +
		QString("00000000") +
		QString("00000000") +
		QString("00000000");
	QString KCPLCIN_输入 =
		QString("00000000") +
		QString("00000000") +
		QString("00000000") +
		QString("00000000");
	QString KMPLCIO_输入 =
		QString("10000011") +
		QString("00000000") +
		QString("00000000") +
		QString("00000000");
	QString BCNPLCOUT_输入 =
		QString("00000000") +
		QString("00000000") +
		QString("00000000") +
		QString("00000000");


	QString CTCPLCIO_输出 = CTCPLCIO_输入;
	QString BCNPLCOUT_输出 = BCNPLCOUT_输入;
	QString KMPLCIO_输出 = KMPLCIO_输入;
	QString KCPLCIN_输出 = KCPLCIN_输入;

	T3TrackController::runPlcScript(CTCPLCIO_输入, KCPLCIN_输入, KMPLCIO_输入, BCNPLCOUT_输入, &plcRuntime, &plcFunction);
	qInfo() << "\nCTCPLCIO";
	for(int i = 0; i < 4; ++i) {
		qInfo() << CTCPLCIO_输出.mid(i * 8, 8) << " " << CTCPLCIO_输入.mid(i * 8, 8);
	}
	qInfo() << "\nKCPLCIN";
	for(int i = 0; i < 4; ++i) {
		qInfo() << KCPLCIN_输出.mid(i * 8, 8) << " " << KCPLCIN_输入.mid(i * 8, 8);
	}
	qInfo() << "\nKMPLCIO";
	for(int i = 0; i < 4; ++i) {
		qInfo() << KMPLCIO_输出.mid(i * 8, 8) << " " << KMPLCIO_输入.mid(i * 8, 8);
	}

	qInfo() << "\nBCNPLCOUT";
	for(int i = 0; i < 4; ++i) {
		qInfo() << BCNPLCOUT_输出.mid(i * 8, 8) << " " << BCNPLCOUT_输入.mid(i * 8, 8);
	}
	return true;
}
#endif // T3UNITTEST_HPP
