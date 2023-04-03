#ifndef T3TRAINMODEL_HPP
#define T3TRAINMODEL_HPP
#include "t3prophelper.h"
class T3TrainModel {
	static bool connectedToTrackModel;
	static bool connectedToTrainController;
	//TRAIN MODEL
  public:

	static void removeTrain(const QString trainId, MODU_ARGS_REF argsref);
	static void createNewTrain(const QString trainId, const QJsonArray path, MODU_ARGS_REF argsref);
	static void trainTravelIterate(const QJsonObject *train, MODU_ARGS_REF argsref);
	static QJsonArray getAllTrainIds( MODU_ARGS_REF argsref);
	static void setFailureOrBrake(const QString trainId, const int index, const bool value, MODU_ARGS_REF argsref);
	static QJsonArray getStringsFromMetaInfo(const QString trainId, MODU_ARGS_REF argsref);
  private:
	//static const QHash<T3TrainModel::TrainProperty, QPair<QString, int>> trainPropertiesMetaDataMap;
};

inline void T3TrainModel::removeTrain(const QString trainId, MODU_ARGS_REF argsref) {
	for(qsizetype i = 0; i < std::get<4>(*argsref)->count(); ++i) {
		QJsonObject currTrainObject = std::get<4>(*argsref)->at(i).toObject();
		if(currTrainObject.value(QString("id")).toString().trimmed() == trainId.trimmed()) {
			std::get<4>(*argsref)->removeAt(i);
			return;
		}
	}
	qFatal("T3Database::removeTrainFromCtc() -> cannot find train id provided.");
}


