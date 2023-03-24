#ifndef T3Database_H
#define T3Database_H

#include <QtCore>
#include <QtQml>
#include "t3trainmodel.hpp"
#include "t3trackmodel.hpp"
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
  private:
	bool firebaseEnabled = true;
	QString firebaseRootUrlString = "https://sprn2023-ece1140-default-rtdb.firebaseio.com/";
	QNetworkAccessManager networkAccessManager;

	const QList<QPair<QString, QJsonArray*>> firebaseMapper{
		{"trackConstantsObjects", &trackConstantsObjects}
		, {"trackVariablesObjects", &trackVariablesObjects}
		, {"trainObjects", &trainObjects}
		, {"dispatchQueue", &dispatchQueue}
	};

  public:
	Q_INVOKABLE void db_toggleFirebase(bool enable);
	Q_INVOKABLE void db_pushToFirebase(uint8_t selector = 0b11111111);
	Q_INVOKABLE void db_pullFromFirebase(uint8_t selector = 0b11111111);
  private:

	//====================================================
	//=====================CTC-OFFICE=====================
	//=====================中央调度中心=====================
	//====================================================
  public:
	QJsonArray dispatchQueue;//当前等待被发车的队列
	Q_PROPERTY(QJsonArray dispatchQueue_QML MEMBER dispatchQueue NOTIFY onDispatchQueueChanged)

	QJsonObject stationToBlockIdMap;//对于一个站对应多个铁轨块的哈希表
	Q_PROPERTY(QJsonObject stationToBlockIdMap_QML MEMBER stationToBlockIdMap NOTIFY onStationToBlockIdMapChanged)

	Q_INVOKABLE QJsonArray ctc_getPossiblePathsFromMetaInfo(const QJsonArray dispatchMetaInfo);
	Q_INVOKABLE QJsonArray ctc_getPossiblePathsFromCsv(const QString filePath, const QJsonArray dispatchMetaInfo);
	Q_INVOKABLE void ctc_enqueueDispatchRequest(const QJsonArray dispatchMetaInfo, const QJsonArray path);
	Q_INVOKABLE void ctc_discardDispatchRequest(const int index);
  signals:
	void onDispatchQueueChanged();
	void onStationToBlockIdMapChanged();
  private:
	void ctc_iterate();
	//====================================================
	//=====================TRACK-MODEL====================
	//=====================铁路铁轨模型=====================
	//====================================================
  public:
	QJsonArray trackConstantsObjects;//do not modify directly after initialization. use the setTrackProperty() slot instead!
	Q_PROPERTY(QJsonArray trackConstantsObjects_QML MEMBER trackConstantsObjects NOTIFY onTrackConstantsObjectsChanged)

	QJsonArray trackVariablesObjects;//do not modify directly after initialization. use the setTrackProperty() slot instead!
	Q_PROPERTY(QJsonArray trackVariablesObjects_QML MEMBER trackVariablesObjects NOTIFY onTrackVariablesObjectsChanged)

	Q_INVOKABLE void km_addTrackFromCsv(const QString filePath);
	Q_INVOKABLE void km_setTrackProperty(QString blockId, int trackProperty, QVariant value);
	Q_INVOKABLE QVariant km_getTrackProperty(QString blockId, int trackProperty);

  signals:
	void onTrackConstantsObjectsChanged();
	void onTrackVariablesObjectsChanged();
  private:
	void km_iterate();
	//====================================================
	//==================TRACK-CONTROLLER==================
	//=====================铁路铁轨控制=====================
	//====================================================
  public:
  signals:
  private:
	void kc_iterate();
	//====================================================
	//=====================TRAIN-MODEL====================
	//=====================铁道火车模型=====================
	//====================================================
  public:
	QJsonArray trainObjects;//do not modify directly after initialization. use the setTrainProperty() slot instead!
	Q_PROPERTY(QJsonArray trainObjects_QML MEMBER trainObjects NOTIFY onTrainObjectsChanged)

	Q_INVOKABLE void nm_setTrainProperty(QString trainId, int trainProperty, QVariant value);
	Q_INVOKABLE QVariant nm_getTrainProperty(QString trainId, int trainProperty);
	Q_INVOKABLE void nm_removeTrain(const QString trainId);
  signals:
	void onTrainObjectsChanged();
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
	const QHash<T3TrackModel::TrackProperty, QPair<QString, int>> trackPropertiesMetaDataMap
			= {std::make_pair(T3TrackModel::TrackProperty::CommandedSpeed, QPair<QString, int>(QString("commandedSpeed"), qMetaTypeId<float>()))
			   , std::make_pair(T3TrackModel::TrackProperty::Authority, QPair<QString, int>(QString("authority"), qMetaTypeId<bool>()))
			   , std::make_pair(T3TrackModel::TrackProperty::SwitchPostion, QPair<QString, int>(QString("switchPosition"), qMetaTypeId<bool>()))
			   , std::make_pair(T3TrackModel::TrackProperty::ForwardLight, QPair<QString, int>(QString("forwardLight"), qMetaTypeId<QString>()))
			   , std::make_pair(T3TrackModel::TrackProperty::ReversedLight, QPair<QString, int>(QString("backwardLight"), qMetaTypeId<QString>()))
			   , std::make_pair(T3TrackModel::TrackProperty::CrossingPosition, QPair<QString, int>(QString("crossingPosition"), qMetaTypeId<bool>()))
			   , std::make_pair(T3TrackModel::TrackProperty::TrainOnBlock, QPair<QString, int>(QString("trainOnBlock"), qMetaTypeId<QString>()))
			   , std::make_pair(T3TrackModel::TrackProperty::Failure, QPair<QString, int>(QString("failure"), qMetaTypeId<QString>()))
			   , std::make_pair(T3TrackModel::TrackProperty::Heaters, QPair<QString, int>(QString("heaters"), qMetaTypeId<QString>()))
			   , std::make_pair(T3TrackModel::TrackProperty::PeopleOnStation, QPair<QString, int>(QString("peopleOnStation"), qMetaTypeId<uint16_t>()))
			   , std::make_pair(T3TrackModel::TrackProperty::MaintainanceMode, QPair<QString, int>(QString("maintainanceMode"), qMetaTypeId<bool>()))
			  };

	const QHash<T3TrainModel::TrainProperty, QPair<QString, int>> trainPropertiesMetaDataMap
			= {std::make_pair(T3TrainModel::TrainProperty::Id, QPair<QString, int>(QString("id"), qMetaTypeId<QString>()))
			   , std::make_pair(T3TrainModel::TrainProperty::Length, QPair<QString, int>(QString("length"), qMetaTypeId<float>()))
			   , std::make_pair(T3TrainModel::TrainProperty::Height, QPair<QString, int>(QString("height"), qMetaTypeId<float>()))
			   , std::make_pair(T3TrainModel::TrainProperty::Width, QPair<QString, int>(QString("width"), qMetaTypeId<float>()))
			   , std::make_pair(T3TrainModel::TrainProperty::Mass, QPair<QString, int>(QString("mass"), qMetaTypeId<float>()))
			   , std::make_pair(T3TrainModel::TrainProperty::Acceleration, QPair<QString, int>(QString("acceleration"), qMetaTypeId<float>()))
			   , std::make_pair(T3TrainModel::TrainProperty::Velocity, QPair<QString, int>(QString("velocity"), qMetaTypeId<float>()))
			   , std::make_pair(T3TrainModel::TrainProperty::CrewCount, QPair<QString, int>(QString("crewCount"), qMetaTypeId<uint16_t>()))
			   , std::make_pair(T3TrainModel::TrainProperty::PassangerCount, QPair<QString, int>(QString("passangerCount"), qMetaTypeId<uint16_t>()))
			   , std::make_pair(T3TrainModel::TrainProperty::Aircon, QPair<QString, int>(QString("aircon"), qMetaTypeId<QString>()))
			   , std::make_pair(T3TrainModel::TrainProperty::ExteriorLights, QPair<QString, int>(QString("exteriorLights"), qMetaTypeId<bool>()))
			   , std::make_pair(T3TrainModel::TrainProperty::InteriorLights, QPair<QString, int>(QString("interiorLights"), qMetaTypeId<bool>()))
			   , std::make_pair(T3TrainModel::TrainProperty::LeftDoors, QPair<QString, int>(QString("leftDoors"), qMetaTypeId<bool>()))
			   , std::make_pair(T3TrainModel::TrainProperty::RightDoors, QPair<QString, int>(QString("rightDoors"), qMetaTypeId<bool>()))
			   , std::make_pair(T3TrainModel::TrainProperty::Brakes, QPair<QString, int>(QString("brakes"), qMetaTypeId<QString>()))
			   , std::make_pair(T3TrainModel::TrainProperty::Failures, QPair<QString, int>(QString("failures"), qMetaTypeId<QString>()))
			   , std::make_pair(T3TrainModel::TrainProperty::Pid_Prev_e, QPair<QString, int>(QString("pid_prev_e"), qMetaTypeId<float>()))
			   , std::make_pair(T3TrainModel::TrainProperty::Pid_Prev_y, QPair<QString, int>(QString("pid_prev_y"), qMetaTypeId<float>()))
			   , std::make_pair(T3TrainModel::TrainProperty::Pid_Sum_e, QPair<QString, int>(QString("pid_sum_e"), qMetaTypeId<float>()))
			   , std::make_pair(T3TrainModel::TrainProperty::Pid_Dt, QPair<QString, int>(QString("pid_dt"), qMetaTypeId<float>()))
			   , std::make_pair(T3TrainModel::TrainProperty::Pid_R, QPair<QString, int>(QString("pid_r"), qMetaTypeId<float>()))
			   , std::make_pair(T3TrainModel::TrainProperty::Pid_Ki, QPair<QString, int>(QString("pid_ki"), qMetaTypeId<float>()))
			   , std::make_pair(T3TrainModel::TrainProperty::Pid_Kp, QPair<QString, int>(QString("pid_kp"), qMetaTypeId<float>()))
			   , std::make_pair(T3TrainModel::TrainProperty::Pid_Kd, QPair<QString, int>(QString("pid_kd"), qMetaTypeId<float>()))
			   , std::make_pair(T3TrainModel::TrainProperty::Path, QPair<QString, int>(QString("path"), qMetaTypeId<QJsonArray>()))
			  };

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
	for(uint8_t i = 0; i < firebaseMapper.size(); ++i) {
		if((selector & (1 << i)) == 0) continue;
		const QString currJsonTitleToPush = firebaseMapper.at(i).first;
		const QJsonArray* currJsonValToPush = firebaseMapper.at(i).second;
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
	for(uint8_t i = 0; i < firebaseMapper.size(); ++i) {
		if((selector & (1 << i)) == 0) continue;
		const QString currJsonTitleToPull = firebaseMapper.at(i).first;
		QJsonArray* currJsonValToPull = firebaseMapper.at(i).second;
		QNetworkRequest networkRequest(QUrl(firebaseRootUrlString + QString("/%1.json").arg(currJsonTitleToPull)));
		//networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
		QNetworkReply* networkReply = networkAccessManager.get(networkRequest);
		if(networkReply->error() != QNetworkReply::NetworkError::NoError)
			qInfo() << QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(networkReply->error());
		connect(networkReply, &QNetworkReply::readyRead, [this, networkReply, &currJsonValToPull]() {
			QByteArray serializedObjs = networkReply->readAll();
			*currJsonValToPull = QJsonDocument::fromJson(serializedObjs).array();
			Q_EMIT onTrackVariablesObjectsChanged();
		});
	}
}

