import QtQuick 2.12
import QtQuick.Dialogs 1.3
Item {
	id:root
	width: 1000
	height:T3Styling.margin_r
	readonly property bool metadataReady:csvMetadataReady
										 && tInp_dispatchFrom.customAcceptableInput
										 && tInp_dispatchTo.acceptableInput
	readonly property bool csvMetadataReady:tInp_dispatchAt.customAcceptableInput
											&& tInp_dispatchTrain.acceptableInput
	readonly property variant trackConstantsObjects_OA: t3databaseQml.trackConstantsObjects_QML

	property bool pathSelectionMode_b: false
	property variant possiblePaths_A :[];
	readonly property variant pathIds_sA:findUniqueIdsFromPaths(possiblePaths_A)
	property int currSelectedPathInd_n:-1;
	readonly property variant currSelectedPath_A :
		currSelectedPathInd_n===-1?[]:possiblePaths_A[currSelectedPathInd_n];

	signal dispatchRequested(metaInfo_sA:variant,path_A:variant)
	signal dispatchCancelled()
	Rectangle{
		anchors.fill: parent
		color: T3Styling.cBgSubSub_c
	}


	Rectangle{
		id:rect_seperator
		anchors{
			left:rect_dispatchInfoInputBlock.right
			top:parent.top
			bottom:parent.bottom
			topMargin: T3Styling.lineWidth_r
			bottomMargin: T3Styling.lineWidth_r
			leftMargin: T3Styling.spacing_r
		}
		width: T3Styling.lineWidth_r*0.5
		radius: width
		color: T3Styling.cFgSubSub_c
	}

	T3Button{
		id:cust_cancelButton
		anchors{
			top:parent.top
			//left:rect_dispatchAt.right
			left:rect_seperator.right
			leftMargin: T3Styling.spacing_r
			bottom: parent.bottom
		}
		//releasedColor_c: !root.metadataReady?T3Styling.cRed_c:Qt.darker(pressedColor_c)
		//pressedColor_c: !root.metadataReady?T3Styling.cRed_c:T3Styling.cGreen_c
		width: T3Styling.fontSubSub_r*5
		buttonLabel_s: "Cancel"
		onButtonClicked: {
			if(root.pathSelectionMode_b){
				root.pathSelectionMode_b = false;
				root.possiblePaths_A = [];
				root.currSelectedPathInd_n = -1;
			}
			else dispatchCancelled();
		}
	}

	T3Button{
		id:cust_okButton
		anchors{
			top:parent.top
			//left:rect_dispatchAt.right
			left:cust_cancelButton.right
			leftMargin: T3Styling.spacing_r
			bottom: parent.bottom
		}
		releasedColor_c: !(root.metadataReady||pathSelectionMode_b)?T3Styling.cRed_c:Qt.darker(pressedColor_c)
		pressedColor_c: !(root.metadataReady||pathSelectionMode_b)?T3Styling.cRed_c:T3Styling.cGreen_c
		width: T3Styling.fontSubSub_r*5
		buttonLabel_s: root.pathSelectionMode_b?"Dispatch":"Search"
		opacity: (root.metadataReady||pathSelectionMode_b)?1:0.5
		onButtonClicked: {

			if(!root.metadataReady&&!pathSelectionMode_b) return;
			let dispatchTrainId_s = tInp_dispatchTrain.text;

			let dispatchFrom_s = tInp_dispatchFrom.text;
			let dispatchFrom_sA = dispatchFrom_s.split(" ");

			let disptachTo_s = tInp_dispatchTo.text
			let dispatchTo_sA = disptachTo_s.split(" ");

			if(dispatchFrom_s==="  "){
				//the dispatch origin is yard:
				if(dispatchTo_sA[0]==="G")
					dispatchFrom_s = "G_J_62";
				else if(dispatchTo_sA[0]==="R")
					dispatchFrom_s = "R_C_9";
				else if(dispatchTo_sA[0]==="B")
					dispatchFrom_s = "B_A_1";
				else{
					rect_errMessage.showMessage("Dispatch Fail: Unable to recognize line from given destination!");
					return;
				}
				dispatchFrom_sA = dispatchFrom_s.split("_");
			}else
				dispatchFrom_s = dispatchFrom_sA.join("_");

			disptachTo_s = dispatchTo_sA.join("_");

			//check if lines are the same
			if(dispatchFrom_sA[0]!==dispatchTo_sA[0]){
				//console.log(dispatchFrom_sA[0],dispatchTo_sA[0])
				rect_errMessage.showMessage("Dispatch Fail: Rail blocks belong to different line.");
				return;
			}

			let dbIndex_n = -1;
			if(dispatchFrom_sA[0]==="G")
				dbIndex_n =1;
			else if(dispatchFrom_sA[0]==="R")
				dbIndex_n = 0;
			else if(dispatchFrom_sA[0]==="B")
				dbIndex_n =2;
			else{
				rect_errMessage.showMessage("Dispatch Fail: Unable to recognize line from given origin!");
				return;
			}

			//check if FROM line block exists
			if(dbIndex_n===-1||trackConstantsObjects_OA[dbIndex_n]["blocksMap"][dispatchFrom_s]===undefined){
				rect_errMessage.showMessage("Dispatch Fail: Dispatch Origin " + dispatchFrom_s +" is not a valid block");
				return;
			}

			//check if TO line block exists
			if(dbIndex_n===-1||trackConstantsObjects_OA[dbIndex_n]["blocksMap"][disptachTo_s]===undefined){
				rect_errMessage.showMessage("Dispatch Fail: Dispatch Destination " + disptachTo_s +" is not a valid block");
				return;
			}

			let dispatchTime_s= tInp_dispatchAt.text;
			if(dispatchTime_s===":")
				dispatchTime_s = t3databaseQml.currentTime_QML.split(":").slice(0,-1).join(":");
			if(!pathSelectionMode_b){
				root.possiblePaths_A = t3databaseQml.ctc_getPossiblePathsFromMetaInfo([dispatchTrainId_s,dispatchFrom_s,disptachTo_s,dispatchTime_s]);
				if(root.possiblePaths_A.length>0) root.pathSelectionMode_b = true;
			}else{
				if(currSelectedPathInd_n===-1){
					rect_errMessage.showMessage("Dispatch Fail: Please select a dispatch path.");
					return;
				}
				dispatchRequested([dispatchTrainId_s,dispatchFrom_s,disptachTo_s,dispatchTime_s],currSelectedPath_A);
				//console.log([dispatchTrainId_s,dispatchFrom_s,disptachTo_s,dispatchTime_s,root.currSelectedPath_A])
				possiblePaths_A = [];
				currSelectedPathInd_n = -1;
				pathSelectionMode_b = false;
			}
			//dispatchRequested([dispatchTrainId_s,dispatchFrom_s,disptachTo_s,dispatchTime_s]);
		}
	}

	Rectangle{
		id:rect_seperator2
		anchors{
			left:cust_okButton.right
			top:parent.top
			bottom:parent.bottom
			topMargin: T3Styling.lineWidth_r
			bottomMargin: T3Styling.lineWidth_r
			leftMargin: T3Styling.spacing_r
		}
		width: T3Styling.lineWidth_r*0.5
		radius: width
		color: T3Styling.cFgSubSub_c
	}

	T3Button{
		id:cust_loadFromCsvButton
		anchors{
			top:parent.top
			//left:rect_dispatchAt.right
			left:rect_seperator2.right
			leftMargin: T3Styling.spacing_r
			bottom: parent.bottom
		}
		//releasedColor_c: !root.metadataReady?T3Styling.cRed_c:Qt.darker(pressedColor_c)
		//pressedColor_c: !root.metadataReady?T3Styling.cRed_c:T3Styling.cGreen_c
		width: T3Styling.fontSubSub_r*10
		opacity:(root.csvMetadataReady)?1:0.5
		//releasedColor_c: !(root.csvMetadataReady)?T3Styling.cRed_c:Qt.darker(pressedColor_c)
		pressedColor_c: !(root.csvMetadataReady)?releasedColor_c:T3Styling.cBgMain_c
		buttonLabel_s: "Import Schedule"
		onButtonClicked: if(csvMetadataReady)fDia_selectScheduleDialog.open()
	}

	Rectangle{
		id: rect_dispatchInfoInputBlock
		anchors{
			left:parent.left
			top: parent.top
			bottom:parent.bottom
			//right:rect_seperator.left
			//rightMargin: T3Styling.spacing_r
		}
		width:text_dispatchFTrain.width+rect_dispatchTrain.width
			  +text_dispatchFrom.width +rect_dispatchFrom.width
			  +text_dispatchTo.width + rect_dispatchTo.width
			  +text_dispatchAt.width +rect_dispatchAt.width
		color: "transparent"
		opacity: root.pathSelectionMode_b?0:1
		Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}

		T3Text{
			anchors{
				top:parent.top
				left:parent.left
				bottom: parent.bottom
			}
			width: T3Styling.fontSubSub_r*5
			textPixelSize_r: T3Styling.fontSubSub_r
			id:text_dispatchFTrain
			textContent_s: "TRAIN:"
			textColor_c: T3Styling.cFgSub_c
		}

		Rectangle{
			id:rect_dispatchTrain
			anchors{
				top:parent.top
				left:text_dispatchFTrain.right
				bottom: parent.bottom
			}
			width: T3Styling.fontSubSub_r*6
			radius: T3Styling.margin_r
			color: tInp_dispatchTrain.focus?T3Styling.cBgMain_c:T3Styling.cFgSubSub_c
			Behavior on color{PropertyAnimation{}}
			T3Text{
				anchors{
					fill:parent
				}
				id:text_dispatchTrainHint
				textPixelSize_r: T3Styling.fontSubSub_r
				textContent_s: "####"
				visible: tInp_dispatchTrain.text===""
				textColor_c: T3Styling.cFgSub_c
			}
			TextInput{
				id:tInp_dispatchTrain
				anchors{
					fill:parent
				}
				readOnly:root.pathSelectionMode_b
				cursorDelegate: Item{}
				font.bold: acceptableInput
				color: acceptableInput?T3Styling.cFgMain_c:text===""?T3Styling.cFgMain_c:T3Styling.cYellow_c
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				inputMask: ">Nnnn"
				opacity:text==="0000"?0:1

				text:""
				//displayText: "Yard"
				font.family: "Inter"
				font.pixelSize: T3Styling.fontSubSub_r
				onFocusChanged: if(focus||!acceptableInput) clear()
			}
		}

		T3Text{
			anchors{
				top:parent.top
				left:rect_dispatchTrain.right
				bottom: parent.bottom
			}
			width: T3Styling.fontSubSub_r*5
			textPixelSize_r: T3Styling.fontSubSub_r
			id:text_dispatchFrom
			textContent_s: "FROM:"
			textColor_c: T3Styling.cFgSub_c
		}
		Rectangle{
			id:rect_dispatchFrom
			anchors{
				top:parent.top
				left:text_dispatchFrom.right
				bottom: parent.bottom
			}
			width: T3Styling.fontSubSub_r*6
			radius: T3Styling.margin_r
			color: tInp_dispatchFrom.focus?T3Styling.cBgMain_c:T3Styling.cFgSubSub_c
			Behavior on color{PropertyAnimation{}}
			T3Text{
				id:text_dispatchFromHint
				anchors{
					fill:parent
				}
				textPixelSize_r: T3Styling.fontSubSub_r
				textContent_s: "YARD"
				visible: tInp_dispatchFrom.text==="  "
				textColor_c: T3Styling.cFgMain_c
			}
			TextInput{
				id:tInp_dispatchFrom
				readOnly:root.pathSelectionMode_b
				readonly property bool customAcceptableInput:acceptableInput||tInp_dispatchFrom.text==="  "
				anchors{
					fill:parent
				}
				cursorDelegate: Item{}
				width: T3Styling.fontSubSub_r*6
				font.bold: acceptableInput
				color: acceptableInput?T3Styling.cFgMain_c:text==="  "?T3Styling.cFgMain_c:T3Styling.cYellow_c
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				inputMask: ">A A 900"
				text:""

				font.family: "Inter"
				font.pixelSize: T3Styling.fontSubSub_r
				onFocusChanged: if(focus||!acceptableInput) clear()
			}
		}
		T3Text{
			anchors{
				top:parent.top
				left:rect_dispatchFrom.right
				bottom: parent.bottom
			}
			width: T3Styling.fontSubSub_r*5
			textPixelSize_r: T3Styling.fontSubSub_r
			id:text_dispatchTo
			textContent_s: "DEST:"
			textColor_c: T3Styling.cFgSub_c
		}
		Rectangle{
			id:rect_dispatchTo
			anchors{
				top:parent.top
				left:text_dispatchTo.right
				bottom: parent.bottom
			}
			width: T3Styling.fontSubSub_r*6
			radius: T3Styling.margin_r
			color: tInp_dispatchTo.focus?T3Styling.cBgMain_c:T3Styling.cFgSubSub_c
			Behavior on color{PropertyAnimation{}}
			T3Text{
				anchors{
					fill:parent
				}
				textPixelSize_r: T3Styling.fontSubSub_r
				textContent_s: "X X ###"
				visible: tInp_dispatchTo.text==="  "
				textColor_c: T3Styling.cFgSub_c
			}
			TextInput{
				id:tInp_dispatchTo
				readOnly:root.pathSelectionMode_b
				anchors{
					fill:parent
				}
				cursorDelegate: Item{}
				width: T3Styling.fontSubSub_r*6
				font.bold: acceptableInput
				color: acceptableInput?T3Styling.cFgMain_c:text===":"?T3Styling.cFgMain_c:T3Styling.cYellow_c
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				inputMask: ">A A 900"
				text:""
				//displayText: "Yard"
				font.family: "Inter"
				font.pixelSize: T3Styling.fontSubSub_r
				onFocusChanged: if(focus||!acceptableInput) clear()
			}
		}
		T3Text{
			anchors{
				top:parent.top
				left:rect_dispatchTo.right
				bottom: parent.bottom
			}
			width: T3Styling.fontSubSub_r*5
			textPixelSize_r: T3Styling.fontSubSub_r
			id:text_dispatchAt
			textContent_s: "TIME:"
			textColor_c: T3Styling.cFgSub_c
		}
		Rectangle{
			id:rect_dispatchAt
			anchors{
				top:parent.top
				left:text_dispatchAt.right
				bottom: parent.bottom
			}
			width: T3Styling.fontSubSub_r*6
			radius: T3Styling.margin_r
			color: tInp_dispatchAt.focus?T3Styling.cBgMain_c:T3Styling.cFgSubSub_c
			Behavior on color{PropertyAnimation{}}
			T3Text{
				anchors{
					fill:parent
				}
				textPixelSize_r: T3Styling.fontSubSub_r
				textContent_s:tInp_dispatchAt.text===":"?t3databaseQml.currentTime_QML.split(":").slice(0,-1).join(":"):""
				textColor_c: T3Styling.cFgMain_c
			}
			TextInput{
				readOnly:root.pathSelectionMode_b
				id:tInp_dispatchAt
				readonly property bool customAcceptableInput
				: !(!acceptableInput||parseInt(text.split(":")[0])>23||parseInt(text.split(":")[1])>59)||text===":"
				anchors{
					fill:parent
				}
				cursorDelegate: Item{}
				font.bold: tInp_dispatchAt.customAcceptableInput
				color: tInp_dispatchAt.customAcceptableInput?T3Styling.cFgMain_c:text===":"?T3Styling.cFgMain_c:T3Styling.cYellow_c
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
				inputMask: ">99:99"
				text:""
				opacity: text===":"?0:1
				//displayText: "Yard"
				font.family: "Inter"
				font.pixelSize: T3Styling.fontSubSub_r
				onFocusChanged: if(!tInp_dispatchAt.customAcceptableInput) clear()
			}
		}
	}
	Rectangle{
		id: rect_pathSelectionBlock
		anchors{
			left:parent.left
			top: parent.top
			bottom:parent.bottom
			right:rect_seperator.left
			rightMargin: T3Styling.spacing_r
		}
		color: "transparent"
		opacity: root.pathSelectionMode_b?1:0
		T3Text{
			anchors{
				top:parent.top
				left:parent.left
				bottom: parent.bottom
			}
			width: T3Styling.fontSubSub_r*5
			textPixelSize_r: T3Styling.fontSubSub_r
			id:text_via
			textContent_s: {
				let pCount_n = 0;
				pathIds_sA.forEach((it_s)=>pCount_n+=(it_s.includes("_")?0:1));
				if(pCount_n===0) return "VIA:";
				else if(pCount_n === pathIds_sA.length) return "PATH:"
				else return "P/VIA:";
			}
			textColor_c: T3Styling.cFgSub_c
		}
		Grid{
			id:grid_viaButtons
			rows: 1
			columns: root.possiblePaths_A.length
			columnSpacing: T3Styling.spacing_r
			readonly property real unitWidth_r:
				(grid_viaButtons.width - columnSpacing*(columns-1))/columns
			anchors{
				left:text_via.right
				top:parent.top
				bottom: parent.bottom
				right:parent.right
			}
			Repeater{
				id:reap_viaButtons
				model:root.pathIds_sA
				T3Button{
					width: grid_viaButtons.unitWidth_r
					height: grid_viaButtons.height
					buttonLabel_s: modelData.split("_").join(" ")
					releasedColor_c:
						root.currSelectedPathInd_n === index?T3Styling.cBgMain_c:T3Styling.cFgSubSub_c
					onButtonClicked: {
						root.currSelectedPathInd_n = index;
					}
				}
			}
		}

		Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}

	}

	Rectangle{
		id:rect_errMessage
		anchors.fill: parent
		radius: T3Styling.margin_r
		color: Qt.darker(T3Styling.cRed_c)
		opacity: 0.0
		SequentialAnimation{
			id:anim_showAndDisappear
			alwaysRunToEnd: true
			PropertyAnimation{
				target:rect_errMessage
				property:"opacity"
				from:0.0
				to:1.0
				duration: 500
			}
			PropertyAnimation{
				target:rect_errMessage
				property:"opacity"
				from:1.0
				to:1.0
				duration: 1000
			}
			PropertyAnimation{
				target:rect_errMessage
				property:"opacity"
				from:1.0
				to:0.0
				duration: 500
			}
		}
		T3Text{
			id:text_errMessage
			anchors.fill: parent
			textContent_s: "ERR MESSAGE"
			textColor_c: T3Styling.cFgMain_c
			textPixelSize_r: T3Styling.fontSubSub_r
		}
		function showMessage(errMsg:string){
			text_errMessage.textContent_s = errMsg;
			anim_showAndDisappear.start()
		}
	}
	function findUniqueIdsFromPaths(paths_A){
		let length_n = paths_A.length;
		let maps_SA = paths_A.map((it_sA)=> new Set(it_sA));
		let ids_sA = Array(length_n).fill().map(()=>"");
		for(let i_n = 0;i_n<length_n;++i_n){
			let currPath_sA = [...paths_A[i_n]];
			maps_SA.forEach((it_S,ind_n)=>{
								if(ind_n===i_n) return;
								currPath_sA = currPath_sA.filter((it_s)=>it_S.has(it_s)===false);
							});
			ids_sA[i_n] = currPath_sA.length>0?currPath_sA[0]:("P"+i_n).toString();
		}
		//console.log(ids_sA)
		return ids_sA;
	}


	FileDialog{
		id:fDia_selectScheduleDialog
		title: "Please select Schedule CSV file"
		selectExisting: true
		selectMultiple: false
		selectFolder:false
		nameFilters: ["Line CSV File (*.csv)"]
		onAccepted: {
			console.log(fDia_selectScheduleDialog.fileUrl);
			root.possiblePaths_A = t3databaseQml.ctc_getPossiblePathsFromCsv
					(fDia_selectScheduleDialog.fileUrl,[tInp_dispatchTrain.text,"NA","NA",tInp_dispatchAt.text]);

			if(root.possiblePaths_A.length>0) root.pathSelectionMode_b = true;
			dispatchRequested(["SCED",root.possiblePaths_A[0][0],root.possiblePaths_A[0][root.possiblePaths_A[0].length-1],"01:01"],root.possiblePaths_A[0]);
			possiblePaths_A = [];
			currSelectedPathInd_n = -1;
			pathSelectionMode_b = false;
		}
	}



}