inline void T3TrainModel::createNewTrain(const QString trainId, const QJsonArray path, MODU_ARGS_REF argsref) {
	QJsonObject trainObject;
	trainObject.insert(QString("NM_ID"), trainId);
	trainObject.insert(QString("NM_LENGTH"), 32.2);
	trainObject.insert(QString("NM_HEIGHT"), 3.42);
	trainObject.insert(QString("NM_WIDTH"), 2.65);
	trainObject.insert(QString("NM_MASS"), 40.9e3);
	trainObject.insert(QString("NM_ACCELERATION"), 0.0);
	//trainObject.insert(QString("velocity"), 0.0);
	trainObject.insert(QString("NM_CREWCOUNT"), 1);
	trainObject.insert(QString("NM_PASSANGERCOUNT"), 0);
	trainObject.insert(QString("NM_TEMPERATURE"), 0.0);
	trainObject.insert(QString("NM_AUTOMODE"), true);
	trainObject.insert(QString("COM[NC_NM]_EXTLIGHT"), false);
	trainObject.insert(QString("COM[NC_NM]_INTLIGHT"), false);
	trainObject.insert(QString("COM[NC_NM]_LDOOR"), false);
	trainObject.insert(QString("COM[NC_NM]_RDOOR"), false);
	trainObject.insert(QString("COM[NC_NM]_EBRAKE"), false);
	trainObject.insert(QString("COM[NC_NM]_SBRAKE"), false);
	trainObject.insert(QString("COM[NC_NM]_FAILURE0"), false);
	trainObject.insert(QString("COM[NC_NM]_FAILURE1"), false);
	trainObject.insert(QString("COM[NC_NM]_FAILURE2"), false);
	trainObject.insert(QString("NC_PREVE"), 0.0);//error
	trainObject.insert(QString("NC_PREVY"), 0.0);//velocity
	trainObject.insert(QString("NC_SUME"), 0.0);
	trainObject.insert(QString("NC_DT"), 1.0);//
	trainObject.insert(QString("NC_R"), 0.0);//velocity set point
	trainObject.insert(QString("NC_KI"), 0.0);//
	trainObject.insert(QString("NC_KP"), 0.0);
	trainObject.insert(QString("NC_U"), 0.0);//output power
	trainObject.insert(QString("NM_BLOCKID"), path.at(0).toString());
	std::get<4>(*argsref)->push_front(trainObject);
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
inline void T3TrainModel::trainTravelIterate(const QJsonObject *train, MODU_ARGS_REF argsref) {
	//trainOnBlock formats: ID_DIRECTION_PERCENT
//	Q_ASSERT(quintupleBlocks.size() == 5);
//	QStringList trainOnBlock = quintupleBlocks.at(2).second->value("trainOnBlock").toString().split("_");
//	Q_ASSERT(trainOnBlock.size() == 3);
//	QString trainId = trainOnBlock.at(0);
//	Q_ASSERT(trainId == train->value("id").toString());
//	QString trainDirection = trainOnBlock.at(1);
//	bool toFloatConversionStatus = true;
//	float trainTravelledPercent = trainOnBlock.at(2).toFloat(&toFloatConversionStatus);
//	Q_ASSERT(toFloatConversionStatus);

//	//iterate per one second
//	float dt = 1 / 60 / 60; //1 second in hours
//	float v = train->value("velocity").toDouble();
//	Q_ASSERT(toFloatConversionStatus);
//	float ds = dt * v;

//	float totalBlockLength = quintupleBlocks.at(2).first->value("length").toDouble();
//	float remainingBlockLength = totalBlockLength * trainTravelledPercent;
//	if(remainingBlockLength < ds) {
//		//time to traverse to next block
//		bool reversedTravel = trainDirection.contains("R");
//		if(reversedTravel) Q_ASSERT(trainDirection.contains("F"));
//		const QJsonObject* nextOrPrevBlockConObj = quintupleBlocks.at(reversedTravel ? 1 : 3).first;
//		QJsonObject* nextOrPrevBlockVarObj = quintupleBlocks.at(reversedTravel ? 1 : 3).second;

//		totalBlockLength = nextOrPrevBlockConObj->value("length").toDouble();
//		float newInitialBlockLength = ds - remainingBlockLength;
//		remainingBlockLength = totalBlockLength - newInitialBlockLength;
//		trainTravelledPercent = 1 - remainingBlockLength / totalBlockLength;

//		const QString prevBlock1Id = quintupleBlocks.at(2).first->value("prevBlock1").toString();
//		bool isAtViewBorder = (prevBlock1Id.contains("START") && reversedTravel) || (prevBlock1Id.contains("END") && !reversedTravel);
//		if(isAtViewBorder) trainDirection = trainDirection.contains("F") ? "R" : "F";
//		trainOnBlock = QStringList{trainId, trainDirection, QString::number(trainTravelledPercent)};

//		quintupleBlocks.at(2).second->insert("trainOnBlock", QString(""));//remove train from last block
//		nextOrPrevBlockVarObj->insert("trainOnBlock", trainOnBlock.join("_"));//add train to new block
//	} else {
//		//stays on current block
//		remainingBlockLength -= ds;
//		trainTravelledPercent = 1 - remainingBlockLength / totalBlockLength;
//		trainOnBlock = QStringList{trainId, trainDirection, QString::number(trainTravelledPercent)};
//		quintupleBlocks.at(2).second->insert("trainOnBlock", trainOnBlock.join("_"));//update train from last block
	//	}
}

inline QJsonArray T3TrainModel::getAllTrainIds(MODU_ARGS_REF argsref) {
	QJsonArray toRet;
	QStringList allTrainIds;
	for(const QJsonValue& currTrainObject : qAsConst(*std::get<4>(*argsref))) {
		Q_ASSERT(currTrainObject.isObject() && currTrainObject.toObject().contains("NM_ID"));
		allTrainIds.push_back(currTrainObject.toObject().value("NM_ID").toString());
	}
	toRet.push_back(QJsonArray::fromStringList(allTrainIds));
	return toRet;
}

inline void T3TrainModel::setFailureOrBrake(const QString trainId, const int index, const bool value, MODU_ARGS_REF argsref) {
	switch (index) {
	case 0:
		SET_TRAIN_F(trainId, "COM[NC_NM]_FAILURE0", value, argsref);
		break;
	case 1:
		SET_TRAIN_F(trainId, "COM[NC_NM]_FAILURE1", value, argsref);
		break;
	case 2:
		SET_TRAIN_F(trainId, "COM[NC_NM]_FAILURE2", value, argsref);
		break;
	case 3:
		SET_TRAIN_F(trainId, "COM[NC_NM]_EBRAKE", value, argsref);
		break;
	default:
		qFatal("T3TrainModel::setFailureOrBrake() -> index not recognized");
		break;
	}
	return;
}

inline QJsonArray T3TrainModel::getStringsFromMetaInfo(const QString trainId, MODU_ARGS_REF argsref) {
	QVariantList metaInfo;
	{
		//0 - left door
		bool leftDoorOpened = GET_TRAIN_F(trainId, "COM[NC_NM]_LDOOR", argsref).toBool();
		metaInfo.append(leftDoorOpened ? "OPENED" : "CLOSED");
	}
	{
		//1- right door
		bool rightDoorOpened = GET_TRAIN_F(trainId, "COM[NC_NM]_RDOOR", argsref).toBool();
		metaInfo.append(rightDoorOpened ? "OPENED" : "CLOSED");
	}
	{
		//2- service brake
		bool sBrake = GET_TRAIN_F(trainId, "COM[NC_NM]_SBRAKE", argsref).toBool();
		metaInfo.append(sBrake ? "APPLIED" : "RELEASED");
	}
	{
		//3,4 - emergency brake
		bool eBrake = GET_TRAIN_F(trainId, "COM[NC_NM]_EBRAKE", argsref).toBool();
		metaInfo.append(eBrake ? "APPLIED" : "RELEASED");
		metaInfo.append(eBrake);
	}

	const QString blockId = GET_TRAIN_F(trainId, "NM_BLOCKID", argsref).toString();
	if(blockId == "")
		qFatal("T3TrainController::getMetaInfo() -> BLOCK ID is empty.");
	const QString BCNPLCOUT = GET_TRACKVAR_F(blockId, "COM[KC|KM]_BCNPLCOUT", argsref).toString();
	//metaInfo.append(0);//unused
	{
		//5- speed limit
		unsigned int speedLimit =  GET_TRACKCON_F(blockId, "speedLimit", argsref).toUInt();
		metaInfo.append(speedLimit);
	}

	{
		//6- light signal
		unsigned int authorizedNumberBlock =  BCNPLCOUT.midRef(2, 8).toUInt(nullptr, 2);
		if(authorizedNumberBlock > 5) metaInfo.append(QString("green"));
		if(authorizedNumberBlock > 1) metaInfo.append(QString("yellow"));
		else metaInfo.append(QString("red"));
	}
	{
		//7- powert
		float power  = GET_TRAIN_F(trainId, "NC_U", argsref).toFloat();
		metaInfo.append(power);
	}
	{
		//8- imperial velocity
		float impVelocity  = KMH2MPH_F(GET_TRAIN_F(trainId, "NC_PREVY", argsref).toFloat());
		metaInfo.append(impVelocity);
	}
	{
		//9- imperial acceleration
		float impAccel  = M2FOOT_F(GET_TRAIN_F(trainId, "NM_ACCELERATION", argsref).toFloat());
		metaInfo.append(impAccel);
	}
	{
		//10-length string
		float length = M2FOOT_F(GET_TRAIN_F(trainId, "NM_LENGTH", argsref).toFloat());
		metaInfo.append(QString::number(length, 'g', 3) + " FT");
	}
	{
		//11 height string
		float height = M2FOOT_F(GET_TRAIN_F(trainId, "NM_HEIGHT", argsref).toFloat());
		metaInfo.append(QString::number(height, 'g', 3) + " FT");
	}
	{
		//12-width string
		float width = M2FOOT_F(GET_TRAIN_F(trainId, "NM_WIDTH", argsref).toFloat());
		metaInfo.append(QString::number(width, 'g', 3) + " FT");
	}
	{
		//13-mass string
		float mass = (GET_TRAIN_F(trainId, "NM_MASS", argsref).toFloat());
		metaInfo.append(QString::number(mass, 'g', 3) + " T");
	}
	{
		//14,15 temp string - no conversion neede
		float temperature = (GET_TRAIN_F(trainId, "NM_TEMPERATURE", argsref).toFloat());
		metaInfo.append(QString::number(temperature, 'g', 3) + " F");
		metaInfo.append(temperature);
	}
	{
		//16-ext light string
		bool extLight = (GET_TRAIN_F(trainId, "COM[NC_NM]_EXTLIGHT", argsref).toBool());
		metaInfo.append(QString(extLight ? "ON" : "OFF"));
	}
	{
		//17-int light string
		bool intLight = (GET_TRAIN_F(trainId, "COM[NC_NM]_INTLIGHT", argsref).toBool());
		metaInfo.append(QString(intLight ? "ON" : "OFF"));
	}
	{
		//18,19-failure 1 string
		bool f1 = (GET_TRAIN_F(trainId, "COM[NC_NM]_FAILURE0", argsref).toBool());
		metaInfo.append(f1 ? "ERROR" : "OK");
		metaInfo.append(f1);
	}
	{
		//20,21 failure 2 string
		bool f2 = (GET_TRAIN_F(trainId, "COM[NC_NM]_FAILURE1", argsref).toBool());
		metaInfo.append(f2 ? "ERROR" : "OK");
		metaInfo.append(f2);
	}
	{
		//22,23-failure 3 string
		bool f3 = (GET_TRAIN_F(trainId, "COM[NC_NM]_FAILURE2", argsref).toBool());
		metaInfo.append(f3 ? "ERROR" : "OK");
		metaInfo.append(f3);
	}
	return QJsonArray::fromVariantList(metaInfo);
}





#endif // T3TrainModel_HPP
