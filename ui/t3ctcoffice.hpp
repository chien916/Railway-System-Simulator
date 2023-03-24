#ifndef T3CTCOFFICE_HPP
#define T3CTCOFFICE_HPP
#include <QtCore>
class T3CTCOffice {
  public:
	static bool connectedToTrackController;
	//DB-Related Operations

	/**
	 * Steps for dispatching process;
	 *
	 * 1. User fill in dispatch train id, origin, destination, time
	 * 2. User select desired block path
	 * 3. Adds the request{trainid_s,origin_s,desti_s,time_s,path_O} to queue
	 * 4. Pops the requests from queue when time is ready, adds to trainObjects and trackVariableObjects
	 *
	 */
	static QJsonArray searchPathsFromCsv(const QString filePath, const QJsonObject* stationToBlockIdMap, const QJsonArray dispatchMetaInfo, const QJsonArray* trackConstantsObjects);
	static void enqueueDispatchRequest(QJsonArray* queue, const QJsonArray dispatchMetaInfo, const QJsonArray path);
	static void discardDispatchRequest(int index, QJsonArray* queue);
	static QList<QJsonObject> popFromDispatchQueueAtTime(QJsonArray* queue, QTime currTime);
	static QJsonArray searchPathsFromMetaInfo(const QJsonArray dispatchMetaInfo, const QJsonArray* trackConstantsObjects);
  private:
	static QList<QList<QString>> searchPaths (const QString originBlockId, const QString destBlockId, QSet<QString> pathSet
							  , const QString& startingBlock1, const QString& startingBlock2, const QString& endingBlock1, const QString& endingBlock2
							  , QJsonObject* targetedBlockMap,  QString allowedDirection);

};


inline QList<QList<QString> > T3CTCOffice::searchPaths(const QString originBlockId, const QString destBlockId, QSet<QString> pathSet
		, const QString & startingBlock1, const QString & startingBlock2, const QString & endingBlock1, const QString & endingBlock2
		, QJsonObject * targetedBlockMap,  QString allowedDirection) {
	QList<QString> path;
	QString currBlockId = originBlockId;
	path.append(currBlockId);
	pathSet.insert(currBlockId);
	if(currBlockId == destBlockId) {
		return QList<QList<QString>>({path});
	}
	//qDebug() << pathSet;
	for(qsizetype trial = 1; trial < 1000; ++trial) {
		QJsonObject currBlockObject = targetedBlockMap->value(currBlockId).toObject();
		QVarLengthArray<QString> blockCandidatesAndTraversables = {
			(currBlockObject.value("prevBlock1").toString()),
			(currBlockObject.value("prevBlock2").toString()),
			(currBlockObject.value("nextBlock1").toString()),
			(currBlockObject.value("nextBlock2").toString())
		};
		//handles border block
		bool allowedDirectionShouldChange = true;
		if(allowedDirection == "REVERSED" && blockCandidatesAndTraversables.at(0) == "START_T")
			blockCandidatesAndTraversables[0] = startingBlock2;
		else if(allowedDirection == "REVERSED" && blockCandidatesAndTraversables.at(0) ==  "START_B")
			blockCandidatesAndTraversables[0] = startingBlock1;
		else if(allowedDirection == "FORWARD" && blockCandidatesAndTraversables.at(2) ==  "END_T")
			blockCandidatesAndTraversables[2]  = endingBlock2;
		else if(allowedDirection == "FORWARD" && blockCandidatesAndTraversables.at(2) == "END_B")
			blockCandidatesAndTraversables[2] = endingBlock1;
		else allowedDirectionShouldChange = false;

		//determines if block is traversable. If not, remove it from list
		for(qsizetype i = 3; i >= 0; --i) {
			QString currBlockCandidatesAndTraversable
				= blockCandidatesAndTraversables.at(i);
			if(currBlockCandidatesAndTraversable.split("_").size() != 3
					|| pathSet.contains(currBlockCandidatesAndTraversable)
					|| (currBlockObject.value("direction").toString() == "FORWARD" && i < 2)
					|| (currBlockObject.value("direction").toString() == "REVERSED" && i >= 2)
					|| (allowedDirection == "FORWARD" && i < 2)
					|| (allowedDirection == "REVERSED" && i >= 2))
				blockCandidatesAndTraversables.remove(i);
		}
		//traverse
		if(allowedDirectionShouldChange) {
			if(allowedDirection == "FORWARD") allowedDirection = "REVERSED";
			else if(allowedDirection == "REVERSED") allowedDirection = "FORWARD";
		}

		if(blockCandidatesAndTraversables.size() == 0) {
			//dead path found
			return QList<QList<QString>>();
		} else if(blockCandidatesAndTraversables.size() == 1) {
			//single path found
			currBlockId = blockCandidatesAndTraversables.first();
			path.append(currBlockId);
			pathSet.insert(currBlockId);
			if(currBlockId == destBlockId) {
				return QList<QList<QString>>({path});
			}
			//qDebug() << pathSet;
		} else {
			//multiple paths found
			//path.append(currBlockId);
			//pathSet.insert(currBlockId);
			QList<QList<QString>> ret;
			for(QString& candidate : blockCandidatesAndTraversables) {
				//pathSet.insert(currBlockId);//backtracking insert
				QList<QList<QString>> currRet = searchPaths(candidate, destBlockId, pathSet
												, startingBlock1, startingBlock2, endingBlock1, endingBlock2, targetedBlockMap, allowedDirection);
				if(currRet == QList<QList<QString>>()) continue;
				for(QList<QString>& currPathFromRet : currRet) {
					//currPathFromRet.push_front(candidate);
					currPathFromRet = path + currPathFromRet;
					ret.push_back(currPathFromRet);
				}
				//pathSet.remove(currBlockId);//backtracking remove
			}
			return ret;
		}
	}
	return QList<QList<QString>>();//trial exceeds 1000 - should never happen
}


