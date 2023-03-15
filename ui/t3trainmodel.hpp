#ifndef T3TrainModel_HPP
#define T3TrainModel_HPP


#include <QtCore>
class T3TrainModel {
	static bool connectedToTrackModel;
	static bool connectedToTrainController;
	//TRAIN MODEL
	static float nm_plant(float);
	static void nm_trainTravelIterate(const QJsonObject* train, QVarLengthArray<QPair<const QJsonObject*, QJsonObject*>, 5>& quintupleBlocks);

	//TRAIN CONTROLLER
	static void nc_pidIterate(QJsonObject* train);

	static void nc_controlSystemIterate(QJsonObject* train, const QVarLengthArray<QPair<const QJsonObject*, QJsonObject*>, 5>& quintupleBlocks);
};

/**
 * @brief T3TrainModel::nc_pidIterate
 * @param train
 *
 * 使用条件：此函数只会在火车引擎控制系统开启时被调用
 */
inline void T3TrainModel::nc_pidIterate(QJsonObject *train) {
	//retrieve pid data from train database
	float r = train->value(QString("pid_r")).toDouble();
	float prev_e = train->value(QString("pid_prev_e")).toDouble();
	float prev_y = train->value(QString("pid_prev_y")).toDouble();
	float sum_e = train->value(QString("pid_sum_e")).toDouble();
	float dt = train->value(QString("pid_dt")).toDouble();
	float kp = train->value(QString("pid_kp")).toDouble();
	float ki = train->value(QString("pid_ki")).toDouble();
	float kd = train->value(QString("pid_kd")).toDouble();

	//calculate new values
	float e = r - prev_y;
	float P = kp * e;
	float I = ki * sum_e * dt;
	float D = kd * (prev_e + e) * dt * 0.5;//trapozoidal integration
	float u = P + I + D;
	float y = u;//CHANGE IT HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	//overwrite pid data with new values
	train->insert("velocity", y);
	train->insert("pid_prev_y", y);
	train->insert("pid_prev_e", e);
	train->insert("pid_sum_e", sum_e + e);

}

inline float T3TrainModel::nm_plant(float) {

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
inline void T3TrainModel::nm_trainTravelIterate(const QJsonObject *train, QVarLengthArray<QPair<const QJsonObject *, QJsonObject *>, 5> &quintupleBlocks) {
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


/**
 * @brief T3TrainModel::nc_controlSystemIterate
 * @param train
 * @param quintupleBlocks
 * TRAIN CONTROLLER子函数
 * 对于在当前铁轨块上的火车，利用铁轨块上的某些信息，自动切换火车非引擎相关的设施开关，例如内车灯，外车灯，空调等
 *
 * 内车灯：当时间在晚上的时候开启，白天的时候关闭
 * 外车灯：当火车进入隧道以后开启，在时间为晚上的时候开启，白天的时候关闭
 *
 * 此函数只修改当前铁轨块，和火车
 *
 * 使用条件：此函数只会在火车设施控制系统开启时被调用
 * 危殆条件处理 ：利用紧急制动立刻停止火车，开启所有车灯
 *
 */
inline void T3TrainModel::nc_controlSystemIterate(QJsonObject *train, const QVarLengthArray<QPair<const QJsonObject *, QJsonObject *>, 5>& quintupleBlocks) {
	//train id matching check
	Q_ASSERT(quintupleBlocks.size() == 5);
	QStringList trainOnBlock = quintupleBlocks.at(2).second->value("trainOnBlock").toString().split("_");
	Q_ASSERT(trainOnBlock.size() == 3);
	QString trainId = trainOnBlock.at(0);
	Q_ASSERT(trainId == train->value("id").toString());
	//properties needed
	const QJsonObject* currBlockConObj = quintupleBlocks.at(2).first;
	bool isUnderground = currBlockConObj->value("underground").toBool();
	bool isNight = false;//TO-DO!! Implement time!

}




#endif // T3TrainModel_HPP
