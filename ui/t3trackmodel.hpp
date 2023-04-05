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
	static void setIOMetaInfo(const QString blockId, QJsonArray metaInfo, MODU_ARGS_REF argsref);
	static void updateTrainOccupancyOnAllBlocks(MODU_ARGS_REF argsref);
	static void toggleConnection(bool newConnectionState, MODU_ARGS_REF argsref);
	static QVarLengthArray<QString, 4> getNeighboringBlocks(const QString blockId, MODU_ARGS_REF argsref);
	static void updateTrainPositionOnAllBlocks(MODU_ARGS_REF argsref);
  private:
	static QString getPrevOrNextBlock(const QString blockId, bool getPrev, bool* borderReached, MODU_ARGS_REF argsref);
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
			currBlockVarObj.insert("KM_PEOPLEONSTATION", static_cast<uint16_t>(QRandomGenerator::global()->bounded(0, 100)));
			currBlockVarObj.insert("KM_TRAINTOSTATIONCOUNT", static_cast<uint16_t>(0));
			currBlockVarObj.insert("KM_STATIONTOTRAINCOUNT", static_cast<uint16_t>(0));
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
	retRaw[17] = numberHelper == 0 ? "-" : (QString(helper[1] == '1' ? "TOP " : "BOTTOM ")
											+ QString(helper[0] == '1' ? "BACK" : "FORWARD")); //	,"Authority Direction"
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

inline void T3TrackModel::updateTrainOccupancyOnAllBlocks(MODU_ARGS_REF argsref) {
	for(const QJsonValue& currLine : qAsConst(*std::get<3>(*argsref))) {
		for(auto& blockId : currLine.toObject().keys()) {
			QString KMPLCIO = GET_TRACKVAR_F(blockId, "COM[KC|KM]_KMPLCIO", argsref).toString();
			const QVarLengthArray<QString, 4> neighboringBlocks = getNeighboringBlocks(blockId, argsref);
			KMPLCIO[1] = GET_TRACKVAR_F(neighboringBlocks.at(0), "KM_TRAINONBLOCK", argsref).toString().isEmpty() ? '0' : '1';
			KMPLCIO[2] = GET_TRACKVAR_F(neighboringBlocks.at(1), "KM_TRAINONBLOCK", argsref).toString().isEmpty() ? '0' : '1';
			KMPLCIO[3] = GET_TRACKVAR_F(blockId, "KM_TRAINONBLOCK", argsref).toString().isEmpty() ? '0' : '1';
			KMPLCIO[4] = GET_TRACKVAR_F(neighboringBlocks.at(2), "KM_TRAINONBLOCK", argsref).toString().isEmpty() ? '0' : '1';
			KMPLCIO[5] = GET_TRACKVAR_F(neighboringBlocks.at(3), "KM_TRAINONBLOCK", argsref).toString().isEmpty() ? '0' : '1';
			SET_TRACKVAR_F(blockId, "COM[KC|KM]_KMPLCIO", KMPLCIO, argsref);
		}
	}
}

inline void T3TrackModel::toggleConnection(bool newConnectionState, MODU_ARGS_REF argsref) {
	for(const QJsonValue& currLine : qAsConst(*std::get<3>(*argsref))) {
		for(auto& blockId : currLine.toObject().keys()) {
			QString KMPLCIO = GET_TRACKVAR_F(blockId, "COM[KC|KM]_KMPLCIO", argsref).toString();
			KMPLCIO[0] = newConnectionState ? '1' : '0';
			SET_TRACKVAR_F(blockId, "COM[KC|KM]_KMPLCIO", KMPLCIO, argsref);
		}
	}
}

inline QVarLengthArray<QString, 4> T3TrackModel::getNeighboringBlocks(const QString blockId, MODU_ARGS_REF argsref) {
	QVarLengthArray<QString, 4> toReturn = {"", "", "", ""};
	QPair<bool, bool> onViewBorder = qMakePair(false, false);
	//determine prev block -> at index 1
	toReturn.replace(1, getPrevOrNextBlock(blockId, true, &onViewBorder.first, argsref));
	//determine prev prev block -> at index 0
	toReturn.replace(0, getPrevOrNextBlock(toReturn.at(1), onViewBorder.first ? false : true, &onViewBorder.first, argsref));
	//determine next block -> at index 2
	toReturn.replace(2, getPrevOrNextBlock(blockId, false, &onViewBorder.second, argsref));
	//determine next next block -> at index 0
	toReturn.replace(3, getPrevOrNextBlock(toReturn.at(2), onViewBorder.second ? true : false, &onViewBorder.second, argsref));
	return toReturn;
}

