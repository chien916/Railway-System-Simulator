#ifndef T3TRAINCONTROLLER_HPP
#define T3TRAINCONTROLLER_HPP
#include <QtCore>
class T3TrainController {
  public:
	static bool connectedToTrainModel;
	static void pidIterate(QJsonObject *train);
	static void ctrlSystIterate(QJsonObject *train, QJsonObject *currblockVariablesObject, const QJsonObject *currblockConstantsObject, const QTime currentTime);
};

/**
 * @brief T3TrainController::nc_pidIterate
 * @param train
 *
 * 使用条件：此函数只会在火车引擎控制系统开启时被调用
 */
inline void T3TrainController::pidIterate(QJsonObject *train) {
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

/**
 * @brief T3TrainController::nc_controlSystemIterate
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
inline void T3TrainController::ctrlSystIterate(QJsonObject *train, QJsonObject *currblockVariablesObject, const QJsonObject *currblockConstantsObject, const QTime currentTime) {
	//train id matching check
	//Q_ASSERT(quintupleBlocks.size() == 5);
	QStringList trainOnBlock = currblockVariablesObject->value("trainOnBlock").toString().split("_");
	Q_ASSERT(trainOnBlock.size() == 3);
	QString trainId = trainOnBlock.at(0);
	Q_ASSERT(trainId == train->value("id").toString());
	//properties needed
	bool isUnderground = currblockConstantsObject->value("underground").toBool();
	bool isNight = false;//TO-DO!! Implement time!
}
#endif // T3TRAINCONTROLLER_HPP
