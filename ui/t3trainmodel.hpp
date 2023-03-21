#ifndef T3TrainModel_HPP
#define T3TrainModel_HPP


#include <QtCore>
class T3TrainModel {
	static bool connectedToTrackModel;
	static bool connectedToTrainController;
	//TRAIN MODEL
  public:

	enum TrainProperty : uint8_t {
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
		, Pid_Prev_e = 16
		, Pid_Prev_y = 17
		, Pid_Sum_e = 18
		, Pid_Dt = 19
		, Pid_R = 20
		, Pid_Ki = 21
		, Pid_Kp = 22
		, Pid_Kd = 23
	};

	static void setTrainProperty(QString trainId, TrainProperty trainProperty, QVariant value, QJsonArray* trainObjects, const QHash<T3TrainModel::TrainProperty, QPair<QString, int>>*const trainPropertiesMetaDataMap);
	static QVariant getTrainProperty(QString trainId, TrainProperty trainProperty, const  QJsonArray* trainObjects, const QHash<T3TrainModel::TrainProperty, QPair<QString, int>>*const trainPropertiesMetaDataMap);

	static void removeTrain(const QString trainId, QJsonArray* trainObjects);
	static void createNewTrain(const QString trainId, QJsonArray* trainObjects);
	static void trainTravelIterate(const QJsonObject *train, QVarLengthArray<QPair<const QJsonObject *, QJsonObject *>, 5> &quintupleBlocks);


  private:
	//static const QHash<T3TrainModel::TrainProperty, QPair<QString, int>> trainPropertiesMetaDataMap;
};




inline void T3TrainModel::setTrainProperty(QString trainId, TrainProperty trainProperty, QVariant value, QJsonArray* trainObjects, const QHash<T3TrainModel::TrainProperty, QPair<QString, int>>*const trainPropertiesMetaDataMap) {
	QPair<QString, int> metaData = trainPropertiesMetaDataMap->value(trainProperty);
	if(!value.canConvert(metaData.second))
		qFatal("T3TrainModel::setTrainProperty() -> property value required and insetTrackPropertyted format doesn't match");
	value.convert(metaData.second);
	for(qsizetype i = 0; i < trainObjects->size(); ++i) {
		if(!trainObjects->at(i).isObject())
			qFatal("T3TrainModel::setTrainProperty() -> current train object is not an object.");
		QJsonObject currTrainObject = trainObjects->at(i).toObject();
		if(currTrainObject.find(QString("id")) == currTrainObject.end())
			qFatal("T3TrainModel::setTrainProperty() -> current train object does not has id field.");
		if(currTrainObject.value("id") == trainId) {
			if(currTrainObject.find(metaData.first) == currTrainObject.end())
				qFatal("T3TrainModel::setTrainProperty() -> current train object does not has the requested field.");
			currTrainObject[metaData.first] = value.toJsonValue();
			trainObjects->operator[](i) = currTrainObject;
			return;
		}
	}
	qFatal("T3TrainModel::setTrainProperty() -> cannot find train with ID provided");
}

inline QVariant T3TrainModel::getTrainProperty(QString trainId, TrainProperty trainProperty, const  QJsonArray* trainObjects, const QHash<T3TrainModel::TrainProperty, QPair<QString, int>>*const trainPropertiesMetaDataMap) {
	QPair<QString, int> metaData = trainPropertiesMetaDataMap->value(trainProperty);
	for(qsizetype i = 0; i < trainObjects->size(); ++i) {
		if(!trainObjects->at(i).isObject())
			qFatal("T3TrainModel::getTrainProperty() -> current train object is not an object.");
		QJsonObject currTrainObject = trainObjects->at(i).toObject();
		if(currTrainObject.find(QString("id")) == currTrainObject.end())
			qFatal("T3TrainModel::getTrainProperty() -> current train object does not has id field.");
		if(currTrainObject.value("id") == trainId) {
			if(currTrainObject.find(metaData.first) == currTrainObject.end())
				qFatal("T3TrainModel::getTrainProperty() -> current train object does not has the requested field.");
			QVariant res = currTrainObject[metaData.first].toVariant();
			if(!res.canConvert(metaData.second))
				qFatal("T3TrainModel::setTrainProperty() -> property value required and insetTrackPropertyted format doesn't match");
			res.convert(metaData.second);
			return res;
		}
	}
	qFatal("T3TrainModel::setTrainProperty() -> cannot find train with ID provided");
	return QVariant();
}

inline void T3TrainModel::removeTrain(const QString trainId, QJsonArray *trainObjects) {
	for(qsizetype i = 0; i < trainObjects->count(); ++i) {
		QJsonObject currTrainObject = trainObjects->at(i).toObject();
		if(currTrainObject.value(QString("id")).toString().trimmed() == trainId.trimmed()) {
			trainObjects->removeAt(i);
			return;
		}
	}
	qFatal("T3Database::removeTrainFromCtc() -> cannot find train id provided.");
}


