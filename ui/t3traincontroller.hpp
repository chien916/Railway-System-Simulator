#ifndef T3TRAINCONTROLLER_HPP
#define T3TRAINCONTROLLER_HPP
#include "t3prophelper.h"

class T3TrainController {
  public:
	static bool connectedToTrainModel;
	static void pidIterate(QJsonObject *train);
	//static void ctrlSystIterate(QJsonObject *train, QJsonObject *currblockVariablesObject, const QJsonObject *currblockConstantsObject, const QTime currentTime);
	static void setKpAndKi(const QString trainId, float kp, float ki, MODU_ARGS_REF argsref);
	static void setBrake(const QString trainId, bool emergency, bool value, MODU_ARGS_REF argsref);
	static void setCtrlParams(const QString trainId, QJsonArray metaInfo, MODU_ARGS_REF argsref);
	static QJsonArray getMetaInfo(const QString trainId, MODU_ARGS_REF argsref);//does not include pid
};

/**
 * @brief T3TrainController::nc_pidIterate
 * @param train
 *
 * 使用条件：此函数只会在火车引擎控制系统开启时被调用
 */
inline void T3TrainController::pidIterate(QJsonObject *train) {
//	//retrieve pid data from train database
//	float r = train->value(QString("pid_r")).toDouble();
//	float prev_e = train->value(QString("pid_prev_e")).toDouble();
//	float prev_y = train->value(QString("pid_prev_y")).toDouble();
//	float sum_e = train->value(QString("pid_sum_e")).toDouble();
//	float dt = train->value(QString("pid_dt")).toDouble();
//	float kp = train->value(QString("pid_kp")).toDouble();
//	float ki = train->value(QString("pid_ki")).toDouble();
//	float kd = train->value(QString("pid_kd")).toDouble();

//	//calculate new values
//	float e = r - prev_y;
//	float P = kp * e;
//	float I = ki * sum_e * dt;
//	float D = kd * (prev_e + e) * dt * 0.5;//trapozoidal integration
//	float u = P + I + D;
//	float y = u;//CHANGE IT HERE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//	//overwrite pid data with new values
//	train->insert("velocity", y);
//	train->insert("pid_prev_y", y);
//	train->insert("pid_prev_e", e);
//	train->insert("pid_sum_e", sum_e + e);
}

inline void T3TrainController::setKpAndKi(const QString trainId, float kp, float ki, MODU_ARGS_REF argsref) {
	SET_TRAIN_F(trainId, "NC_KP", kp, argsref);
	SET_TRAIN_F(trainId, "NC_KI", ki, argsref);
}


inline void T3TrainController::setCtrlParams(const QString trainId, QJsonArray metaInfo, MODU_ARGS_REF argsref) {

	//meta info check
	Q_ASSERT(metaInfo.count() == 7);
	for(qsizetype i = 0; i < metaInfo.count(); ++i) {
		if(i != metaInfo.count() - 1)Q_ASSERT(metaInfo.at(i).isBool());
		else Q_ASSERT(metaInfo.at(i).isDouble());
	}
	SET_TRAIN_F(trainId, "COM[NC_NM]_SBRAKE", metaInfo.at(0).toBool(), argsref);
	SET_TRAIN_F(trainId, "COM[NC_NM]_EBRAKE", metaInfo.at(1).toBool(), argsref);
	SET_TRAIN_F(trainId, "COM[NC_NM]_LDOOR", metaInfo.at(2).toBool(), argsref);
	SET_TRAIN_F(trainId, "COM[NC_NM]_RDOOR", metaInfo.at(3).toBool(), argsref);
	SET_TRAIN_F(trainId, "COM[NC_NM]_EXTLIGHT", metaInfo.at(4).toBool(), argsref);
	SET_TRAIN_F(trainId, "COM[NC_NM]_INTLIGHT", metaInfo.at(5).toBool(), argsref);
	SET_TRAIN_F(trainId, "NC_R", metaInfo.at(6).toDouble(), argsref);
}

