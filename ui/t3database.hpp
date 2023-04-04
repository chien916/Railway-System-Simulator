#ifndef T3DATABASE_H
#define T3DATABASE_H

#include "t3prophelper.h"
#include "t3trainmodel.hpp"
#include "t3trackmodel.hpp"
#include "t3trackcontroller.hpp"
#include "t3traincontroller.hpp"
#include "t3ctcoffice.hpp"
//#include "t3trackmodel.hpp"


/**
 * @brief The T3Database class
 * This class will store all relative information of rail track
 * block data. Both constants and variables are stored in the
 * form of json array.
 *
 * The constants are loaded from the modified CSV file provided
 * by Dr. Profeta. It includes the property of rail block such
 * as crossing, direction of block, grade, length, available
 * next and previous blocks, speed limit, station information,
 * underground information
 * --Keep in mind that the crossing, prev/next block, station
 * field only indicates IF THIS BLOCK IS A STATION/CROSSING..
 * and the real-time state of these infrastructures is stored
 * in the variable objects.
 * --Notify changes whenever new line is being added
 * (Every notify will make UI re-render line map, takes long time)
 *
 * The variables on the other hand includes the dynamic state
 * of insfrastructure, such as the crossing is up or down (given
 * that the current block IS a crossing), the current switch
 * position. It also stores non-infrastructure related information
 * such as train occupancy, beacon information (authority and
 * commanded speed). The PLC program is NOT stored here!!!
 * --Notify changes whenever train controller writes new value
 *
 * QML Exposed Variables:
 * Notify
 *
 */

using MODU_ARGS_REF
	= const std::tuple<const std::function<QVariant(const QString, const QString, const QJsonArray*)>*
	  , const std::function<void(const QString, const QString, QJsonArray*, const QVariant)>*
	  ,  QJsonArray*, QJsonArray*, QJsonArray*>*;

class T3Database: public QObject {
	friend class T3UnitTest;
	Q_OBJECT

	//====================================================
	//=====================FIREBASE DB====================
	//=====================网络在线数据库===================
	//====================================================
  public:
	enum DatabaseTitle : uint8_t {
		TrackConstantsObjects = 0b00000001
		, TrackVariablesObjects = 0b00000010
		, TrainObjects = 0b00000100
		, DispatchQueue = 0b00001000
	};
	Q_ENUM(DatabaseTitle);

	QJsonArray trackConstantsObjects;//do not modify directly after initialization. use the setTrackProperty() slot instead!
	Q_PROPERTY(QJsonArray trackConstantsObjects_QML MEMBER trackConstantsObjects NOTIFY onTrackConstantsObjectsChanged)

	QJsonArray trackVariablesObjects;//do not modify directly after initialization. use the setTrackProperty() slot instead!
	Q_PROPERTY(QJsonArray trackVariablesObjects_QML MEMBER trackVariablesObjects NOTIFY onTrackVariablesObjectsChanged)

	Q_PROPERTY(QJsonArray trackIds_QML READ km_getAllTrackIds NOTIFY onTrackConstantsObjectsChanged)

	QJsonArray trainObjects;//do not modify directly after initialization. use the setTrainProperty() slot instead!
	Q_PROPERTY(QJsonArray trainObjects_QML MEMBER trainObjects NOTIFY onTrainObjectsChanged)

	Q_PROPERTY(QJsonArray trainIds_QML READ nm_getAllTrainIds NOTIFY onTrainObjectsChanged)

	QJsonArray dispatchQueue;//当前等待被发车的队列
	Q_PROPERTY(QJsonArray dispatchQueue_QML MEMBER dispatchQueue NOTIFY onDispatchQueueChanged)

	QJsonObject stationToBlockIdMap;//对于一个站对应多个铁轨块的哈希表
	//Q_PROPERTY(QJsonObject stationToBlockIdMap_QML MEMBER stationToBlockIdMap NOTIFY onStationToBlockIdMapChanged)

  private:

	bool firebaseEnabled = false;
	bool localFileEnabled = false;
	QString firebaseRootUrlString = FIREBASE_URL;
	QString filepathString = QDir::currentPath();
	QNetworkAccessManager networkAccessManager;

