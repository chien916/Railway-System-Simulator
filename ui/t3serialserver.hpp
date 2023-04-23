#ifndef T3SERIALSERVER_HPP
#define T3SERIALSERVER_HPP
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define MAXCURSORIND 5
#define MAXVIEWIND 2
#define MAXPROPIND 9
#define MAXCHARCODE '9'
#define MINCHARCODE '0'

#define _RWPROP_READ true
#define _RWPROP_WRITE false

#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#endif


/**
 * @brief The T3SerialServer class
 */
class T3SerialServer {

  public:
#ifdef DEBUG
	T3SerialServer();
#else
	T3SerialServer(void(*readFunction)(char*, int), void(*writeFunction)(char*, int), int(*availableFunction)(void));
#endif
	void flush(char* lcd);
	void back() {
		event(1);
	}
	void next() {
		event(2);
	}
	void up() {
		event(3);
	}
	void down() {
		event(4);
	}
	void okay() {
		event(5);
	}
  private:
	void event(short buttonIndex);
	void update();
	void(*readFunction)(char*, int);
	void(*writeFunction)( char*, int );
	int(*availableFunction)(void);

	char lcdDisp[7] = "------";

	int viewIndex = 0, cursorIndex = 0, propIndex = 0;
	//0 - scroll view
	//1 - select modify varialble
	//2 - modifying variable
	char ioBuffer[7] = "000000";
	void rwProp(bool read);
	const char vars[MAXPROPIND + 1][7] {
		"TRA ID",
		"EXT LT",
		"INT LT",
		"LEF DR",
		"RIG DR",
		"EME BR",
		"SER BR",
		"SET PT",
		"CUR VL",
		"CUR PW",
	};
	const uint8_t varsConstraints[MAXPROPIND + 1] {
		//0b[READONLY][TWOSTATE][MAXVALUE]
		0b10001111u,
		0b01000001u,
		0b01000001u,
		0b01000001u,
		0b01000001u,
		0b01000001u,
		0b01000001u,
		0b00000011u,
		0b10111111u,
		0b10111111u,
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

	char cmdBuffer[37] = "?{___}{___________________}{______}!";// 2 27 28
};

#ifdef DEBUG
inline T3SerialServer::T3SerialServer() {};
#else
inline T3SerialServer::T3SerialServer(void (*readFunction)(char *, int), void (*writeFunction)(char *, int), int (*availableFunction)()) {
	this->readFunction = readFunction;
	this->writeFunction = writeFunction;
	this->availableFunction = availableFunction;
}
#endif



inline void T3SerialServer::flush(char* lcd) {
	update();
	strncpy(lcd, lcdDisp, MAXCURSORIND + 1);
}

inline void T3SerialServer::event(short buttonIndex) {
#ifdef DEBUG
	//printf("Button Clicked: &hu\n", buttonIndex);
#endif
	switch(viewIndex) {
	case 0://menu scrool mode
		if(buttonIndex == 1) {//left button
			propIndex = (propIndex == 0) ? MAXPROPIND : (propIndex - 1);
		} else if(buttonIndex == 2) {//right button
			propIndex = (propIndex == MAXPROPIND ) ? 0 : (propIndex + 1);
		} else if(buttonIndex == 3) {//top button

		} else if(buttonIndex == 4) {//bottom button

		} else if(buttonIndex == 5) {//ok
			rwProp(true);
			viewIndex = ((varsConstraints[propIndex] & 0b01000000u) == 0)
						? ((cursorIndex = 5) - 3)  //go to modifyin value mode (initially 2 but should be alternating btwn 2 and 3)
						: ((cursorIndex = 5) - 1); // go to binary value modifying mode always 4 and not alternating
		}
		break;
	case 2://modifying value mode
	case 3:
	case 4://binary modifying value mode
		if(buttonIndex == 1) {
			viewIndex = 0;//go back to main scooll view
			//cursorIndex = (cursorIndex == 0 ) ? (MAXCURSORIND ) : (cursorIndex + 1);
		} else if(buttonIndex == 2) {
			cursorIndex = ((varsConstraints[propIndex] & 0b01000000u) == 0)
						  ? ((cursorIndex == MAXCURSORIND ) ? (0) : (cursorIndex + 1))//non-binary stated value
						  : 5;//binary stated value
			//cursorIndex = (cursorIndex == 0 ) ? (MAXCURSORIND ) : (cursorIndex + 1);
		} else if(buttonIndex == 3 || buttonIndex == 4) {
			if((varsConstraints[propIndex] & 0b10000000u) != 0) return;//readonly -> do not change!
			if((varsConstraints[propIndex] & (0b00100000u >> cursorIndex)) == 0) return; //not in changeable digit range -> do not change!
			if(buttonIndex == 3) {
				ioBuffer[cursorIndex] = ((varsConstraints[propIndex] & 0b01000000u) == 0) ?
										(ioBuffer[cursorIndex] == MAXCHARCODE) ? MINCHARCODE : (ioBuffer[cursorIndex] + 1) :
										((ioBuffer[cursorIndex] == '0' ? '1' : '0'));

			} else {
				ioBuffer[cursorIndex] = ((varsConstraints[propIndex] & 0b01000000u) == 0) ?
										(ioBuffer[cursorIndex] == MINCHARCODE) ? MAXCHARCODE : (ioBuffer[cursorIndex] - 1) :
										((ioBuffer[cursorIndex] == '0' ? '1' : '0'));
			}
		} else if(buttonIndex == 5) {
			rwProp(_RWPROP_WRITE);
			viewIndex = 0;//go back to main scooll view
		}
		break;
	}
}

inline void T3SerialServer::update() {
	switch(viewIndex) {
	case 0://menu scrool mode
		strncpy(lcdDisp, vars[propIndex], MAXCURSORIND + 1);
		break;
	case 1://loading
		strncpy(lcdDisp, "--->", MAXCURSORIND + 1);
		break;
	case 2://modifying value mode
		strncpy(lcdDisp, ioBuffer, MAXCURSORIND + 1);
		lcdDisp[cursorIndex] = '_';
		viewIndex = 3;
		break;
	case 3://modifying value mode 2
		strncpy(lcdDisp, ioBuffer, MAXCURSORIND + 1);
		viewIndex = 2;
		break;
	case 4:
		strncpy(lcdDisp, ioBuffer[MAXCURSORIND] == '1' ? "  ON  " : "  OFF ", MAXCURSORIND + 1);
		break;
	}
}

inline void T3SerialServer::rwProp(bool read) {

	int prevViewIndex = this->viewIndex;
	viewIndex = 1;
	const char _set[4] = "SET";
	const char _get[4] = "GET";
	const char _rec[4] = "REC";
	while(true) {
		//clears serial read buffer
		char helper;
#ifndef DEBUG
		while(this->availableFunction() > 1) this->readFunction(&helper, 1);
#endif

		//form command to cmdBuffer
		for(int i = 0; i <= 2; ++i) cmdBuffer[i + 2] = read ? _get[i] : _set[i];
		for(int i = 0; i <= 18; ++i) cmdBuffer[i + 7] = mapper[propIndex][i];
		for(int i = 0; i <= 5; ++i) cmdBuffer[i + 28] = ioBuffer[i];

		//write command to serial
#ifdef DEBUG
		printf("COMMAND BUFFER -> %s\n", cmdBuffer);
		break;
#else
		this->writeFunction(cmdBuffer, 35);

		//wait until available
		while(this->availableFunction() < sizeof(cmdBuffer) / sizeof(char)) {}

		//read received serial
		this->readFunction(cmdBuffer, sizeof(cmdBuffer) / sizeof(char));

		//check header, if not rec then do it again
		if(strncmp(cmdBuffer, _rec, 3) != 0) continue;

#endif
	}
	viewIndex = prevViewIndex;
}




#endif // T3SERIALSERVER_HPP