inline QJsonArray T3CTCOffice::searchPathsFromCsv(const QString filePath, const QJsonObject *stationToBlockIdMap, const QJsonArray dispatchMetaInfo, const QJsonArray *trackConstantsObjects) {
	QList<QList<QString>> stationBlockIds;
	QTime delayTime(0, 0, 0, 0);
	QString yardId = "";

	//Read-in CSV
	{
		QString newFilePath = QString(filePath).replace("file:///", "");
		QFile file(newFilePath);
		if(!newFilePath.contains(".csv")) {
			qFatal("T3Database::addTrackFromCsv() -> CSV bad format: file received is not csv extensions");
		}
		if(!file.open(QIODevice::ReadOnly)) {
			qFatal(file.errorString().toUtf8());
		}

		while(!file.atEnd()) { //each iteration is one block (one line in csv)
			QStringList currLineSplitted = QString::fromUtf8(file.readLine()).trimmed().split(',');
			//check file format
			if(yardId == "") {
				if(currLineSplitted.length() != 3
						|| !currLineSplitted.at(0).toLower().contains("line")
						|| !currLineSplitted.at(1).toLower().contains("infrastructure")
						|| !currLineSplitted.at(2).toLower().contains("time"))
					qFatal("T3CTCOffice::searchPathFromCsv() -> CSV bad format: This is not a schedule CSV.");
				yardId = "UNKNOWN";
				continue;
			} else if(yardId == "UNKNOWN") {
				QString lineFull = currLineSplitted.at(0).trimmed().toLower();
				if(lineFull.contains("green"))
					yardId = "G_J_62";
				else if(lineFull.contains("red"))
					yardId = "R_C_9";
				else if(lineFull.contains("blue"))
					yardId = "B_A_1";
			}
			if(currLineSplitted.length() != 3) continue;
			QString stationName = currLineSplitted.at(1).trimmed();
			bool floatConversionSuccess = false;
			float minutesToTraverse =  currLineSplitted.at(2).trimmed().toFloat(&floatConversionSuccess);
			if(!floatConversionSuccess)
				qFatal(QString("T3CTCOffice::searchPathFromCsv() ->Cannot parse float " + currLineSplitted.at(2)).toLocal8Bit());
			if(!stationToBlockIdMap->contains(stationName))
				qFatal(QString("T3CTCOffice::searchPathFromCsv() -> Cannot find db-mapped station name " + stationName).toLocal8Bit());
			QJsonArray blockIdsCorrespodingToStation = stationToBlockIdMap->value(stationName).toArray();
			QStringList blockIdsCorrespodingToStationStringList;
			for(QJsonArray::iterator i = blockIdsCorrespodingToStation.begin(); i < blockIdsCorrespodingToStation.end(); ++i) {
				blockIdsCorrespodingToStationStringList.push_back(i->toString());
			}
			stationBlockIds.push_back(blockIdsCorrespodingToStationStringList);
			delayTime = delayTime.addSecs(60 * minutesToTraverse);
		}
	}

	if(stationBlockIds.length() == 0)
		qFatal("T3CTCOffice::searchPathFromCsv() -> Entry count is 0");
	//QList<QList<QString>> retFwd = searchPaths(yardId, stationBlockIds.last(),QSet<QString>(),)
	QTime arrivalTime = QTime::fromString(dispatchMetaInfo.at(3).toString("HH:mm"));
	QTime dispatchTime = arrivalTime.addSecs(-60 * delayTime.minute()).addSecs(delayTime.second());
	//QJsonArray newDispatchMetaInfo = {dispatchMetaInfo.at(0).toString(), yardId, stationBlockIds.last(), dispatchTime.toString("HH:mm")};
	QJsonArray finalPath;
	QList<QList<QString>> finalPaths;

	//count the number of stations has two block IDs match
	QHash<qsizetype, qsizetype> stationToBinamacIndMap;
	{
		for(qsizetype i = 0; i < stationBlockIds.length(); ++i) {
			if(stationBlockIds.at(i).length() == 2)
				stationToBinamacIndMap.insert(i, stationToBinamacIndMap.size());
		}
	}

	//get a aggregation of paths, each one maps to an distinct combination of station blocks
	QJsonArray possibleCombPaths;
	{
		for(quint32 comb = 0
						   ; comb < (static_cast<quint32>(1) << stationToBinamacIndMap.size())
				; ++comb) {

			//get the current station block ids for this combination
			QList<QString> currStationBlockIds;
			{
				for(qsizetype i = 0; i < stationBlockIds.length(); ++i) {
					if(stationToBinamacIndMap.contains(i) && ((comb & (1 << stationToBinamacIndMap.value(i))) != 0))
						currStationBlockIds.push_back(stationBlockIds.at(i).at(1));
					else
						currStationBlockIds.push_back(stationBlockIds.at(i).at(0));
				}
			}

			//get an optimal path connecting all stations of current combinations -> only one path will be known, the optimal one
			QJsonArray possiblePathForThisComb;
			{
				for(qsizetype i = 0; i < currStationBlockIds.length() - 1; ++i) 	{ //for each possible block id of path-finding destination station
					//between each 2 stations, find all possible paths connecting 2 stations
					QJsonArray currDispatchMetaInfo = {"____", i == -1 ? yardId : currStationBlockIds.at(i), currStationBlockIds.at(i + 1), "__:__"};
					QJsonArray currPossiblePaths = searchPathsFromMetaInfo(currDispatchMetaInfo, trackConstantsObjects);

					//find the smallest path connecting the current 2 stations
					QPair<qsizetype, qsizetype> smallest = qMakePair(-1, 0);
					{
						for(qsizetype j = 0; j < currPossiblePaths.size(); ++j) { //for each possible path of current path-finding origin and destination block
							QJsonArray currPossiblePath = currPossiblePaths.at(j).toArray();
							if(smallest.first == -1 || currPossiblePath.size() < smallest.second) {
								smallest.first = j;
								smallest.second = currPossiblePath.size();
							}
						}
					}

					//append this path connecting 2 stations to possiblePathForThisComb only if the path connecting 2 stations can be found
					if(smallest.first != -1) {
						QJsonArray bestPossiblePath = currPossiblePaths.at(smallest.first).toArray();
						for(qsizetype j = 0; j < bestPossiblePath.size(); ++j) {
							possiblePathForThisComb.push_back(bestPossiblePath.at(j).toString());
						}
					} else {
						possiblePathForThisComb = QJsonArray();//clear the possible path for this combination of stations
						Q_ASSERT(possiblePathForThisComb.isEmpty());//well, just to make sure I cleared the jsonarray :*)
						break;//stop searching path for the next 2 stations -> this combination doesn't work!
					}

				}
			}

			//if an optimal path connecting all stations can be found, append this path to possibleCombPaths
			if(possiblePathForThisComb.size() > 0) {
				possibleCombPaths.push_back(possiblePathForThisComb);
			}
		}
	}
	return possibleCombPaths;
}


