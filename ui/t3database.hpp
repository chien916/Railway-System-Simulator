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
	//Context variables exposed to QML:

	QJsonArray trackConstantsObjects;//do not modify directly after initialization. use the setTrackProperty() slot instead!
	Q_PROPERTY(QJsonArray trackConstantsObjects_QML MEMBER trackConstantsObjects NOTIFY onTrackConstantsObjectsChanged)

	QJsonArray trackVariablesObjects;//do not modify directly after initialization. use the setTrackProperty() slot instead!
	Q_PROPERTY(QJsonArray trackVariablesObjects_QML MEMBER trackVariablesObjects NOTIFY onTrackVariablesObjectsChanged)

	QJsonArray trainObjects;//do not modify directly after initialization. use the setTrainProperty() slot instead!
	Q_PROPERTY(QJsonArray trainObjects_QML MEMBER trainObjects NOTIFY onTrainObjectsChanged)

	QJsonArray dispatchQueue;
	Q_PROPERTY(QJsonArray dispatchQueue_QML MEMBER dispatchQueue NOTIFY onDispatchQueueChanged)

	QJsonObject stationToBlockIdMap;
	Q_PROPERTY(QJsonObject stationToBlockIdMap_QML MEMBER stationToBlockIdMap NOTIFY onStationToBlockIdMapChanged)

	Q_PROPERTY(QString currentTime_QML READ getCurrentTime NOTIFY onCurrentTimeChanged)

  Q_SIGNALS:
	void onTrackConstantsObjectsChanged();
	void onTrackVariablesObjectsChanged();
	void onTrainObjectsChanged();
	void onCurrentTimeChanged();
	void onDispatchQueueChanged();
	void onStationToBlockIdMapChanged();
  public:
	T3Database(QObject *parent = nullptr);
	//Track properties definiations
	enum TrackProperty {
		CommandedSpeed = 0 //real/float
		, Authority = 1 //bool
		, SwitchPostion = 2 //bool
		, ForwardLight = 3 //string
		, ReversedLight = 4 // string
		, CrossingPosition = 5//bool
		, TrainOnBlock = 6 //string
		, Failure = 7 //string
		, Heaters = 8 //string
		, PeopleOnStation = 9 //int
		, MaintainanceMode = 10
	};
	Q_ENUM(TrackProperty);
	//Train properties definitions
	enum TrainProperty {
		Id = 0
		, Length = 1
		, Height = 2
		, Width = 3
		, Mass = 4
		, Acceleration = 5
		, Velocity = 6
		, CrewCount = 7
		, PassangerCount = 8
		, Aircon = 9
		, ExteriorLights = 10
		, InteriorLights = 11
		, LeftDoors = 12
		, RightDoors = 13
		, Brakes = 14
		, Failures = 15
		, Pid_Prev_e
		, Pid_Prev_y
		, Pid_Sum_e
		, Pid_Dt
		, Pid_R
		, Pid_Ki
		, Pid_Kp
		, Pid_Kd
	};
	Q_ENUM(TrainProperty);


  private:
	const QHash<T3Database::TrackProperty, QPair<QString, int>> trackPropertiesMetaDataMap
	= std::initializer_list<std::pair<T3Database::TrackProperty, QPair<QString, int>>> {
		std::make_pair(T3Database::TrackProperty::CommandedSpeed, QPair<QString, int>(QString("commandedSpeed"), qMetaTypeId<float>()))
		, std::make_pair(T3Database::TrackProperty::Authority, QPair<QString, int>(QString("authority"), qMetaTypeId<bool>()))
		, std::make_pair(T3Database::TrackProperty::SwitchPostion, QPair<QString, int>(QString("switchPosition"), qMetaTypeId<bool>()))
		, std::make_pair(T3Database::TrackProperty::ForwardLight, QPair<QString, int>(QString("forwardLight"), qMetaTypeId<QString>()))
		, std::make_pair(T3Database::TrackProperty::ReversedLight, QPair<QString, int>(QString("backwardLight"), qMetaTypeId<QString>()))
		, std::make_pair(T3Database::TrackProperty::CrossingPosition, QPair<QString, int>(QString("crossingPosition"), qMetaTypeId<bool>()))
		, std::make_pair(T3Database::TrackProperty::TrainOnBlock, QPair<QString, int>(QString("trainOnBlock"), qMetaTypeId<QString>()))
		, std::make_pair(T3Database::TrackProperty::Failure, QPair<QString, int>(QString("failure"), qMetaTypeId<QString>()))
		, std::make_pair(T3Database::TrackProperty::Heaters, QPair<QString, int>(QString("heaters"), qMetaTypeId<QString>()))
		, std::make_pair(T3Database::TrackProperty::PeopleOnStation, QPair<QString, int>(QString("peopleOnStation"), qMetaTypeId<uint16_t>()))
		, std::make_pair(T3Database::TrackProperty::MaintainanceMode, QPair<QString, int>(QString("maintainanceMode"), qMetaTypeId<bool>()))
	};

	const QHash<T3Database::TrainProperty, QPair<QString, int>> trainPropertiesMetaDataMap
	= std::initializer_list<std::pair<T3Database::TrainProperty, QPair<QString, int>>> {
		std::make_pair(T3Database::TrainProperty::Id, QPair<QString, int>(QString("id"), qMetaTypeId<QString>()))
		, std::make_pair(T3Database::TrainProperty::Length, QPair<QString, int>(QString("length"), qMetaTypeId<float>()))
		, std::make_pair(T3Database::TrainProperty::Height, QPair<QString, int>(QString("height"), qMetaTypeId<float>()))
		, std::make_pair(T3Database::TrainProperty::Width, QPair<QString, int>(QString("width"), qMetaTypeId<float>()))
		, std::make_pair(T3Database::TrainProperty::Mass, QPair<QString, int>(QString("mass"), qMetaTypeId<float>()))
		, std::make_pair(T3Database::TrainProperty::Acceleration, QPair<QString, int>(QString("acceleration"), qMetaTypeId<float>()))
		, std::make_pair(T3Database::TrainProperty::Velocity, QPair<QString, int>(QString("velocity"), qMetaTypeId<float>()))
		, std::make_pair(T3Database::TrainProperty::CrewCount, QPair<QString, int>(QString("crewCount"), qMetaTypeId<uint16_t>()))
		, std::make_pair(T3Database::TrainProperty::PassangerCount, QPair<QString, int>(QString("passangerCount"), qMetaTypeId<uint16_t>()))
		, std::make_pair(T3Database::TrainProperty::Aircon, QPair<QString, int>(QString("aircon"), qMetaTypeId<QString>()))
		, std::make_pair(T3Database::TrainProperty::ExteriorLights, QPair<QString, int>(QString("exteriorLights"), qMetaTypeId<bool>()))
		, std::make_pair(T3Database::TrainProperty::InteriorLights, QPair<QString, int>(QString("interiorLights"), qMetaTypeId<bool>()))
		, std::make_pair(T3Database::TrainProperty::LeftDoors, QPair<QString, int>(QString("leftDoors"), qMetaTypeId<bool>()))
		, std::make_pair(T3Database::TrainProperty::RightDoors, QPair<QString, int>(QString("rightDoors"), qMetaTypeId<bool>()))
		, std::make_pair(T3Database::TrainProperty::Brakes, QPair<QString, int>(QString("brakes"), qMetaTypeId<QString>()))
		, std::make_pair(T3Database::TrainProperty::Failures, QPair<QString, int>(QString("failures"), qMetaTypeId<QString>()))
		, std::make_pair(T3Database::TrainProperty::Pid_Prev_e, QPair<QString, int>(QString("pid_prev_e"), qMetaTypeId<float>()))
		, std::make_pair(T3Database::TrainProperty::Pid_Prev_y, QPair<QString, int>(QString("pid_prev_y"), qMetaTypeId<float>()))
		, std::make_pair(T3Database::TrainProperty::Pid_Sum_e, QPair<QString, int>(QString("pid_sum_e"), qMetaTypeId<float>()))
		, std::make_pair(T3Database::TrainProperty::Pid_Dt, QPair<QString, int>(QString("pid_dt"), qMetaTypeId<float>()))
		, std::make_pair(T3Database::TrainProperty::Pid_R, QPair<QString, int>(QString("pid_r"), qMetaTypeId<float>()))
		, std::make_pair(T3Database::TrainProperty::Pid_Ki, QPair<QString, int>(QString("pid_ki"), qMetaTypeId<float>()))
		, std::make_pair(T3Database::TrainProperty::Pid_Kp, QPair<QString, int>(QString("pid_kp"), qMetaTypeId<float>()))
		, std::make_pair(T3Database::TrainProperty::Pid_Kd, QPair<QString, int>(QString("pid_kd"), qMetaTypeId<float>()))
	};

	//QString plcProgram = "function(a){return a;}";

  public:

	Q_INVOKABLE void pushToFirebase();
	Q_INVOKABLE void pullFromFirebase();

	//CTC-Office related operations
	Q_INVOKABLE QJsonArray ctc_getPossiblePathsFromMetaInfo(const QJsonArray dispatchMetaInfo);
	Q_INVOKABLE QJsonArray ctc_getPossiblePathsFromCsv(const QString filePath, const QJsonArray dispatchMetaInfo);
	Q_INVOKABLE void ctc_enqueueDispatchRequest(const QJsonArray dispatchMetaInfo, const QJsonArray path);
	//Track-Model related operations
	Q_INVOKABLE void km_addTrackFromCsv(const QString filePath);
	Q_INVOKABLE QString getCurrentTime();
	Q_INVOKABLE void setTimerRate(int rate);

	Q_INVOKABLE void setTrackProperty(QString blockId, T3Database::TrackProperty trackProperty, QVariant value);
	Q_INVOKABLE QVariant getTrackProperty(QString blockId, T3Database::TrackProperty trackProperty);
	Q_INVOKABLE void setTrainProperty(QString trainId, T3Database::TrainProperty trainProperty, QVariant value);
	Q_INVOKABLE QVariant getTrainProperty(QString trainId, T3Database::TrainProperty trainProperty);
	Q_INVOKABLE void addTrainFromCtc(const QString trainId, const QString originBlockId, const QString destiBlockId/*, bool moveDirection*/);
	Q_INVOKABLE void removeTrainFromCtc(const QString trainId);

	float pid_plant(float);
	void trackIterate();
	void trainIterate();
	QString getPrevOrNextBlock(QString currBlockId, const QJsonObject * currTrackCon, QJsonObject * currTrackVar
							   , bool reversedLook, bool * viewReversed);

  private:
	//FIREBASE RELATED
	QString firebaseRootUrlString = "https://sprn2023-ece1140-default-rtdb.firebaseio.com/";
	QNetworkAccessManager networkAccessManager;
	QTime currentTime;
	uint8_t timerRate = 1;



	// QObject interface
  protected:
	void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
};

