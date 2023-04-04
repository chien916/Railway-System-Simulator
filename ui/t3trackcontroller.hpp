#ifndef T3TRACKCONTROLLER_HPP
#define T3TRACKCONTROLLER_HPP

#include "t3prophelper.h"

class T3TrackController {
  public:
	static void iterate(MODU_ARGS_REF argsref, QJSEngine* plcRuntime, QJSValue* plcFunction);
	static bool connectedToCTCOffice;
	static bool connectedToTrackModel;
	static void addPlcScriptFromCsv(const QString filePath, QJSEngine*runTime, QJSValue* function);
	static void runPlcScript(QString &CTCPLCIO, QString &BCNPLCOUT, QString& KMPLCIO, QString& KCPLCIN
							 ,  QJSEngine*runTime,  QJSValue* function);
	static void processPlc(const QString blockId, QJSEngine* plcRuntime, QJSValue* plcFunction, MODU_ARGS_REF argsref);
	static void processAllPlc(QJSEngine* plcRuntime, QJSValue* plcFunction, MODU_ARGS_REF argsref);
	static QJsonArray readPlcToMetaInfo(const QString blockId, MODU_ARGS_REF argsref);
	static void writePlcFromMetaInfo(const QString blockId, const QJsonArray metaInfo, MODU_ARGS_REF argsref);
	static QJsonArray getAllPlcBinaries(const QString blockId, MODU_ARGS_REF argsref);
};

inline void T3TrackController::iterate(MODU_ARGS_REF argsref, QJSEngine* plcRuntime, QJSValue* plcFunction ) {
	//for each route
	for(int lineInd = 0; lineInd <  std::get<3>(*argsref)->size(); ++lineInd) {
		QStringList blockIds = std::get<3>(*argsref)->at(lineInd).toObject().keys();
		for(int blockInd = 0; blockInd < blockIds.size(); ++blockInd) {
			//operations for all blocks (very expensive)
		}
	}
}

inline void T3TrackController::addPlcScriptFromCsv(const QString filePath, QJSEngine*runTime, QJSValue* function) {
	QString newFilePath = QString(filePath).replace("file:///", "");
	QFile file(newFilePath);
	if (!file.open(QFile::ReadOnly | QFile::Text)) return;
	QTextStream in(&file);
	QString readInStr = in.readAll();
	QJSValue retrievedCode = runTime->evaluate("(" + readInStr + ")");
	//test run to see if script works
	if(retrievedCode.isCallable()) *function = retrievedCode;
	else qInfo() << "T3TrackController::addPlcScriptFromCsv() -> added PLC is not valid.";
}



inline void T3TrackController::runPlcScript(QString &CTCPLCIO, QString &BCNPLCOUT, QString &KMPLCIO, QString &KCPLCIN,  QJSEngine*runTime,  QJSValue* function) {
	QJsonArray CTCPLCIO_jsonArray, BCNPLCOUT_jsonArray, KMPLCIO_jsonArray, KCPLCIN_jsonArray;
	for(uint i = 0; i < 32; ++i) {
		CTCPLCIO_jsonArray.push_back(CTCPLCIO[i] == '1');
		KMPLCIO_jsonArray.push_back(KMPLCIO[i] == '1');
		KCPLCIN_jsonArray.push_back(KCPLCIN[i] == '1');
		BCNPLCOUT_jsonArray.push_back(BCNPLCOUT[i] == '1');
	}
	QJSValueList plcArgs = {
		runTime->toScriptValue(CTCPLCIO_jsonArray),
		runTime->toScriptValue(KCPLCIN_jsonArray),
		runTime->toScriptValue(KMPLCIO_jsonArray),
		runTime->toScriptValue(BCNPLCOUT_jsonArray)
	};
	if(!function->isCallable())
		qFatal( "T3TrackController::runPlcScript() -> PLC Script is not callable");
	function->call(plcArgs);
	CTCPLCIO_jsonArray = runTime->fromScriptValue<QJsonArray>(plcArgs.at(0));
	KCPLCIN_jsonArray = runTime->fromScriptValue<QJsonArray>(plcArgs.at(1));
	KMPLCIO_jsonArray = runTime->fromScriptValue<QJsonArray>(plcArgs.at(2));
	BCNPLCOUT_jsonArray = runTime->fromScriptValue<QJsonArray>(plcArgs.at(3));

	for(uint i = 0; i < 32; ++i) {
		CTCPLCIO[i] = CTCPLCIO_jsonArray.at(i).toBool() ? '1' : '0';
		KMPLCIO[i] = KMPLCIO_jsonArray.at(i).toBool() ? '1' : '0';
		KCPLCIN[i] = KCPLCIN_jsonArray.at(i).toBool() ? '1' : '0';
		BCNPLCOUT[i] = BCNPLCOUT_jsonArray.at(i).toBool() ? '1' : '0';
	}
}