	using DB_MAPPER_T = QVarLengthArray<std::tuple<QString, QJsonArray*, void(T3Database::*)(void)>, 4>;
	const DB_MAPPER_T DB_MAPPER = {
		{"trackConstantsObjects", &trackConstantsObjects, &T3Database::onTrackConstantsObjectsChanged}
		, {"trackVariablesObjects", &trackVariablesObjects, &T3Database::onTrackVariablesObjectsChanged}
		, {"trainObjects", &trainObjects, &T3Database::onTrainObjectsChanged}
		, {"dispatchQueue", &dispatchQueue, &T3Database::onDispatchQueueChanged}
	};
	const std::function<QVariant(const QString, const QString, const QJsonArray*)> GET_PROP =
	[](const QString blockId, const QString prop, const QJsonArray* objects) {
		for(qsizetype i = 0; i < objects->size(); ++i) { //for every line
			QJsonObject currObj = objects->at(i).toObject();
			if(currObj.contains("NM_ID") && currObj.value("NM_ID") == blockId) {//handles train object
				return currObj.value(prop).toVariant();
				currObj = currObj.value("blocksMap").toObject();
			}
			if(currObj.contains("blocksMap")) { //handles track constants objects
				currObj = currObj.value("blocksMap").toObject();
			}
			if(currObj.contains(blockId)) {
				return currObj.value(blockId).toObject().value(prop).toVariant();
			}
		}
		//throw std::exception();
		qFatal(QString("T3Database::GET_PROP(%1,%2) failed").arg(blockId, prop).toLocal8Bit());
		return QVariant();
	};
	const std::function<void(const QString, const QString, QJsonArray*, const QVariant)> SET_PROP =
	[](const QString blockId, const QString prop, QJsonArray* objects, const QVariant val) {
		for(qsizetype i = 0; i < objects->size(); ++i) { //for every line
			QJsonObject currObj = objects->at(i).toObject();
			if(currObj.contains("NM_ID") && currObj.value("NM_ID") == blockId) {//handles train object
				currObj.insert(prop, QJsonValue::fromVariant(val));
				objects->operator[](i) = currObj;
				return;
			}
			if(currObj.contains(blockId) && currObj.value(blockId).isObject()) {//handles track variables object
				QJsonObject currBlockObj = currObj.value(blockId).toObject();
				currBlockObj.insert(prop, QJsonValue::fromVariant(val));
				currObj.insert(blockId, currBlockObj);
				objects->operator[](i) = currObj;
				return;
			}
		}
		//throw std::exception();
		qFatal(QString("T3Database::SET_PROP(%1,%2) failed").arg(blockId).arg(prop).toLocal8Bit());
	};



  public:
	const std::tuple<const std::function<QVariant(const QString, const QString, const QJsonArray*)>*
	, const std::function<void(const QString, const QString, QJsonArray*, const QVariant)>*
	,  QJsonArray*, QJsonArray*, QJsonArray*> MODU_ARGS  = {
		&GET_PROP, &SET_PROP, &trackConstantsObjects, &trackVariablesObjects, &trainObjects
	};
	Q_INVOKABLE void db_toggle(bool  enableFirebase, bool enableFile);
	Q_INVOKABLE void db_push(unsigned int selector_in = 0b11111111);
	Q_INVOKABLE void db_pull(unsigned int selector_in = 0b11111111);
  signals:
	void onTrackConstantsObjectsChanged();
	void onTrackVariablesObjectsChanged();
	void onTrainObjectsChanged();
	void onDispatchQueueChanged();
	void onStationToBlockIdMapChanged();

	//====================================================
	//=====================CTC-OFFICE=====================
	//=====================中央调度中心=====================
	//====================================================
  public:


	Q_INVOKABLE QJsonArray ctc_getPossiblePathsFromMetaInfo(const QJsonArray dispatchMetaInfo);
	Q_INVOKABLE QJsonArray ctc_getPossiblePathsFromCsv(const QString filePath, const QJsonArray dispatchMetaInfo);
	Q_INVOKABLE void ctc_enqueueDispatchRequest(const QJsonArray dispatchMetaInfo, const QJsonArray path);
	Q_INVOKABLE void ctc_discardDispatchRequest(const int index);
	Q_INVOKABLE QJsonArray ctc_readPlcInputFromMetaInfo(const QString blockId);
	Q_INVOKABLE void ctc_writeToPlcInputFromMetaInfo(const QString blockId, const QJsonArray metaInfo);
	Q_INVOKABLE QJsonArray ctc_searchPathForAuthority(const QString originBlock, const QString destiBlock);
	Q_INVOKABLE void ctc_toggleConnection(bool newConnectionState);
  signals:


