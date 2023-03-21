#ifndef T3TRACKMODEL_H
#define T3TRACKMODEL_H


#include <QtCore>
class T3TrackModel {
  public:
	enum TrackProperty : uint8_t {
		CommandedSpeed = 0 //real/float
		, Authority = 1 //bool
		, SwitchPostion = 2 //bool
		, ForwardLight = 3 //string
		, ReversedLight = 4 // string
		, CrossingPosition = 5//bool
		, TrainOnBlock = 6 //string
		, Failure = 7 //string
		, Heaters = 8 //string
		, PeopleOnStation = 9 //int
		, MaintainanceMode = 10
	};

	static bool connectedToTrackController;
	static bool connectedToTrainModel;
	static void placeTrainOnTrack(QString trainId, QString blockId, bool isForward, QJsonArray* trackVariableObjects);

	static void trackIterate(QJsonArray* trackVariableObjects, const QJsonArray* trackConstantsObjects);
	static void plcIterate(QVarLengthArray<QPair<const QJsonObject*, QJsonObject*>, 5>& quintupleBlocks);
	static void addTrackFromCsv(const QString filePath, QJsonArray* trackConstantsObjects, QJsonArray*trackVariablesObjects, QJsonObject* stationToIdMapObject);

	static void setTrackProperty(QString blockId, T3TrackModel::TrackProperty trackProperty, QVariant value, QJsonArray*trackVariablesObjects
								 , const QHash<T3TrackModel::TrackProperty, QPair<QString, int>>*const trackPropertiesMetaDataMap);
	static QVariant getTrackProperty(QString blockId, T3TrackModel::TrackProperty trackProperty, const QJsonArray *trackVariablesObjects
									 , const QHash<T3TrackModel::TrackProperty, QPair<QString, int>>*const trackPropertiesMetaDataMap);

  private:
	//static const QHash<T3TrackModel::TrackProperty, QPair<QString, int>> trackPropertiesMetaDataMap;

	static QString getPrevOrNextBlock(QString currBlockId
									  , const QJsonObject *currTrackCon, QJsonObject *currTrackVar
									  , bool reversedLook, bool *viewReversed);
};



inline void T3TrackModel::addTrackFromCsv(const QString filePath, QJsonArray* trackConstantsObjects
		, QJsonArray*trackVariablesObjects, QJsonObject* stationToIdMapObject) {
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
		{
			QJsonObject currBlockConObj;
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
					}
					currBlockConObj.insert(currTitle, currValueAsString);
				} else if(currType == "b") {
					if(currValueAsString == "TRUE")
						currBlockConObj.insert(currTitle, true);
					else if(currValueAsString == "FALSE")
						currBlockConObj.insert(currTitle, false);
					else
						qFatal("T3TrackModel::addTrackFromCsv() -> CSV bad format: title stated as boolean but value is not boolean");
				} else if(currType == "f" || currType == "r") {
					bool conversionIsSucessful;
					float currValue = currValueAsString.toFloat(&conversionIsSucessful);
					if(!conversionIsSucessful)
						qFatal("T3TrackModel::addTrackFromCsv() -> CSV bad format: title stated as float but value is not float");
					currBlockConObj.insert(currTitle, currValue);
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
			currTrackConObj.insert(currLineSplitted.at(0), currBlockConObj);
		}

		//initializing fields for variables for this block
		{
			QJsonObject currBlockVarObj;
			currBlockVarObj.insert("commandedSpeed", static_cast<uint16_t>(0));
			currBlockVarObj.insert("authority", true);
			currBlockVarObj.insert("switchPosition", false);
			if(currLineSplitted.at(12).contains("BIDIRECTIONAL")
					|| currLineSplitted.at(12).contains("FORWARD"))
				currBlockVarObj.insert("forwardLight", QString("clear"));
			else
				currBlockVarObj.insert("forwardLight", QString(""));
			if(currLineSplitted.at(12).contains("BIDIRECTIONAL")
					|| currLineSplitted.at(12).contains("REVERSED"))
				currBlockVarObj.insert("reversedLight", QString("clear"));
			else
				currBlockVarObj.insert("reversedLight", QString(""));
			if(currLineSplitted.at(10) == "TRUE")
				currBlockVarObj.insert("crossingPosition", false);
			else
				currBlockVarObj.insert("crossingPosition", QString(""));
			currBlockVarObj.insert("trainOnBlock", QString(""));
			currBlockVarObj.insert("failure", QString(""));
			currBlockVarObj.insert("heaters", QString(""));
			currBlockVarObj.insert("peopleOnStation", static_cast<float>(0.0));
			currBlockVarObj.insert("maintainanceMode", false);
			currTrackVarObj.insert(currLineSplitted.at(0), currBlockVarObj);
		}
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
		trackVariablesObjects->push_back(currTrackVarObj);
		trackConstantsObjects->push_back(currTrackConObjWrapper);
	}

	QFile fileToWriteLog(QString(newFilePath).replace(".csv", "_ConstantsJson.json"));
	if(fileToWriteLog.open(QFile::WriteOnly) && trackConstantsObjects->count() > 0 && trackConstantsObjects->at(0).isObject())
		fileToWriteLog.write(QJsonDocument(trackConstantsObjects->last().toObject()).toJson());
	else
		qFatal(fileToWriteLog.errorString().toUtf8());

	QFile fileToWriteLog2(QString(newFilePath).replace(".csv", "_VariablesJson.json"));
	if(fileToWriteLog2.open(QFile::WriteOnly) && trackVariablesObjects->count() > 0 && trackVariablesObjects->at(0).isObject())
		fileToWriteLog2.write(QJsonDocument(trackVariablesObjects->last().toObject()).toJson());
	else
		qFatal(fileToWriteLog.errorString().toUtf8());
	fileToWriteLog2.close();
}

