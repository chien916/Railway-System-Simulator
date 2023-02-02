import QtQuick 2.12
import QtGraphicalEffects 1.12
import QtQuick.Dialogs 1.3
Item {
	id:root
	implicitHeight: 700
	implicitWidth: 1000
	property int lineCount_i:0
	//	readonly property variant trackDatabase_O:t3database_QML
	readonly property variant lineConstantsObjects_OA: t3databaseQml.trackConstantsObjects_QML
	readonly property variant lineVariablesObjects_OA: t3databaseQml.trackVariablesObjects_QML
	property variant coordinates_nA : lineConstantsObjects_OA.map(it=>solveForCoors_f(it))
	property bool configMode_b: false
	property bool menuMode_b:false
	Timer{
		interval: 100
		repeat: true
		running: true
		onTriggered: {
			text_clockFront.text = Qt.formatDateTime(new Date(), "hh:mm:ss")
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
			y:T3Styling.margin_r
			width: root.width*0.3
			height: T3Styling.margin_r*2
			buttonLabel_s: "CTC MENU"
			onButtonClicked: {
				if(configMode_b) menuMode_b = false
				else menuMode_b = !menuMode_b
			}
		}

		Text{
			id:text_clockFront
			height: t3bu_dispatchNewTrain.height
			y:t3bu_dispatchNewTrain.y
			anchors.right: colu_column.right
			font.pixelSize: T3Styling.fontMain_r
//			fontSizeMode: Text.Fit
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			color: T3Styling.cFgMain_c
			text:Qt.formatDateTime(new Date(), "hh:mm:ss")
			font.family: "DSEG14 Classic"
		}

		Text{
			height: t3bu_dispatchNewTrain.height
			y:t3bu_dispatchNewTrain.y
			anchors.right: colu_column.right
			font.pixelSize: T3Styling.fontMain_r
//			fontSizeMode: Text.Fit
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			color: T3Styling.cFgMain_c
			text: "~~:~~:~~"
			font.family: "DSEG14 Classic"
			opacity: 0.15
		}

		//		T3Button{
		//			id:t3bu_timeAndDate
		//			anchors.right: colu_column.right
		//			y:T3Styling.margin_r
		//			width: root.width*0.2
		//			height: T3Styling.margin_r*2
		//			buttonLabel_s: Qt.formatDateTime(new Date()
		//											 , "hh:mm:ss")
		//		}

		//		T3Button{
		//			id:t3bu_loadSchedule
		//			x:t3bu_dispatchNewTrain.x*2+t3bu_dispatchNewTrain.width
		//			y:t3bu_dispatchNewTrain.y
		//			width: t3bu_dispatchNewTrain.width
		//			height: T3Styling.margin_r
		//			buttonLabel_s: "Load Schedule from File"
		//		}

		//		T3ParamConfigUnit{
		//			valueLabel_s: "Automatic Mode"
		//			width: t3bu_dispatchNewTrain.width
		//			x:t3bu_loadSchedule.x+t3bu_loadSchedule.width+t3bu_dispatchNewTrain.x
		//			y:t3bu_dispatchNewTrain.y
		//		}

		Column{
			id:colu_column
			anchors.fill: parent
			spacing: T3Styling.margin_r
			anchors{
				topMargin: T3Styling.margin_r*4
				bottomMargin: T3Styling.margin_r
				leftMargin: T3Styling.margin_r
				rightMargin: T3Styling.margin_r
			}
			//			anchors.margins: T3Styling.margin_r
			Repeater{
				model:2
				delegate:T3CTCLineGrid{
					lineConstantsObject_O: lineConstantsObjects_OA[index]
					lineVariablesObject_O: lineVariablesObjects_OA[index]
					coordinates_A: coordinates_nA[index]
					height:(root.height-T3Styling.margin_r*6)/2
					width: colu_column.width
					onBlockClicked: {
						//block identifier
						if(!menuMode_b)
							configMode_b = true;
					}
				}
			}
		}

		GaussianBlur {
			anchors{
				topMargin: T3Styling.margin_r*4
				bottomMargin: T3Styling.margin_r
				leftMargin: T3Styling.margin_r
				rightMargin: T3Styling.margin_r
				fill: parent
			}

			source: colu_column
			radius: 50
			samples: 100
			opacity: configMode_b||menuMode_b?1:0
			Behavior on opacity {PropertyAnimation{easing.type: Easing.OutCirc}}
		}


		Rectangle{
			id:rect_railBlockSelected
			x:configMode_b?root.width*1/3-width/2:-width
			Behavior on x{ SpringAnimation { spring: 2; damping: 0.2 }}
			y:colu_column.y+colu_column.height/2-height/2
			height: colu_column.height*5/6
			width: root.width*0.3
			color:T3Styling.cBgSub_c
			radius: T3Styling.margin_r
			//			opacity: configMode_b?1:0
			//			Behavior on opacity {PropertyAnimation{easing.type: Easing.OutCirc}}
			T3CTCRailBlock{
				id:rBlo_railBlockSelected
				anchors.margins: T3Styling.margin_r
				anchors.fill: parent
			}
		}



		T3CTCConfigBlock{
			x:configMode_b?root.width*2/3-width/2:root.width+width
			Behavior on x{ SpringAnimation { spring: 2; damping: 0.2 }}
			y:rect_railBlockSelected.y
			height: rect_railBlockSelected.height
			width: root.width*0.3
			//			opacity: configMode_b?1:0
			//			Behavior on opacity {PropertyAnimation{easing.type: Easing.OutCirc}}
			onApplyClicked: configMode_b = false
		}

		T3CTCMainMenu{
			x:menuMode_b?root.width*1/2-width/2:-width
			Behavior on x{ SpringAnimation { spring: 2; damping: 0.2 }}
			y:rect_railBlockSelected.y
			height: rect_railBlockSelected.height
			width: root.width*0.6
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



	function solveForCoors_f(rootObj_O) {
		let blocksMap_O = rootObj_O["blocksMap"];
		let currTopBlockId_s = rootObj_O["startingBlock1"];
		let currBottomBlockId_s = rootObj_O["startingBlock2"];
		let currTopBlockLine_A = [];
		let currBottomBlockLine_A = [];
		let currBlockGridList_A = [];
		while (true) {
			let currIsDoubleLine_b = currBottomBlockId_s !== "";
			let currTopBlockObj_O = blocksMap_O[currTopBlockId_s];
			let currBottomBlockObj_O
				= currIsDoubleLine_b ? blocksMap_O[currBottomBlockId_s] : null;
			let spreadLength_f = (blockLine1_A, blockLine2_A) => {
				let theSmallerLength_n
				= Math.min(blockLine1_A.length, blockLine2_A.length);
				let theBiggerLength_n
				= Math.max(blockLine1_A.length, blockLine2_A.length);
				let theShorterBlockLine_O
				= blockLine1_A.length === theSmallerLength_n
				? blockLine1_A : blockLine2_A;
				let newSmallerWidth_n = theBiggerLength_n / theSmallerLength_n;
				for (let currTopBlock_A of theShorterBlockLine_O) {
					currTopBlock_A[1] = newSmallerWidth_n;
				}
			};
			if (!currIsDoubleLine_b
					&& currTopBlockId_s === rootObj_O["endingBlock1"]
					&& currBottomBlockId_s === rootObj_O["endingBlock2"]) {
				currTopBlockLine_A.push([currTopBlockId_s, 1]);
				currBlockGridList_A.push([[...currTopBlockLine_A], []]);
				break;
			} else if (currIsDoubleLine_b
					   && currTopBlockId_s === rootObj_O["endingBlock1"]
					   && currBottomBlockId_s === rootObj_O["endingBlock2"]) {
				currTopBlockLine_A.push([currTopBlockId_s, 1]);
				currBottomBlockLine_A.push([currBottomBlockId_s, 1]);
				spreadLength_f(currTopBlockLine_A, currBottomBlockLine_A);
				currBlockGridList_A.push([[...currTopBlockLine_A], [...currBottomBlockLine_A]]);
				break;
			} else if (currIsDoubleLine_b
					   && currTopBlockId_s !== currBottomBlockId_s
					   && (((currTopBlockId_s === rootObj_O["endingBlock1"]
							 || (currTopBlockObj_O["prevBlock2"] !== ""
								 && currTopBlockObj_O["prevBlock2"] !== "PASSIVE"))
							&& currBottomBlockId_s !== rootObj_O["endingBlock2"]))) {
				currBottomBlockLine_A.push([currBottomBlockId_s, 1]);
				currBottomBlockId_s = currBottomBlockObj_O["nextBlock1"];
			} else if (currIsDoubleLine_b
					   && currTopBlockId_s !== currBottomBlockId_s
					   && (((currBottomBlockId_s === rootObj_O["endingBlock2"]
							 || (currBottomBlockObj_O["prevBlock2"] !== ""
								 && currBottomBlockObj_O["prevBlock2"] !== "PASSIVE"))
							&& currTopBlockId_s !== rootObj_O["endingBlock1"]))) {
				currTopBlockLine_A.push([currTopBlockId_s, 1]);
				currTopBlockId_s = currTopBlockObj_O["nextBlock1"];
			} else if (currIsDoubleLine_b
					   && (currTopBlockObj_O["prevBlock2"] !== "" && currTopBlockObj_O["prevBlock2"] !== "PASSIVE")
					   && (currBottomBlockObj_O["prevBlock2"] !== "" && currBottomBlockObj_O["prevBlock2"] !== "PASSIVE")) {
				spreadLength_f(currTopBlockLine_A, currBottomBlockLine_A);
				currBlockGridList_A.push([[...currTopBlockLine_A], [...currBottomBlockLine_A]]);
				currTopBlockLine_A = [];
				currBottomBlockLine_A = [];
				currIsDoubleLine_b = false;
				currBlockGridList_A.push([[[currTopBlockId_s, 2]],[]]);//switch has min width 2
				// currTopBlockLine_A.push([currTopBlockId_s, 1]);
				currTopBlockId_s = currTopBlockObj_O["nextBlock1"];
				currBottomBlockId_s = currTopBlockObj_O["nextBlock2"];
			} else if (!currIsDoubleLine_b
					   && !currIsDoubleLine_b
					   && currTopBlockObj_O["nextBlock2"] !== "") {
				// currTopBlockLine_A.push([currTopBlockId_s, 1]);
				currBlockGridList_A.push([[...currTopBlockLine_A], []]);
				currBlockGridList_A.push([[[currTopBlockId_s, 2]],[]]);//switch has min width 2
				currTopBlockLine_A = [];
				currIsDoubleLine_b = true;
				currTopBlockId_s = currTopBlockObj_O["nextBlock1"];
				currBottomBlockId_s = currTopBlockObj_O["nextBlock2"];
			} else if (currIsDoubleLine_b) {
				currTopBlockLine_A.push([currTopBlockId_s, 1]);
				currBottomBlockLine_A.push([currBottomBlockId_s, 1]);
				currTopBlockId_s = currTopBlockObj_O["nextBlock1"];
				currBottomBlockId_s = currBottomBlockObj_O["nextBlock1"];
			} else if (!currIsDoubleLine_b) {
				currTopBlockLine_A.push([currTopBlockId_s, 1]);
				currTopBlockId_s = currTopBlockObj_O["nextBlock1"];
			}
		}
		let currBlockFlattenedTopList_A = [];
		let currBlockFlattenedBottomList_A = [];
		let currTopX_n = 0;
		let currBottomX_n = 0;
		for (let currBlockGrid_A of currBlockGridList_A) {
			for (let currTopBlock_A of currBlockGrid_A[0]) {
				currBlockFlattenedTopList_A.push([currTopBlock_A[0], [0, currTopX_n],currTopBlock_A[1]]);
				currTopX_n += currTopBlock_A[1];
			}
			for (let currBottomBlock_A of currBlockGrid_A[1]) {
				currBlockFlattenedBottomList_A.push([currBottomBlock_A[0], [1, currBottomX_n],currBottomBlock_A[1]]);
				currBottomX_n += currBottomBlock_A[1];
			}
			currBottomX_n = currTopX_n;
		}
		let concatedBlockFlattenedList_A
			= currBlockFlattenedTopList_A.concat(currBlockFlattenedBottomList_A);

		return concatedBlockFlattenedList_A;
	}
}