  private:
	void ctc_iterate();
	//====================================================
	//==================TRACK-CONTROLLER==================
	//=====================铁路铁轨控制=====================
	//====================================================
  public:
	//Q_INVOKABLE QJsonArray kc_collectPlcInput(const QString blockId);
	Q_INVOKABLE void kc_addPlcScriptFromCsv(const QString filePath);
	Q_INVOKABLE QJsonArray kc_readPlcToMetaInfo(const QString blockId);
	Q_INVOKABLE void kc_writePlcFromMetaInfo(const QString blockId, const QJsonArray metaInfo);
	Q_INVOKABLE QJsonArray kc_getAllPlcBinaries(const QString blockId);
	Q_INVOKABLE void kc_processPlc(const QString blockId);
	QJSEngine plcRuntime;
	QJSValue plcFunction;
  signals:
  private:

	void kc_iterate();
	//====================================================
	//=====================TRACK-MODEL====================
	//=====================铁路铁轨模型=====================
	//====================================================
  public:
	Q_INVOKABLE QJsonArray km_getDisplayStrings(const QString blockId);
	Q_INVOKABLE void km_addTrackFromCsv(const QString filePath);
	Q_INVOKABLE void km_setIOFromMetaInfo(const QString blockId, const QJsonArray metaInfo);
	Q_INVOKABLE QJsonArray km_getIOFromMetaInfo(const QString blockId);
	QJsonArray km_getAllTrackIds();
  signals:

  private:
	void km_iterate();

	//====================================================
	//=====================TRAIN-MODEL====================
	//=====================铁道火车模型=====================
	//====================================================
  public:
	Q_INVOKABLE QJsonArray nm_getAllTrainIds();
	Q_INVOKABLE void nm_removeTrain(const QString trainId);
	Q_INVOKABLE void nm_setFailureOrBrake(const QString trainId, const int index, const bool value);
	Q_INVOKABLE QJsonArray nm_getStringsFromMetaInfo(const QString trainId);
  signals:

  private:
	void nm_iterate();

	//====================================================
	//==================TRAIN-CONTROLLER==================
	//=====================铁道控制模型=====================
	//====================================================
  public:
	Q_INVOKABLE void nc_setKpAndKi(const QString trainId, float kp, float ki);
	Q_INVOKABLE void nc_setCtrlParams(const QString trainId, QJsonArray metaInf);

	Q_INVOKABLE QJsonArray nc_getMetaInfo(const QString trainId);
  signals:
  private:
	void nc_iterate();
	//====================================================
	//=====================MICELLANOUS====================
	//=========================其他========================
	//====================================================
  private:

	uint8_t timerRate = 1;
	bool timerRunning = false;
	void nextClockCycle();

  protected:
	void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
  public:
	QTime currentTime;
	Q_PROPERTY(QString currentTime_QML READ getCurrentTime NOTIFY onCurrentTimeChanged)
	Q_INVOKABLE void toggleTimer(bool newTimerState);
	Q_INVOKABLE void _TEST_ITERATE() {
		nextClockCycle();
		this->currentTime = this->currentTime.addMSecs(100 * static_cast<int>(timerRate));
		Q_EMIT onCurrentTimeChanged();
	}
	T3Database(QObject *parent = nullptr);
	bool busy = false;
	Q_INVOKABLE QString getCurrentTime();
	Q_INVOKABLE void setTimerRate(int rate);

  signals:
	void onCurrentTimeChanged();

};





