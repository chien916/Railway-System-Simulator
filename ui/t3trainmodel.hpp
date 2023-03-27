#ifndef T3TrainModel_HPP
#define T3TrainModel_HPP


#include <QtCore>
using MODU_ARGS_REF
	= const std::tuple<const std::function<QVariant(const QString, const QString, const QJsonArray*)>*
	  , const std::function<void(const QString, const QString, QJsonArray*, const QVariant)>*
	  ,  QJsonArray*, QJsonArray*, QJsonArray*>*;
class T3TrainModel {
	static bool connectedToTrackModel;
	static bool connectedToTrainController;
	//TRAIN MODEL
  public:

	static void removeTrain(const QString trainId, QJsonArray* trainObjects);
	static void createNewTrain(const QString trainId, const QJsonArray path, QJsonArray* trainObjects);
	static void trainTravelIterate(const QJsonObject *train, QVarLengthArray<QPair<const QJsonObject *, QJsonObject *>, 5> &quintupleBlocks);


  private:
	//static const QHash<T3TrainModel::TrainProperty, QPair<QString, int>> trainPropertiesMetaDataMap;
};

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


inline void T3TrainModel::createNewTrain(const QString trainId, const QJsonArray path, QJsonArray *trainObjects) {
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
	trainObject.insert(QString("NM_AIRCON"), QString(""));
	trainObject.insert(QString("NM_DESTINATION"), QString(""));
	trainObject.insert(QString("COM[NC_NM]_EXTLIGHT"), false);
	trainObject.insert(QString("COM[NC_NM]_INTLIGHT"), false);
	trainObject.insert(QString("COM[NC_NM]_LDOOR"), false);
	trainObject.insert(QString("COM[NC_NM]_RDOOR"), false);
	trainObject.insert(QString("COM[NC_NM]_BRAKE"), QString(""));
	trainObject.insert(QString("COM[NC_NM]_FAILURE"), QString(""));
	trainObject.insert(QString("NC_PREVE"), 0.0);
	trainObject.insert(QString("NC_PREVY"), 0.0);
	trainObject.insert(QString("NC_SUME"), 0.0);
	trainObject.insert(QString("NC_DT"), 1.0);
	trainObject.insert(QString("NC_R"), 0.0);
	trainObject.insert(QString("NC_KI"), 0.0);
	trainObject.insert(QString("NC_KP"), 0.0);
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