inline void T3TrackController::processPlc(const QString blockId, QJSEngine *plcRuntime, QJSValue *plcFunction, MODU_ARGS_REF argsref) {
	QString CTCPLCIO = GET_TRACKVAR_F(blockId, "COM[CTC|KC]_CTCPLCIO", argsref).toString();
	QString BCNPLCOUT = GET_TRACKVAR_F(blockId, "COM[KC|KM]_BCNPLCOUT", argsref).toString();
	QString KMPLCIO = GET_TRACKVAR_F(blockId, "COM[KC|KM]_KMPLCIO", argsref).toString();
	QString KCPLCIN = GET_TRACKVAR_F(blockId, "KC_KCPLCIN", argsref).toString();
	runPlcScript(CTCPLCIO, BCNPLCOUT, KMPLCIO, KCPLCIN, plcRuntime, plcFunction);
	SET_TRACKVAR_F(blockId, "COM[CTC|KC]_CTCPLCIO", CTCPLCIO, argsref);
	SET_TRACKVAR_F(blockId, "COM[KC|KM]_BCNPLCOUT", BCNPLCOUT, argsref);
	SET_TRACKVAR_F(blockId, "COM[KC|KM]_KMPLCIO", KMPLCIO, argsref);
	SET_TRACKVAR_F(blockId, "KC_KCPLCIN", KCPLCIN, argsref);
}

inline void T3TrackController::processAllPlc(QJSEngine *plcRuntime, QJSValue *plcFunction, MODU_ARGS_REF argsref) {
	qDebug() << "I;m running";
	for(qsizetype i = 0; i < std::get<3>(*argsref)->size(); ++i) {//for each line
		QStringList allBlockIds = std::get<3>(*argsref)->at(i).toObject().keys();
		for(const QString& blockId : qAsConst(allBlockIds)) {//for each block on current line
			processPlc(blockId, plcRuntime, plcFunction, argsref);
		}
	}

}

