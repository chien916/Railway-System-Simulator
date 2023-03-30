import QtQuick 2.12
import QtQuick.Controls 2.15
import QtQuick.Window 2.12

ApplicationWindow {
	id:root
	width: 1920
	height: 1080
	//visibility: "FullScreen"
	visible: true
	title: qsTr("Hello World")
	Rectangle{
		id:bg
		color: T3Styling.cBgMain_c
		anchors.fill: parent
	}
	Grid{
		id:grid_buttons
		columns:6
		rows:1
		y:10
		height: 20
		width: parent.width
		columnSpacing: T3Styling.spacing_r
		property real unitWidth_r: (width-(columns-1)*columnSpacing)/columns;
		T3Button{
			id:pdb
			height:parent.height
			width:parent.unitWidth_r
			buttonLabel_s: "PULL FROM JSON"
			//anchors.horizontalCenter: bg.horizontalCenter
			onButtonClicked: {
				t3databaseQml.db_pull()
			}
		}
		T3Button{
			id:pdb2
			height:parent.height
			width:parent.unitWidth_r
			buttonLabel_s: "PUSH TO JSON"
			//anchors.horizontalCenter: bg.horizontalCenter
			onButtonClicked: {
				t3databaseQml.db_toggle(false,true)
				t3databaseQml.db_push()
				t3databaseQml.db_toggle(false,false)
				btn_fileIo.currState = false
			}
		}

		T3Button{
			height:parent.height
			width:parent.unitWidth_r
			buttonLabel_s: "PREVIOUS VIEW"
			//anchors.right: pdb.left
			//anchors.rightMargin: T3Styling.margin_r
			onButtonClicked: {
				if(dm.vind===0) dm.vind=4;
				else dm.vind = dm.vind-1;
			}
		}

		T3Button{
			id:btn
			height:parent.height
			width:parent.unitWidth_r
			buttonLabel_s: "NEXT VIEW"
			//anchors.leftMargin: T3Styling.margin_r
			//anchors.left:pdb.right
			onButtonClicked: {
				if(dm.vind===4) dm.vind=0;
				else dm.vind = dm.vind+1;
			}
		}

		T3Button{
			property bool currState: false
			id:btn_timer
			height:parent.height
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
			id:btn_fileIo
			height:parent.height
			width:parent.unitWidth_r
			buttonLabel_s: "TURN FILE IO " + (currState?"OFF":"ON")
			//anchors.leftMargin: T3Styling.margin_r
			//anchors.left:pdb.right
			onButtonClicked: {
				currState = !currState;
				t3databaseQml.db_toggle(false,currState)
			}
		}
	}


	I2DemoTerain{
		id:dm
		y:20
		anchors{
			top:grid_buttons.bottom
			bottom: bg.bottom
			left:bg.left
			right:bg.right
		}

	}



}