inline void T3CTCOffice::enqueueDispatchRequest(QJsonArray * queue, const QJsonArray dispatchMetaInfo, const QJsonArray path) {
	QJsonObject dispatchObject;
	if(dispatchMetaInfo.size() != 4)
		qFatal("T3CTCOffice::addToDispatchQueueWithPaths() meta information is not four");
	QTime dispatchTime = QTime::fromString(dispatchMetaInfo.at(3).toString(), "HH:mm");
	if(!dispatchTime.isValid())
		qFatal("T3CTCOffice::addToDispatchQueueWithPaths() time from meta info is not valid");
	dispatchObject.insert(QString("trainId"), dispatchMetaInfo.at(0).toString());
	dispatchObject.insert(QString("origin"), dispatchMetaInfo.at(1).toString());
	dispatchObject.insert(QString("destination"), dispatchMetaInfo.at(2).toString());
	dispatchObject.insert(QString("time"), dispatchTime.toString("HH:mm"));
	dispatchObject.insert(QString("path"), path);
	for(qsizetype i = 0; i < queue->size(); ++i) {
		QTime currDispatchTime = QTime::fromString(queue->at(i).toObject().value("time").toString(), "HH:mm");
		if(!currDispatchTime.isValid())
			qFatal("T3CTCOffice::addToDispatchQueueWithPaths() time from queue is not valid");
		if(currDispatchTime > dispatchTime) {
			queue->insert(i, dispatchObject);
			return;
		}
	}
	queue->append(dispatchObject);//if empty or later than all
}

