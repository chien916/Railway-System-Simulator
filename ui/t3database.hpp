#ifndef T3Database_H
#define T3Database_H

#include <QtCore>
#include <QtQml>
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

	QJsonArray trainObjects;//do not modify directly after initialization. use the setTrainProperty() slot instead!
	Q_PROPERTY(QJsonArray trainObjects_QML MEMBER trainObjects NOTIFY onTrainObjectsChanged)

	QJsonArray dispatchQueue;//当前等待被发车的队列
	Q_PROPERTY(QJsonArray dispatchQueue_QML MEMBER dispatchQueue NOTIFY onDispatchQueueChanged)

	QJsonObject stationToBlockIdMap;//对于一个站对应多个铁轨块的哈希表
	Q_PROPERTY(QJsonObject stationToBlockIdMap_QML MEMBER stationToBlockIdMap NOTIFY onStationToBlockIdMapChanged)

  private:

	bool firebaseEnabled = false;
	QString firebaseRootUrlString = "https://sprn2023-ece1140-default-rtdb.firebaseio.com/";
	QNetworkAccessManager networkAccessManager;

	using DB_MAPPER_T = QVarLengthArray<std::tuple<QString, QJsonArray*, void(T3Database::*)(void)>, 4>;
	const DB_MAPPER_T DB_MAPPER = {
		{"trackConstantsObjects", &trackConstantsObjects, &T3Database::onTrackConstantsObjectsChanged}
		, {"trackVariablesObjects", &trackVariablesObjects, &T3Database::onTrackVariablesObjectsChanged}
		, {"trainObjects", &trainObjects, &T3Database::onTrainObjectsChanged}
		, {"dispatchQueue", &dispatchQueue, &T3Database::onDispatchQueueChanged}
	};
	using GET_PROP_T = std::function<QVariant(const QString, const QString, const QJsonArray*)>;
	using SET_PROP_T = std::function<void(const QString, const QString, QJsonArray*, const QVariant)>;
	const GET_PROP_T GET_PROP = [](const QString blockId, const QString prop, const QJsonArray* objects) {
		for(qsizetype i = 0; i < objects->size(); ++i) { //for every line
			QJsonObject currObj = objects->at(i).toObject();
			if(currObj.contains("id") && currObj.value("id") == blockId) {//handles train object
				return currObj.value(blockId).toObject().value(prop).toVariant();
				currObj = currObj.value("blocksMap").toObject();
			}
			if(currObj.contains("blocksMap")) { //handles track constants objects
				currObj = currObj.value("blocksMap").toObject();
			}
			if(currObj.contains(blockId)) {
				return currObj.value(blockId).toObject().value(prop).toVariant();
			}
		}
		throw std::exception();
		qFatal(QString("T3Database::GET_PROP(%1,%2) failed").arg(blockId).arg(prop).toLocal8Bit());
		return QVariant();
	};
	const SET_PROP_T SET_PROP = [](const QString blockId, const QString prop, QJsonArray* objects, const QVariant val) {
		for(qsizetype i = 0; i < objects->size(); ++i) { //for every line
			QJsonObject currObj = objects->at(i).toObject();
			if(currObj.contains("id") && currObj.value("id") == blockId) {//handles train object
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
		throw std::exception();
		qFatal(QString("T3Database::SET_PROP(%1,%2) failed").arg(blockId).arg(prop).toLocal8Bit());
	};

	const std::tuple<const std::function<QVariant(const QString, const QString, const QJsonArray*)>*
	, const std::function<void(const QString, const QString, QJsonArray*, const QVariant)>*
	,  QJsonArray*, QJsonArray*, QJsonArray*> MODU_ARGS  = {
		&GET_PROP, &SET_PROP, &trackConstantsObjects, &trackVariablesObjects, &trackVariablesObjects
	};

  public:
	Q_INVOKABLE void db_toggleFirebase(bool enable);
	Q_INVOKABLE void db_pushToFirebase(uint8_t selector = 0b11111111);
	Q_INVOKABLE void db_pullFromFirebase(uint8_t selector = 0b11111111);
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
  signals:


  private:
	void ctc_iterate();
	//====================================================
	//==================TRACK-CONTROLLER==================
	//=====================铁路铁轨控制=====================
	//====================================================
  public:
	Q_INVOKABLE QJsonArray kc_collectPlcInput(const QString blockId);
	Q_INVOKABLE void kc_addPlcScriptFromCsv(const QString filePath);
	Q_INVOKABLE QJsonArray kc_generatePlcOutput(QJsonArray plcInput);
	Q_INVOKABLE void kc_writePlcOutput(const QString blockId, QJsonArray plcOutput);

  signals:
  private:
	QJSEngine plcRuntime;
	QJSValue plcFunction;
	void kc_iterate();
	//====================================================
	//=====================TRACK-MODEL====================
	//=====================铁路铁轨模型=====================
	//====================================================
  public:

	Q_INVOKABLE void km_addTrackFromCsv(const QString filePath);

  signals:

  private:
	void km_iterate();

	//====================================================
	//=====================TRAIN-MODEL====================
	//=====================铁道火车模型=====================
	//====================================================
  public:



	Q_INVOKABLE void nm_removeTrain(const QString trainId);
  signals:

  private:
	void nm_iterate();
	//====================================================
	//==================TRAIN-CONTROLLER==================
	//=====================铁道控制模型=====================
	//====================================================
  public:
  signals:
  private:
	void nc_iterate();
	//====================================================
	//=====================MICELLANOUS====================
	//=========================其他========================
	//====================================================
  private:

	QTime currentTime;
	uint8_t timerRate = 1;
  protected:
	void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
  public:
	Q_PROPERTY(QString currentTime_QML READ getCurrentTime NOTIFY onCurrentTimeChanged)
	T3Database(QObject *parent = nullptr);

	Q_INVOKABLE QString getCurrentTime();
	Q_INVOKABLE void setTimerRate(int rate);
  signals:
	void onCurrentTimeChanged();

};





inline T3Database::T3Database(QObject * parent) : QObject(parent) {
	//	//TESTING FOR RAIL LOADING
	this->currentTime = QTime::currentTime();
	km_addTrackFromCsv("C:/Users/YIQ25/Documents/Academics/ECE1140/Resources/T3RedLine.csv");
	km_addTrackFromCsv("C:/Users/YIQ25/Documents/Academics/ECE1140/Resources/T3GreenLine.csv");
	km_addTrackFromCsv("C:/Users/YIQ25/Documents/Academics/ECE1140/Resources/T3BlueLine.csv");
	kc_addPlcScriptFromCsv(":/T3KCPlcScript.js");
	db_pushToFirebase();
	//ctc_getPossiblePaths(QJsonArray{QString("1234"), QString("R_C_8"), QString("R_B_6"), QString("12:34")});
}

//====================================================
//=====================FIREBASE DB====================
//=====================网络在线数据库===================
//====================================================

inline void T3Database::db_toggleFirebase(bool enableFirebase) {
	if(enableFirebase == this->firebaseEnabled) return;
	if(enableFirebase) db_pushToFirebase();
	firebaseEnabled = enableFirebase;
}

inline void T3Database::db_pushToFirebase(uint8_t selector) {
	if(!firebaseEnabled) return;
	for(uint8_t i = 0; i < DB_MAPPER.size(); ++i) {
		if((selector & (1 << i)) == 0) continue;
		const QString currJsonTitleToPush = std::get<0>(DB_MAPPER.at(i));
		const QJsonArray* currJsonValToPush = std::get<1>(DB_MAPPER.at(i));
		const QByteArray serializedObj = QJsonDocument(*currJsonValToPush).toJson();
		QNetworkRequest networkRequest(QUrl(firebaseRootUrlString + QString("/%1.json").arg(currJsonTitleToPush)));
		networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
		QNetworkReply* networkReply = networkAccessManager.put(networkRequest, serializedObj);
		if(networkReply->error() != QNetworkReply::NetworkError::NoError)
			qInfo() << QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(networkReply->error());
	}
}

inline void T3Database::db_pullFromFirebase(uint8_t selector) {
	if(!firebaseEnabled) return;
	for(uint8_t i = 0; i < DB_MAPPER.size(); ++i) {
		if((selector & (1 << i)) == 0) continue;
		const QString currJsonTitleToPull = std::get<0>(DB_MAPPER.at(i));
		QJsonArray* currJsonValToPull =  std::get<1>(DB_MAPPER.at(i));
		QNetworkRequest networkRequest(QUrl(firebaseRootUrlString + QString("/%1.json").arg(currJsonTitleToPull)));
		//networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
		QNetworkReply* networkReply = networkAccessManager.get(networkRequest);
		if(networkReply->error() != QNetworkReply::NetworkError::NoError)
			qInfo() << QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(networkReply->error());
		connect(networkReply, &QNetworkReply::readyRead, [this, networkReply, &currJsonValToPull, i]() {
			QByteArray serializedObjs = networkReply->readAll();
			*currJsonValToPull = QJsonDocument::fromJson(serializedObjs).array();
			Q_EMIT (this->*std::get<2>(DB_MAPPER.at(i)))();
		});
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
	db_pushToFirebase(DispatchQueue);
}

inline void T3Database::ctc_discardDispatchRequest(const int index) {
	T3CTCOffice::discardDispatchRequest(index, &dispatchQueue);
	Q_EMIT onDispatchQueueChanged();
	db_pushToFirebase(DispatchQueue);
}

inline QJsonArray T3Database::ctc_readPlcInputFromMetaInfo(const QString blockId) {
	return T3CTCOffice::readPlcInputToMetaInfo(blockId, &MODU_ARGS);
}


inline void T3Database::ctc_writeToPlcInputFromMetaInfo(const QString blockId, const QJsonArray metaInfo) {
	T3CTCOffice::writeToPlcInputFromMetaInfo(blockId, &metaInfo, &MODU_ARGS);
	Q_EMIT onTrackVariablesObjectsChanged();
	db_pushToFirebase(TrackVariablesObjects);
}

inline QJsonArray T3Database::ctc_searchPathForAuthority(const QString originBlock, const QString destiBlock) {
	return T3CTCOffice::searchPathForAuthority(originBlock, destiBlock, &MODU_ARGS);
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



inline QJsonArray T3Database::kc_collectPlcInput(const QString blockId) {
	return T3TrackController::collectPlcInput(blockId, &trackVariablesObjects);
}

inline void T3Database::kc_addPlcScriptFromCsv(const QString filePath) {
	T3TrackController::addPlcScriptFromCsv(filePath, &plcRuntime, &plcFunction);
}

inline QJsonArray T3Database::kc_generatePlcOutput(QJsonArray plcInput) {
	return T3TrackController::generatePlcOutput(plcInput, &plcRuntime, &plcFunction);
}

inline void T3Database::kc_writePlcOutput(const QString blockId, QJsonArray plcOutput) {
	T3TrackController::writePlcOutput(blockId, &plcOutput, &trackVariablesObjects);
	Q_EMIT onTrackVariablesObjectsChanged();
	db_pushToFirebase(TrackVariablesObjects);
}


inline void T3Database::kc_iterate() {

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
	db_pushToFirebase(TrackConstantsObjects | TrackVariablesObjects);
}

inline void T3Database::km_iterate() {

}



//====================================================
//=====================TRAIN-MODEL====================
//=====================铁道火车模型=====================
//====================================================



inline void T3Database::nm_removeTrain(const QString trainId) {
	T3TrainModel::removeTrain(trainId, &trainObjects);
	Q_EMIT onTrainObjectsChanged();
	db_pushToFirebase(TrainObjects);
	Q_INVOKABLE void removeTrainFromCtc(const QString trainId);
}

inline void T3Database::nm_iterate() {

}


//====================================================
//==================TRAIN-CONTROLLER==================
//=====================铁路火车控制=====================
//====================================================

inline void T3Database::nc_iterate() {

}



//====================================================
//=====================MICELLANOUS====================
//=========================其他========================
//====================================================

inline void T3Database::timerEvent(QTimerEvent *event) {
	Q_UNUSED(event);
	//update time
	this->currentTime = this->currentTime.addMSecs(100 * static_cast<int>(timerRate));
	Q_EMIT onCurrentTimeChanged();
	//check queue
	ctc_iterate();

}

inline QString T3Database::getCurrentTime() {
	return this->currentTime.toString("HH:mm:ss");
}

inline void T3Database::setTimerRate(int rate) {
	this->timerRate = static_cast<uint8_t>(rate);
}

#endif // T3Database_H

