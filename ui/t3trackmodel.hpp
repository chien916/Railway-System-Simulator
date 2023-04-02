#ifndef T3TRACKMODEL_H
#define T3TRACKMODEL_H
#include "t3prophelper.h"

class T3TrackModel {
  public:
	static void placeTrain(const QString trainId, const QString blockId, bool isMovingForward, MODU_ARGS_REF argsref);
	static void placeTrainFromDispatchRequest(const QList<QJsonObject>* requests, MODU_ARGS_REF argsref);
	static void addTrackFromCsv(const QString filePath, QJsonObject* stationToIdMapObject, MODU_ARGS_REF argsref);
	static QJsonArray getAllTrackIds(MODU_ARGS_REF argsref);
	static QJsonArray getDisplayStrings(const QString blockId, MODU_ARGS_REF argsref);
	static QJsonArray getIOMetaInfo(const QString blockId, MODU_ARGS_REF argsref);
	static void setIOMetaInfo(const QString blockId, QJsonArray metaInfo, MODU_ARGS_REF argsref );
  private:

};



inline void T3TrackModel::addTrackFromCsv(const QString filePath, QJsonObject* stationToIdMapObject, MODU_ARGS_REF argsref) {
	QString newFilePath = QString(filePath).replace("file:///", "");
	qsizetype requiredColumnCount = 0;
	QFile file(newFilePath);
	if(!newFilePath.contains(".csv")) {
		qFatal("T3TrackModel::addTrackFromCsv() -> CSV bad format: file received is not csv extensions");
	}
	if(!file.open(QIODevice::ReadOnly)) {
		qFatal(file.errorString().toUtf8());
	}
	QList<QList<QString>> titles;
	QJsonObject currTrackConObj;
	QJsonObject currTrackVarObj;
	QPair<QString, QString> startingBlock{QString(), QString()};
	QPair<QString, QString> endingBlock{QString(), QString()};
	while(!file.atEnd()) {//each iteration is one block (one line in csv)
		QString currLineRaw = QString::fromUtf8(file.readLine()).trimmed();
		if(titles.empty()) {
			if(currLineRaw.contains
					("blockId_s,length_r,grade_r,speedLimit_r,elevation_r,prevBlock1_s,prevBlock2_s,nextBlock1_s,nextBlock2_s,underground_b,crossing_b,station_s,direction_s")) {
				QStringList titleAndTypes = currLineRaw.split(',');
				requiredColumnCount = titleAndTypes.count();
				foreach (const QString& currTitleAndTypes, titleAndTypes) {
					QStringList TitleAndTypesSplitted = currTitleAndTypes.split('_');
					if(TitleAndTypesSplitted.length() != 2)
						qFatal("T3TrackModel::addTrackFromCsv() -> CSV bad format: title name and type format wrong");
					titles.append(TitleAndTypesSplitted);
				}
			} else
				qFatal("T3TrackModel::addTrackFromCsv() -> CSV bad format: title doesn't match.");
			continue;
		}
		QStringList currLineSplitted = currLineRaw.split(',');
		if(currLineSplitted.count() != requiredColumnCount) {
			qFatal("T3TrackModel::addTrackFromCsv() -> CSV bad format: content column count mismatch.");
		}



		//intializing fields for constants for this block
		QString KMPLCIO = QString(32, '0');
		QJsonObject currBlockConObj;
		{
			for(qsizetype i = 1; i < requiredColumnCount; ++i) {//block id is eliminated at each object
				QString currType = titles.at(i).at(1);
				QString currTitle = titles.at(i).at(0);
				QString currValueAsString = currLineSplitted.at(i);
				if(currType == "s") {
					if(currValueAsString.contains("START_T")) {
						startingBlock.first = currLineSplitted.at(0);
					} else if(currValueAsString.contains("START_B")) {
						startingBlock.second = currLineSplitted.at(0);
					} else if(currValueAsString.contains("END_T")) {
						endingBlock.first = currLineSplitted.at(0);
					} else if(currValueAsString.contains("END_B")) {
						endingBlock.second = currLineSplitted.at(0);
					}
					if(currTitle == "station" && !currValueAsString.isEmpty()) {
						QStringList stationWithSides = currValueAsString.split("_");
						if(stationWithSides.length() != 2)
							qFatal("T3TrackModel::addTrackFromCsv() -> Station name with side spllitted length is not 2.");
						QJsonArray toInsert;
						if(stationToIdMapObject->contains(stationWithSides.at(0)))
							toInsert = stationToIdMapObject->value(stationWithSides.at(0)).toArray();
						toInsert.push_back(currLineSplitted.at(0));
						stationToIdMapObject->insert(stationWithSides.at(0), toInsert);
						if(stationWithSides.at(1).contains("L")) KMPLCIO[26] = '1';
						if(stationWithSides.at(1).contains("R")) KMPLCIO[27] = '1';
					}
					if(currTitle == "direction") {
						if(currValueAsString == "BIDIRECTIONAL") {
							KMPLCIO[22] = KMPLCIO[23] = '1';
						} else if(currValueAsString == "FORWARD") {
							KMPLCIO[22] = '1';
						} else if(currValueAsString == "REVERSED") {
							KMPLCIO[23] = '1';
						}
					}
					if(currTitle == "nextBlock2" && currValueAsString != "" && currValueAsString != "PASSIVE")
						KMPLCIO[25] = '1';
					if(currTitle == "prevBlock2" && currValueAsString != ""  && currValueAsString != "PASSIVE")
						KMPLCIO[24]  = '1';
					currBlockConObj.insert(currTitle, currValueAsString);
				} else if(currType == "b") {
					if(currValueAsString == "TRUE") {
						currBlockConObj.insert(currTitle, true);
						if(currTitle == "crossing") KMPLCIO[29] = '1';
						if(currTitle == "underground") KMPLCIO[28] = '1';
					} else if(currValueAsString == "FALSE")
						currBlockConObj.insert(currTitle, false);
					else
						qFatal("T3TrackModel::addTrackFromCsv() -> CSV bad format: title stated as boolean but value is not boolean");

				} else if(currType == "f" || currType == "r") {
					bool conversionIsSucessful;
					float currValue = currValueAsString.toFloat(&conversionIsSucessful);
					if(!conversionIsSucessful)
						qFatal("T3TrackModel::addTrackFromCsv() -> CSV bad format: title stated as float but value is not float");
					currBlockConObj.insert(currTitle, currValue);
					if(currTitle == "speedLimit") {
						QString truncatedSpeedLimit = QString::number(static_cast<uint8_t>(currValue), 2).rightJustified(8, '0');
						for(uint i = 15; i >= 8; --i) {
							KMPLCIO[i] = truncatedSpeedLimit[i - 8];
						}
					}
				} else if(currType == "i") {
					bool conversionIsSucessful;
					int currValue = currValueAsString.toInt(&conversionIsSucessful);
					if(!conversionIsSucessful)
						qFatal("T3TrackModel::addTrackFromCsv() -> CSV bad format: title stated as float but value is not float");
					currBlockConObj.insert(currTitle, currValue);
				} else {
					qFatal("T3TrackModel::addTrackFromCsv() -> CSV bad format: title type not recognized");
				}
			}

		}

		//initializing fields for variables for this block
		QJsonObject currBlockVarObj;
		{
			currBlockVarObj.insert("CTC_AUTHPATH", QString());
			currBlockVarObj.insert("COM[CTC|KC]_CTCPLCIO", QString(32, '0'));
			currBlockVarObj.insert("KC_KCPLCIN", QString(32, '0'));
			currBlockVarObj.insert("COM[KC|KM]_KMPLCIO", KMPLCIO);
			currBlockVarObj.insert("COM[KC|KM]_BCNPLCOUT", QString(32, '0'));
			currBlockVarObj.insert("KM_TRAINONBLOCK", QString());
			currBlockVarObj.insert("KM_HEATER", false);
			currBlockVarObj.insert("KM_PEOPLEONSTATION", static_cast<uint16_t>(0));
			currBlockVarObj.insert("KM_ENVTEMPERATURE", static_cast<uint16_t>(0));
		}

		currTrackConObj.insert(currLineSplitted.at(0), currBlockConObj);
		currTrackVarObj.insert(currLineSplitted.at(0), currBlockVarObj);

	}
	if(startingBlock.first.isEmpty() || endingBlock.first.isEmpty()) {
		qFatal("T3TrackModel::addTrackFromCsv() -> CSV bad format: starting and ending block cannot be found.");
	}

	//add a wrapper layer to constants object to include starting and ending block for UI rendering purposes
	{
		QJsonObject currTrackConObjWrapper;
		currTrackConObjWrapper.insert("startingBlock1", startingBlock.first);
		currTrackConObjWrapper.insert("endingBlock1", endingBlock.first);
		currTrackConObjWrapper.insert("startingBlock2", startingBlock.second);
		currTrackConObjWrapper.insert("endingBlock2",  endingBlock.second);
		currTrackConObjWrapper.insert("blocksMap", currTrackConObj);
		std::get<3>(*argsref)->push_back(currTrackVarObj);
		std::get<2>(*argsref)->push_back(currTrackConObjWrapper);
	}

	QFile fileToWriteLog(QString(newFilePath).replace(".csv", "_ConstantsJson.json"));
	if(fileToWriteLog.open(QFile::WriteOnly) && std::get<2>(*argsref)->count() > 0 && std::get<2>(*argsref)->at(0).isObject())
		fileToWriteLog.write(QJsonDocument(std::get<2>(*argsref)->last().toObject()).toJson());
	else
		qFatal(fileToWriteLog.errorString().toUtf8());

	QFile fileToWriteLog2(QString(newFilePath).replace(".csv", "_VariablesJson.json"));
	if(fileToWriteLog2.open(QFile::WriteOnly) && std::get<3>(*argsref)->count() > 0 && std::get<3>(*argsref)->at(0).isObject())
		fileToWriteLog2.write(QJsonDocument(std::get<3>(*argsref)->last().toObject()).toJson());
	else
		qFatal(fileToWriteLog.errorString().toUtf8());
	fileToWriteLog2.close();
}

