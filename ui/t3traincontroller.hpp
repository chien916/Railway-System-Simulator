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
	static void updateControlSystemsOnAllTrains(QTime currentTime, MODU_ARGS_REF argsref);
	static void updatePiOnAllTrains(MODU_ARGS_REF argsref);
	static QString hardwareCmdEval(QString command, MODU_ARGS_REF argsref);
};

/**
 * @brief T3TrainController::nc_pidIterate
 * @param train
 *
 * 使用条件：此函数只会在火车引擎控制系统开启时被调用
 */
inline void T3TrainController::pidIterate(QJsonObject *train) {
//	//retrieve pid data from train database
//	float r = currTrain.value(QString("pid_r")).toDouble();
//	float prev_e = currTrain.value(QString("pid_prev_e")).toDouble();
//	float prev_y = currTrain.value(QString("pid_prev_y")).toDouble();
//	float sum_e = currTrain.value(QString("pid_sum_e")).toDouble();
//	float dt = currTrain.value(QString("pid_dt")).toDouble();
//	float kp = currTrain.value(QString("pid_kp")).toDouble();
//	float ki = currTrain.value(QString("pid_ki")).toDouble();
//	float kd = currTrain.value(QString("pid_kd")).toDouble();

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
	SET_TRAIN_F(trainId, "NC_R", MPH2KMH_F(metaInfo.at(6).toDouble()), argsref);
}

inline QJsonArray T3TrainController::getMetaInfo(const QString trainId, MODU_ARGS_REF argsref) {
	QVariantList metaInfo;
	metaInfo.push_back(GET_TRAIN_F(trainId, "COM[NC_NM]_SBRAKE", argsref).toBool());//0
	metaInfo.push_back(GET_TRAIN_F(trainId, "COM[NC_NM]_LDOOR", argsref).toBool());//1
	metaInfo.push_back(GET_TRAIN_F(trainId, "COM[NC_NM]_RDOOR", argsref).toBool());//2
	metaInfo.push_back(GET_TRAIN_F(trainId, "COM[NC_NM]_EXTLIGHT", argsref).toBool());//3
	metaInfo.push_back(GET_TRAIN_F(trainId, "COM[NC_NM]_INTLIGHT", argsref).toBool());//4
	metaInfo.push_back(GET_TRAIN_F(trainId, "COM[NC_NM]_EBRAKE", argsref).toBool());//5
	metaInfo.push_back(qRound(KMH2MPH_F(GET_TRAIN_F(trainId, "NC_R", argsref).toFloat()))); //current set point 6
	metaInfo.push_back(qRound(KMH2MPH_F(GET_TRAIN_F(trainId, "NC_PREVY", argsref).toFloat()) )); //7 current velocity
	{
		const float e = GET_TRAIN_F(trainId, "NC_PREVE", argsref).toFloat();
		const float kp = GET_TRAIN_F(trainId, "NC_KP", argsref).toFloat();
		const float ki = GET_TRAIN_F(trainId, "NC_KI", argsref).toFloat();
		const float p = e * kp;
		const float sum_e =  GET_TRAIN_F(trainId, "NC_SUME", argsref).toFloat();
		const float i = ki * sum_e;
		metaInfo.push_back(qRound(e)); //8 error
		metaInfo.push_back(qRound(p)); //9 p
		metaInfo.push_back(qRound(i)); //10 i
		metaInfo.push_back(QString::number(kp, 'f', 4)); //11
		metaInfo.push_back(QString::number(ki, 'f', 4)); //12
	}
	{
		const QString blockId = GET_TRAIN_F(trainId, "NM_BLOCKID", argsref).toString();
		if(blockId == "")
			qFatal("T3TrainController::getMetaInfo() -> BLOCK ID is empty.");
		const QString BCNPLCOUT = GET_TRACKVAR_F(blockId, "COM[KC|KM]_BCNPLCOUT", argsref).toString();
		metaInfo.push_back(static_cast<bool>(BCNPLCOUT[28] == '1'));//13 underground
		metaInfo.push_back((BCNPLCOUT.midRef(10, 8).toUInt(nullptr, 2)));//14 commanded speed
		metaInfo.push_back(static_cast<bool>(BCNPLCOUT[0] == '1'));//15 authority
		metaInfo.push_back((BCNPLCOUT.midRef(2, 8).toUInt(nullptr, 2)));//16 authority block number
		if(BCNPLCOUT[26] == '1' && BCNPLCOUT[27] == '1') {
			metaInfo.push_back(QString("PLATFORM | TRAIN | --------"));
		} else if(BCNPLCOUT[26] == '1') {
			metaInfo.push_back(QString("PLATFORM | TRAIN | PLATFORM"));
		} else if(BCNPLCOUT[27] == '1') {
			metaInfo.push_back(QString("-------- | TRAIN | PLATFORM"));
		} else {
			metaInfo.push_back(QString(""));
		}//17 stationinfo string
	}
	metaInfo.push_back(qAbs(qRound(GET_TRAIN_F(trainId, "NC_U", argsref).toFloat()))); //18 currPower

	return QJsonArray::fromVariantList(metaInfo);
}