/**
 * @brief T3Database::setTrackProperty
 * @param request
 * Patch variables with certain values
 *
 */
inline void T3Database::setTrackProperty(QString blockId, T3Database::TrackProperty trackProperty, QVariant value) {
	QPair<QString, int> metaData = trackPropertiesMetaDataMap.value(trackProperty);
	if(!value.canConvert(metaData.second))
		qFatal("T3Database::setTrackProperty() -> property value required and insetTrackPropertyted format doesn't match");
	value.convert(metaData.second);
	bool fieldIsFound = false;
	for(qsizetype i = 0; i < trackVariablesObjects.size(); ++i) { //for every line
		QJsonObject currTrackVarObj = trackVariablesObjects[i].toObject();
		if(currTrackVarObj.find(blockId) != currTrackVarObj.end()) {
			QJsonObject currBlockVarObj = currTrackVarObj[blockId].toObject();
			currBlockVarObj[metaData.first] = value.toJsonValue();
			currTrackVarObj[blockId] = currBlockVarObj;
			trackVariablesObjects[i] = currTrackVarObj;
			fieldIsFound = true;
			break;
		}
	}
	if(!fieldIsFound)
		qFatal("T3Database::setTrackProperty() -> cannot find the block id insetTrackPropertyted");
	Q_EMIT onTrackVariablesObjectsChanged();
}

