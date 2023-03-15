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
	static QList<QList<QString>> searchPaths (const QString originBlockId, const QString destBlockId, QSet<QString> pathSet
							  , const QString& startingBlock1, const QString& startingBlock2, const QString& endingBlock1, const QString& endingBlock2
							  , QJsonObject* targetedBlockMap);
	static void addToDispatchQueueWithPaths(QJsonArray* queue, const QJsonArray dispatchMetaInfo, const QJsonArray path);
	static QJsonArray popFromDispatchQueueAtTime(QJsonArray* queue, QTime currTime);
	static QJsonArray calculateViewCooridnates(QJsonObject* rootObj_O);
};

inline QList<QList<QString> > T3CTCOffice::searchPaths(const QString originBlockId, const QString destBlockId, QSet<QString> pathSet
		, const QString &startingBlock1, const QString &startingBlock2, const QString &endingBlock1, const QString &endingBlock2
		, QJsonObject* targetedBlockMap) {
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
		if(blockCandidatesAndTraversables.at(0) == "START_T")
			blockCandidatesAndTraversables[0] = startingBlock2;
		else if(blockCandidatesAndTraversables.at(0) ==  "START_B")
			blockCandidatesAndTraversables[0] = startingBlock1;
		if(blockCandidatesAndTraversables.at(2) ==  "END_T")
			blockCandidatesAndTraversables[2]  = endingBlock2;
		else if(blockCandidatesAndTraversables.at(2) == "END_B")
			blockCandidatesAndTraversables[2] = endingBlock1;
		//determines if block is traversable. If not, remove it from list
		for(qsizetype i = 3; i >= 0; --i) {
			QString currBlockCandidatesAndTraversable
				= blockCandidatesAndTraversables.at(i);
			if(currBlockCandidatesAndTraversable.split("_").size() != 3
					|| pathSet.contains(currBlockCandidatesAndTraversable)
					|| (currBlockObject.value("direction").toString() == "FORWARD" && i < 2)
					|| (currBlockObject.value("direction").toString() == "REVERSED" && i >= 2))
				blockCandidatesAndTraversables.remove(i);
		}
		//traverse
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
												, startingBlock1, startingBlock2, endingBlock1, endingBlock2, targetedBlockMap);
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


inline void T3CTCOffice::addToDispatchQueueWithPaths(QJsonArray *queue, const QJsonArray dispatchMetaInfo, const QJsonArray path) {
	QJsonObject dispatchObject;
	if(dispatchMetaInfo.size() != 4)
		qFatal("T3CTCOffice::addToDispatchQueueWithPaths() meta information is not four");
	QTime dispatchTime = QTime::fromString(dispatchMetaInfo.at(3).toString(), "HH:mm");
	if(!dispatchTime.isValid())
		qFatal("T3CTCOffice::addToDispatchQueueWithPaths() time from meta info is not valid");
	for(qsizetype i = 0; i < queue->size(); ++i) {
		QTime currDispatchTime = QTime::fromString(queue->at(i).toObject().value("dispatchTime").toString(), "HH:mm");
		if(!currDispatchTime.isValid())
			qFatal("T3CTCOffice::addToDispatchQueueWithPaths() time from queue is not valid");
		if(currDispatchTime > dispatchTime) {
			queue->insert(i, dispatchMetaInfo);
			return;
		}
	}
	queue->append(dispatchMetaInfo);//if empty or later than all
}

inline QJsonArray T3CTCOffice::popFromDispatchQueueAtTime(QJsonArray *queue, QTime currTime) {
	QJsonArray ret;
	do {
		if(queue->empty()) break;
		QTime currDispatchTime = QTime::fromString(queue->first().toObject().value("dispatchTime").toString(), "HH:mm");
		if(!currDispatchTime.isValid())
			qFatal("T3CTCOffice::popFromDispatchQueueAtTime() time from queue is not valid");
		if(currDispatchTime > currTime) break;
		ret.push_back(queue->takeAt(0));
	} while(true);
	return ret;
}

