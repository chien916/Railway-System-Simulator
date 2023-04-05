#ifndef T3CTCOFFICE_HPP
#define T3CTCOFFICE_HPP
#include "t3prophelper.h"

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
	static QJsonArray searchPathsFromCsv(const QString filePath, const QJsonObject* stationToBlockIdMap, const QJsonArray dispatchMetaInfo, MODU_ARGS_REF argsref);
	static void enqueueDispatchRequest(QJsonArray* queue, const QJsonArray dispatchMetaInfo, const QJsonArray path);
	static void discardDispatchRequest(int index, QJsonArray* queue);
	static QList<QJsonObject> popFromDispatchQueueAtTime(QJsonArray* queue, QTime currTime);
	static QJsonArray searchPathsFromMetaInfo(const QJsonArray dispatchMetaInfo, MODU_ARGS_REF argsref);
	static void writeToPlcInputFromMetaInfo(const QString blockId, const QJsonArray metaInfo,  MODU_ARGS_REF argsref);
	static QJsonArray readPlcInputToMetaInfo(const QString blockId,  MODU_ARGS_REF argsref);
	static QJsonArray searchPathForAuthority(const QString originBlock, const QString destiBlock,  MODU_ARGS_REF argsref);
	static void toggleConnection(bool newConnectionState, MODU_ARGS_REF argsref);
  private:
	static void setAuthorityFromPath(const QJsonArray* authorityPath, bool b,  MODU_ARGS_REF argsref);
	static QList<QList<QString>> searchPaths (const QString originBlockId, const QString destBlockId, QSet<QString> pathSet
							  , const QString& startingBlock1, const QString& startingBlock2, const QString& endingBlock1, const QString& endingBlock2
							  , QJsonObject* targetedBlockMap,  QString allowedDirection);
	static QVarLengthArray<bool, 2> determineAuthorityDirection(const QString originBlockId, const QString nextBlockId,  MODU_ARGS_REF argsref);
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


inline QVarLengthArray<bool, 2> T3CTCOffice::determineAuthorityDirection(const QString currBlockId, const QString nextOrPrevBlockId,  MODU_ARGS_REF argsref) {
	//reversed shouldgoup
	for(qsizetype i = 0; i < std::get<2>(*argsref)->size(); ++i) {
		const QJsonObject currLineBlockMapObject = std::get<2>(*argsref)->at(i).toObject().value("blocksMap").toObject();
		if(currLineBlockMapObject.contains(currBlockId)) {
			const QString startingBlock1 = std::get<2>(*argsref)->at(i).toObject().value("startingBlock1").toString();
			const QString startingBlock2 = std::get<2>(*argsref)->at(i).toObject().value("startingBlock2").toString();
			const QString endingBlock1 = std::get<2>(*argsref)->at(i).toObject().value("endingBlock1").toString();
			const QString endingBlock2 = std::get<2>(*argsref)->at(i).toObject().value("endingBlock2").toString();
			QString nextBlock1 = currLineBlockMapObject.value(currBlockId).toObject().value("nextBlock1").toString();
			QString nextBlock2 = currLineBlockMapObject.value(currBlockId).toObject().value("nextBlock2").toString();
			QString prevBlock1 = currLineBlockMapObject.value(currBlockId).toObject().value("prevBlock1").toString();
			QString prevBlock2 = currLineBlockMapObject.value(currBlockId).toObject().value("prevBlock2").toString();
			if(nextOrPrevBlockId == nextBlock1 )return {false, true};
			else if(nextOrPrevBlockId == nextBlock2) return {false, false};
			else if(nextOrPrevBlockId == prevBlock1)return {true, true};
			else if(nextOrPrevBlockId == prevBlock2)return {true, false};
			else if("START_T" == prevBlock1 && startingBlock2 == nextOrPrevBlockId) return {true, true};
			else if("START_B" == prevBlock1 && startingBlock1 == nextOrPrevBlockId) return {true, true};
			else if("END_T" == nextBlock1 && endingBlock2 == nextOrPrevBlockId)return {false, true};
			else if("END_B" == nextBlock1 && endingBlock1 == nextOrPrevBlockId)return {false, true};
			else qFatal("T3CTCOffice::determineAuthorityDirection() -> Origin Track found, but nextOrPrevBlock is not neighbouring block.");
		}
	}
	qFatal("T3CTCOffice::determineAuthorityDirection() -> Origin Track not found");
	return QVarLengthArray<bool, 2>();
}