inline QJsonArray T3TrackModel::getAllTrackIds(MODU_ARGS_REF argsref) {
	QJsonArray toRet;
	for(const QJsonValue& currTrackLineObject : qAsConst(*std::get<2>(*argsref))) {//use track constants object
		Q_ASSERT(currTrackLineObject.isObject() && currTrackLineObject.toObject().contains("blocksMap"));
		toRet.push_back(QJsonArray::fromStringList(currTrackLineObject.toObject().value("blocksMap").toObject().keys()));
	}
	return toRet;
}

inline QJsonArray T3TrackModel::getDisplayStrings(const QString blockId, MODU_ARGS_REF argsref) {
	QVector<QString> retRaw(30, QString());
	QString KMPLCIO = GET_TRACKVAR_F(blockId, "COM[KC|KM]_KMPLCIO", argsref).toString();
	QString BCNPLCOUT  = GET_TRACKVAR_F(blockId, "COM[KC|KM]_BCNPLCOUT", argsref).toString();
	QStringRef helper;
	uint numberHelper = 0.0f;
	retRaw[0] = " ";//	,"/TRACK PROPERTIES"
	retRaw[1] = blockId.split("_").join(" ");//	,"Track ID",
	retRaw[2] = QString::number((GET_TRACKCON_F(blockId, "grade", argsref).toFloat()), 'g', 3); //	,"Grade"
	retRaw[3] = QString::number(M2FOOT_F(GET_TRACKCON_F(blockId, "elevation", argsref).toFloat()), 'g', 3) + " ft"; //	,"Elevation"
	retRaw[4] = QString::number(M2YARD_F(GET_TRACKCON_F(blockId, "length", argsref).toFloat()), 'g', 3) + " yd"; //	,"Length"
	retRaw[5] = QString::number(KMH2MPH_F(GET_TRACKCON_F(blockId, "speedLimit", argsref).toFloat()), 'g', 3) + " mph";//	,"Speed Limit"
	retRaw[6] = GET_TRACKCON_F(blockId, "direction", argsref).toString();//	,"Direction of Travel"
	retRaw[7] = GET_TRACKCON_F(blockId, "crossing", argsref).toBool() ? "-" : KMPLCIO[19] == '1' ? "CLOSED" : "OPEN"; //	,"Railway Crossings"
	retRaw[8] = GET_TRACKVAR_F(blockId, "KM_HEATER", argsref).toBool() ? "ON" : "OFF"; //	,"Track Heaters"
	retRaw[9] = " ";//	,"/TRACK STATUS"
	retRaw[10] = "-";//	"Enviroment Temperature"
	retRaw[11] = KMPLCIO[3] == '1' ? "ON" : "OFF";//	,"Train Occupancy"
	retRaw[12] = (KMPLCIO[24] == '1' || KMPLCIO[25] == '1') ? (
					 QString(KMPLCIO[24] == '1' ? "LEFT " : "RIGHT ") + QString(KMPLCIO[18] == '1' ? "UP" : "DOWN")
				 ) : "-"; //	,"Switch Position"
	helper = KMPLCIO.midRef(16, 2);
	retRaw[13] = KMPLCIO[22] == '1' ? (
					 helper == "00" ? "RED" :	(helper == "01" ? "YELLOW" : "GREEN")
				 ) : "-"; //	,"Left Signal Light"
	helper = KMPLCIO.midRef(20, 2);
	retRaw[14] = KMPLCIO[23] == '1' ? (
					 helper == "00" ? "RED" :	(helper == "01" ? "YELLOW" : "GREEN")
				 ) : "-";;//	,"Right Signal Light"
	retRaw[15] = " ";//	,"/BEACON"
	retRaw[16] = BCNPLCOUT[28] == '1' ? "YES" : "NO";//	,"Underground"
	helper = BCNPLCOUT.midRef(2, 8);
	numberHelper = helper.toUInt(nullptr, 2);
	retRaw[18] = QString::number(numberHelper);//	,"Authority Block Number"
	helper = BCNPLCOUT.midRef(0, 2);
	retRaw[17] = numberHelper == 0 ? "-" : (QString(helper[1] == '1' ? "TOP " : "BOTTOM ") + QString(helper[0] == '1' ? "BACK" : "FORWARD")); //	,"Authority Direction"
	helper = BCNPLCOUT.midRef(10, 8);
	numberHelper = helper.toUInt(nullptr, 2);
	retRaw[19] = QString::number(numberHelper);//	,"Commanded Speed"
	retRaw[20] = " ";//	,"/STATION PROPERTIES"
	QStringList stationhelper = GET_TRACKCON_F(blockId, "station", argsref).toString().split("_");
	retRaw[21] = (stationhelper.size() == 0 || stationhelper.first() == "") ? "-" : stationhelper.at(0); //	,"Station Name"
	retRaw[22] = (stationhelper.size() == 0 || stationhelper.first() == "") ? "-" :
				 (QString(stationhelper.at(1).contains("L") ? "LEFT " : "")
				  + QString(stationhelper.at(1).contains("R") ? "RIGHT" : "")).trimmed(); //	,"Station Sides"
	retRaw[23] = QString::number(GET_TRACKVAR_F(blockId, "KM_PEOPLEONSTATION", argsref).toUInt());//	,"People on Station"
	retRaw[24] = "?";//	,"People Boarding"
	retRaw[25] = "?";//	,"People Disembarking"
	retRaw[26] = " ";//	,"/FAILURE MODES"
	retRaw[27] = KMPLCIO[6] == '1' ? "YES" : "NO" ; //	,"Broken Rail Failure"
	retRaw[28] = KMPLCIO[7] == '1' ? "YES" : "NO" ; //	,"Track Circuit Failure"
	retRaw[29] = KMPLCIO[30] == '1' ? "YES" : "NO" ;//	,"Power Failure"
	return QJsonArray::fromStringList(QStringList::fromVector(retRaw));
}