inline void T3TrainController::updateControlSystemsOnAllTrains(QTime currentTime, MODU_ARGS_REF argsref) {
	for(const QJsonValue currTrainRaw : qAsConst(*std::get<4>(*argsref))) {
		QJsonObject currTrain = currTrainRaw.toObject();
		QString trainId = currTrain.value("NM_ID").toString();
		QString blockId = currTrain.value("NM_BLOCKID").toString();
		QString BCNPLCOUT = GET_TRACKVAR_F(blockId, "COM[KC|KM]_BCNPLCOUT", argsref).toString();
		//failure -> emergency brake activated!
		if(currTrain.value("COM[NC_NM]_FAILURE0").toBool()
				|| currTrain.value("COM[NC_NM]_FAILURE1").toBool()
				|| currTrain.value("COM[NC_NM]_FAILURE2").toBool()) {
			SET_TRAIN_F(trainId, "COM[NC_NM]_EBRAKE", true, argsref);
		}
		if(!currTrain.value("NM_AUTOMODE").toBool()) continue;
		//light control
		if(BCNPLCOUT.at(28) == '1' || currentTime < QTime(7, 00) || currentTime > QTime(19, 30)) { //if in tunnel, turn on interior light and exterior light
			SET_TRAIN_F(trainId, "COM[NC_NM]_EXTLIGHT", true, argsref);
			SET_TRAIN_F(trainId, "COM[NC_NM]_INTLIGHT", true, argsref);
		} else {
			SET_TRAIN_F(trainId, "COM[NC_NM]_EXTLIGHT", false, argsref);
			SET_TRAIN_F(trainId, "COM[NC_NM]_INTLIGHT", false, argsref);
		}
		//station  control[time left]
		int secondsLeft = currTrain.value("COM[NC_NM]_SECLEFT").toInt();
		if((BCNPLCOUT.at(26) == '1' || BCNPLCOUT.at(27) == '1')) {
			if(GET_TRAIN_F(trainId, "NC_PREVY", argsref).toInt() > 5) {
				SET_TRAIN_F(trainId, "COM[NC_NM]_SBRAKE", true, argsref);
			} else if(secondsLeft == -1) {
				secondsLeft = 20;
				SET_TRAIN_F(trainId, "COM[NC_NM]_SBRAKE", true, argsref);
			} else if(secondsLeft >= 5 && currTrain.value("NC_PREVY").toDouble() < 2.0) {
				if(BCNPLCOUT.at(26) == '1') SET_TRAIN_F(trainId, "COM[NC_NM]_LDOOR", true, argsref);
				else SET_TRAIN_F(trainId, "COM[NC_NM]_LDOOR", false, argsref);
				if(BCNPLCOUT.at(27) == '1') SET_TRAIN_F(trainId, "COM[NC_NM]_RDOOR", true, argsref);
				else SET_TRAIN_F(trainId, "COM[NC_NM]_RDOOR", false, argsref);
				SET_TRAIN_F(trainId, "COM[NC_NM]_SBRAKE", true, argsref);
				secondsLeft -= 1;
			} else if(secondsLeft < 5 && secondsLeft != -1) {
				SET_TRAIN_F(trainId, "COM[NC_NM]_RDOOR", false, argsref);
				SET_TRAIN_F(trainId, "COM[NC_NM]_LDOOR", false, argsref);
				SET_TRAIN_F(trainId, "COM[NC_NM]_SBRAKE", false, argsref);
				secondsLeft = -2;
			}
			SET_TRAIN_F(trainId, "COM[NC_NM]_SECLEFT", secondsLeft, argsref);
		} else if(secondsLeft == -2) {
			secondsLeft = -1;
			SET_TRAIN_F(trainId, "COM[NC_NM]_SECLEFT", secondsLeft, argsref);
		}
		//heater control
		float currCabinTemperature = currTrain.value("NM_TEMPERATURE").toDouble();
		bool heaterIsOn = currTrain.value("NM_HEATER").toBool();
		if(currCabinTemperature < 65.0f && !heaterIsOn) {
			heaterIsOn = true;
			currCabinTemperature += 0.01;
			SET_TRAIN_F(trainId, "NM_TEMPERATURE", currCabinTemperature, argsref);
			SET_TRAIN_F(trainId, "NM_HEATER", heaterIsOn, argsref);
		} else if(currCabinTemperature > 70.0f && heaterIsOn) {
			heaterIsOn = false;
			SET_TRAIN_F(trainId, "NM_HEATER", heaterIsOn, argsref);
		}
		//commanded speed pickups
		float commandedSpeed = BCNPLCOUT.midRef(10, 8).toUInt(nullptr, 2);
		qDebug() << "Picked up speed = " << commandedSpeed;
		SET_TRAIN_F(trainId, "NC_R", commandedSpeed, argsref);
	}
}