inline T3Database::T3Database(QObject * parent) : QObject(parent) {
#ifndef Q_OS_WIN
	qFatal("T3Database::T3Database() -> Platform is not Windows.");
#endif
	//	//TESTING FOR RAIL LOADING
	this->currentTime = QTime::currentTime();
	T3TrackModel::addTrackFromCsv(INITIAL_LINE_CSV_DIR + QString("/T3RedLine.csv"), &stationToBlockIdMap, &MODU_ARGS);
	T3TrackModel::addTrackFromCsv(INITIAL_LINE_CSV_DIR + QString("/T3GreenLine.csv"), &stationToBlockIdMap, &MODU_ARGS);
	//T3TrackModel::addTrackFromCsv(INITIAL_LINE_CSV_DIR + QString("/T3BlueLine.csv"), &stationToBlockIdMap, &MODU_ARGS);
	T3TrackController::addPlcScriptFromCsv(":/T3KCPlcScript.js", &plcRuntime, &plcFunction);
	//test place new train
	QJsonArray path = T3CTCOffice::searchPathForAuthority("R_A_3", "R_B_5", &MODU_ARGS);
	T3TrainModel::createNewTrain("TRA1", path, &MODU_ARGS);
	T3TrackModel::placeTrain("TRA1", "R_A_3", true, &MODU_ARGS);

//	path = T3CTCOffice::searchPathForAuthority("R_B_5", "R_A_3", &MODU_ARGS);
//	T3TrainModel::createNewTrain("TRA2", path, &MODU_ARGS);
//	T3TrackModel::placeTrain("TRA2", "R_B_5", true, &MODU_ARGS);
	//QProcess::startDetached("explorer " + filepathString);
	T3CTCOffice::toggleConnection(true, &MODU_ARGS);
	//localFileEnabled = true;
	//db_push();
	//localFileEnabled = false;
	//ctc_getPossiblePaths(QJsonArray{QString("1234"), QString("R_C_8"), QString("R_B_6"), QString("12:34")});
}

//====================================================
//=====================FIREBASE DB====================
//=====================网络在线数据库===================
//====================================================

inline void T3Database::db_toggle(bool enableFirebase, bool enableFile) {
	firebaseEnabled = enableFirebase;
	localFileEnabled = enableFile;
}

inline void T3Database::db_push(unsigned int selector_in) {
	for(uint8_t i = 0; i < DB_MAPPER.size(); ++i) {
		if((static_cast<uint8_t>(selector_in) & (1 << i)) == 0) continue;
		Q_EMIT (this->*std::get<2>(DB_MAPPER.at(i)))();
	}
}

inline void T3Database::db_pull(unsigned int selector_in) {
	for(uint8_t i = 0; i < DB_MAPPER.size(); ++i) {
		if((static_cast<uint8_t>(selector_in) & (1 << i)) == 0) continue;
		const QString currJsonTitleToPush = std::get<0>(DB_MAPPER.at(i));
		QJsonArray* currJsonValToPush = std::get<1>(DB_MAPPER.at(i));
		const QByteArray serializedObj = QJsonDocument(*currJsonValToPush).toJson();
		QFile fileIO(filepathString + QString("/%1.json").arg(currJsonTitleToPush));
		if(fileIO.open(QFile::WriteOnly)) {
			fileIO.write(serializedObj);
			fileIO.close();
			QScopedPointer<QProcess> proc(new QProcess(this));
			proc->start(INITIAL_LINE_CSV_DIR + QString("JSONedit.exe ") + filepathString + QString("/%1.json").arg(currJsonTitleToPush));
			while(!proc->waitForFinished(-1)) {}
		} else
			throw std::exception(fileIO.errorString().toUtf8());
		if(fileIO.open(QFile::ReadOnly)) {
			QJsonArray newlyRead = QJsonDocument::fromJson(fileIO.readAll()).array();
			if(newlyRead.size() != (*currJsonValToPush).size())
				qFatal("T3Database::db_pull() -> Json size mismatch");
			fileIO.close();
			currJsonValToPush->operator=(newlyRead);
			qDebug() << currJsonTitleToPush << " written";
		} else
			throw std::exception(fileIO.errorString().toUtf8());
		db_push(selector_in);
	}
}


//====================================================
//=====================CTC-OFFICE=====================
//=====================中央调度中心=====================
//====================================================
inline QJsonArray T3Database::ctc_getPossiblePathsFromMetaInfo(const QJsonArray dispatchMetaInfo) {
	return T3CTCOffice::searchPathsFromMetaInfo(dispatchMetaInfo, &MODU_ARGS);
}

inline QJsonArray T3Database::ctc_getPossiblePathsFromCsv(const QString filePath, const QJsonArray dispatchMetaInfo) {
	return T3CTCOffice::searchPathsFromCsv(filePath, &stationToBlockIdMap, dispatchMetaInfo, &MODU_ARGS);
}

inline void T3Database::ctc_enqueueDispatchRequest(const QJsonArray dispatchMetaInfo, const QJsonArray path) {
	T3CTCOffice::enqueueDispatchRequest(&dispatchQueue, dispatchMetaInfo, path);
	Q_EMIT onDispatchQueueChanged();
	db_push(DispatchQueue);
}