inline void T3TrackModel::updateTrainPositionOnAllBlocks(MODU_ARGS_REF argsref) {
	qDebug() << "I;m running";
	for(const QJsonValue currTrainRaw : (*std::get<4>(*argsref))) {
		QJsonObject currTrain = currTrainRaw.toObject();
		//Oh dear here comes the physics
		QString trainId = currTrain.value("NM_ID").toString();
		QString blockId = currTrain.value("NM_BLOCKID").toString();
		float length = GET_TRACKCON_F(blockId, "length", argsref).toDouble();
		float velocity = KMH2MS_F(currTrain.value("NC_PREVY").toDouble());
		const float dt = 1.0f;
		//traverse through block
		float ds = velocity  * dt;
		QStringList trainOnBlockSplit = GET_TRACKVAR_F(blockId, "KM_TRAINONBLOCK", argsref).toString().split("_");
		Q_ASSERT(trainOnBlockSplit.count() == 3);
		bool isMovingForward = trainOnBlockSplit.at(1).contains("F");
		float percentTravelled = trainOnBlockSplit.at(2).toFloat();
		percentTravelled += ds / length;
		qDebug() << QString("TRAIN %1 SEC AT LENG %2 TRACK TRAVELLED WITH %3 M/S(km/h) SPEED %4 DIR %5 M -> %6 , %7")
				 .arg(dt)
				 .arg(length)
				 .arg(velocity)
				 .arg(isMovingForward ? "RIGHT" : "LEFT ")
				 .arg(ds)
				 .arg(trainId)
				 .arg(blockId);
		qDebug() << QString("PREV PERC %1 , NEW PERC %2")
				 .arg(trainOnBlockSplit.at(2).toFloat())
				 .arg(percentTravelled);
		while(percentTravelled > 1.0f) {
			bool viewReverse = false;
			QString blockId_new = getPrevOrNextBlock(blockId, !isMovingForward, &viewReverse, argsref);
			float length_new =  GET_TRACKCON_F(blockId_new, "length", argsref).toDouble();
			float percentTravelled_new = (length * (percentTravelled - 1.0f)) / length_new;
			if(viewReverse) trainOnBlockSplit.replace(1, trainOnBlockSplit.at(1).contains("F") ? "R" : "F");
			trainOnBlockSplit.replace(2, QString::number(percentTravelled_new));
			//clears the train info from previous block
			SET_TRACKVAR_F(blockId, "KM_TRAINONBLOCK", QString(""), argsref);
			blockId = blockId_new;
			SET_TRAIN_F(trainId, "NM_BLOCKID", blockId, argsref);
			percentTravelled = percentTravelled_new;
			length = length_new;
			qDebug() << QString("[TVS] TRAIN %1 SEC AT LENG %2 TRACK TRAVELLED WITH %3 M/S(km/h) SPEED %4 DIR %5 M -> %6 ， %7")
					 .arg(dt)
					 .arg(length_new)
					 .arg(velocity)
					 .arg(viewReverse ? "DIFF" : "SAME")
					 .arg(ds)
					 .arg(trainId)
					 .arg(blockId);
			qDebug() << QString("PREV PERC %1 , NEW PERC %2")
					 .arg(trainOnBlockSplit.at(2).toFloat())
					 .arg(percentTravelled_new);
		}
		trainOnBlockSplit.replace(2, QString::number(percentTravelled));
		SET_TRACKVAR_F(blockId, "KM_TRAINONBLOCK", trainOnBlockSplit.join("_"), argsref);
	}
}

inline QString T3TrackModel::getPrevOrNextBlock(const QString blockId, bool getPrev, bool* borderReached, MODU_ARGS_REF argsref) {
	Q_ASSERT(borderReached != nullptr);
	(*borderReached) = false;
	QString toReturn;
	QString KMPLCIO = GET_TRACKVAR_F(blockId, "COM[KC|KM]_KMPLCIO", argsref).toString();
	if(KMPLCIO.at(getPrev ? 24 : 25) == '1' && KMPLCIO.at(18) == '0') {
		//has a switch and the switch position is down
		toReturn = GET_TRACKCON_F(blockId, getPrev ? "prevBlock2" : "nextBlock2", argsref).toString();
	} else {
		//has no switch or the switch position is up
		toReturn = GET_TRACKCON_F(blockId, getPrev ? "prevBlock1" : "nextBlock1", argsref).toString();
	}
	Q_ASSERT(toReturn != "");
	for(QPair<QString, QString>& arg : QVector<QPair<QString, QString>> {
	qMakePair(QString("START_T"), QString("startingBlock2")),
		qMakePair(QString("START_B"), QString("startingBlock1")),
		qMakePair(QString("END_T"), QString("endingBlock2")),
		qMakePair(QString("END_B"), QString("endingBlock1"))
	}) {
		if(toReturn.contains(arg.first)) {
			for(qsizetype i = 0; i < std::get<2>(*argsref)->size(); ++i) {
				if(std::get<2>(*argsref)->at(i).toObject().value("blocksMap").toObject().contains(blockId)) {
					toReturn = std::get<2>(*argsref)->at(i).toObject().value(arg.second).toString();
					(*borderReached) = true;
				}
			}
			Q_ASSERT(*borderReached);
		}
	}
	Q_ASSERT(toReturn != "");
	return toReturn;
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



#endif // T3TRACKMODEL_H