inline QJsonArray T3TrackModel::getIOMetaInfo(const QString blockId, MODU_ARGS_REF argsref) {
	QString KMPLCIO = GET_TRACKVAR_F(blockId, "COM[KC|KM]_KMPLCIO", argsref).toString();
	QVariantList metaInfo = {
		static_cast<float>(GET_TRACKVAR_F(blockId, "KM_ENVTEMPERATURE", argsref).toUInt()),//enviroment temperature
		//failture mode 1
		static_cast<bool>(KMPLCIO[6] == '1'),
		//failure mode 2
		static_cast<bool>(KMPLCIO[7] == '1'),
		//failure mode 3
		static_cast<bool>(KMPLCIO[30] == '1')
	};
	return QJsonArray::fromVariantList(metaInfo);
}

inline void T3TrackModel::setIOMetaInfo(const QString blockId, QJsonArray metaInfo, MODU_ARGS_REF argsref) {
	if(metaInfo.size() != 4
			|| !metaInfo.at(0).isDouble()
			|| !metaInfo.at(1).isBool()
			|| !metaInfo.at(2).isBool()
			|| !metaInfo.at(3).isBool())
		qFatal("T3TrackModel::setIOMetaInfo() -> meta info bad format");
	QString KMPLCIO = GET_TRACKVAR_F(blockId, "COM[KC|KM]_KMPLCIO", argsref).toString();
	SET_TRACKVAR_F(blockId, "KM_ENVTEMPERATURE", static_cast<uint8_t>(qRound(metaInfo.at(0).toDouble())), argsref);
	KMPLCIO[6] = metaInfo.at(1).toBool() ? '1' : '0';
	KMPLCIO[7] = metaInfo.at(2).toBool() ? '1' : '0';
	KMPLCIO[30] = metaInfo.at(3).toBool() ? '1' : '0';
	SET_TRACKVAR_F(blockId, "COM[KC|KM]_KMPLCIO", KMPLCIO, argsref);
}