inline QJsonArray T3TrackController::readPlcToMetaInfo(const QString blockId, MODU_ARGS_REF argsref) {
	QString KCPLCIN = GET_TRACKVAR_F(blockId, "KC_KCPLCIN", argsref).toString();
	QString BCNPLCOUT = GET_TRACKVAR_F(blockId, "COM[KC|KM]_BCNPLCOUT", argsref).toString();
	QList<QVariant> metaInfo = {
		KCPLCIN.at(1) == '1',//0-maintance mode
		static_cast<float>(KMH2MPH_F(GET_TRACKCON_F(blockId, "speedLimit", argsref).toUInt())),//1-speed limit
		static_cast<float>(KMH2MPH_F(GET_TRACKVAR_F(blockId, "COM[CTC|KC]_CTCPLCIO", argsref).toString().midRef(2, 8).toUInt(nullptr, 2))),//2-suggested speed
		static_cast<float>(KMH2MPH_F(KCPLCIN.midRef(2, 8).toUInt(nullptr, 2))),//3-comanded speed
		QString("red"),//4-left signal -> handles later
		QString("up"),//5-switch -> handles later
		KCPLCIN.at(13) == '1' ? "closed" : "open",//6-gate
		QString("red"),//7-right signal -> handles later
		KCPLCIN.at(16) == '1',//8-auth prev next
		KCPLCIN.at(17) == '1',//9-auth up down
		static_cast<float>(KCPLCIN.midRef(2, 8).toUInt(nullptr, 2))//10-auth block number
	};
	{
		//left signal
		QStringRef leftSignal = KCPLCIN.midRef(10, 2);
		if(leftSignal == "01") metaInfo[4] = QString("yellow");
		else if(leftSignal == "10") metaInfo[4] = QString("green");
		else metaInfo[4] = QString("red");

		//right signal
		QStringRef rightSignal = KCPLCIN.midRef(14, 2);
		if(rightSignal == "01") metaInfo[7] = QString("yellow");
		else if(rightSignal == "10") metaInfo[7] = QString("green");
		else metaInfo[7] = QString("red");

		//switch position
		QStringRef switchPosition = KCPLCIN.midRef(26, 2);
		if(switchPosition == "11") metaInfo[5] = QString("top");
		else if(switchPosition == "01")metaInfo[5] = QString("bottom");
		else metaInfo[5] = QString("auto");
	}
	QList<QVariant> metaInfo2 = {
		KCPLCIN.at(1) == '1',//0-maintance mode
		static_cast<float>(KMH2MPH_F(GET_TRACKCON_F(blockId, "speedLimit", argsref).toUInt())),//1-speed limit
		static_cast<float>(KMH2MPH_F(GET_TRACKVAR_F(blockId, "COM[CTC|KC]_CTCPLCIO", argsref).toString().midRef(2, 8).toUInt(nullptr, 2))),//2-suggested speed
		static_cast<float>(KMH2MPH_F(BCNPLCOUT.midRef(10, 8).toUInt(nullptr, 2))),//3-comanded speed
		QString("red"),//4-left signal -> handles later
		QString("up"),//5-switch -> handles later
		KCPLCIN.at(13) == '1' ? "closed" : "open",//6-gate
		QString("red"),//7-right signal -> handles later
		BCNPLCOUT.at(0) == '1',//8-auth prev next
		BCNPLCOUT.at(1) == '1',//9-auth up down
		static_cast<float>(BCNPLCOUT.midRef(2, 8).toUInt(nullptr, 2))//10-auth block number
	};
	{
		//left signal
		QStringRef leftSignal = KCPLCIN.midRef(10, 2);
		if(leftSignal == "01") metaInfo2[4] = QString("yellow");
		else if(leftSignal == "10") metaInfo2[4] = QString("green");
		else metaInfo2[4] = QString("red");

		//right signal
		QStringRef rightSignal = KCPLCIN.midRef(14, 2);
		if(rightSignal == "01") metaInfo2[7] = QString("yellow");
		else if(rightSignal == "10") metaInfo2[7] = QString("green");
		else metaInfo2[7] = QString("red");

		//switch position
		QStringRef switchPosition = KCPLCIN.midRef(26, 2);
		if(switchPosition == "11") metaInfo2[5] = QString("top");
		else if(switchPosition == "01")metaInfo2[5] = QString("bottom");
		else metaInfo2[5] = QString("auto");
	}
	/**
	 *  * inputs from track controller
	 * -> connection[0]
	 * -> maintanacdeMode[1]
	 * -> manual commanded speed[2:9]
	 * -> left signal[10:11] //CAN BE OVERWRITTEN UNDER MAINTANCE MODE
	 * -> RESERVED[12]
	 * -> gate [13] //CAN BE OVERWRITTEN UNDER MAINTANCE MODE
	 * -> right signal [14:15] //CAN BE OVERWRITTEN UNDER MAINTANCE MODE
	 * -> authority direction [16:17] //CAN BE OVERWRITTEN UNDER MAINTANCE MODE
	 * -> authority block numbers [18:25] //CAN BE OVERWRITTEN UNDER MAINTANCE MODE
	 * -> switch position from kc [26:27] //00-auto 01-bottom 11-top //CAN BE OVERWRITTEN UNDER MAINTANCE MODE
	 */
	return QJsonArray{QJsonArray::fromVariantList(metaInfo), QJsonArray::fromVariantList(metaInfo2)};
}