inline QJsonArray T3CTCOffice::calculateViewCooridnates(QJsonObject* rootObj_O) {

//	QJsonValue blocksMap_O = (*rootObj_O)["blocksMap"];
//	QJsonValue currTopBlockId_s = (*rootObj_O)["startingBlock1"];
//	QJsonValue currBottomBlockId_s = (*rootObj_O)["startingBlock2"];
//	QJsonArray currTopBlockLine_A = QJsonArray();
//	QJsonArray currBottomBlockLine_A = QJsonArray();
//	QJsonArray currBlockGridList_A = QJsonArray();
//	while (true) {
//		QJsonValue currIsDoubleLine_b = currBottomBlockId_s.toString() != "";
//		QJsonValue currTopBlockObj_O = blocksMap_O.toObject()[currTopBlockId_s.toString()];
//		QJsonValue currBottomBlockObj_O
//			= currIsDoubleLine_b.toBool() ? blocksMap_O[currBottomBlockId_s.toString()] : QJsonValue(QJsonValue::Null);
////		std::function<void(QJsonValue&, QJsonValue&)> spreadLength_f = [&](QJsonValue & blockLine1_A, QJsonValue & blockLine2_A) {
////			QJsonValue theSmallerLength_n
////				= qMin(blockLine1_A.toArray().size(), blockLine2_A.toArray().size());
////			QJsonValue theBiggerLength_n
////				= qMax(blockLine1_A.toArray().size(), blockLine2_A.toArray().size());
////			QJsonValue theShorterBlockLine_O
////				= blockLine1_A.toArray().size() == theSmallerLength_n.toInt()
////				  ? blockLine1_A : blockLine2_A;
////			QJsonValue newSmallerWidth_n = theBiggerLength_n.toDouble() / theSmallerLength_n.toDouble();
////			{
////				QJsonObject theShorterBlockLine_O_TEMP = theShorterBlockLine_O.toObject();
////				for(QJsonObject::iterator currTopBlock_A = theShorterBlockLine_O_TEMP.begin()
////					;currTopBlock_A != theShorterBlockLine_O_TEMP.end();++currTopBlock_A) {
////					currTopBlock_A[1] = newSmallerWidth_n;
////				}
////			}
////		};

//		if (!currIsDoubleLine_b.toBool()
//				&& currTopBlockId_s.toString() == (*rootObj_O)["endingBlock1"].toString()
//				&& currBottomBlockId_s.toString() == (*rootObj_O)["endingBlock2"].toString()) {
//			currTopBlockLine_A.push([currTopBlockId_s, 1]);
//			currBlockGridList_A.push([[...currTopBlockLine_A], []]);
//			break;
//		} else if (currIsDoubleLine_b
//				   && currTopBlockId_s == = rootObj_O["endingBlock1"]
//											&& currBottomBlockId_s == = rootObj_O["endingBlock2"]) {
//			currTopBlockLine_A.push([currTopBlockId_s, 1]);
//			currBottomBlockLine_A.push([currBottomBlockId_s, 1]);
//			spreadLength_f(currTopBlockLine_A, currBottomBlockLine_A);
//			currBlockGridList_A.push([[...currTopBlockLine_A], [...currBottomBlockLine_A]]);
//			break;
//		} else if (currIsDoubleLine_b
//				   && currTopBlockId_s != = currBottomBlockId_s
//											&& (((currTopBlockId_s == = rootObj_O["endingBlock1"]
//													|| (currTopBlockObj_O["prevBlock2"] != = ""
//															&& currTopBlockObj_O["prevBlock2"] != = "PASSIVE"))
//													&& currBottomBlockId_s != = rootObj_O["endingBlock2"]))) {
//			currBottomBlockLine_A.push([currBottomBlockId_s, 1]);
//			currBottomBlockId_s = currBottomBlockObj_O["nextBlock1"];
//		} else if (currIsDoubleLine_b
//				   && currTopBlockId_s != = currBottomBlockId_s
//											&& (((currBottomBlockId_s == = rootObj_O["endingBlock2"]
//													|| (currBottomBlockObj_O["prevBlock2"] != = ""
//															&& currBottomBlockObj_O["prevBlock2"] != = "PASSIVE"))
//													&& currTopBlockId_s != = rootObj_O["endingBlock1"]))) {
//			currTopBlockLine_A.push([currTopBlockId_s, 1]);
//			currTopBlockId_s = currTopBlockObj_O["nextBlock1"];
//		} else if (currIsDoubleLine_b
//				   && (currTopBlockObj_O["prevBlock2"] != = "" && currTopBlockObj_O["prevBlock2"] != = "PASSIVE")
//				   && (currBottomBlockObj_O["prevBlock2"] != = "" && currBottomBlockObj_O["prevBlock2"] != = "PASSIVE")) {
//			spreadLength_f(currTopBlockLine_A, currBottomBlockLine_A);
//			currBlockGridList_A.push([[...currTopBlockLine_A], [...currBottomBlockLine_A]]);
//			currTopBlockLine_A = [];
//			currBottomBlockLine_A = [];
//			currIsDoubleLine_b = false;
//			currBlockGridList_A.push([[[currTopBlockId_s, 2]], []]); //switch has min width 2
//			// currTopBlockLine_A.push([currTopBlockId_s, 1]);
//			currTopBlockId_s = currTopBlockObj_O["nextBlock1"];
//			currBottomBlockId_s = currTopBlockObj_O["nextBlock2"];
//		} else if (!currIsDoubleLine_b
//				   && !currIsDoubleLine_b
//				   && currTopBlockObj_O["nextBlock2"] != = "") {
//			// currTopBlockLine_A.push([currTopBlockId_s, 1]);
//			currBlockGridList_A.push([[...currTopBlockLine_A], []]);
//			currBlockGridList_A.push([[[currTopBlockId_s, 2]], []]); //switch has min width 2
//			currTopBlockLine_A = [];
//			currIsDoubleLine_b = true;
//			currTopBlockId_s = currTopBlockObj_O["nextBlock1"];
//			currBottomBlockId_s = currTopBlockObj_O["nextBlock2"];
//		} else if (currIsDoubleLine_b) {
//			currTopBlockLine_A.push([currTopBlockId_s, 1]);
//			currBottomBlockLine_A.push([currBottomBlockId_s, 1]);
//			currTopBlockId_s = currTopBlockObj_O["nextBlock1"];
//			currBottomBlockId_s = currBottomBlockObj_O["nextBlock1"];
//		} else if (!currIsDoubleLine_b) {
//			currTopBlockLine_A.push([currTopBlockId_s, 1]);
//			currTopBlockId_s = currTopBlockObj_O["nextBlock1"];
//		}
//	}
//	let currBlockFlattenedTopList_A = [];
//	let currBlockFlattenedBottomList_A = [];
//	let currTopX_n = 0;
//	let currBottomX_n = 0;
//	for (let currBlockGrid_A of currBlockGridList_A) {
//		for (let currTopBlock_A of currBlockGrid_A[0]) {
//			currBlockFlattenedTopList_A.push([currTopBlock_A[0], [0, currTopX_n], currTopBlock_A[1]]);
//			currTopX_n += currTopBlock_A[1];
//		}
//		for (let currBottomBlock_A of currBlockGrid_A[1]) {
//			currBlockFlattenedBottomList_A.push([currBottomBlock_A[0], [1, currBottomX_n], currBottomBlock_A[1]]);
//			currBottomX_n += currBottomBlock_A[1];
//		}
//		currBottomX_n = currTopX_n;
//	}
//	let concatedBlockFlattenedList_A
//		= currBlockFlattenedTopList_A.concat(currBlockFlattenedBottomList_A);
//	return concatedBlockFlattenedList_A;

}

#endif // T3CTCOFFICE_HPP