inline void T3TrackModel::placeTrain(const QString trainId, const QString blockId, bool isMovingForward, MODU_ARGS_REF argsref) {
	QString trainOnBlockConcatStr = QString("%1_%2_0.5").arg(trainId, (isMovingForward ? "F" : "R"));//direction doesnt matter for now
	SET_TRACKVAR_F(blockId, "KM_TRAINONBLOCK", trainOnBlockConcatStr, argsref);
}

inline void T3TrackModel::placeTrainFromDispatchRequest(const QList<QJsonObject> *poppedRequests, MODU_ARGS_REF argsref) {
	for(const QJsonObject& currRequest : *poppedRequests) {
		QString trainId = currRequest.value("trainId").toString();
		QString origin = currRequest.value("origin").toString();
		QString destination = currRequest.value("destination").toString();
		QJsonArray path = currRequest.value("path").toArray();
		//determine if the train is moving forward or backward
		bool isForward = true;
		QString trainOnBlockConcatStr = QString("%1_%2_0.0").arg(trainId).arg(isForward ? "F" : "R");
		//place train on track
		SET_TRACKVAR_F(origin, "KM_TRAINONBLOCK", trainOnBlockConcatStr, argsref);
	}
}

//inline void T3TrackModel::trackIterate(MODU_ARGS_REF argsref) {
//	for(qsizetype i = 0; i < std::get<3>(*argsref)->count(); ++i) {
//		QJsonObject currTrackVarObj = std::get<3>(*argsref)->at(i).toObject();
//		const QJsonObject currTrackConObj = std::get<2>(*argsref)->at(i).toObject().value("blocksMap").toObject();
//		QStringList blockIds = currTrackVarObj.keys();
//		for(qsizetype j = 0; j < blockIds.count(); ++j) {