inline QJsonArray T3CTCOffice::searchPathsFromCsv(const QString filePath, const QJsonObject *stationToBlockIdMap, const QJsonArray dispatchMetaInfo,  MODU_ARGS_REF argsref) {
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
	//QTime arrivalTime = QTime::fromString(dispatchMetaInfo.at(3).toString("HH:mm"));
	//QTime dispatchTime = arrivalTime.addSecs(-60 * delayTime.minute()).addSecs(delayTime.second());
	//QJsonArray newDispatchMetaInfo = {dispatchMetaInfo.at(0).toString(), yardId, stationBlockIds.last(), dispatchTime.toString("HH:mm")};

	//QJsonArray finalPath;
	//QList<QList<QString>> finalPaths;

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
					QJsonArray currPossiblePaths = searchPathsFromMetaInfo(currDispatchMetaInfo, argsref);

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


inline QJsonArray T3CTCOffice::searchPathsFromMetaInfo(const QJsonArray dispatchMetaInfo, MODU_ARGS_REF argsref) {
	//first, locate the correct trackConstantObject from trackConstantObjects
	QJsonObject targetedBlockMap;
	QString startingBlock1, startingBlock2, endingBlock1, endingBlock2;
	for(qsizetype i = 0; i < std::get<2>(*argsref)->size(); ++i) {
		//assume trackConstantObjects is in right format
		QJsonObject currBlocksMap = std::get<2>(*argsref)->at(i).toObject().value("blocksMap").toObject();
		if(currBlocksMap.contains(dispatchMetaInfo.at(1).toString())
				&& currBlocksMap.contains(dispatchMetaInfo.at(2).toString())) {
			targetedBlockMap = currBlocksMap;
			startingBlock1 = std::get<2>(*argsref)->at(i).toObject().value("startingBlock1").toString();
			startingBlock2 = std::get<2>(*argsref)->at(i).toObject().value("startingBlock2").toString();
			endingBlock1 = std::get<2>(*argsref)->at(i).toObject().value("endingBlock1").toString();
			endingBlock2 = std::get<2>(*argsref)->at(i).toObject().value("endingBlock2").toString();
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

inline void T3CTCOffice::writeToPlcInputFromMetaInfo(const QString blockId, const QJsonArray metaInfo,  MODU_ARGS_REF argsref) {
	//metaInfo [maintananceMode,suggestedSpeed,switchPosition,authorityTo]
	if(metaInfo.size() != 4
			|| !metaInfo.at(0).isBool()
			|| !metaInfo.at(1).isDouble()
			|| !metaInfo.at(2).isBool()
			|| !metaInfo.at(3).isString())
		qFatal("T3CTCOffice::writeToPlcInputFromMetaInfo() -> meta information incorrect.");
	QString authorityTo = metaInfo.at(3).toString();
	QJsonArray authorityPath;
	//if current requested new authority is not empty, clear the authority for related blocks
	if(authorityTo != "__" && !authorityTo.isEmpty()) {
		authorityPath = searchPathForAuthority(blockId, authorityTo, argsref);
		//after clearning the authorities, set the authorities,speed,etc  for all blocks included in this authority path
		for(qsizetype i = 0; i < authorityPath.size(); ++i) {
			//for each block included in this authority path, find all blocks authorized on this current block, and clear their authority
			QJsonArray allBlocksAuthorizedFromCurrBlock =
				(*std::get<0>(*argsref))(authorityPath.at(0).toString(), "CTC_AUTHPATH", std::get<3>(*argsref)).toJsonArray();
			if(!allBlocksAuthorizedFromCurrBlock.isEmpty() && allBlocksAuthorizedFromCurrBlock.first().toString() != "")
				setAuthorityFromPath(&allBlocksAuthorizedFromCurrBlock, false, argsref);
		}
		setAuthorityFromPath(&authorityPath, true, argsref);
	}
	//set authority, commanded speed, etc ONLY for current block
	QString CTCPLCIO = GET_TRACKVAR_F(blockId, "COM[CTC|KC]_CTCPLCIO", argsref).toString();
	{
		Q_ASSERT(CTCPLCIO.size() == 32);
		CTCPLCIO[0] =  '1';//if sent always connected assumed
		CTCPLCIO[1] = metaInfo.at(0).toBool() ? '1' : '0';
		//authority is handled from previous step
		QString truncatedSuggestedSpeed = QString::number(static_cast<uint8_t>(MPH2KMH_F(metaInfo.at(1).toDouble())), 2).rightJustified(8, '0');
		for(uint i = 9; i >= 2; --i) {
			CTCPLCIO[i] = truncatedSuggestedSpeed[i - 2];
		}
		CTCPLCIO[21] =  metaInfo.at(2).toBool() ? '1' : '0';
	}
	SET_TRACKVAR_F(blockId, "COM[CTC|KC]_CTCPLCIO", CTCPLCIO, argsref);
}

inline QJsonArray T3CTCOffice::readPlcInputToMetaInfo(const QString blockId, MODU_ARGS_REF argsref) {
	std::tuple<bool, float, bool, QString> metaInfoValue;
	QString CTCPLCIO = 	(*std::get<0>(*argsref))(blockId, "COM[CTC|KC]_CTCPLCIO", std::get<3>(*argsref)).toString();
	Q_ASSERT(CTCPLCIO.length() == 32);
	QStringList AUTHPATH_splitted = (*std::get<0>(*argsref))(blockId, "CTC_AUTHPATH", std::get<3>(*argsref)).toString().split("|");
	QString truncatedSuggestedSpeed = QString(8, '0');
	for(uint i = 9; i >= 2; --i) {
		truncatedSuggestedSpeed[i - 2] = CTCPLCIO[i];
	}
	std::get<0>(metaInfoValue) = CTCPLCIO[1] == '1';
	std::get<1>(metaInfoValue) = MPH2KMH_F(truncatedSuggestedSpeed.toInt(nullptr, 2));
	std::get<2>(metaInfoValue) = CTCPLCIO[10] == '1';
	std::get<3>(metaInfoValue) = AUTHPATH_splitted.last();
	return QJsonArray{std::get<0>(metaInfoValue), std::get<1>(metaInfoValue), std::get<2>(metaInfoValue), std::get<3>(metaInfoValue)};
}

inline QJsonArray T3CTCOffice::searchPathForAuthority(const QString originBlock, const QString destiBlock, MODU_ARGS_REF argsref) {
	QJsonArray metaInfoForPathFinding
		= {"____", originBlock, destiBlock, "__:__"};
	QJsonArray paths = searchPathsFromMetaInfo(metaInfoForPathFinding, argsref);
	if(paths.size() == 0) return QJsonArray();
	//which path is the shortest? use that one!
	qsizetype currMinInd = 0;
	for(qsizetype i = 0; i < paths.count(); ++i) {
		qsizetype currLength = paths.at(i).toArray().count();
		if(currLength > 0
				&& paths.at(i).toArray().count() < paths.at(currMinInd).toArray().count())
			currMinInd = i;
	}
	return paths.at(currMinInd).toArray();
}

inline void T3CTCOffice::toggleConnection(bool newConnectionState, MODU_ARGS_REF argsref) {
	for(const QJsonValue& currLine : qAsConst(*std::get<3>(*argsref))) {
		for(auto& blockId : currLine.toObject().keys()) {
			QString CTCPLCIO = 	(*std::get<0>(*argsref))(blockId, "COM[CTC|KC]_CTCPLCIO", std::get<3>(*argsref)).toString();
			CTCPLCIO[0] = newConnectionState ? '1' : '0';
			(*std::get<1>(*argsref))(blockId, "COM[CTC|KC]_CTCPLCIO", std::get<3>(*argsref), CTCPLCIO);
		}
	}
}




inline void T3CTCOffice::setAuthorityFromPath(const QJsonArray *authorityPath, bool b, MODU_ARGS_REF argsref) {
	QString pathAsString;
	if(b) {
		for(qsizetype i = 0; i < authorityPath->size(); ++i) {
			if(i > 0) pathAsString += "|";
			pathAsString = pathAsString + authorityPath->at(i).toString();
		}
	}
	for(qsizetype k = 0; k < authorityPath->size() - 1; ++k) {
		QPair<QString, QString> blockIdPair = {authorityPath->at(k).toString(), authorityPath->at(k + 1).toString()};
		(*std::get<1>(*argsref))(blockIdPair.first, "CTC_AUTHPATH", std::get<3>(*argsref), pathAsString);
		QString CTCPLCIO = (*std::get<0>(*argsref))(blockIdPair.first, "COM[CTC|KC]_CTCPLCIO", std::get<3>(*argsref)).toString();
		QVarLengthArray<bool, 2> authorityDirection = determineAuthorityDirection(blockIdPair.first, blockIdPair.second, argsref);
//			(b && k <= authorityPath->size() - 1)
		/*? */
//			: std::initializer_list<bool> {false, false};
		QString numberOfBlockAuthorizing = b ? QString::number(static_cast<uint8_t>(authorityPath->size() - k), 2).rightJustified(8, '0') : QString(8, '0');
		Q_ASSERT(authorityDirection.size() == 2);
		CTCPLCIO[11] = authorityDirection.at(0) ? '1' : '0';
		CTCPLCIO[12] = authorityDirection.at(1) ? '1' : '0';
		for(uint i = 20; i >= 13; --i) {
			CTCPLCIO[i] = numberOfBlockAuthorizing[i - 13];
		}
		CTCPLCIO[1] = b ? '1' : '0';
		Q_ASSERT(CTCPLCIO.size() == 32);
		(*std::get<1>(*argsref))(blockIdPair.first, "COM[CTC|KC]_CTCPLCIO", std::get<3>(*argsref), CTCPLCIO);
	}
}
#endif // T3CTCOFFICE_HPP