inline void T3Database::ctc_discardDispatchRequest(const int index) {
	T3CTCOffice::discardDispatchRequest(index, &dispatchQueue);
	Q_EMIT onDispatchQueueChanged();
	db_push(DispatchQueue);
}

inline QJsonArray T3Database::ctc_readPlcInputFromMetaInfo(const QString blockId) {
	return T3CTCOffice::readPlcInputToMetaInfo(blockId, &MODU_ARGS);
}

inline void T3Database::ctc_writeToPlcInputFromMetaInfo(const QString blockId, const QJsonArray metaInfo) {
	T3CTCOffice::writeToPlcInputFromMetaInfo(blockId, metaInfo, &MODU_ARGS);
}




inline QJsonArray T3Database::ctc_searchPathForAuthority(const QString originBlock, const QString destiBlock) {
	return T3CTCOffice::searchPathForAuthority(originBlock, destiBlock, &MODU_ARGS);
}

inline void T3Database::ctc_toggleConnection(bool newConnectionState) {
	T3CTCOffice::toggleConnection(newConnectionState, &MODU_ARGS);
	db_push(TrackVariablesObjects);
}

/**
 * @brief T3Database::ctc_iterate
 * 检查当前时间，并且判断发车队列中有没有需要发车的（当前时间大于计划时间)
 */
inline void T3Database::ctc_iterate() {
	//
}

//====================================================
//==================TRACK-CONTROLLER==================
//=====================铁路铁轨控制=====================
//====================================================




inline void T3Database::kc_addPlcScriptFromCsv(const QString filePath) {
	T3TrackController::addPlcScriptFromCsv(filePath, &plcRuntime, &plcFunction);
}

inline QJsonArray T3Database::kc_readPlcToMetaInfo(const QString blockId) {
	return T3TrackController::readPlcToMetaInfo(blockId, &MODU_ARGS);
}

inline void T3Database::kc_writePlcFromMetaInfo(const QString blockId, const QJsonArray metaInfo) {
	T3TrackController::writePlcFromMetaInfo(blockId, metaInfo, &MODU_ARGS);
	db_push(TrackVariablesObjects);
}

inline QJsonArray T3Database::kc_getAllPlcBinaries(const QString blockId) {
	return T3TrackController::getAllPlcBinaries(blockId, &MODU_ARGS);
}

inline void T3Database::kc_processPlc(const QString blockId) {
	T3TrackController::processPlc(blockId, &plcRuntime, &plcFunction, &MODU_ARGS);
	db_push(TrackVariablesObjects);
}

inline void T3Database::kc_iterate() {
	T3TrackController::iterate(&MODU_ARGS, &plcRuntime, &plcFunction);
	db_push(TrackVariablesObjects);
}

inline QJsonArray T3Database::km_getDisplayStrings(const QString blockId) {
	return T3TrackModel::getDisplayStrings(blockId, &MODU_ARGS);
}

//====================================================
//=====================TRACK-MODEL====================
//=====================铁路铁轨模型=====================
//====================================================


inline void T3Database::km_addTrackFromCsv(const QString filePath) {
	T3TrackModel::addTrackFromCsv(filePath, &stationToBlockIdMap, &MODU_ARGS);
	Q_EMIT onTrackConstantsObjectsChanged();
	Q_EMIT onTrackVariablesObjectsChanged();
	Q_EMIT onStationToBlockIdMapChanged();
	db_push(TrackConstantsObjects | TrackVariablesObjects);
}

inline void T3Database::km_setIOFromMetaInfo(const QString blockId, const QJsonArray metaInfo) {
	T3TrackModel::setIOMetaInfo(blockId, metaInfo, &MODU_ARGS);
	db_push(TrackVariablesObjects);
}

inline QJsonArray T3Database::km_getIOFromMetaInfo(const QString blockId) {
	return T3TrackModel::getIOMetaInfo(blockId, &MODU_ARGS);
}

inline QJsonArray T3Database::km_getAllTrackIds() {
	return T3TrackModel::getAllTrackIds(&MODU_ARGS);
}

inline void T3Database::km_iterate() {

}

inline QJsonArray T3Database::nm_getAllTrainIds() {
	return T3TrainModel::getAllTrainIds(&MODU_ARGS);
}