//			//determine prev2 prev1 next1 next2 blockId and object
//			QString currBlockId = blockIds.at(j);
//			const QJsonObject currBlockConObj = currTrackConObj.value(currBlockId).toObject();
//			QJsonObject currBlockVarObj = currTrackVarObj.value(currBlockId).toObject();

//			bool prevViewReversed = false, nextViewReversed = false;

//			QString prev1BlockId
//				= getPrevOrNextBlock(currBlockId, &currTrackConObj, &currTrackVarObj, true != prevViewReversed, &prevViewReversed);
//			QString prev2BlockId
//				= getPrevOrNextBlock(prev1BlockId, &currTrackConObj, &currTrackVarObj, true != prevViewReversed, &prevViewReversed);
//			QString next1BlockId
//				= getPrevOrNextBlock(currBlockId, &currTrackConObj, &currTrackVarObj, true != nextViewReversed, &nextViewReversed);
//			QString next2BlockId
//				= getPrevOrNextBlock(next1BlockId, &currTrackConObj, &currTrackVarObj, true != nextViewReversed, &nextViewReversed);

//			const QJsonObject prev1BlockConObj = currTrackConObj.value(prev1BlockId).toObject();
//			const QJsonObject prev2BlockConObj = currTrackConObj.value(prev2BlockId).toObject();
//			const QJsonObject next1BlockConObj = currTrackConObj.value(next1BlockId).toObject();
//			const QJsonObject next2BlockConObj = currTrackConObj.value(next2BlockId).toObject();