inline QVariant T3Database::getTrackProperty(QString blockId, TrackProperty trackProperty) {
	QPair<QString, int> metaData = trackPropertiesMetaDataMap.value(trackProperty);
	for(qsizetype i = 0; i < trackVariablesObjects.size(); ++i) { //for every line
		QJsonObject currTrackVarObj = trackVariablesObjects[i].toObject();
		if(currTrackVarObj.find(blockId) != currTrackVarObj.end()) {
			QJsonObject currBlockVarObj = currTrackVarObj[blockId].toObject();
			QVariant valueRequested = currBlockVarObj[metaData.first].toVariant();
			if(valueRequested.convert(metaData.second)) {
				//qInfo() << "T3Database::getTrackProperty() -> " << valueRequested;
				return valueRequested;
			} else
				qFatal("T3Database::getTrackProperty() -> cannot convert value to the right type");
		}
	}
	qFatal("T3Database::getTrackProperty() -> cannot find the block id insetTrackPropertyted");
	return QVariant();//should not reach this step..
}

inline void T3Database::setTrainProperty(QString trainId, TrainProperty trainProperty, QVariant value) {
	QPair<QString, int> metaData = trainPropertiesMetaDataMap.value(trainProperty);
	if(!value.canConvert(metaData.second))
		qFatal("T3Database::setTrainProperty() -> property value required and insetTrackPropertyted format doesn't match");
	value.convert(metaData.second);
	for(qsizetype i = 0; i < trainObjects.size(); ++i) {
		if(!trainObjects.at(i).isObject())
			qFatal("T3Database::setTrainProperty() -> current train object is not an object.");
		QJsonObject currTrainObject = trainObjects.at(i).toObject();
		if(currTrainObject.find(QString("id")) == currTrainObject.end())
			qFatal("T3Database::setTrainProperty() -> current train object does not has id field.");
		if(currTrainObject.value("id") == trainId) {
			if(currTrainObject.find(metaData.first) == currTrainObject.end())
				qFatal("T3Database::setTrainProperty() -> current train object does not has the requested field.");
			currTrainObject[metaData.first] = value.toJsonValue();
			return;
		}
	}
	qFatal("T3Database::setTrainProperty() -> cannot find train with ID provided");
}

