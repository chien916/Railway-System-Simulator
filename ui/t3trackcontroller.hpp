#ifndef T3TRACKCONTROLLER_HPP
#define T3TRACKCONTROLLER_HPP

#include <QtCore>
#include <QtQml>
class T3TrackController {
  public:
	static bool connectedToCTCOffice;
	static bool connectedToTrackModel;
	static void addPlcScriptFromCsv(const QString filePath, QJSEngine*runTime, QJSValue* function);
	static QJsonArray collectPlcInput(const QString blockId, const QJsonArray* trackVariablesObjects);
	static QJsonArray generatePlcOutput(const QJsonArray plcInput, QJSEngine*runTime, QJSValue* function);
	static void writePlcOutput(const QString blockId, const QJsonArray* plcOutput, QJsonArray* trackVariablesObjects);
};

inline void T3TrackController::addPlcScriptFromCsv(const QString filePath, QJSEngine*runTime, QJSValue* function) {
	QString newFilePath = QString(filePath).replace("file:///", "");
	QFile file(newFilePath);
	if (!file.open(QFile::ReadOnly | QFile::Text)) return;
	QTextStream in(&file);
	QJSValue retrievedCode = runTime->evaluate(in.readAll());
	//test run to see if script works
	if(retrievedCode.isCallable()) *function = retrievedCode;
}

inline QJsonArray T3TrackController::collectPlcInput(const QString blockId, const QJsonArray *trackVariablesObjects) {
	for(qsizetype i = 0; i < trackVariablesObjects->size(); ++i) { //for every line
		QJsonObject currTrackVarObj = trackVariablesObjects->at(i).toObject();
		if(currTrackVarObj.find(blockId) != currTrackVarObj.end()) {
			QJsonObject currBlockVarObj = currTrackVarObj[blockId].toObject();
			QString plcInput = currBlockVarObj["plcInput"].toString();
			Q_ASSERT(plcInput.size() == 32);
			QJsonArray plcInputJsonArray;
			for(qsizetype i = 0; i < 32; ++i) {
				plcInputJsonArray.push_back(plcInput.at(i) == QChar('1') ? true : false);
			}
			return plcInputJsonArray;
			break;
		}
	}
	qFatal("T3CTCOffice::collectPlcInput() -> cannot find the block id insetTrackPropertyted");
	return QJsonArray();
}

inline QJsonArray T3TrackController::generatePlcOutput(const QJsonArray plcInput, QJSEngine*runTime, QJSValue* function) {
	//boolean array to 32bit input
	uint32_t plcInputValue = 0;
	Q_ASSERT(plcInput.size() == 32);
	for(qsizetype i = 0; i < 32; ++i) {
		plcInputValue <<= 1;
		plcInputValue += plcInput.at(i).toBool() ? 1 : 0;
	}
	QJSValue plcScriptArg = runTime->toScriptValue<uint32_t>(plcInputValue);
	QJSValue plcScriptRet = function->call(QJSValueList{plcScriptArg});
	uint32_t plcOutputValue = runTime->fromScriptValue<uint32_t>(plcScriptRet);
	QJsonArray plcOutputJsonArray;
	for(qsizetype i = 0; i < 32; ++i) {
		plcOutputJsonArray.push_back((plcOutputValue & (1 << i)) != 0 ? true : false);
	}
	return plcOutputJsonArray;
}

inline void T3TrackController::writePlcOutput(const QString blockId, const QJsonArray* plcOutput, QJsonArray *trackVariablesObjects) {
	//boolean array to 32bit input
	uint32_t plcOutputValue = 0;
	Q_ASSERT(plcOutput->size() == 32);
	for(qsizetype i = 0; i < 32; ++i) {
		plcOutputValue <<= 1;
		plcOutputValue += plcOutput->at(i).toBool() ? 1 : 0;
	}
	bool fieldIsFound = false;
	for(qsizetype i = 0; i < trackVariablesObjects->size(); ++i) { //for every line
		QJsonObject currTrackVarObj = trackVariablesObjects->at(i).toObject();
		if(currTrackVarObj.find(blockId) != currTrackVarObj.end()) {
			QJsonObject currBlockVarObj = currTrackVarObj[blockId].toObject();
			currBlockVarObj["plcOutput"] = QString::number(plcOutputValue);
			currTrackVarObj[blockId] = currBlockVarObj;
			trackVariablesObjects->operator[](i) = currTrackVarObj;
			fieldIsFound = true;
			break;
		}
	}
	if(!fieldIsFound)
		qFatal("T3TrackController::writePlcOutput() -> cannot find the block id");
}


#endif // T3TRACKCONTROLLER_HPP
