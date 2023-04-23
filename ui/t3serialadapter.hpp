#ifndef T3SERIALADAPTER_HPP
#define T3SERIALADAPTER_HPP
#include "t3prophelper.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QtCore>
class T3SerialAdapter: public QObject {
	Q_OBJECT
  public:
	MODU_ARGS_REF argsref = nullptr;
	T3SerialAdapter(QObject *parent = nullptr);
	QSerialPort serialPort;
	bool scanForSTM32(void);
	void clock();
	void setArgsRef( MODU_ARGS_REF argsref);
  signals:
	void hardwareConnected();
	void hardwareDisconnected();
	void onTrainObjectsChanged();
	// QObject interface
  protected:
	void parseRequestAndRespond(const QString command);
	void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
};

inline bool T3SerialAdapter::scanForSTM32(void) {
	const auto serialPortInfos = QSerialPortInfo::availablePorts();
	for (const QSerialPortInfo &portInfo : serialPortInfos) {
		if(portInfo.manufacturer() == "STMicroelectronics") {
			qDebug() << "\n"
					 << "Port:" << portInfo.portName() << "\n"
					 << "Location:" << portInfo.systemLocation() << "\n"
					 << "Description:" << portInfo.description() << "\n"
					 << "Manufacturer:" << portInfo.manufacturer() << "\n"
					 << "Serial number:" << portInfo.serialNumber() << "\n"
					 << "Vendor Identifier:"
					 << (portInfo.hasVendorIdentifier()
						 ? QByteArray::number(portInfo.vendorIdentifier(), 16)
						 : QByteArray()) << "\n"
					 << "Product Identifier:"
					 << (portInfo.hasProductIdentifier()
						 ? QByteArray::number(portInfo.productIdentifier(), 16)
						 : QByteArray());

			serialPort.setPort(portInfo);
			uint8_t result =
				(serialPort.setBaudRate(QSerialPort::Baud9600) << 4u) |
				(serialPort.setDataBits(QSerialPort::Data8) << 3u) |
				(serialPort.setStopBits(QSerialPort::OneStop) << 2u) |
				(serialPort.setFlowControl(QSerialPort::NoFlowControl) << 1u) |
				(serialPort.open(QSerialPort::ReadWrite) << 0u);
			if(result != 0b11111) {
				qInfo() << "T3SerialAdapter::scanForSTM32() -> STM32 Device Found but cannot be configured: " << QString::number(static_cast<unsigned int>(result), 2);
				return false;
			} else {
				qInfo() << "T3SerialAdapter::() -> STM32 Connected !.";
				emit hardwareConnected();
				return true;
			}
		}
	}
	qInfo() << "T3SerialAdapter::scanForSTM32() -> No STM32 Device Found.";
	return false;
}

inline void T3SerialAdapter::clock() {
	//
	QIODevice::OpenMode openMode = serialPort.openMode();
	switch(openMode) {
	case QIODevice::OpenModeFlag::NotOpen:
		scanForSTM32();
		break;
	case QIODevice::OpenModeFlag::ReadWrite:
		if(serialPort.canReadLine()) {
			QString readedLine = serialPort.readLine();
			qDebug() << readedLine;
			parseRequestAndRespond(readedLine);
		}
		break;
	}
}

inline void T3SerialAdapter::setArgsRef(MODU_ARGS_REF  argsref) {
	this->argsref = argsref;
}


inline void T3SerialAdapter::parseRequestAndRespond(const QString command) {
	if(argsref == nullptr) return;
	qsizetype positionStartDelimiter = command.indexOf("?");
	qsizetype positionEndDelimiter = command.indexOf("!");
	if((positionStartDelimiter >= positionEndDelimiter) || (positionStartDelimiter != 0)) return;
	QList<QString> splittedCommand = command.mid(positionStartDelimiter + 2, 38).split("}{");
	if(splittedCommand.count() != 4) return;
	//QMetaType metaType = GET_TRAIN_F();
	//check if this train exists
	for(qsizetype i = 0; i < std::get<4>(*argsref)->count(); ++i) {
		QJsonObject currTrainObject = std::get<4>(*argsref)->at(i).toObject();
		if(currTrainObject.value(QString("NM_ID")).toString().trimmed() == splittedCommand.at(3).trimmed()) {
			QString propName = splittedCommand.at(1).trimmed();
			if(!currTrainObject.contains(splittedCommand.at(1).trimmed())) return;
			QString newCommand = command;
			if(splittedCommand.at(0).compare(QString("SET")) == 0) {
				newCommand.replace("SET", "REC");
				//QJsonValue::Type typeOfCurrentValue = currentValue.type();
				if(propName == "COM[NC_NM]_EXTLIGHT"
						|| propName == "COM[NC_NM]_INTLIGHT"
						|| propName == "COM[NC_NM]_LDOOR"
						|| propName == "COM[NC_NM]_RDOOR"
						|| propName == "COM[NC_NM]_EBRAKE"
						|| propName == "COM[NC_NM]_SBRAKE")
					currTrainObject.insert(propName, static_cast<bool>(splittedCommand.at(2).toUInt()));
				else if(propName == "NC_R")
					currTrainObject.insert(propName, static_cast<int>(splittedCommand.at(2).toUInt()));
				else if(propName == "NC_KP" || propName == "NC_KI")
					currTrainObject.insert(propName, 0.01f * static_cast<float>(splittedCommand.at(2).toUInt()));
				std::get<4>(*argsref)->insert(i, currTrainObject);
				Q_EMIT onTrainObjectsChanged();
			} else {
				newCommand.replace("GET", "REC");
				QJsonValue currentValue = currTrainObject.value(propName);
				currentValue = currTrainObject.value(propName);
				QString newValueAsString = "0000";
				switch(currentValue.type()) {
				case QJsonValue::Type::Bool:
					newValueAsString = currentValue.toBool() ? "000001" : "000000";
					break;
				case QJsonValue::Type::Double:
					newValueAsString = QString::number((currentValue.toInt(88888) == 88888) ? (qRound(currentValue.toDouble() * 100.0f)) : (currentValue.toInt()));
					if(newValueAsString.count() < 6) newValueAsString = newValueAsString.rightJustified( 6, '0');
					else if(newValueAsString.count() > 6) newValueAsString = newValueAsString.mid(0, 6);
					break;
				default:
					break;
				}
				for(uint i = 28; i < (28 + 6); ++i) {
					newCommand.operator[](i) = newValueAsString.at(i - 28);
				}
				serialPort.write(newCommand.toLocal8Bit());
			}
			return;
		}
	}

}

inline void T3SerialAdapter::timerEvent(QTimerEvent *event) {
	Q_UNUSED(event);
	clock();
}

inline T3SerialAdapter::T3SerialAdapter(QObject *parent) : QObject(parent) {
	QObject::connect(&serialPort, &QSerialPort::aboutToClose, this, [this]() {
		qInfo() << "T3SerialAdapter::() -> STM32 Disconnected !";
		emit this->hardwareDisconnected();
	});
}

#endif // T3SERIALADAPTER_HPP