//====================================================
//=====================TRAIN-MODEL====================
//=====================铁道火车模型=====================
//====================================================



inline void T3Database::nm_removeTrain(const QString trainId) {
	T3TrainModel::removeTrain(trainId, &MODU_ARGS);
	Q_EMIT onTrainObjectsChanged();
	db_push(TrainObjects);
	Q_INVOKABLE void removeTrainFromCtc(const QString trainId);
}

inline void T3Database::nm_setFailureOrBrake(const QString trainId, const int index, const bool value) {
	T3TrainModel::setFailureOrBrake(trainId, index, value, &MODU_ARGS);
	db_push(TrainObjects);
}

inline QJsonArray T3Database::nm_getStringsFromMetaInfo(const QString trainId) {
	return T3TrainModel::getStringsFromMetaInfo(trainId, &MODU_ARGS);
}

inline void T3Database::nm_iterate() {

}



//====================================================
//==================TRAIN-CONTROLLER==================
//=====================铁路火车控制=====================
//====================================================

inline void T3Database::nc_iterate() {

}




inline void T3Database::nc_setKpAndKi(const QString trainId, float kp, float ki) {
	T3TrainController::setKpAndKi(trainId, kp, ki, &MODU_ARGS);
	db_push(TrainObjects);
}

inline void T3Database::nc_setCtrlParams(const QString trainId, QJsonArray metaInfo) {
	T3TrainController::setCtrlParams(trainId, metaInfo, &MODU_ARGS);
	db_push(TrainObjects);
}

inline QJsonArray T3Database::nc_getMetaInfo(const QString trainId) {
	return T3TrainController::getMetaInfo(trainId, &MODU_ARGS);
}

//====================================================
//=====================MICELLANOUS====================
//=========================其他========================
//====================================================

inline void T3Database::timerEvent(QTimerEvent * event) {
	Q_UNUSED(event);
	if(this->timerRunning == false) return;
	//update time
	this->currentTime = this->currentTime.addMSecs(100 * static_cast<int>(timerRate));
	Q_EMIT onCurrentTimeChanged();
	if(!busy) nextClockCycle();

}

inline void T3Database::toggleTimer(bool newTimerState) {
	this->timerRunning = newTimerState;
}

inline QString T3Database::getCurrentTime() {
	return this->currentTime.toString("HH:mm:ss");
}

inline void T3Database::setTimerRate(int rate) {
	this->timerRate = static_cast<uint8_t>(rate);
}

inline void T3Database::nextClockCycle() {
	//===========================CTC OFFICE===============================
	busy = true;
//	//把所有比当前时间小的请求全部取出
	QList<QJsonObject> requestsReady = T3CTCOffice::popFromDispatchQueueAtTime(&(this->dispatchQueue), currentTime);
	//把每一个request都dispatch出去

	//火车当前会占用一些区块,更新train model上的occupancy plc output
	T3TrainModel::createNewTrainFromDispatchRequests(&requestsReady, &this->MODU_ARGS);
	T3TrackModel::placeTrainFromDispatchRequest(&requestsReady, &this->MODU_ARGS);
	T3TrackModel::updateTrainOccupancyOnAllBlocks(&this->MODU_ARGS);
	T3TrackController::processAllPlc(&this->plcRuntime, &this->plcFunction, &this->MODU_ARGS);
//	T3TrackModel::updateTrainPositionOnAllBlocks(&this->MODU_ARGS);

//	//===========================TRACK CONTROLLER===============================

//	//--因为有了新的authority和train occupancy，在每一个block上运行plc程序，判定新的状态


//	//===========================TRAIN MODEL & TRACK MODEL===============================

//	//--利用TRAIN MODEL的现有速度，计算位移，并且在TRACK MODEL上更新
	T3TrainModel::createNewTrainFromDispatchRequests(&requestsReady, &this->MODU_ARGS);
//	T3TrainModel::embarkAndDisembarkPassangerOnAllTrains(&this->MODU_ARGS);
	T3TrainModel::updateTrainVelocityOnAllTrains(&this->MODU_ARGS);

//	T3TrainController::updateControlSystemsOnAllTrains(currentTime, &this->MODU_ARGS);
	T3TrainController::updatePiOnAllTrains(&this->MODU_ARGS);

	db_push();
	busy = false;
}

#endif // T3Database_H

