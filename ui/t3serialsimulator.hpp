//#ifndef T3SERIALSIMULATOR_HPP
//#define T3SERIALSIMULATOR_HPP
//#include <QtCore>
//#include "t3serialserver.hpp"



//class T3SerialSimulator: QObject {
//	Q_OBJECT
//  public:
//	T3SerialSimulator(QObject *parent = nullptr);

//	static QVector<char> buffer;
//	static std::array<bool, 6> buttons;
//	Q_PROPERTY(QStringList lcd READ getLcd NOTIFY onLcdChanged)
//	QStringList getLcd();


//	Q_INVOKABLE void clickButton(int whichButton);

//	void(*readFunction)(char*, int) = [](char* c, int n) {
//		if(n > buffer.size()) {
//			qInfo() << "overflow";
//		}
//		for(int i = 0; i < n; ++i) {
//			c[i] = buffer.takeFirst();
//		}
//		return ;
//	};

//	size_t(*writeFunction)(const char*) = [](const char* c) {
//		for(int i = 0; c[i] != '\0'; ++i) {
//			buffer.push_back(c[i]);
//		}
//		return static_cast<size_t>(2);
//	};

//	int(*availableFunction)(void) = [](void) {
//		return buffer.size();
//	};

//	T3SerialServer serialServer = T3SerialServer(this->readFunction, this->writeFunction, this->availableFunction);
//	// QObject interface
//  protected:
//	QString toQString() {
//		QString toRet;
//		for(int i = 0; i < buffer.count(); ++i) {
//			toRet.push_back(buffer.at(i));
//		}
//		return toRet;
//	};
//	void timerEvent(QTimerEvent *event) Q_DECL_OVERRIDE;
//  signals:
//	void onLcdChanged();
//};


//inline T3SerialSimulator::T3SerialSimulator(QObject *parent) : QObject(parent) {

//}

//inline void T3SerialSimulator::clickButton(int whichButton) {
//	std::get<0>(buttons) = std::get<1>(buttons) = std::get<2>(buttons) = std::get<3>(buttons) = std::get<4>(buttons) = std::get<5>(buttons) = false;
//	if(whichButton == 0) std::get<0>(buttons) = true;
//	else if(whichButton == 1) std::get<1>(buttons) = true;
//	else if(whichButton == 2) std::get<2>(buttons) = true;
//	else if(whichButton == 3) std::get<3>(buttons) = true;
//	else if(whichButton == 4) std::get<4>(buttons) = true;
//	serialServer.event(&std::get<0>(buttons), &std::get<1>(buttons), &std::get<2>(buttons), &std::get<3>(buttons), &std::get<4>(buttons), &std::get<5>(buttons));
//}


//inline void T3SerialSimulator::timerEvent(QTimerEvent *event) {

//	Q_UNUSED(event);

//}
//#endif // T3SERIALSIMULATOR_HPP
