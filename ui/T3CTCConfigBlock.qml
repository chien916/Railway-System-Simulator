import QtQuick 2.12

Item {
	signal applyClicked();
	property string blockId_s:"B_A_2"
	onBlockId_sChanged: {db2view()}
	property int dbIndex_n:-1
	readonly property variant configModel_nA:[
		//[textonly,twostate,labal,unit]
		"F_T_Maintance Mode_"
		,"F_T_Authority_"
		,"F_F_Suggested Speed_mph"
		,"F_T_Switch Position_"
	]
	Rectangle{
		id:rect_canvas
		radius: T3Styling.margin_r
		color: T3Styling.cBgSub_c
		anchors.fill: parent
//		Column{
//			anchors{
//				margins: T3Styling.margin_r
//				fill: parent
//			}
//			id:colu_allConfigs
//			Text{
//				text:"eet"
//			}
//		}
		Column{
			id:col_column
			anchors.fill: parent
			anchors{
				topMargin: T3Styling.margin_r
				leftMargin: T3Styling.margin_r
				rightMargin: T3Styling.margin_r
				bottomMargin: T3Styling.margin_r*3
			}
//			clip: true
			spacing: T3Styling.spacing_r
			Repeater{
				id:reap_repeater
				model:configModel_nA
				delegate:		T3ParamUnit{
					height:(rect_canvas.height-T3Styling.margin_r*4
							-col_column.spacing*(configModel_nA.length-1))/configModel_nA.length
					maxValue_r: index===2?100:1
					minValue_r: 0
					fixedPoint_i: 2
					readOnly_b: index===0?false:reap_repeater.itemAt(0).actualValue_r<0.5
					paramConfig_A:modelData
					width: col_column.width
				}
			}
		}
		T3Button{
			anchors{
				bottom:rect_canvas.bottom
				left:rect_canvas.left
//				right:rect_canvas.right
				margins: T3Styling.margin_r
			}
			height: T3Styling.margin_r
			width: (rect_canvas.width-T3Styling.margin_r*3)/2
			buttonLabel_s: "APPLY"
			onButtonClicked: {
				view2db();
				applyClicked();
			}
		}

		T3Button{
			anchors{
				bottom:rect_canvas.bottom
//				left:rect_canvas.left
				right:rect_canvas.right
				margins: T3Styling.margin_r
			}
			height: T3Styling.margin_r
			width: (rect_canvas.width-T3Styling.margin_r*3)/2
			buttonLabel_s: "CANCEL"
			onButtonClicked: applyClicked()
		}
	}

//	Timer{
//		interval:1000
//		repeat:true
//		running:true
//		onTriggered:{
//		console.log(dataModel_nA)
//		}
//	}

	function db2view(){
		if(!t3databaseQml.trackVariablesObjects_QML||dbIndex_n===-1) return;
		//console.log(t3databaseQml.km_getTrackProperty(blockId_s,0))
		reap_repeater.itemAt(0).valueratio_r = t3databaseQml.km_getTrackProperty(blockId_s,10)?1.0:0.0
		reap_repeater.itemAt(1).valueratio_r = t3databaseQml.km_getTrackProperty(blockId_s,1)?1.0:0.0
		reap_repeater.itemAt(2).valueratio_r
				= t3databaseQml.km_getTrackProperty(blockId_s,0)/100
		reap_repeater.itemAt(3).valueratio_r = t3databaseQml.km_getTrackProperty(blockId_s,2)?1.0:0.0
	}

	function view2db(){
		if(!t3databaseQml.trackVariablesObjects_QML||dbIndex_n===-1) return;
		t3databaseQml.km_setTrackProperty(blockId_s,10,reap_repeater.itemAt(0).actualValue_r>0.5);
		t3databaseQml.km_setTrackProperty(blockId_s,1,reap_repeater.itemAt(1).actualValue_r>0.5);
		t3databaseQml.km_setTrackProperty(blockId_s,0,reap_repeater.itemAt(2).actualValue_r);
		t3databaseQml.km_setTrackProperty(blockId_s,2,reap_repeater.itemAt(3).actualValue_r>0.5);
	}

	//for testing only
	Component.onCompleted: {
		db2view();
	}

	implicitWidth: 250
	implicitHeight: 500
}