inline void T3TrackModel::setTrackProperty(QString blockId, TrackProperty trackProperty, QVariant value, QJsonArray *trackVariablesObjects
		, const QHash<T3TrackModel::TrackProperty, QPair<QString, int>>*const trackPropertiesMetaDataMap) {
	QPair<QString, int> metaData = trackPropertiesMetaDataMap->value(trackProperty);
	if(!value.canConvert(metaData.second))
		qFatal("T3Database::setTrackProperty() -> property value required and insetTrackPropertyted format doesn't match");
	value.convert(metaData.second);
	bool fieldIsFound = false;
	for(qsizetype i = 0; i < trackVariablesObjects->size(); ++i) { //for every line
		QJsonObject currTrackVarObj = trackVariablesObjects->at(i).toObject();
		if(currTrackVarObj.find(blockId) != currTrackVarObj.end()) {
			QJsonObject currBlockVarObj = currTrackVarObj[blockId].toObject();
			currBlockVarObj[metaData.first] = value.toJsonValue();
			currTrackVarObj[blockId] = currBlockVarObj;
			trackVariablesObjects->operator[](i) = currTrackVarObj;
			fieldIsFound = true;
			break;
		}
	}
	if(!fieldIsFound)
		qFatal("T3Database::setTrackProperty() -> cannot find the block id insetTrackPropertyted");
}

inline QVariant T3TrackModel::getTrackProperty(QString blockId, TrackProperty trackProperty, const QJsonArray *trackVariablesObjects
		, const QHash<T3TrackModel::TrackProperty, QPair<QString, int>>*const trackPropertiesMetaDataMap) {
	QPair<QString, int> metaData = trackPropertiesMetaDataMap->value(trackProperty);
	for(qsizetype i = 0; i < trackVariablesObjects->size(); ++i) { //for every line
		QJsonObject currTrackVarObj = trackVariablesObjects->at(i).toObject();
		if(currTrackVarObj.find(blockId) != currTrackVarObj.end()) {
			QJsonObject currBlockVarObj = currTrackVarObj[blockId].toObject();
			QVariant valueRequested = currBlockVarObj[metaData.first].toVariant();
			if(valueRequested.convert(metaData.second)) {
				//qInfo() << "T3Database::getTrackProperty() -> " << valueRequested;
				return valueRequested;
			} else
				qFatal("T3Database::getTrackProperty() -> cannot convert value to the right type");
		}
	}
	qFatal("T3Database::getTrackProperty() -> cannot find the block id insetTrackPropertyted");
	return QVariant();//should not reach this step..
}

inline void T3TrackModel::placeTrainOnTrack(QString trainId, QString blockId, bool isForward, QJsonArray *trackVariableObjects) {

}

