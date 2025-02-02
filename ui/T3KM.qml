import QtQuick 2.12
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.0
Item {
	id:root
	property string blockId_s:cust_trackSelector.currValue_s

	function view2db(){
		if(blockId_s==="") return;
		let metaInfo_A = [
				cust_envTemp.actualValue_r,//env temp
				butt_brokenRail.currState_b,//f1
				butt_powerFailure.currState_b,//f2
				butt_trackCircuit.currState_b//f3
			]
		t3databaseQml.km_setIOFromMetaInfo(blockId_s,metaInfo_A);
	}
	function db2view(includeIO_b){
		if(blockId_s===""||!repe_dataFields) return;
		let displayString_sA = t3databaseQml.km_getDisplayStrings(blockId_s);
		for(let i = 0;i<displayString_sA.length;++i){
			repe_dataFields.itemAt(i).stringToDisp_s = displayString_sA[i];
		}
		if(includeIO_b){
			let metaInfo_A = t3databaseQml.km_getIOFromMetaInfo(blockId_s);
			cust_envTemp.valueratio_r = metaInfo_A[0]/cust_envTemp.maxValue_r;
			butt_brokenRail.currState_b = metaInfo_A[1];
			butt_powerFailure.currState_b = metaInfo_A[2];
			butt_trackCircuit.currState_b = metaInfo_A[3];
		}
	}
	onBlockId_sChanged: {
		rect_frontHelper.runAnimation();
		db2view(true);
	}
	Connections{
		target: t3databaseQml
		function onOnTrackVariablesObjectsChanged(){
			db2view(false);
		}
	}
	implicitHeight: 600
	implicitWidth: 1200
	Rectangle{
		anchors.fill: parent
		//radius: T3Styling.margin_r
		color: T3Styling.cBgSub_c
		T3Text{
			anchors.bottom: parent.bottom
			anchors.right: parent.right
			anchors.rightMargin: T3Styling.margin_r
			textPixelSize_r: T3Styling.fontSubSub_r
			height: T3Styling.margin_r
			width: parent.width
			textContent_s: "T3 Track Model | Mark Roberts"
			textAlign_s: "right"
			textBold_b: true
		}
	}
	FileDialog {
		id: fDia_fileDialog
		title: "Please choose a Track CSV file"
		folder: shortcuts.home
		onAccepted: {
			t3databaseQml.km_addTrackFromCsv(fDia_fileDialog.fileUrls.toString());
		}
		onRejected: {
			//plcFilePath_s = "N/A"
			//plcStatus_s = "Not Loaded"
			//Qt.quit()
		}
		//Component.onCompleted: visible = true
	}
	T3TrackSelectorBlock{
		id:cust_trackSelector
		isTrack_b:true
		model_A: t3databaseQml?t3databaseQml.trackIds_QML:[[""]]
		anchors{
			top:parent.top
			//top:sDis_segDisplay.bottom
			//topMargin: T3Styling.margin_r
			left:parent.left
			bottom:parent.bottom
			margins: T3Styling.margin_r
		}
		width: root.width*0.05
	}
	Rectangle{
		id:item_lineGrid
		anchors{
			left:cust_trackSelector.right
			leftMargin: T3Styling.margin_r
			top:parent.top
			topMargin: T3Styling.margin_r
			bottom: parent.bottom
			bottomMargin: T3Styling.margin_r
		}
		color: "transparent"
		width: root.width*0.3
			T3CTCLineGrid{
				pathsAvailable_sA: [blockId_s]
				width: parent.height
				height: parent.width
				textRotated_b: true
				positionKnown_b: true
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.verticalCenter:  parent.verticalCenter
				dbIndex_i: cust_trackSelector.currColumn_i
				rotation: 90
				onBlockClicked: {
					cust_trackSelector.clickedSomewhereElse_b = true;
					cust_trackSelector.currValue_s = blockId_s;
				}
			}
	}

	Rectangle{
		id:rect_topScreen
		visible: true
		anchors{
			top:parent.top
			topMargin:T3Styling.margin_r
			left:item_lineGrid.right
			leftMargin: T3Styling.margin_r
			right:parent.right
			rightMargin: T3Styling.margin_r
			bottom: rect_bottomScreen.top
			bottomMargin: T3Styling.margin_r
			//rightMargin: root.width*0.4
		}
		color: "transparent"
		Rectangle{
			id:rect_leftScreen
			visible: true
			anchors.fill: parent
			//			anchors{
			//				bottomMargin: T3Styling.margin_r+T3Styling.spacing_r*2
			//			}
			radius: T3Styling.spacing_r
			color: T3Styling.cBgMain_c
			border.width: T3Styling.lineWidth_r
			border.color: T3Styling.cFgSubSub_c
			Rectangle{
				width: T3Styling.lineWidth_r*1.3
				height: parent.height
				radius: width
				color: T3Styling.cFgSubSub_c
				anchors.horizontalCenter: parent.horizontalCenter
			}
			Grid{
				id:grid_dataFields
				anchors{
					fill: parent
					margins: T3Styling.margin_r
				}
				rows:Math.ceil(repe_dataFields.model.length/2)//!
				columns:2
				rowSpacing: T3Styling.lineWidth_r
				columnSpacing: T3Styling.margin_r
				flow: Grid.TopToBottom
				property real unitHeight_r:(height-rowSpacing*(rows-1))/rows
				property real unitWidth_r:(width-columnSpacing*(columns-1))/columns
				Repeater{
					id:repe_dataFields
					model:[
						"/TRACK PROPERTIES"
						,"Track ID"
						,"Grade"
						,"Elevation"
						,"Length"
						,"Speed Limit"
						,"Direction of Travel"
						,"Railway Crossings"
						,"Track Heaters"
						,"/TRACK STATUS"
						,"RESERVERD"
						,"Train Occupancy"
						,"Switch Position"
						,"Left Signal Light"
						,"Right Signal Light"
						,"/BEACON"
						,"Underground"
						,"Authority Direction"
						,"Authority Block Number"
						,"Commanded Speed"
						,"/STATION PROPERTIES"
						,"Station Name"
						,"Station Sides"
						,"People on Station"
						,"People Boarding"
						,"People Disembarking"
						,"/FAILURE MODES"
						,"Broken Rail Failure"
						,"Track Circuit Failure"
						,"Power Failure"
					]
					delegate:Item{
						height: grid_dataFields.unitHeight_r
						width: grid_dataFields.unitWidth_r
						property string stringToDisp_s:"NOT IN SERVICE"
						property bool isTitle_b:modelData?modelData.includes("/"):false
						T3Text{
							anchors.fill: parent
							textColor_c:isTitle_b?T3Styling.cFgMain_c: T3Styling.cFgSub_c
							textContent_s: modelData?modelData.replace("/",""):""
							textAlign_s: isTitle_b?"":"left"
							textBold_b: isTitle_b
						}
						T3Text{
							anchors.fill: parent
							textColor_c: T3Styling.cFgMain_c
							textContent_s: isTitle_b?"":parent.stringToDisp_s
							textAlign_s: "right"
						}
					}
				}

			}
		}
		//		Grid{
		//			id:grid_menuButtons
		//			anchors{
		//				top:rect_topScreen.bottom
		//				left:cust_trackSelector.right
		//				right:parent.right
		//				bottom: parent.bottom
		//			}
		//			height: T3Styling.margin_r
		//			spacing: T3Styling.spacing_r
		//			rows:1
		//			columns:2
		//			Repeater{
		//				model:["Load Track File","Refresh Information"]
		//				delegate:T3Button{
		//					buttonLabel_s: modelData
		//					width: (grid_menuButtons.width-(grid_menuButtons.spacing*grid_menuButtons.columns-1))
		//						   /grid_menuButtons.columns
		//					height: grid_menuButtons.height
		//					//pressedColor_c: /*index===1?T3Styling.cRed_c:*/T3Styling.cBgMain_c
		//					//releasedColor_c:/*index===1?Qt.darker(T3Styling.cRed_c):*/T3Styling.cFgSubSub_c
		//					//delayButton_b: index===1
		//					onButtonClicked: {
		//						if(index===0)
		//							fDia_fileDialog.open()
		//						else if(index===1){
		//							if(blockId_s==="") return;
		//							//							t3databaseQml.kc_processPlc(blockId_s);
		//						}
		//					}
		//				}
		//			}
		//		}


	}



	Rectangle{
		id:rect_bottomScreen
		visible: true
		anchors{
			//						top:rect_topScreen.bottom
			left:item_lineGrid.right
			leftMargin: T3Styling.margin_r
			right:parent.right
			rightMargin: T3Styling.margin_r
			bottom: parent.bottom
			bottomMargin: T3Styling.margin_r
		}
		color: "transparent"
		height: T3Styling.margin_r*5+T3Styling.spacing_r
		T3ParamUnit{
			id:cust_envTemp
			width: parent.width
			opacity: blockId_s===""?0.5:1
			height:T3Styling.margin_r*2
			paramConfig_A: "F_F_Enviromental Temperature_F";
			maxValue_r: 100
			minValue_r: 0
			isWhole_b: true
			readOnly_b: false
		}
		Grid{
			anchors{
				left:parent.left
				right:parent.right
				bottom:parent.bottom
			}
			height: T3Styling.margin_r*rows+rowSpacing*rows-1
			rows:2
			columns: 1
			rowSpacing: T3Styling.spacing_r
			property real unitHeight_r: (height-(rows-1)*rowSpacing)/rows

			Grid{
				width: parent.width
				height: parent.unitHeight_r
				rows:1
				columns: 3
				columnSpacing: T3Styling.spacing_r
				property real unitWidth_r: (width-(columns-1)*columnSpacing)/columns
				T3Button{
					id:butt_brokenRail
					property bool currState_b: false
					width: parent.unitWidth_r
					height:parent.height
					opacity: blockId_s===""?0.5:1
					buttonLabel_s: (currState_b?"Revoke":"Invoke")+" Broken Rail Failure"
					releasedColor_c: currState_b?Qt.darker(T3Styling.cRed_c):T3Styling.cFgSubSub_c
					onButtonClicked: {
						currState_b = !currState_b;
					}
				}
				T3Button{
					id:butt_trackCircuit
					property bool currState_b: false
					width: parent.unitWidth_r
					height:parent.height
					opacity: blockId_s===""?0.5:1
					buttonLabel_s: (currState_b?"Revoke":"Invoke")+" Track Circuit Failure"
					releasedColor_c: currState_b?Qt.darker(T3Styling.cRed_c):T3Styling.cFgSubSub_c
					onButtonClicked: {
						currState_b = !currState_b;

					}
				}
				T3Button{
					id:butt_powerFailure
					property bool currState_b: false
					width: parent.unitWidth_r
					height:parent.height
					opacity: blockId_s===""?0.5:1
					buttonLabel_s: (currState_b?"Revoke":"Invoke")+" Power Failure"
					releasedColor_c: currState_b?Qt.darker(T3Styling.cRed_c):T3Styling.cFgSubSub_c
					onButtonClicked: {
						currState_b = !currState_b;
					}
				}

			}

			Grid{
				width: parent.width
				height: parent.unitHeight_r
				rows:1
				columns: 2
				columnSpacing: T3Styling.spacing_r
				property real unitWidth_r: (width-(columns-1)*columnSpacing)/columns
				T3Button{
					id:butt_loadModel
					width: parent.unitWidth_r
					height:parent.height
					buttonLabel_s:"Load Track CSV"
					onButtonClicked: {
						fDia_fileDialog.open()
					}
				}
				T3Button{
					id:butt_apply
					width: parent.unitWidth_r
					height:parent.height
					buttonLabel_s: "Apply"
					opacity: blockId_s===""?0.5:1
					releasedColor_c: Qt.darker(T3Styling.cGreen_c)
					onButtonClicked: {
						view2db();
					}
				}

			}

		}

	}

	Rectangle{
		id:rect_frontHelper
		anchors{
			left:item_lineGrid.right
			leftMargin: T3Styling.margin_r
			top:parent.top
			topMargin: T3Styling.margin_r
			bottom: rect_topScreen.bottom
			//bottomMargin: T3Styling.spacing_r
			right: parent.right
			rightMargin: T3Styling.margin_r
		}
		color:T3Styling.cBgSub_c
		property bool initState_b:true
		opacity: 1
		border.width: T3Styling.lineWidth_r
		border.color: T3Styling.cFgSubSub_c
		radius: T3Styling.lineWidth_r
		T3Text{
			textContent_s: rect_frontHelper.initState_b?"T3 | TRACK MODEL\n\n\n":""
			textColor_c:T3Styling.cFgMain_c
			anchors.fill: parent
			textPixelSize_r: T3Styling.fontSub_r
			textBold_b: true
			textLetterSpacing_r: T3Styling.fontSubSub_r*0.5
		}
		T3Text{
			id:text_animationLabel
			textContent_s: rect_frontHelper.initState_b?
							   "\n\n\n\n\n\n\n\n\nSelect a track on the left to start":
							   "Loading..."
			textColor_c: rect_frontHelper.initState_b?T3Styling.cFgSub_c:T3Styling.cFgMain_c
			anchors.fill: parent
			textPixelSize_r: T3Styling.fontSubSub_r
			textBold_b: false
			textLetterSpacing_r: T3Styling.fontSubSub_r*0.1
		}
		SequentialAnimation{
			id:animation_transition
			alwaysRunToEnd: true
			PropertyAnimation{
				target:rect_frontHelper
				property: "opacity"
				from: 0
				to:1
				duration:target.initState_b?0:50
				easing.type: Easing.OutCirc
			}
			PropertyAnimation{
				id:mid_anim
				target:rect_frontHelper
				property: "opacity"
				from: 1
				to:1
				duration:300
			}
			PropertyAnimation{
				target:rect_frontHelper
				property: "opacity"
				from: 1
				to:0
				duration:100
				easing.type: Easing.OutCirc
			}
		}
		function runAnimation(){
			mid_anim.duration = Math.random()*800
			text_animationLabel.textContent_s = "Please Wait";
			animation_transition.running = true;
			initState_b = false;
		}
	}
}
