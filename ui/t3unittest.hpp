#ifndef T3UNITTEST_HPP
#define T3UNITTEST_HPP
#include "t3trackcontroller.hpp"
#include "t3database.hpp"
#include "t3trackmodel.hpp"
#include "t3ctcoffice.hpp"
class T3UnitTest {
	T3Database* db = nullptr;
  public:
	T3UnitTest(T3Database* db) {
		this->db = db;
	}
	bool testAll();
  private:
	void ctc_searchPathForAuthorityTest();
	void ctc_enqueueDispatchRequestTest();
	void ctc_discardDispatchRequestTest();
	void ctc_searchPathFromCsvTest();
	void km_addTrackFromCsvTest();
	bool km_plcScriptTest();
	void nm_placeNewTrainTest();
};



inline bool T3UnitTest::testAll() {
	km_addTrackFromCsvTest();
	ctc_searchPathForAuthorityTest();
	ctc_enqueueDispatchRequestTest();
	ctc_discardDispatchRequestTest();
	nm_placeNewTrainTest();
	ctc_searchPathFromCsvTest();
	return true;
}

inline void T3UnitTest::ctc_searchPathForAuthorityTest() {
	Q_ASSERT(T3CTCOffice::searchPathForAuthority("G_H_33", "G_H_35", &db->MODU_ARGS).size() > 0);
	qInfo("ctc_searchPathForAuthorityTest passed");
}

inline void T3UnitTest::ctc_enqueueDispatchRequestTest() {
	T3CTCOffice::enqueueDispatchRequest(&db->dispatchQueue, {"____", "G_H_33", "G_H_35", "00:00"}, {"G_H_33"});
	Q_ASSERT(db->dispatchQueue.size() > 0);
	qInfo("ctc_enqueueDispatchRequestTest passed");
}

inline void T3UnitTest::ctc_discardDispatchRequestTest() {
	if(db->dispatchQueue.size() == 1)T3CTCOffice::discardDispatchRequest(0, &db->dispatchQueue);
	Q_ASSERT(db->dispatchQueue.size() == 0);
	qInfo("ctc_discardDispatchRequestTest passed");
}

inline void T3UnitTest::ctc_searchPathFromCsvTest() {
	QJsonArray ret = T3CTCOffice::searchPathsFromCsv("C:/Users/YIQ25/Documents/Academics/ECE1140/SPRN2023_ECE1140/T3GreenLineSampleSchedule.csv"
					 , &db->stationToBlockIdMap, {"____", "G_H_33", "G_H_35", "00:00"}, &db->MODU_ARGS);
	Q_ASSERT(ret.size() > 0);
	qInfo("ctc_searchPathFromCsvTest passed");
}

inline void T3UnitTest::km_addTrackFromCsvTest() {
	Q_ASSERT(db->trackConstantsObjects.size() == 0);
	T3TrackModel::addTrackFromCsv("C:/Users/YIQ25/Documents/Academics/ECE1140/Resources/T3GreenLine.csv", &db->stationToBlockIdMap, &db->MODU_ARGS);
	Q_ASSERT(db->trackConstantsObjects.size() > 0);
	qInfo("km_addTrackFromCsvTest passed");
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

inline void T3UnitTest::nm_placeNewTrainTest() {
	T3TrainModel::createNewTrain("0001", {"G_H_33", "G_H_35"}, &db->MODU_ARGS);
	Q_ASSERT(db->trainObjects.size() > 0);
	qInfo("nm_placeNewTrainTest passed");
}
#endif // T3UNITTEST_HPP