inline QVariant T3Database::getTrainProperty(QString trainId, TrainProperty trainProperty) {
	QPair<QString, int> metaData = trainPropertiesMetaDataMap.value(trainProperty);
	for(qsizetype i = 0; i < trainObjects.size(); ++i) {
		if(!trainObjects.at(i).isObject())
			qFatal("T3Database::getTrainProperty() -> current train object is not an object.");
		QJsonObject currTrainObject = trainObjects.at(i).toObject();
		if(currTrainObject.find(QString("id")) == currTrainObject.end())
			qFatal("T3Database::getTrainProperty() -> current train object does not has id field.");
		if(currTrainObject.value("id") == trainId) {
			if(currTrainObject.find(metaData.first) == currTrainObject.end())
				qFatal("T3Database::getTrainProperty() -> current train object does not has the requested field.");
			QVariant res = currTrainObject[metaData.first].toVariant();
			if(!res.canConvert(metaData.second))
				qFatal("T3Database::setTrainProperty() -> property value required and insetTrackPropertyted format doesn't match");
			res.convert(metaData.second);
			return res;
		}
	}
	qFatal("T3Database::setTrainProperty() -> cannot find train with ID provided");
	return QVariant();
}

inline void T3Database::addTrainFromCtc(const QString trainId, const QString originBlockId, const QString destiBlockId/*, bool moveDirection*/) {
	QJsonObject trainObject;
	trainObject.insert(QString("id"), trainId);
	trainObject.insert(QString("length"), 32.2);
	trainObject.insert(QString("height"), 3.42);
	trainObject.insert(QString("width"), 2.65);
	trainObject.insert(QString("mass"), 40.9e3);
	trainObject.insert(QString("acceleration"), 0.0);
	trainObject.insert(QString("velocity"), 0.0);
	trainObject.insert(QString("crewCount"), 1);
	trainObject.insert(QString("passangerCount"), 0);
	trainObject.insert(QString("aircon"), QString(""));
	trainObject.insert(QString("exteriorLights"), false);
	trainObject.insert(QString("interiorLights"), false);
	trainObject.insert(QString("leftDoors"), false);
	trainObject.insert(QString("rightDoors"), false);
	trainObject.insert(QString("brakes"), QString(""));
	trainObject.insert(QString("failures"), QString(""));
	trainObject.insert(QString("pid_prev_e"), 0.0);
	trainObject.insert(QString("pid_prev_y"), 0.0);
	trainObject.insert(QString("pid_sum_e"), 0.0);
	trainObject.insert(QString("pid_dt"), 1.0);
	trainObject.insert(QString("pid_r"), 0.0);
	trainObject.insert(QString("pid_ki"), 0.1);
	trainObject.insert(QString("pid_kp"), 0.1);
	trainObject.insert(QString("pid_kd"), 0.1);
	setTrackProperty(originBlockId, TrackProperty::TrainOnBlock, trainId);
	trainObjects.push_front(trainObject);

	//HERE NEEDS CHANGE!
	QStringList trainOnBlock = {QString(trainId), QString("F") /*QString(moveDirection ? "F" : "B")*/, QString::number(0.0)};

	setTrackProperty(originBlockId, TrackProperty::TrainOnBlock, trainOnBlock.join("_"));
	Q_EMIT onTrainObjectsChanged();
	Q_EMIT onTrackVariablesObjectsChanged();
}

