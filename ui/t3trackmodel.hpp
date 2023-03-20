#ifndef T3TRACKMODEL_H
#define T3TRACKMODEL_H


#include <QtCore>
class T3TrackModel {
  public:
	static bool connectedToTrackController;
	static bool connectedToTrainModel;
	static void plcIterate(QVarLengthArray<QPair<const QJsonObject*, QJsonObject*>, 5>& quintupleBlocks);
	static void addTrackFromCsv(const QString filePath, QJsonArray* trackConstantsObjects, QJsonArray*trackVariablesObjects, QJsonObject* stationToIdMapObject);
};

inline void T3TrackModel::addTrackFromCsv(const QString filePath, QJsonArray* trackConstantsObjects, QJsonArray*trackVariablesObjects, QJsonObject* stationToIdMapObject) {
	QString newFilePath = QString(filePath).replace("file:///", "");
	qsizetype requiredColumnCount = 0;
	QFile file(newFilePath);
	if(!newFilePath.contains(".csv")) {
		qFatal("T3Database::addTrackFromCsv() -> CSV bad format: file received is not csv extensions");
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
						qFatal("T3Database::addTrackFromCsv() -> CSV bad format: title name and type format wrong");
					titles.append(TitleAndTypesSplitted);
				}
			} else
				qFatal("T3Database::addTrackFromCsv() -> CSV bad format: title doesn't match.");
			continue;
		}
		QStringList currLineSplitted = currLineRaw.split(',');
		if(currLineSplitted.count() != requiredColumnCount) {
			qFatal("T3Database::addTrackFromCsv() -> CSV bad format: content column count mismatch.");
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
							qFatal("T3Database::addTrackFromCsv() -> Station name with side spllitted length is not 2.");
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
						qFatal("T3Database::addTrackFromCsv() -> CSV bad format: title stated as boolean but value is not boolean");
				} else if(currType == "f" || currType == "r") {
					bool conversionIsSucessful;
					float currValue = currValueAsString.toFloat(&conversionIsSucessful);
					if(!conversionIsSucessful)
						qFatal("T3Database::addTrackFromCsv() -> CSV bad format: title stated as float but value is not float");
					currBlockConObj.insert(currTitle, currValue);
				} else if(currType == "i") {
					bool conversionIsSucessful;
					int currValue = currValueAsString.toInt(&conversionIsSucessful);
					if(!conversionIsSucessful)
						qFatal("T3Database::addTrackFromCsv() -> CSV bad format: title stated as float but value is not float");
					currBlockConObj.insert(currTitle, currValue);
				} else {
					qFatal("T3Database::addTrackFromCsv() -> CSV bad format: title type not recognized");
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
		qFatal("T3Database::addTrackFromCsv() -> CSV bad format: starting and ending block cannot be found.");
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
#endif // T3TRACKMODEL_H