inline QJsonArray T3TrainController::getMetaInfo(const QString trainId, MODU_ARGS_REF argsref) {
	QVariantList metaInfo;
	metaInfo.push_back(GET_TRAIN_F(trainId, "COM[NC_NM]_SBRAKE", argsref).toBool());//0
	metaInfo.push_back(GET_TRAIN_F(trainId, "COM[NC_NM]_LDOOR", argsref).toBool());//1
	metaInfo.push_back(GET_TRAIN_F(trainId, "COM[NC_NM]_RDOOR", argsref).toBool());//2
	metaInfo.push_back(GET_TRAIN_F(trainId, "COM[NC_NM]_EXTLIGHT", argsref).toBool());//3
	metaInfo.push_back(GET_TRAIN_F(trainId, "COM[NC_NM]_INTLIGHT", argsref).toBool());//4
	metaInfo.push_back(GET_TRAIN_F(trainId, "COM[NC_NM]_EBRAKE", argsref).toBool());//5
	metaInfo.push_back(GET_TRAIN_F(trainId, "NC_R", argsref).toFloat());//current set point 6
	metaInfo.push_back(GET_TRAIN_F(trainId, "NC_PREVY", argsref).toFloat());//7 current velocity
	{
		const float e = GET_TRAIN_F(trainId, "NC_PREVE", argsref).toFloat();
		const float kp = GET_TRAIN_F(trainId, "NC_KP", argsref).toFloat();
		const float ki = GET_TRAIN_F(trainId, "NC_KI", argsref).toFloat();
		const float p = e * kp;
		const float sum_e =  GET_TRAIN_F(trainId, "NC_SUME", argsref).toFloat();
		const float i = ki * sum_e;
		metaInfo.push_back(e);//8 error
		metaInfo.push_back(p);//9 p
		metaInfo.push_back(i);//10 i
		metaInfo.push_back(kp);//11
		metaInfo.push_back(ki);//12
	}
	{
		const QString blockId = GET_TRAIN_F(trainId, "NM_BLOCKID", argsref).toString();
		if(blockId == "")
			qFatal("T3TrainController::getMetaInfo() -> BLOCK ID is empty.");
		const QString BCNPLCOUT = GET_TRACKVAR_F(blockId, "COM[KC|KM]_BCNPLCOUT", argsref).toString();
		metaInfo.push_back(static_cast<bool>(BCNPLCOUT[28] == '1'));//13 underground
		metaInfo.push_back(static_cast<float>(BCNPLCOUT.midRef(10, 8).toUInt(nullptr, 2)));//14 commanded speed
		metaInfo.push_back(static_cast<bool>(BCNPLCOUT[0] == '1'));//15 authority
		metaInfo.push_back(static_cast<float>(BCNPLCOUT.midRef(2, 8).toUInt(nullptr, 2)));//16 authority block number
		if(BCNPLCOUT[26] == '1' && BCNPLCOUT[27] == '1') {
			metaInfo.push_back(QString("PLATFORM | TRAIN |          "));
		} else if(BCNPLCOUT[26] == '1') {
			metaInfo.push_back(QString("PLATFORM | TRAIN | PLATFORM"));
		} else if(BCNPLCOUT[27] == '1') {
			metaInfo.push_back(QString("          | TRAIN | PLATFORM"));
		} else {
			metaInfo.push_back(QString(""));
		}//17 stationinfo string
	}
	metaInfo.push_back(GET_TRAIN_F(trainId, "NC_U", argsref).toFloat());//18 currPower

	return QJsonArray::fromVariantList(metaInfo);
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
//inline void T3TrainController::ctrlSystIterate(QJsonObject *train, QJsonObject *currblockVariablesObject, const QJsonObject *currblockConstantsObject, const QTime currentTime) {
//	//train id matching check
//	//Q_ASSERT(quintupleBlocks.size() == 5);
//	QStringList trainOnBlock = currblockVariablesObject->value("trainOnBlock").toString().split("_");
//	Q_ASSERT(trainOnBlock.size() == 3);
//	QString trainId = trainOnBlock.at(0);
//	Q_ASSERT(trainId == train->value("id").toString());
//	//properties needed
//	bool isUnderground = currblockConstantsObject->value("underground").toBool();
//	bool isNight = false;//TO-DO!! Implement time!
//}
#endif // T3TRAINCONTROLLER_HPP