inline void T3Database::removeTrainFromCtc(const QString trainId) {
	for(qsizetype i = 0; i < trainObjects.count(); ++i) {
		QJsonObject currTrainObject = trainObjects.at(i).toObject();
		if(currTrainObject.value(QString("id")).toString().trimmed() == trainId.trimmed()) {
			trainObjects.removeAt(i);
			Q_EMIT onTrainObjectsChanged();
			return;
		}
	}
	qFatal("T3Database::removeTrainFromCtc() -> cannot find train id provided.");
}

inline T3Database::T3Database(QObject * parent) : QObject(parent) {
	//	//TESTING FOR RAIL LOADING
	this->currentTime = QTime::currentTime();
	km_addTrackFromCsv("C:/Users/YIQ25/Documents/Academics/ECE1140/Resources/T3RedLine.csv");
	//	setTrackProperty("R_A_1", TrackProperty::TrainOnBlock, "1234");
	//	setTrackProperty("R_A_1", TrackProperty::PeopleOnStation, "1234");
	km_addTrackFromCsv("C:/Users/YIQ25/Documents/Academics/ECE1140/Resources/T3GreenLine.csv");
	km_addTrackFromCsv("C:/Users/YIQ25/Documents/Academics/ECE1140/Resources/T3BlueLine.csv");
	addTrainFromCtc("1234", "R_A_1", "FWD");
	pushToFirebase();
	//ctc_getPossiblePaths(QJsonArray{QString("1234"), QString("R_C_8"), QString("R_B_6"), QString("12:34")});

}

inline float T3Database::pid_plant(float inp) {
	return inp;
}

inline void T3Database::trainIterate() {
	for(qsizetype i = 0; i < trainObjects.size(); ++i) {
		QJsonObject currTrainObject = trainObjects.at(i).toObject();



		trainObjects.replace(i, currTrainObject);
	}
	Q_EMIT onTrainObjectsChanged();
}

