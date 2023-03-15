import QtQuick 2.12
import QtQuick.Controls 2.15
import QtQuick.Window 2.12

ApplicationWindow {
	id:root
	width: 640
	height: 480
	visibility: "FullScreen"
	visible: true
	title: qsTr("Hello World")
	Rectangle{
		id:bg
		color: T3Styling.cBgMain_c
		anchors.fill: parent
	}
	T3Button{
		y:10
		id:pdb
		buttonLabel_s: "PULL FROM FIREBASE"
		anchors.horizontalCenter: bg.horizontalCenter
		onButtonClicked: {
			t3databaseQml.pullFromFirebase()
		}
	}

	T3Button{
		y:10
		buttonLabel_s: "PREVIOUS VIEW"
		anchors.right: pdb.left
		anchors.rightMargin: T3Styling.margin_r
		onButtonClicked: {
			if(dm.vind===0) dm.vind=4;
			else dm.vind = dm.vind-1;
		}
	}

	T3Button{
		y:10
		id:btn
		buttonLabel_s: "NEXT VIEW"
		anchors.leftMargin: T3Styling.margin_r
		anchors.left:pdb.right
		onButtonClicked: {
			if(dm.vind===4) dm.vind=0;
			else dm.vind = dm.vind+1;
		}
	}

	I2DemoTerain{
		id:dm
		y:20
		anchors{
			top:btn.bottom
			bottom: bg.bottom
			left:bg.left
			right:bg.right
		}

	}



}