inline void T3CTCOffice::discardDispatchRequest(int index, QJsonArray *queue) {
	if(index < 0 && index >= queue->count()) return;
	queue->removeAt(index);
}

inline QList<QJsonObject> T3CTCOffice::popFromDispatchQueueAtTime(QJsonArray * queue, QTime currTime) {
	QList<QJsonObject> ret;
	while(true) {
		if(queue->empty()) break;
		QTime currDispatchTime = QTime::fromString(queue->first().toObject().value("time").toString(), "HH:mm");
		if(!currDispatchTime.isValid())
			qFatal("T3CTCOffice::popFromDispatchQueueAtTime() time from queue is not valid");
		if(currDispatchTime > currTime) break;
		ret.push_back(queue->takeAt(0).toObject());
	}
	return ret;
}


inline QJsonArray T3CTCOffice::searchPathsFromMetaInfo(const QJsonArray dispatchMetaInfo, const QJsonArray * trackConstantsObjects) {
	//first, locate the correct trackConstantObject from trackConstantObjects
	QJsonObject targetedBlockMap;
	QString startingBlock1, startingBlock2, endingBlock1, endingBlock2;
	for(qsizetype i = 0; i < trackConstantsObjects->size(); ++i) {
		//assume trackConstantObjects is in right format
		QJsonObject currBlocksMap = trackConstantsObjects->at(i).toObject().value("blocksMap").toObject();
		if(currBlocksMap.contains(dispatchMetaInfo.at(1).toString())
				&& currBlocksMap.contains(dispatchMetaInfo.at(2).toString())) {
			targetedBlockMap = currBlocksMap;
			startingBlock1 = trackConstantsObjects->at(i).toObject().value("startingBlock1").toString();
			startingBlock2 = trackConstantsObjects->at(i).toObject().value("startingBlock2").toString();
			endingBlock1 = trackConstantsObjects->at(i).toObject().value("endingBlock1").toString();
			endingBlock2 = trackConstantsObjects->at(i).toObject().value("endingBlock2").toString();
			break;
		}
	}
	if(targetedBlockMap.isEmpty()
			|| startingBlock1.isNull() || startingBlock2.isNull()
			|| endingBlock1.isNull() || endingBlock2.isNull())
		qFatal("T3Database::ctc_getPossiblePaths() cannot find the right block map or border block.");

	QList<QList<QString>> retRawForward
					   = T3CTCOffice::searchPaths
						 (dispatchMetaInfo.at(1).toString()//origin block id
						  , dispatchMetaInfo.at(2).toString()//destination block id
						  , QSet<QString>() //empty string set
						  , startingBlock1, startingBlock2, endingBlock1, endingBlock2
						  , &targetedBlockMap
						  , "FORWARD");

	QList<QList<QString>> retRawReversed
					   = T3CTCOffice::searchPaths
						 (dispatchMetaInfo.at(1).toString()//origin block id
						  , dispatchMetaInfo.at(2).toString()//destination block id
						  , QSet<QString>() //empty string set
						  , startingBlock1, startingBlock2, endingBlock1, endingBlock2
						  , &targetedBlockMap
						  , "REVERSED");

	QJsonArray ret;
	if(retRawForward != QList<QList<QString>>())
		for(QList<QString>&currretRaw : retRawForward) {
			ret.append(QJsonArray::fromStringList(currretRaw));
		}
	if(retRawReversed != QList<QList<QString>>())
		for(QList<QString>&currretRaw : retRawReversed) {
			ret.append(QJsonArray::fromStringList(currretRaw));
		}
	return ret;
}

#endif // T3CTCOFFICE_HPP
