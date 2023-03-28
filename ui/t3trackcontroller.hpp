#ifndef T3TRACKCONTROLLER_HPP
#define T3TRACKCONTROLLER_HPP

#include <QtCore>
#include <QtQml>

using MODU_ARGS_REF
	= const std::tuple<const std::function<QVariant(const QString, const QString, const QJsonArray*)>*
	  , const std::function<void(const QString, const QString, QJsonArray*, const QVariant)>*
	  ,  QJsonArray*, QJsonArray*, QJsonArray*>*;

class T3TrackController {
  public:
	static void iterate(MODU_ARGS_REF argsref, QJSEngine* plcRuntime, QJSValue* plcFunction);
	static bool connectedToCTCOffice;
	static bool connectedToTrackModel;
	static void addPlcScriptFromCsv(const QString filePath, QJSEngine*runTime, QJSValue* function);
	static QJsonArray collectPlcInput(const QString blockId, const QJsonArray* trackVariablesObjects);
	static QJsonArray generatePlcOutput(const QJsonArray plcInput, QJSEngine*runTime, QJSValue* function);
	static void writePlcOutput(const QString blockId, const QJsonArray* plcOutput, QJsonArray* trackVariablesObjects);
};

inline void T3TrackController::iterate(MODU_ARGS_REF argsref, QJSEngine* plcRuntime, QJSValue* plcFunction ) {
	//for each route
	for(uint lineInd = 0; lineInd <  std::get<3>(*argsref)->size(); ++lineInd) {
		QStringList blockIds = std::get<3>(*argsref)->at(lineInd).toObject().keys();
		for(uint blockInd = 0; blockInd < blockIds.size(); ++blockInd) {
			QString CTCPLCIO = (*std::get<0>(*argsref))(blockIds.at(blockInd), "COM[CTC|KC]_CTCPLCIO", std::get<3>(*argsref)).toString();
			QString BCNPLCOUT = (*std::get<0>(*argsref))(blockIds.at(blockInd), "COM[KC|KM]_BCNPLCOUT", std::get<3>(*argsref)).toString();
			QString KMPLCIO = (*std::get<0>(*argsref))(blockIds.at(blockInd), "COM[KC|KM]_KMPLCIO", std::get<3>(*argsref)).toString();
			QString KCPLCIN = (*std::get<0>(*argsref))(blockIds.at(blockInd), "KC_KCPLCIN", std::get<3>(*argsref)).toString();
			QJsonArray CTCPLCIO_jsonArray, BCNPLCOUT_jsonArray, KMPLCIO_jsonArray, KCPLCIN_jsonArray;
			for(uint i = 0; i < 32; ++i) {
				CTCPLCIO_jsonArray.push_back(CTCPLCIO[i] == '1');
				BCNPLCOUT_jsonArray.push_back(BCNPLCOUT[i] == '1');
				KMPLCIO_jsonArray.push_back(KMPLCIO[i] == '1');
				KCPLCIN_jsonArray.push_back(KCPLCIN[i] == '1');
			}
			QJSValueList plcArgs = {
				plcRuntime->toScriptValue(CTCPLCIO_jsonArray),
				plcRuntime->toScriptValue(BCNPLCOUT_jsonArray),
				plcRuntime->toScriptValue(KMPLCIO_jsonArray),
				plcRuntime->toScriptValue(KCPLCIN_jsonArray)
			};
			plcFunction->call(plcArgs);
			CTCPLCIO_jsonArray = plcRuntime->fromScriptValue<QJsonArray>(plcArgs.at(0));
			BCNPLCOUT_jsonArray = plcRuntime->fromScriptValue<QJsonArray>(plcArgs.at(1));
			KMPLCIO_jsonArray = plcRuntime->fromScriptValue<QJsonArray>(plcArgs.at(2));
			KCPLCIN_jsonArray = plcRuntime->fromScriptValue<QJsonArray>(plcArgs.at(3));
			for(uint i = 0; i < 32; ++i) {
				CTCPLCIO[i] = CTCPLCIO_jsonArray.at(i).toBool() ? '1' : '0';
				BCNPLCOUT[i] = BCNPLCOUT_jsonArray.at(i).toBool() ? '1' : '0';
				KMPLCIO[i] = KMPLCIO_jsonArray.at(i).toBool() ? '1' : '0';
				KCPLCIN[i] = KCPLCIN_jsonArray.at(i).toBool() ? '1' : '0';
			}
			(*std::get<1>(*argsref))(blockIds.at(blockInd), "COM[CTC|KC]_CTCPLCIO", std::get<3>(*argsref), CTCPLCIO);
			(*std::get<1>(*argsref))(blockIds.at(blockInd), "COM[KC|KM]_BCNPLCOUT", std::get<3>(*argsref), BCNPLCOUT);
			(*std::get<1>(*argsref))(blockIds.at(blockInd), "COM[KC|KM]_KMPLCIO", std::get<3>(*argsref), KMPLCIO);
			(*std::get<1>(*argsref))(blockIds.at(blockInd), "KC_KCPLCIN", std::get<3>(*argsref), KCPLCIN);
		}
	}
}

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