inline void T3TrackController::writePlcFromMetaInfo(const QString blockId, const QJsonArray metaInfo, MODU_ARGS_REF argsref) {


	if(metaInfo.size() != 11
			|| !metaInfo.at(0).isBool()//maintananceMode
			|| !metaInfo.at(1).isDouble()//no use
			|| !metaInfo.at(2).isDouble()//no use
			|| !metaInfo.at(3).isDouble()//commanded speed
			|| !metaInfo.at(4).isString()//left signalk
			|| !metaInfo.at(5).isString()//switch
			|| !metaInfo.at(6).isString()//gate
			|| !metaInfo.at(7).isString()//right signal
			|| !metaInfo.at(8).isBool()//auth direction 1-next 0-prev
			|| !metaInfo.at(9).isBool()//auth switch 1-up 0-down
			|| !metaInfo.at(10).isDouble()//number of authorized blocks
	  ) qFatal( "T3TrackController::writePlcFromMetaInfo() -> meta info bad format");


	QString KCPLCIN = GET_TRACKVAR_F(blockId, "KC_KCPLCIN", argsref).toString();
	/**
	 *  * inputs from track controller
	 * -> connection[0]
	 * -> maintanacdeMode[1]
	 * -> manual commanded speed[2:9]
	 * -> left signal[10:11] //CAN BE OVERWRITTEN UNDER MAINTANCE MODE
	 * -> RESERVED[12]
	 * -> gate [13] //CAN BE OVERWRITTEN UNDER MAINTANCE MODE
	 * -> right signal [14:15] //CAN BE OVERWRITTEN UNDER MAINTANCE MODE
	 * -> authority direction [16:17] //CAN BE OVERWRITTEN UNDER MAINTANCE MODE
	 * -> authority block numbers [18:25] //CAN BE OVERWRITTEN UNDER MAINTANCE MODE
	 * -> switch position from kc [26:27] //00-auto 01-bottom 11-top //CAN BE OVERWRITTEN UNDER MAINTANCE MODE
	 */
	{
		//commanded speed
		QString commandedSpeed = QString::number(static_cast<uint8_t>(metaInfo.at(3).toDouble()), 2).rightJustified(8, '0');
		for(uint i = 0; i < 7; ++i) KCPLCIN[2 + i] = commandedSpeed[i];
		//number of authorized block
		QString numberOfAuthorizedBlocks = QString::number(static_cast<uint8_t>(metaInfo.at(10).toDouble()), 2).rightJustified(8, '0');
		for(uint i = 0; i < 7; ++i) KCPLCIN[18 + i] = numberOfAuthorizedBlocks[i];
	}
	{
		//left signal
		QString leftSignal = metaInfo.at(4).toString();
		if(leftSignal == "yellow") leftSignal = "01";
		else if(leftSignal == "green") leftSignal = "10";
		else leftSignal = "00";
		KCPLCIN[10] = leftSignal[0];
		KCPLCIN[11] = leftSignal[1];

		//right signal
		QString rightSignal = metaInfo.at(5).toString();
		if(rightSignal == "yellow") rightSignal = "01";
		else if(rightSignal == "green") rightSignal = "10";
		else rightSignal = "00";
		KCPLCIN[14] = rightSignal[0];
		KCPLCIN[15] = rightSignal[1];

		//switch position
		QString switchPosition = metaInfo.at(5).toString();
		if(switchPosition == "up") switchPosition = "11";
		else if(switchPosition == "down") switchPosition = "01";
		else switchPosition = "00";
	}
	{
		//maintananceMode
		KCPLCIN[1] = metaInfo.at(0).toBool() ? '1' : '0';
		//gate
		KCPLCIN[13] = metaInfo.at(6).toString() == "closed" ? '1' : '0';
		//authority direction prev or next
		KCPLCIN[16] = metaInfo.at(8).toBool() ? '1' : '0';
		KCPLCIN[17] = metaInfo.at(9).toBool() ? '1' : '0';
	}
	SET_TRACKVAR_F(blockId, "KC_KCPLCIN", KCPLCIN, argsref);

}

inline QJsonArray T3TrackController::getAllPlcBinaries(const QString blockId, MODU_ARGS_REF argsref) {

	QString CTCPLCIO = 	GET_TRACKVAR_F(blockId, "COM[CTC|KC]_CTCPLCIO", argsref).toString();
	QString BCNPLCOUT = 	GET_TRACKVAR_F(blockId, "COM[KC|KM]_BCNPLCOUT", argsref).toString();
	QString KMPLCIO = 	GET_TRACKVAR_F(blockId, "COM[KC|KM]_KMPLCIO", argsref).toString();
	QString KCPLCIN = 	GET_TRACKVAR_F(blockId, "KC_KCPLCIN", argsref).toString();
	QList<QVariant> CTCPLCIO_booleanArray, BCNPLCOUT_booleanArray, KMPLCIO_booleanArray, KCPLCIN_booleanArray;
	for(uint i = 0 ; i < 32; ++i) {
		CTCPLCIO_booleanArray.push_back(static_cast<bool>(CTCPLCIO.at(i) == '1'));
		BCNPLCOUT_booleanArray.push_back(static_cast<bool>(BCNPLCOUT.at(i) == '1'));
		KMPLCIO_booleanArray.push_back(static_cast<bool>(KMPLCIO.at(i) == '1'));
		KCPLCIN_booleanArray.push_back(static_cast<bool>(KCPLCIN.at(i) == '1'));
	}
	return QJsonArray(std::initializer_list<QJsonValue> {
		QJsonArray::fromVariantList(CTCPLCIO_booleanArray),
		QJsonArray::fromVariantList(KMPLCIO_booleanArray),
		QJsonArray::fromVariantList(KCPLCIN_booleanArray),
		QJsonArray::fromVariantList(BCNPLCOUT_booleanArray)
	});
}


#endif // T3TRACKCONTROLLER_HPP