inline void T3TrainModel::createNewTrain(const QString trainId, QJsonArray *trainObjects) {
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
	trainObject.insert(QString("pid_ki"), 0.0);
	trainObject.insert(QString("pid_kp"), 0.0);
	trainObject.insert(QString("pid_kd"), 0.0);
	//setTrackProperty(originBlockId, TrackProperty::TrainOnBlock, trainId);
	trainObjects->push_front(trainObject);
}


/**
 * @brief T3TrainModel::nm_trainTravelIterate
 * @param train 火车实体指针
 * @param quintupleBlocks 五重铁轨块元组
 * --TRAIN MODEL子函数
 * 对于在当前的铁轨块上的火车，根据火车内部存储的速度，计算在一秒内火车的位移，并且将位移信息更新到五重铁轨快元组
 * 如果火车在这一秒将越过当前铁轨块，那么将会擦除当前铁轨块上的火车信息，并且迁移到上一铁轨块或下一铁轨块
 *
 * 此函数一定修改当前铁轨块，可能修改上一个或下一个铁轨块
 *
 * ￥此函数为自然模拟函数，无论火车控制器和铁轨控制器的状态如何，每个时钟此函数无论如何必须被调用！
 *
 * ￥请注意，请确保在调用此函数前，确保当前铁轨块上存在火车，并且铁轨块上火车的标识与传入的火车实体标识一致！
 *
 */
inline void T3TrainModel::trainTravelIterate(const QJsonObject *train, QVarLengthArray<QPair<const QJsonObject *, QJsonObject *>, 5> &quintupleBlocks) {
	//trainOnBlock formats: ID_DIRECTION_PERCENT
	Q_ASSERT(quintupleBlocks.size() == 5);
	QStringList trainOnBlock = quintupleBlocks.at(2).second->value("trainOnBlock").toString().split("_");
	Q_ASSERT(trainOnBlock.size() == 3);
	QString trainId = trainOnBlock.at(0);
	Q_ASSERT(trainId == train->value("id").toString());
	QString trainDirection = trainOnBlock.at(1);
	bool toFloatConversionStatus = true;
	float trainTravelledPercent = trainOnBlock.at(2).toFloat(&toFloatConversionStatus);
	Q_ASSERT(toFloatConversionStatus);

	//iterate per one second
	float dt = 1 / 60 / 60; //1 second in hours
	float v = train->value("velocity").toDouble();
	Q_ASSERT(toFloatConversionStatus);
	float ds = dt * v;

	float totalBlockLength = quintupleBlocks.at(2).first->value("length").toDouble();
	float remainingBlockLength = totalBlockLength * trainTravelledPercent;
	if(remainingBlockLength < ds) {
		//time to traverse to next block
		bool reversedTravel = trainDirection.contains("R");
		if(reversedTravel) Q_ASSERT(trainDirection.contains("F"));
		const QJsonObject* nextOrPrevBlockConObj = quintupleBlocks.at(reversedTravel ? 1 : 3).first;
		QJsonObject* nextOrPrevBlockVarObj = quintupleBlocks.at(reversedTravel ? 1 : 3).second;

		totalBlockLength = nextOrPrevBlockConObj->value("length").toDouble();
		float newInitialBlockLength = ds - remainingBlockLength;
		remainingBlockLength = totalBlockLength - newInitialBlockLength;
		trainTravelledPercent = 1 - remainingBlockLength / totalBlockLength;

		const QString prevBlock1Id = quintupleBlocks.at(2).first->value("prevBlock1").toString();
		bool isAtViewBorder = (prevBlock1Id.contains("START") && reversedTravel) || (prevBlock1Id.contains("END") && !reversedTravel);
		if(isAtViewBorder) trainDirection = trainDirection.contains("F") ? "R" : "F";
		trainOnBlock = QStringList{trainId, trainDirection, QString::number(trainTravelledPercent)};

		quintupleBlocks.at(2).second->insert("trainOnBlock", QString(""));//remove train from last block
		nextOrPrevBlockVarObj->insert("trainOnBlock", trainOnBlock.join("_"));//add train to new block
	} else {
		//stays on current block
		remainingBlockLength -= ds;
		trainTravelledPercent = 1 - remainingBlockLength / totalBlockLength;
		trainOnBlock = QStringList{trainId, trainDirection, QString::number(trainTravelledPercent)};
		quintupleBlocks.at(2).second->insert("trainOnBlock", trainOnBlock.join("_"));//update train from last block
	}
}





#endif // T3TrainModel_HPP
