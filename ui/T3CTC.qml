import QtQuick 2.12
import QtGraphicalEffects 1.12
import QtQuick.Dialogs 1.3
import QtQuick.Window 2.12

Item {
	id:root
	//flags:Qt.Dialog
	//color: "transparent"
	width: 1024
	height: 650
	//maximumHeight: height
	//minimumHeight: height
	//maximumWidth: width
	//minimumWidth: width
	property int lineCount_i:0
	//	readonly property variant trackDatabase_O:t3database_QML
	readonly property variant trackConstantsObjects_OA: t3databaseQml.trackConstantsObjects_QML
	readonly property variant trackVariablesObjects_OA: t3databaseQml.trackVariablesObjects_QML
	//property variant coordinates_nA : trackConstantsObjects_OA.map(it=>solveForCoors_f(it))
	property bool configMode_b: false
	property bool menuMode_b:false
	property bool dispatchMode_b: false
	property bool dispatchQueueMode_b: false
	Timer{
		interval: 1
		repeat: true
		running: true
		onTriggered: {
			text_clockFront.text = t3databaseQml.getCurrentTime()
		}
	}

	Rectangle{
		id:rect_canvas
		anchors{
			fill:parent
		}
		radius: T3Styling.margin_r
		color: T3Styling.cBgSub_c


		T3Button{
			id:t3bu_dispatchNewTrain
			anchors.left: colu_column.left
			y:T3Styling.spacing_r*2
			width: root.width*0.3
			height: T3Styling.margin_r
			buttonLabel_s: (configMode_b||dispatchQueueMode_b)?"GO BACK":"CTC MENU"
			opacity: !dispatchMode_b
			Behavior on opacity {PropertyAnimation{easing.type: Easing.OutCirc}}
			onButtonClicked: {
				if(menuMode_b){
					t3databaseQml.setTimerRate(cust_mainMenu.clockRate_i);
					configMode_b = false;
					dispatchQueueMode_b = false;
					menuMode_b = false;
				}else if(configMode_b||dispatchQueueMode_b){
					configMode_b = false;
					dispatchQueueMode_b = false;
					menuMode_b = false;
				}else if(dispatchMode_b){
					//do nothing
				}else{
					configMode_b = false;
					dispatchQueueMode_b = false;
					menuMode_b = true;
				}
			}
		}

		T3CTCDispatchBlock{
			id:cust_dispatchBlock
			anchors.left: colu_column.left
			y:T3Styling.spacing_r*2
			opacity: dispatchMode_b?1:0
			Behavior on opacity {PropertyAnimation{easing.type: Easing.OutCirc}}
			onDispatchRequested: {
				if(!dispatchMode_b) return;
				t3databaseQml.ctc_enqueueDispatchRequest(metaInfo_sA,path_A);
				dispatchMode_b = !dispatchMode_b;
			}
			onDispatchCancelled: {
				if(!dispatchMode_b) return;
				dispatchMode_b = !dispatchMode_b;
			}
		}

		Text{
			id:text_clockFront
			height: t3bu_dispatchNewTrain.height
			y:t3bu_dispatchNewTrain.y
			anchors.right: colu_column.right
			font.pixelSize: T3Styling.fontSub_r
//			fontSizeMode: Text.Fit
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			color: T3Styling.cFgMain_c
			text:t3databaseQml?t3databaseQml.getCurrentTime():"N/A"
			font.family: "Inter"
		}



		Column{
			id:colu_column
			anchors.fill: parent
			spacing: T3Styling.spacing_r*2
			anchors{
				topMargin: T3Styling.spacing_r*4+T3Styling.margin_r
				bottomMargin: T3Styling.margin_r
				leftMargin: T3Styling.margin_r
				rightMargin: T3Styling.margin_r
			}
			//			anchors.margins: T3Styling.margin_r
			Repeater{
				model:2
				delegate:T3CTCLineGrid{
					//trackConstantsObject_O: trackConstantsObjects_OA[index]["blocksMap"]
					//trackVariablesObject_O: trackVariablesObjects_OA[index]
					dbIndex_i: index
					//coordinates_A: coordinates_nA[index]
					pathsAvailable_sA: cust_dispatchBlock.currSelectedPath_A
					height:(root.height-T3Styling.margin_r-T3Styling.spacing_r*8)/2
					width: colu_column.width
					onBlockClicked: {
						//block identifier
						if(!menuMode_b&&!configMode_b){
							cBloc_configBlock.blockId_s = blockId_s
							rBlo_railBlockSelected.bcnPlcOut_s= trackVariablesObjects_OA[index][blockId_s]["COM[KC|KM]_BCNPLCOUT"]
							rBlo_railBlockSelected.kmPlcIo_s=trackVariablesObjects_OA[index][blockId_s]["COM[KC|KM]_KMPLCIO"]
							rBlo_railBlockSelected.blockId_s = blockId_s;
							configMode_b = true;
						}

					}
				}
			}
		}

		GaussianBlur {
			anchors{
				topMargin:T3Styling.spacing_r*4+T3Styling.margin_r
				bottomMargin: T3Styling.margin_r
				leftMargin: T3Styling.margin_r
				rightMargin: T3Styling.margin_r
				fill: parent
			}

			source: colu_column
			radius: 50
			samples: 100
			opacity: (configMode_b||menuMode_b||dispatchQueueMode_b)?1:0
			Behavior on opacity {PropertyAnimation{easing.type: Easing.OutCirc}}
		}


		Rectangle{
			id:rect_railBlockSelected
			x:configMode_b?root.width*1/3-width/2:-width*5
			Behavior on x{ PropertyAnimation {easing.type: Easing.InOutCirc }}
			y:colu_column.y+colu_column.height/2-height/2
			height: 450
			width: root.width*0.3
			color:T3Styling.cBgSub_c
			radius: T3Styling.margin_r
			//			opacity: configMode_b?1:0
			//			Behavior on opacity {PropertyAnimation{easing.type: Easing.OutCirc}}
			T3CTCRailBlock{
				id:rBlo_railBlockSelected
				noAnimation_b: true
				anchors.margins: T3Styling.margin_r
				anchors.fill: parent
			}
		}



		T3CTCConfigBlock{
			id:cBloc_configBlock
			x:configMode_b?root.width*2/3-width/2:root.width+width*5
			Behavior on x{ PropertyAnimation {easing.type: Easing.InOutCirc }}
			y:rect_railBlockSelected.y
			height: rect_railBlockSelected.height
			width: root.width*0.3
			//			opacity: configMode_b?1:0
			//			Behavior on opacity {PropertyAnimation{easing.type: Easing.OutCirc}}
			onApplyClicked: configMode_b = false
		}



		T3CTCQueueBlock{
			id:cust_dispatchQueueBlock
			width: root.width*0.6
			height: rect_railBlockSelected.height
			x:dispatchQueueMode_b?root.width*1/2-width/2:-width*5
			Behavior on x{ PropertyAnimation {easing.type: Easing.InOutCirc }}
			y:rect_railBlockSelected.y
		}
		T3CTCMainMenu{
			id:cust_mainMenu
			x:menuMode_b?root.width*1/2-width/2:-width*5
			Behavior on x{ PropertyAnimation {easing.type: Easing.InOutCirc }}
			y:rect_railBlockSelected.y
			height: rect_railBlockSelected.height
			width: root.width*0.6
			onMenuButtonClicked: {
				if(metaInfo_s==="manuallyDispatch"){
					dispatchMode_b = true;
					menuMode_b = false;
				}
				else if(metaInfo_s==="viewDispatchQueue"){
					dispatchQueueMode_b = true;
					menuMode_b = false;
				}
			}
		}


		//		Rectangle{
		//			id:rect_selectLineFileButton
		//			width: 40
		//			height: 40
		//			MouseArea{
		//				id:mAre_selectLineFileButton
		//				anchors.fill: parent
		//				onClicked: {
		////					trackDatabase_O.put("R_A_1",6, "1234r")
		//				}
		//					/*fDia_selectLineFileDialog.open()*/
		//			}
		//			FileDialog{
		//				id:fDia_selectLineFileDialog
		//				title: "Please select line CSV file"
		//				selectExisting: true
		//				selectMultiple: false
		//				selectFolder:false
		//				nameFilters: ["Line CSV File (*.csv)"]
		//				onAccepted: {
		//					console.log(fDia_selectLineFileDialog.fileUrl);
		////					root.trackDatabase_O.import(fDia_selectLineFileDialog.fileUrl);
		//				}
		//			}
		//		}


	}


}