//====================================================
//=====================CTC-OFFICE=====================
//=====================中央调度中心=====================
//====================================================
inline QJsonArray T3Database::ctc_getPossiblePathsFromMetaInfo(const QJsonArray dispatchMetaInfo) {
	return T3CTCOffice::searchPathsFromMetaInfo(dispatchMetaInfo, &trackConstantsObjects);
}

inline QJsonArray T3Database::ctc_getPossiblePathsFromCsv(const QString filePath, const QJsonArray dispatchMetaInfo) {
	return T3CTCOffice::searchPathsFromCsv(filePath, &stationToBlockIdMap, dispatchMetaInfo, &trackConstantsObjects);
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

/**
 * @brief T3Database::ctc_iterate
 * 检查当前时间，并且判断发车队列中有没有需要发车的（当前时间大于计划时间)
 */
inline void T3Database::ctc_iterate() {
	//
	const QList<QJsonObject> poppedRequests = T3CTCOffice::popFromDispatchQueueAtTime(&dispatchQueue, currentTime);
	for(const QJsonObject& currRequest : poppedRequests) {
		QString trainId = currRequest.value("trainId").toString();
		QString origin = currRequest.value("origin").toString();
		QString destination = currRequest.value("destination").toString();
		QJsonArray path = currRequest.value("path").toArray();
		//determine if the train is moving forward or backward
		bool isForward = (path.size() > 1)
						 ? T3TrackModel::determineMovingDirection(origin, path.at(1).toString(), &trackConstantsObjects) == "FORWARD"
						 : true;
		QString trainOnBlockConcatStr = QString("%1_%2_0.0").arg(trainId).arg(isForward ? "F" : "R");
		//place train on track
		T3TrackModel::setTrackProperty(origin, T3TrackModel::TrainOnBlock, trainOnBlockConcatStr, &trackVariablesObjects, &trackPropertiesMetaDataMap);

		//add train to train object
		T3TrainModel::createNewTrain(trainId, path, &trainObjects);
	}
	Q_EMIT onTrackVariablesObjectsChanged();
	Q_EMIT onTrainObjectsChanged();
	Q_EMIT onDispatchQueueChanged();
	db_pushToFirebase(TrackVariablesObjects | TrainObjects | DispatchQueue);
}
//====================================================
//==================TRACK-CONTROLLER==================
//=====================铁路铁轨控制=====================
//====================================================

inline void T3Database::kc_iterate() {

}

//====================================================
//=====================TRACK-MODEL====================
//=====================铁路铁轨模型=====================
//====================================================

inline QVariant T3Database::km_getTrackProperty(QString blockId, int trackProperty) {
	if(trackProperty < 0 || trackProperty >= trackPropertiesMetaDataMap.size())
		qFatal("T3Database::km_getTrackProperty() -> trackProperty is invalid");
	return T3TrackModel::getTrackProperty(blockId, static_cast<T3TrackModel::TrackProperty>(trackProperty), &trackVariablesObjects, &trackPropertiesMetaDataMap);
}


inline void T3Database::km_setTrackProperty(QString blockId, int trackProperty, QVariant value) {
	if(trackProperty < 0 || trackProperty >= trackPropertiesMetaDataMap.size())
		qFatal("T3Database::km_setTrackProperty() -> trackProperty is invalid");
	T3TrackModel::setTrackProperty(blockId, static_cast<T3TrackModel::TrackProperty>(trackProperty), value, &trackVariablesObjects, &trackPropertiesMetaDataMap);
	Q_EMIT onTrackVariablesObjectsChanged();
	db_pushToFirebase(TrackVariablesObjects);
}


inline void T3Database::km_addTrackFromCsv(const QString filePath) {
	T3TrackModel::addTrackFromCsv(filePath, &trackConstantsObjects, &trackVariablesObjects, &stationToBlockIdMap);
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

inline QVariant T3Database::nm_getTrainProperty(QString trainId, int trainProperty) {
	if(trainProperty < 0 || trainProperty >= trainPropertiesMetaDataMap.size())
		qFatal("T3Database::nm_getTrainProperty() -> trainProperty is invalid");
	return T3TrainModel::getTrainProperty(trainId, static_cast<T3TrainModel::TrainProperty>(trainProperty), &trainObjects, &trainPropertiesMetaDataMap);
}

inline void T3Database::nm_setTrainProperty(QString trainId, int trainProperty, QVariant value) {
	if(trainProperty < 0 || trainProperty >= trainPropertiesMetaDataMap.size())
		qFatal("T3Database::km_setTrainProperty() -> trainProperty is invalid");
	T3TrainModel::setTrainProperty(trainId, static_cast<T3TrainModel::TrainProperty>(trainProperty), value, &trainObjects, &trainPropertiesMetaDataMap);
	Q_EMIT onTrainObjectsChanged();
	db_pushToFirebase(TrainObjects);
}


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