inline void T3TrainController::updatePiOnAllTrains(MODU_ARGS_REF argsref) {
	for(const QJsonValue currTrainRaw : qAsConst(*std::get<4>(*argsref))) {
		QJsonObject currTrain = currTrainRaw.toObject();
		if(!currTrain.value("NM_AUTOMODE").toBool()) continue;
		QString trainId = currTrain.value("NM_ID").toString();
		QString blockId = currTrain.value("NM_BLOCKID").toString();
		QString BCNPLCOUT = GET_TRACKVAR_F(blockId, "COM[KC|KM]_BCNPLCOUT", argsref).toString();
		float r = currTrain.value(("NC_R")).toDouble();
		float prev_y = currTrain.value(("NC_PREVY")).toDouble();
		float sum_e = currTrain.value(("NC_SUME")).toDouble();
		float dt = currTrain.value(("NC_DT")).toDouble();
		float kp = currTrain.value(("NC_KP")).toDouble();
		float ki = currTrain.value(("NC_KI")).toDouble();
//		if((BCNPLCOUT.at(26) == '1' || BCNPLCOUT.at(2) == '1') ) {//if at station
//			int secondsLeft = currTrain.value("COM[NC_NM]_SECLEFT").toInt();
//			if(secondsLeft == -1) {
//				SET_TRAIN_F(trainId, "COM[NC_NM]_SECLEFT", 30, argsref);//30 seconds of stop time
//				if(r != 0)
//					SET_TRAIN_F(trainId, "NC_R", 0.0f, argsref);//force train to stop
//			}
//		}
		//	//calculate new values
		float e = r - prev_y;
		float P = kp  * e * 20 ;
		float I = ki  * sum_e * 20 ;
		float u = (P + I);
		if(u > 120) u = 120;
		if(u < -120) u = -120;
		if(currTrain.value("COM[NC_NM]_EBRAKE").toBool() && prev_y > 0) {
			sum_e = e = u = 0;
		} else if(currTrain.value("COM[NC_NM]_SBRAKE").toBool() && prev_y > 0) {
			sum_e = e = u = 0;
		} else sum_e += e * dt;
		//float y = prev_y + u;


//		if(currTrain.value("COM[NC_NM]_EBRAKE").toBool() || currTrain.value("COM[NC_NM]_SBRAKE").toBool()) {
//			if(qAbs(prev_y) < 3) {
//				sum_e = 0;
//				e = 0;
//			}
//		}
		qDebug() << QString("E = %1 , VELOCITY = %2 -> (P = %3 , I = %4 ) -> POWER = %5")
				 .arg(e).arg(prev_y).arg(P).arg(I).arg(u);
		SET_TRAIN_F(trainId, "NC_SUME", (sum_e), argsref);
		SET_TRAIN_F(trainId, "NC_PREVE", (e), argsref);
		SET_TRAIN_F(trainId, "NC_U", u, argsref);
		//SET_TRAIN_F(trainId, "NC_PREVY", y, argsref);
		//SET_TRAIN_F(trainId, "NM_ACCELERATION", qMax(0.0f, y - prev_y), argsref);
	}
}

inline QString T3TrainController::hardwareCmdEval(QString command, MODU_ARGS_REF argsref) {

}


#endif // T3TRAINCONTROLLER_HPP
