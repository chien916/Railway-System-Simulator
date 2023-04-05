import QtQuick 2.12
import QtQuick.Controls 2.15
import QtQuick.Window 2.12
import QtMultimedia 5.15
Window {
	id:root
	width: 260
	height: 500


	//visibility: "FullScreen"
	visible: true
	title: qsTr("T3")
	//opacity: ((imag_prof.rotation)/360)/2+0.5
	property bool musicPlay_b:true
	Audio{
		id:music
		source: "qrc:/EXTMUS.mp3"
		muted: true
		loops: Audio.Infinite
		autoPlay: false
		volume: musicPlay_b?1:0
		playbackRate:1.5
		Behavior on volume {PropertyAnimation{easing.type: Easing.InOutCirc
				duration:1000}}
		onStatusChanged: {
			playbackRate = 0.5+(Math.random())
//			console.log(playbackRate)
//			anim.running = false;
//			anim.from = anim.to;
//			anim.to = Math.floor(Math.random()*360)
//			anim.duration= 10000
//			anim.running = true;
		}


	}

	function initAllWindow(ind:int ){
		if(ind===0) Qt.createComponent("T3WindowWrapper.qml").createObject(null,{loaderSource_s:"T3CTC.qml",height_i:700,width_i:1024}).show()
		if(ind===1) Qt.createComponent("T3WindowWrapper.qml").createObject(null,{loaderSource_s:"T3KC.qml",height_i:500,width_i:1200}).show()
		if(ind===2) Qt.createComponent("T3WindowWrapper.qml").createObject(null,{loaderSource_s:"T3KM.qml",height_i:600,width_i:1000}).show()
		if(ind===3) Qt.createComponent("T3WindowWrapper.qml").createObject(null,{loaderSource_s:"T3NM.qml",height_i:600,width_i:1200}).show()
		if(ind===4) Qt.createComponent("T3WindowWrapper.qml").createObject(null,{loaderSource_s:"T3NC.qml",height_i:350,width_i:1000}).show()
	}
	Rectangle{
		id:bg
		color: T3Styling.cBgMain_c
		anchors.fill: parent
	}
	T3Text{
		id: imag_prof
		anchors.top:parent.top
		anchors.margins:T3Styling.spacing_r
		anchors.horizontalCenter: bg.horizontalCenter
		height: 250
		width: 250
		rotation: 0
		textPixelSize_r:T3Styling.fontMain_r
		textColor_c:  T3Styling.cFgMain_c
		textContent_s: t3databaseQml.currentTime_QML
		MouseArea{
			anchors.fill: parent
			onClicked: t3databaseQml._TEST_ITERATE()
		}
	}

/*Image {
		id: imag_prof
		anchors.top:parent.top
		anchors.margins:T3Styling.spacing_r
		anchors.horizontalCenter: bg.horizontalCenter
		height: 250
		width: 250
		source: "qrc:/1516358265460-modified.png"
		rotation: 20
		PropertyAnimation{
			id:anim
			easing.type: Easing.InOutCirc
			target:imag_prof
			property: "rotation"
			from:0
			to:360
			running: true
			duration: 100
//				onFinished: {
//					duration = 10000/music.playbackRate
//					running = true;
//				}
			onFinished: {
				from = to;
				to = Math.floor(Math.random()*360)
				duration= Math.floor(Math.random()* (1000 - 200 + 1) + 200)
				running = true;
			}
		}
		MouseArea{
			anchors.fill: parent
			onClicked: musicPlay_b = !musicPlay_b
		}
	}*/

	Grid{
		id:grid_buttons
		columns:1
		rows:8
		anchors.top:imag_prof.bottom
		anchors.left:parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.margins:T3Styling.spacing_r
		rowSpacing: T3Styling.spacing_r
		property real unitWidth_r: width
		property real unitHeight_r: (height-(rows-1)*rowSpacing)/rows;
		T3Button{
			id:pdb_trackChange
			height:parent.unitHeight_r
			width:parent.unitWidth_r
			buttonLabel_s: "Track Database Debug".toUpperCase()
			//anchors.horizontalCenter: bg.horizontalCenter
			onButtonClicked: {
				t3databaseQml.db_pull(2)
			}
		}
		T3Button{
			id:pdb_trainChange
			height:parent.unitHeight_r
			width:parent.unitWidth_r
			buttonLabel_s: "Train Database Debug".toUpperCase()
			//anchors.horizontalCenter: bg.horizontalCenter
			onButtonClicked: {
				t3databaseQml.db_pull(4)
			}
		}
		T3Button{
			property bool currState: false
			id:btn_timer
			height:parent.unitHeight_r
			width:parent.unitWidth_r
			buttonLabel_s: "TURN TIMER " + (currState?"OFF":"ON")
			//anchors.leftMargin: T3Styling.margin_r
			//anchors.left:pdb.right
			onButtonClicked: {
				currState = !currState;
				t3databaseQml.toggleTimer(currState);
			}
		}

		T3Button{
			property bool currState: false
			height:parent.unitHeight_r
			width:parent.unitWidth_r
			buttonLabel_s: "CTC OFFICE"
			releasedColor_c: Qt.darker(T3Styling.cRed_c)
			//anchors.leftMargin: T3Styling.margin_r
			//anchors.left:pdb.right
			onButtonClicked: {
				initAllWindow(0)
			}
		}
		T3Button{
			property bool currState: false
			height:parent.unitHeight_r
			width:parent.unitWidth_r
			buttonLabel_s: "TRACK CONTROLLER"
			releasedColor_c: Qt.darker(T3Styling.cYellow_c)
			//anchors.leftMargin: T3Styling.margin_r
			//anchors.left:pdb.right
			onButtonClicked: {
				initAllWindow(1)
			}
		}
		T3Button{
			property bool currState: false
			height:parent.unitHeight_r
			width:parent.unitWidth_r
			buttonLabel_s: "TRACK MODEL"
			releasedColor_c: Qt.darker(T3Styling.cYellow_c)
			//anchors.leftMargin: T3Styling.margin_r
			//anchors.left:pdb.right
			onButtonClicked: {
				initAllWindow(2)
			}
		}
		T3Button{
			property bool currState: false
			height:parent.unitHeight_r
			width:parent.unitWidth_r
			buttonLabel_s: "TRAIN MODEL"
			releasedColor_c: Qt.darker(T3Styling.cBlue_c)
			//anchors.leftMargin: T3Styling.margin_r
			//anchors.left:pdb.right
			onButtonClicked: {
				initAllWindow(3)
			}
		}
		T3Button{
			property bool currState: false
			height:parent.unitHeight_r
			width:parent.unitWidth_r
			buttonLabel_s: "TRAIN CONTROLLER"
			releasedColor_c: Qt.darker(T3Styling.cBlue_c)
			//anchors.leftMargin: T3Styling.margin_r
			//anchors.left:pdb.right
			onButtonClicked: {
				initAllWindow(4)
			}
		}
	}
}