inline QString T3Database::getPrevOrNextBlock(QString currBlockId
		, const QJsonObject *currTrackCon, QJsonObject *currTrackVar
		, bool reversedLook, bool *viewReversed) {
	const QJsonObject currBlockCon = currTrackCon->value(currBlockId).toObject();
	QJsonObject currBlockVar = currTrackVar->value(currBlockId).toObject();
	*viewReversed = true;
	//input is current block pair (constants,variables) and bool indicating if going backward
	//output is the block pair calculated, either previous or next based on args[1]
	QVarLengthArray<QString, 2> prevNextBlockIds{QString(), QString()};
	for(int i = 1; i <= 2; ++i) {
		QString istr = QString::number(i);
		prevNextBlockIds.replace(i - 1, currBlockCon.value(reversedLook ? "prevBlock" + istr  : "nextBlock" + istr).toString());
		if(prevNextBlockIds.at(i - 1).compare(reversedLook ? "END_T" : "START_T") == 0)
			prevNextBlockIds.replace(i - 1,  currTrackCon->value(reversedLook ? "endingBlock2" : "startingBlock2").toString());
		else if(prevNextBlockIds.at(i - 1).compare(reversedLook ? "END_B" : "START_B") == 0)
			prevNextBlockIds.replace(i - 1, currTrackCon->value(reversedLook ? "endingBlock1" : "startingBlock1").toString());
		else *viewReversed = false;
	}
	const bool currSwitchPosition = currBlockVar.value("switchPosition").toBool();
	QString prevNextBlockId
		= (prevNextBlockIds.at(1) != "" && prevNextBlockIds.at(1) != "PASSIVE" && !currSwitchPosition)
		  ? prevNextBlockIds.at(1) : prevNextBlockIds.at(0) ;
	return prevNextBlockId;
}

inline void T3Database::timerEvent(QTimerEvent *event) {
	//update time
	this->currentTime = this->currentTime.addMSecs(100 * static_cast<int>(timerRate));
	Q_EMIT onCurrentTimeChanged();
	//check queue

}

inline void T3Database::pushToFirebase() {
	const QList<QPair<QString, QJsonArray*>> args{
		{"trackConstantsObjects", &trackConstantsObjects}
		, {"trackVariablesObjects", &trackVariablesObjects}
		, {"trainObjects", &trainObjects}
	};
	for(const QPair<QString, QJsonArray*>& arg : args) {
		QByteArray serializedObj = QJsonDocument(*arg.second).toJson();
		QNetworkRequest networkRequest(QUrl(firebaseRootUrlString + QString("/%1.json").arg(arg.first)));
		networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
		QNetworkReply* networkReply = networkAccessManager.put(networkRequest, serializedObj);
		if(networkReply->error() != QNetworkReply::NetworkError::NoError)
			qInfo() << QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(networkReply->error());
	}
}

inline void T3Database::pullFromFirebase() {
	const QList<QPair<QString, QJsonArray*>> args{
		{"trackConstantsObjects", &trackConstantsObjects}
		, {"trackVariablesObjects", &trackVariablesObjects}
		, {"trainObjects", &trainObjects}
	};
	for(const QPair<QString, QJsonArray*>& arg : args) {
		QNetworkRequest networkRequest(QUrl(firebaseRootUrlString + QString("/%1.json").arg(arg.first)));
		//networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, QString("application/json"));
		QNetworkReply* networkReply = networkAccessManager.get(networkRequest);
		if(networkReply->error() != QNetworkReply::NetworkError::NoError)
			qInfo() << QMetaEnum::fromType<QNetworkReply::NetworkError>().valueToKey(networkReply->error());
		connect(networkReply, &QNetworkReply::readyRead, [this, networkReply, arg]() {
			QByteArray serializedObjs = networkReply->readAll();
			*arg.second = QJsonDocument::fromJson(serializedObjs).array();
			Q_EMIT onTrackVariablesObjectsChanged();
		});
	}
}

inline QJsonArray T3Database::ctc_getPossiblePathsFromMetaInfo(const QJsonArray dispatchMetaInfo) {
	return T3CTCOffice::searchPathsFromMetaInfo(dispatchMetaInfo, &trackConstantsObjects);
}

inline QJsonArray T3Database::ctc_getPossiblePathsFromCsv(const QString filePath, const QJsonArray dispatchMetaInfo) {
	return T3CTCOffice::searchPathsFromCsv(filePath, &stationToBlockIdMap, dispatchMetaInfo, &trackConstantsObjects);
}