//			QJsonObject prev1BlockVarObj = currTrackVarObj.value(prev1BlockId).toObject();
//			QJsonObject prev2BlockVarObj = currTrackVarObj.value(prev2BlockId).toObject();
//			QJsonObject next1BlockVarObj = currTrackVarObj.value(next1BlockId).toObject();
//			QJsonObject next2BlockVarObj = currTrackVarObj.value(next2BlockId).toObject();

//			QVarLengthArray<QPair<const QJsonObject*, QJsonObject*>, 5> quintupleBlocks = {
//				qMakePair(&prev2BlockConObj, &prev2BlockVarObj)
//				, qMakePair(&prev1BlockConObj, &prev1BlockVarObj)
//				, qMakePair(&currBlockConObj, &currBlockVarObj)
//				, qMakePair(&next1BlockConObj, &next1BlockVarObj)
//				, qMakePair(&next2BlockConObj, &next2BlockVarObj)
//			};

//			T3TrackModel::plcIterate(quintupleBlocks);
////			//----STAGE 2 : TRAIN POSITION UPDATE
//		}
//		std::get<3>(*argsref)->replace(i, currTrackVarObj);
//	}
//}


//inline QString T3TrackModel::getPrevOrNextBlock(QString currBlockId
//		, const QJsonObject *currTrackCon, QJsonObject *currTrackVar
//		, bool reversedLook, bool *viewReversed) {
//	const QJsonObject currBlockCon = currTrackCon->value(currBlockId).toObject();
//	QJsonObject currBlockVar = currTrackVar->value(currBlockId).toObject();
//	*viewReversed = true;
//	//input is current block pair (constants,variables) and bool indicating if going backward
//	//output is the block pair calculated, either previous or next based on args[1]
//	QVarLengthArray<QString, 2> prevNextBlockIds{QString(), QString()};
//	for(int i = 1; i <= 2; ++i) {
//		QString istr = QString::number(i);
//		prevNextBlockIds.replace(i - 1, currBlockCon.value(reversedLook ? "prevBlock" + istr  : "nextBlock" + istr).toString());
//		if(prevNextBlockIds.at(i - 1).compare(reversedLook ? "END_T" : "START_T") == 0)
//			prevNextBlockIds.replace(i - 1,  currTrackCon->value(reversedLook ? "endingBlock2" : "startingBlock2").toString());
//		else if(prevNextBlockIds.at(i - 1).compare(reversedLook ? "END_B" : "START_B") == 0)
//			prevNextBlockIds.replace(i - 1, currTrackCon->value(reversedLook ? "endingBlock1" : "startingBlock1").toString());
//		else *viewReversed = false;
//	}
//	const bool currSwitchPosition = currBlockVar.value("switchPosition").toBool();
//	QString prevNextBlockId
//		= (prevNextBlockIds.at(1) != "" && prevNextBlockIds.at(1) != "PASSIVE" && !currSwitchPosition)
//		  ? prevNextBlockIds.at(1) : prevNextBlockIds.at(0) ;
//	return prevNextBlockId;
//}




#endif // T3TRACKMODEL_H