inline void T3TrackModel::trackIterate(QJsonArray *trackVariablesObjects, const QJsonArray* trackConstantsObjects) {
	for(qsizetype i = 0; i < trackVariablesObjects->count(); ++i) {
		QJsonObject currTrackVarObj = trackVariablesObjects->at(i).toObject();
		const QJsonObject currTrackConObj = trackConstantsObjects->at(i).toObject().value("blocksMap").toObject();
		QStringList blockIds = currTrackVarObj.keys();
		for(qsizetype j = 0; j < blockIds.count(); ++j) {

			//determine prev2 prev1 next1 next2 blockId and object
			QString currBlockId = blockIds.at(j);
			const QJsonObject currBlockConObj = currTrackConObj.value(currBlockId).toObject();
			QJsonObject currBlockVarObj = currTrackVarObj.value(currBlockId).toObject();

			bool prevViewReversed = false, nextViewReversed = false;

			QString prev1BlockId
				= getPrevOrNextBlock(currBlockId, &currTrackConObj, &currTrackVarObj, true != prevViewReversed, &prevViewReversed);
			QString prev2BlockId
				= getPrevOrNextBlock(prev1BlockId, &currTrackConObj, &currTrackVarObj, true != prevViewReversed, &prevViewReversed);
			QString next1BlockId
				= getPrevOrNextBlock(currBlockId, &currTrackConObj, &currTrackVarObj, true != nextViewReversed, &nextViewReversed);
			QString next2BlockId
				= getPrevOrNextBlock(next1BlockId, &currTrackConObj, &currTrackVarObj, true != nextViewReversed, &nextViewReversed);

			const QJsonObject prev1BlockConObj = currTrackConObj.value(prev1BlockId).toObject();
			const QJsonObject prev2BlockConObj = currTrackConObj.value(prev2BlockId).toObject();
			const QJsonObject next1BlockConObj = currTrackConObj.value(next1BlockId).toObject();
			const QJsonObject next2BlockConObj = currTrackConObj.value(next2BlockId).toObject();

			QJsonObject prev1BlockVarObj = currTrackVarObj.value(prev1BlockId).toObject();
			QJsonObject prev2BlockVarObj = currTrackVarObj.value(prev2BlockId).toObject();
			QJsonObject next1BlockVarObj = currTrackVarObj.value(next1BlockId).toObject();
			QJsonObject next2BlockVarObj = currTrackVarObj.value(next2BlockId).toObject();

			QVarLengthArray<QPair<const QJsonObject*, QJsonObject*>, 5> quintupleBlocks = {
				qMakePair(&prev2BlockConObj, &prev2BlockVarObj)
				, qMakePair(&prev1BlockConObj, &prev1BlockVarObj)
				, qMakePair(&currBlockConObj, &currBlockVarObj)
				, qMakePair(&next1BlockConObj, &next1BlockVarObj)
				, qMakePair(&next2BlockConObj, &next2BlockVarObj)
			};

			T3TrackModel::plcIterate(quintupleBlocks);
//			//----STAGE 2 : TRAIN POSITION UPDATE
		}
		trackVariablesObjects->replace(i, currTrackVarObj);
	}
}

inline void T3TrackModel::plcIterate(QVarLengthArray<QPair<const QJsonObject*, QJsonObject*>, 5>& quintupleBlocks) {
	Q_ASSERT(quintupleBlocks.size() == 5);
	bool currHasCrossing = quintupleBlocks.at(2).second->value("crossing").toBool();

	//get occupancy of prev2 prev1 curr next1 next2
	bool prev2occupancy = quintupleBlocks.at(0).second->value("trainOnBlock").toString().compare("") != 0;
	bool prev1occupancy = quintupleBlocks.at(1).second->value("trainOnBlock").toString().compare("") != 0;
	bool curroccupancy = quintupleBlocks.at(2).second->value("trainOnBlock").toString().compare("") != 0;
	bool next1occupancy = quintupleBlocks.at(3).second->value("trainOnBlock").toString().compare("") != 0;
	bool next2occupancy = quintupleBlocks.at(4).second->value("trainOnBlock").toString().compare("") != 0;

	QString reversedLight = prev1occupancy ? "stop" : (prev2occupancy ? "approach" : "clear");
	QString forwardLight = next1occupancy ? "stop" : (next1occupancy ? "approach" : "clear");
	QString crossing = currHasCrossing
					   ? ((prev2occupancy || prev1occupancy || curroccupancy || next1occupancy || next2occupancy)
						  ? "down" : "up")
					   : "";

	quintupleBlocks.at(2).second->insert("forwardLight", forwardLight);
	quintupleBlocks.at(2).second->insert("reversedLight", reversedLight);
	quintupleBlocks.at(2).second->insert("crossingPosition", crossing);
}

inline QString T3TrackModel::getPrevOrNextBlock(QString currBlockId
		, const QJsonObject *currTrackCon, QJsonObject *currTrackVar
		, bool reversedLook, bool *viewReversed) {
	const QJsonObject currBlockCon = currTrackCon->value(currBlockId).toObject();
	QJsonObject currBlockVar = currTrackVar->value(currBlockId).toObject();
	*viewReversed = true;
	//input is current block pair (constants,variables) and bool indicating if going backward
	//output is the block pair calculated, either previous or next based on args[1]
	QVarLengthArray<QString, 2> prevNextBlockIds{QString(), QString()};
	for(int i = 1; i <= 2; ++i) {
		QString istr = QString::number(i);
		prevNextBlockIds.replace(i - 1, currBlockCon.value(reversedLook ? "prevBlock" + istr  : "nextBlock" + istr).toString());
		if(prevNextBlockIds.at(i - 1).compare(reversedLook ? "END_T" : "START_T") == 0)
			prevNextBlockIds.replace(i - 1,  currTrackCon->value(reversedLook ? "endingBlock2" : "startingBlock2").toString());
		else if(prevNextBlockIds.at(i - 1).compare(reversedLook ? "END_B" : "START_B") == 0)
			prevNextBlockIds.replace(i - 1, currTrackCon->value(reversedLook ? "endingBlock1" : "startingBlock1").toString());
		else *viewReversed = false;
	}
	const bool currSwitchPosition = currBlockVar.value("switchPosition").toBool();
	QString prevNextBlockId
		= (prevNextBlockIds.at(1) != "" && prevNextBlockIds.at(1) != "PASSIVE" && !currSwitchPosition)
		  ? prevNextBlockIds.at(1) : prevNextBlockIds.at(0) ;
	return prevNextBlockId;
}




#endif // T3TRACKMODEL_H