inline void T3Database::ctc_enqueueDispatchRequest(const QJsonArray dispatchMetaInfo, const QJsonArray path) {
	T3CTCOffice::enqueueDispatchRequest(&dispatchQueue, dispatchMetaInfo, path);
	Q_EMIT onDispatchQueueChanged();
}

inline void T3Database::km_addTrackFromCsv(const QString filePath) {
	T3TrackModel::addTrackFromCsv(filePath, &trackConstantsObjects, &trackVariablesObjects, &stationToBlockIdMap);
	Q_EMIT onTrackConstantsObjectsChanged();
	Q_EMIT onTrackVariablesObjectsChanged();
	Q_EMIT onStationToBlockIdMapChanged();
}

inline QString T3Database::getCurrentTime() {
	return this->currentTime.toString("HH:mm:ss");
}

inline void T3Database::setTimerRate(int rate) {
	this->timerRate = static_cast<uint8_t>(rate);
}

inline void T3Database::trackIterate() {

	for(qsizetype i = 0; i < trackVariablesObjects.count(); ++i) {
		QJsonObject currTrackVarObj = trackVariablesObjects.at(i).toObject();
		const QJsonObject currTrackConObj = trackConstantsObjects.at(i).toObject().value("blocksMap").toObject();
		QStringList blockIds = currTrackVarObj.keys();
		for(qsizetype j = 0; j < blockIds.count(); ++j) {

			//determine prev2 prev1 next1 next2 blockId and object
			QString currBlockId = blockIds.at(j);
			const QJsonObject currBlockConObj = currTrackConObj.value(currBlockId).toObject();
			QJsonObject currBlockVarObj = currTrackVarObj.value(currBlockId).toObject();

			bool prevViewReversed = false, nextViewReversed = false;

			QString prev1BlockId
				= getPrevOrNextBlock(currBlockId, &currTrackConObj, &currTrackVarObj, true != prevViewReversed, &prevViewReversed);
			QString prev2BlockId
				= getPrevOrNextBlock(prev1BlockId, &currTrackConObj, &currTrackVarObj, true != prevViewReversed, &prevViewReversed);
			QString next1BlockId
				= getPrevOrNextBlock(currBlockId, &currTrackConObj, &currTrackVarObj, true != nextViewReversed, &nextViewReversed);
			QString next2BlockId
				= getPrevOrNextBlock(next1BlockId, &currTrackConObj, &currTrackVarObj, true != nextViewReversed, &nextViewReversed);

			const QJsonObject prev1BlockConObj = currTrackConObj.value(prev1BlockId).toObject();
			const QJsonObject prev2BlockConObj = currTrackConObj.value(prev2BlockId).toObject();
			const QJsonObject next1BlockConObj = currTrackConObj.value(next1BlockId).toObject();
			const QJsonObject next2BlockConObj = currTrackConObj.value(next2BlockId).toObject();

			QJsonObject prev1BlockVarObj = currTrackVarObj.value(prev1BlockId).toObject();
			QJsonObject prev2BlockVarObj = currTrackVarObj.value(prev2BlockId).toObject();
			QJsonObject next1BlockVarObj = currTrackVarObj.value(next1BlockId).toObject();
			QJsonObject next2BlockVarObj = currTrackVarObj.value(next2BlockId).toObject();

			QVarLengthArray<QPair<const QJsonObject*, QJsonObject*>, 5> quintupleBlocks = {
				qMakePair(&prev2BlockConObj, &prev2BlockVarObj)
				, qMakePair(&prev1BlockConObj, &prev1BlockVarObj)
				, qMakePair(&currBlockConObj, &currBlockVarObj)
				, qMakePair(&next1BlockConObj, &next1BlockVarObj)
				, qMakePair(&next2BlockConObj, &next2BlockVarObj)
			};

			T3TrackModel::plcIterate(quintupleBlocks);
//			//----STAGE 2 : TRAIN POSITION UPDATE
		}
		trackVariablesObjects.replace(i, currTrackVarObj);
	}
}

#endif // T3Database_H

