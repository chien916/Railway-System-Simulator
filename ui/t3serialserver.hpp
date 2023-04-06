#ifndef T3SERIALSERVER_HPP
#define T3SERIALSERVER_HPP
#include <cstring>
#include <cstdlib>
#include <cstdint>

#define MAXCURSORIND 16
#define MAXVIEWIND 2
#define MAXPROPIND 9
#define MAXCHARCODE '9'
#define MINCHARCODE '0'

/**
 * @brief The T3SerialServer class
 */
class T3SerialServer {

  public:
	T3SerialServer(void(*readFunction)(char*, int), size_t(*writeFunction)(const char*), int(*availableFunction)(void));
	void flush(char* top, char* bottom);
	void event(const bool* back, const bool* left, const bool* right, const bool* top, const bool* bottom, const bool* ok);
  private:
	void update();
	enum Prop : int {
		ID = 0,
		EXTLIGHT = 1,
		INTLIGHT = 2,
		LDOOR = 3,
		RDOOR = 4,
		EBRAKE = 5,
		SBRAKE = 6,
		SETPOINT = 7,
		VELOCITY = 8,
		POWER = 9,
	};
	void(*readFunction)(char*, int);
	size_t(*writeFunction)(const char*);
	int(*availableFunction)(void);

	char topCharRow[18] = "00000000000000000";
	char bottomCharRow[18] = "00000000000000000";

	int viewIndex = 0, cursorIndex = 0, propIndex = 0;
	//0 - scroll view
	//1 - select modify varialble
	//2 - modifying variable
	char ioBuffer[5] = "0000";
	void rwProp(bool read);

	const char vars[MAXPROPIND + 1][18] {
		"TRAIN ID         ",
		"EXTERIOR LIGHT   ",
		"INTERIOR LIGHT   ",
		"LEFT DOOR        ",
		"RIGHT DOOR       ",
		"EMERGENCY BRAKE  ",
		"SERVICE BRAKE    ",
		"VELOCITY SETPOINT",
		"CURRENT VELOCITY ",
		"OUTPUT POWER     ",
	};
	const char mapper[MAXPROPIND + 1][20] {
		"NM_ID              ",
		"COM[NC_NM]_EXTLIGHT",
		"COM[NC_NM]_INTLIGHT",
		"COM[NC_NM]_LDOOR   ",
		"COM[NC_NM]_RDOOR   ",
		"COM[NC_NM]_EBRAKE  ",
		"COM[NC_NM]_SBRAKE  ",
		"NC_R               ",
		"NC_PREVY         ",
		"NC_U               "
	};

	char cmdBuffer[35] = "?{___}{___________________}{____}!";// 2 27 28
};

inline void T3SerialServer::flush(char* top, char* bottom) {

	strncpy(top, topCharRow, MAXCURSORIND + 1);
	strncpy(bottom, bottomCharRow, MAXCURSORIND + 1);
}

inline void T3SerialServer::event(const bool* back, const bool* left, const bool* right, const bool* top, const bool* bottom, const bool* ok) {
	switch(viewIndex) {
	case 0://menu scrool mode
		if(*back) {
		} else if(*left) {
			propIndex = (propIndex == 0) ? MAXPROPIND : (propIndex - 1);
		} else if(*right) {
			propIndex = (propIndex == MAXPROPIND ) ? 0 : (propIndex + 1);
		} else if(*top) {

		} else if(*bottom) {

		} else if(*ok) {
			rwProp(true);
			viewIndex = 2;//go back to main scooll view
		}
		break;
	case 2://modifying value mode
		if(*back) {
			viewIndex = 0;//go back to main scooll view
		} else if(*left) {
			cursorIndex = (cursorIndex == 0 ) ? (MAXCURSORIND ) : (cursorIndex + 1);
		} else if(*right) {
			cursorIndex = (cursorIndex == MAXCURSORIND ) ? ( 0) : (cursorIndex - 1);
		} else if(*top) {
			if(ioBuffer[cursorIndex] < MINCHARCODE || ioBuffer[cursorIndex] > MAXCURSORIND) return;
			ioBuffer[cursorIndex] = (ioBuffer[cursorIndex] == MAXCURSORIND) ? MINCHARCODE : (ioBuffer[cursorIndex] + 1);
		} else if(*bottom) {
			if(bottomCharRow[cursorIndex] < MINCHARCODE || bottomCharRow[cursorIndex] > MAXCURSORIND) return;
			ioBuffer[cursorIndex] = (ioBuffer[cursorIndex] == MINCHARCODE) ? MAXCURSORIND : (ioBuffer[cursorIndex] - 1);
		} else if(*ok) {
			rwProp(false);
			viewIndex = 0;//go back to main scooll view
		}
		break;
	}
	update();
}

inline void T3SerialServer::update() {
	switch(viewIndex) {
	case 0://menu scrool mode
		rwProp(true);
		strncpy(topCharRow, vars[propIndex], MAXCURSORIND + 1);
		strncpy(bottomCharRow, ioBuffer, MAXCURSORIND + 1);
		break;
	case 1://loading
		strncpy(topCharRow, "DATA LOADING...", MAXCURSORIND + 1);
		strncpy(bottomCharRow, "PLEASE WAIT!", MAXCURSORIND + 1);
		break;
	case 2://modifying value mode
		strncpy(topCharRow, vars[propIndex], MAXCURSORIND + 1);
		strncpy(bottomCharRow, ioBuffer, MAXCURSORIND + 1);
		bottomCharRow[cursorIndex] = '_';
		break;
	}
}

inline void T3SerialServer::rwProp(bool read) {
	int prevViewIndex = this->viewIndex;
	viewIndex = 1;
	update();
	const char _set[4] = "SET";
	const char _get[4] = "GET";
	const char _rec[4] = "REC";
	while(true) {
		//clears serial read buffer
		char helper;
		while(this->availableFunction() > 1) this->readFunction(&helper, 1);


		//form command to cmdBuffer
		for(int i = 0; i <= 2; ++i) cmdBuffer[i + 0] = read ? _get[i] : _set[i];
		for(int i = 0; i <= 19; ++i) cmdBuffer[i + 2] = mapper[propIndex][i];
		for(int i = 0; i <= 3; ++i) cmdBuffer[i + 19] = ioBuffer[i];

		//write command to serial
		this->writeFunction(cmdBuffer);

		//wait until available
		while(this->availableFunction() < sizeof(cmdBuffer) / sizeof(char)) {}

		//read received serial
		this->readFunction(cmdBuffer, sizeof(cmdBuffer) / sizeof(char));

		//check header, if not rec then do it again
		if(strncmp(cmdBuffer, _rec, 3) != 0) continue;
	}
	viewIndex = prevViewIndex;
}




#endif // T3SERIALSERVER_HPP
