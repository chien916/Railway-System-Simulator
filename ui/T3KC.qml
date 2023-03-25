import QtQuick 2.12
import QtQuick.Dialogs 1.0
Item {
	id:root
	property real plcInputs_r:0xfff3ffff
	property real plcOutputs_r:13
	property string plcFilePath_s:"N/A"
	property string plcStatus_s:"Not Loaded"
	property bool hasGate_b: true
	property string switchSide_s:"left"
	property string direction_s:"bidirectional"
	property string blockId_s:"B_A_5"
	implicitHeight: 500
	implicitWidth: 1000
	readonly property variant plcInputsMapper_O:{
		"connectionCtc":[0,0],
		"authority":[1,1],
		"suggestedSpeed":[2,9],
		"leftWwitchPosition":[10,10],
		"rightSwitchPosition":[11,11],
		"connectionKm":[16,16],
		"prevPrevOccup":[17,17],
		"prevOccup":[18,18],
		"currOccup":[19,19],
		"nextOccup":[20,20],
		"nextNextOccup":[21,21]
	}
	readonly property variant plcOutputsMapper_O:{
		"currOccup":[0,0],
		"authority":[4,4],
		"commandedSpeed":[5,12],
		"leftSwitchPosition":[16,16],
		"rightSwitchPosition":[17,17],
		"leftLightSignal":[18,19],
		"rightLightSignal":[20,21],
		"crossing":[22,22],//gate
		"prevOccup":[24,24],
		"nextOccup":[25,25],
	}
	function getValueFromPlcInputs(prop_s:string){
		let bitsPosition_nA = plcInputsMapper_O[prop_s];
		if(bitsPosition_nA===undefined) return;
		let bitsMask_n = 0;
		for(let i_n = bitsPosition_nA[0];i_n<bitsPosition_nA[1];++i_n){
			bitsMask_n = bitsMask_n|(1<<i_n);
		}
		let retrieved_n = root.plcInputs_r&bitsMask_n;
		let shiftedRetrieved_n = retrieved_n>>bitsPosition_nA[0];
		return shiftedRetrieved_n;
	}
	function setValueToPlcOutputs(prop_s:string,val_n:number){
		let bitsPosition_nA = plcInputsMapper_O[prop_s];
		if(bitsPosition_nA===undefined) return;
		let bitsMask_n = 0;
		for(let i_n = bitsPosition_nA[0];i_n<bitsPosition_nA[1];++i_n){
			bitsMask_n = bitsMask_n|(1<<i_n);
		}
		let clearedOutputs_n = root.plcOutputs_r&(~bitsMask_n);
		let shiftedPushing_n = (val_n<<bitsPosition_nA[0])|clearedOutputs_n;
		return shiftedPushing_n;
	}
	Rectangle{
		anchors.fill: parent
		radius: T3Styling.margin_r
		color: T3Styling.cBgSub_c
		T3Text{
			anchors.bottom: parent.bottom
			anchors.right: parent.right
			anchors.rightMargin: T3Styling.margin_r
			textPixelSize_r: T3Styling.fontSubSub_r
			height: T3Styling.margin_r
			width: parent.width
			textContent_s: "T3 Track Controller | Avi Dave"
			textAlign_s: "right"
			textBold_b: true
		}
	}
	FileDialog {
		id: fDia_fileDialog
		title: "Please choose a PLC file"
		folder: shortcuts.home
		onAccepted: {
			plcFilePath_s = fDia_fileDialog.fileUrls.toString()
			plcStatus_s = "Loaded"
			//Qt.quit()
		}
		onRejected: {
			plcFilePath_s = "N/A"
			plcStatus_s = "Not Loaded"
			//Qt.quit()
		}
		//Component.onCompleted: visible = true
	}
	Item{
		id:item_canvas
		anchors{
			top:parent.top
			left:parent.left
			right:parent.horizontalCenter
			bottom: parent.bottom
		}

		anchors.margins: T3Styling.margin_r
		Rectangle{
			id:rect_leftScreen
			anchors{
				top:parent.top
				//top:sDis_segDisplay.bottom
				//topMargin: T3Styling.margin_r
				left:item_canvas.left
				bottom:item_canvas.bottom
				bottomMargin: T3Styling.margin_r+T3Styling.spacing_r*2
			}
			width:item_canvas.width/**0.7*/
			radius: T3Styling.spacing_r
			color: T3Styling.cBgMain_c
			border.width: T3Styling.lineWidth_r
			border.color: T3Styling.cFgSubSub_c
			Item{
				id:item_leftScreenCanvas
				anchors.fill: parent
				anchors.margins: T3Styling.margin_r
				Grid{
					id:grid_label
					rows:2
					columns:1
					anchors{
						top:parent.top
						left:parent.left
						right:parent.right
					}
					height: T3Styling.fontSub_r*rows+T3Styling.spacing_r*(rows-1)
					Repeater{
						model:[["PLC File Path",root.plcFilePath_s],
						["PLC Program Status",root.plcStatus_s]]
						delegate:Item{
							width: grid_label.width
							height: T3Styling.fontSub_r
							T3Text{
								anchors.fill: parent
								textContent_s: modelData[0]
								textAlign_s: "left"
								textColor_c: T3Styling.cFgSub_c
							}
							T3Text{
								anchors.fill: parent
								textContent_s: modelData[1]
								textAlign_s: "right"
								textColor_c: T3Styling.cFgMain_c
							}
						}
					}
				}
				Rectangle{
					id:rect_seperatorInputLabel
					anchors{
						left:parent.left
						right: parent.right
						top:grid_label.bottom
						topMargin: T3Styling.spacing_r
					}
					height: T3Styling.lineWidth_r
					radius: height
					color: T3Styling.cFgSubSub_c
				}

				Item{
					id:item_inputLabels
					anchors{
						top:rect_seperatorInputLabel.bottom
						topMargin: T3Styling.spacing_r
					}
					width: grid_label.width
					height: T3Styling.fontSub_r
					T3Text{
						anchors.fill: parent
						textContent_s: "PLC Input Binaries"
						textAlign_s: "left"
						textColor_c: T3Styling.cFgSub_c
					}
					T3Text{
						anchors.fill: parent
						textContent_s: "0x"+plcInputs_r.toString(16).padStart(8,"0").toUpperCase()
						textAlign_s: "right"
						textColor_c: T3Styling.cBlue_c
					}
				}

				Grid{
					id:grid_inputBinaries
					rows:2
					columns:16
					anchors{
						top:item_inputLabels.bottom
						topMargin: T3Styling.spacing_r
						left:parent.left
						right:parent.right
					}
					columnSpacing: T3Styling.lineWidth_r
					rowSpacing: columnSpacing
					readonly property real unitWidth_r:
						(width-(columns-1)*columnSpacing)/columns


					Repeater{
						model:parent.columns*parent.rows
						delegate:	Rectangle{
							width: grid_inputBinaries.unitWidth_r
							height: width
							radius: width/4
							color: (plcInputs_r&(1<<(31-index)))!==0?T3Styling.cBlue_c:T3Styling.cFgSubSub_c
							T3Text{
								anchors.fill: parent
								textContent_s: (plcInputs_r&(1<<(31-index)))!==0?"T":"F"
								textColor_c: (plcInputs_r&(1<<(31-index)))!==0?T3Styling.cBgMain_c:T3Styling.cBgMain_c
							}
						}
					}
				}

				Rectangle{
					id:rect_seperatorOutputLabel
					anchors{
						left:parent.left
						right: parent.right
						top:grid_inputBinaries.bottom
						topMargin: T3Styling.spacing_r*2
					}
					height: T3Styling.lineWidth_r
					radius: height
					color: T3Styling.cFgSubSub_c
				}

				Item{
					id:item_outputLabels
					anchors{
						top:rect_seperatorOutputLabel.bottom
						topMargin: T3Styling.spacing_r
					}
					width: grid_label.width
					height: T3Styling.fontSub_r
					T3Text{
						anchors.fill: parent
						textContent_s: "PLC Output Binaries"
						textAlign_s: "left"
						textColor_c: T3Styling.cFgSub_c
					}
					T3Text{
						anchors.fill: parent
						textContent_s: "0x"+plcOutputs_r.toString(16).padStart(8,"0").toUpperCase()
						textAlign_s: "right"
						textColor_c: T3Styling.cGreen_c
					}
				}

				Grid{
					id:grid_outputBinaries
					rows:2
					columns:16
					anchors{
						top:item_outputLabels.bottom
						topMargin: T3Styling.spacing_r
						left:parent.left
						right:parent.right
					}
					columnSpacing: T3Styling.lineWidth_r
					rowSpacing: columnSpacing
					readonly property real unitWidth_r:
						(width-(columns-1)*columnSpacing)/columns
					property color currentColor:T3Styling.cGreen_c
					Repeater{
						model:parent.columns*parent.rows
						delegate:	Rectangle{
							width: grid_inputBinaries.unitWidth_r
							height: width
							radius: width/4
							color: (plcOutputs_r&(1<<(31-index)))!==0?grid_outputBinaries.currentColor:T3Styling.cFgSubSub_c
							T3Text{
								anchors.fill: parent
								textContent_s: (plcOutputs_r&(1<<(31-index)))!==0?"T":"F"
								textColor_c: T3Styling.cBgMain_c
							}
						}
					}
					SequentialAnimation{
						alwaysRunToEnd: true
						loops: Animation.Infinite
						running: custom_maintainanceMode.valueratio_r>0.5
						PropertyAnimation{
							alwaysRunToEnd: true
							target: grid_outputBinaries
							property:"currentColor"
							from:T3Styling.cGreen_c
							to:T3Styling.cFgSubSub_c
							duration:300
						}
						PropertyAnimation{
							alwaysRunToEnd: true
							target: grid_outputBinaries
							property:"currentColor"
							from:T3Styling.cFgSubSub_c
							to:T3Styling.cGreen_c
							duration:300
						}
					}
				}


			}
		}
		Grid{
			id:grid_menuButtons
			anchors{
				top:rect_leftScreen.bottom
				topMargin: T3Styling.spacing_r
				bottom: parent.bottom
				left:rect_leftScreen.left
				right:rect_leftScreen.right
			}
			spacing: T3Styling.spacing_r
			rows:1
			columns:2
			Repeater{
				model:["Load Local PLC File","Toggle Manual / Automatic Mode"]
				delegate:T3Button{
					buttonLabel_s: modelData
					width: (grid_menuButtons.width-(grid_menuButtons.spacing*grid_menuButtons.columns-1))
						   /grid_menuButtons.columns
					height: grid_menuButtons.height
					pressedColor_c: index===1?T3Styling.cRed_c:T3Styling.cBgMain_c
					releasedColor_c:index===1?Qt.darker(T3Styling.cRed_c):T3Styling.cFgSubSub_c
					delayButton_b: index===1
					onButtonClicked: {
						if(index===0)
							fDia_fileDialog.open()
					}
				}
			}
		}


	}

	Rectangle{
		id:rect_canvas
		radius: T3Styling.margin_r
		color: T3Styling.cBgSub_c
		anchors{
			left:parent.horizontalCenter
			top:parent.top
			bottom:parent.bottom
			right:parent.right
		}
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
			anchors{
				top:parent.top
				left:parent.left
				right:parent.right
				bottom:parent.bottom
			}
			clip:true
			height: parent.height*0.3
			anchors{
				topMargin: T3Styling.margin_r
				leftMargin: T3Styling.margin_r
				rightMargin: T3Styling.margin_r
				bottomMargin: T3Styling.margin_r*3
			}
//			clip: true
			spacing: T3Styling.spacing_r
			T3ParamUnit{
				id:custom_maintainanceMode
				height:T3Styling.fontMain_r*1
				maxValue_r: 1;
				minValue_r: 0
				fixedPoint_i: 2
				readOnly_b:false
				paramConfig_A:"F_T_Maintance Mode_"
				width: col_column.width
				onValueratio_rChanged: {
					if(valueratio_r===0) root.db2view(false);
				}
			}
			Row{
				width: col_column.width
				height:T3Styling.fontMain_r*1
				spacing: T3Styling.spacing_r
				T3ParamUnit{
					id:cust_speedLimit
					opacity:custom_maintainanceMode.valueratio_r>0.5?0.8:0.5
					Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
					height:T3Styling.fontMain_r*1
					maxValue_r: 100;
					minValue_r: 0
					fixedPoint_i: 2
					readOnly_b: true
					paramConfig_A:"F_F_Speed Limit_mph"
					width: (parent.width-parent.spacing)/2
				}
				T3ParamUnit{
					id:cust_suggestedSpeed
					opacity:custom_maintainanceMode.valueratio_r>0.5?0.8:0.5
					Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
					height:T3Styling.fontMain_r*1
					maxValue_r: 100;
					minValue_r: 0
					fixedPoint_i: 2
					readOnly_b: true
					paramConfig_A:"F_F_Suggested Speed_mph"
					width: (parent.width-parent.spacing)/2
				}


			}


			T3ParamUnit{
				id:cust_commandedSpeed
				opacity:custom_maintainanceMode.valueratio_r>0.5?1:0.5
				Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
				height:T3Styling.fontMain_r*1
				maxValue_r: 100;
				minValue_r: 0
				fixedPoint_i: 2
				readOnly_b: !custom_maintainanceMode.valueratio_r>0.5
				paramConfig_A:"F_F_Commanded Speed_mph"
				width: col_column.width
			}
			Item{
				width: parent.width
				height: T3Styling.spacing_r
			}



			Grid{
				id:grid_dialTexts
				rows:1
				columns: 4
				height: T3Styling.fontSubSub_r
				width: parent.width
				columnSpacing: 0
				property real unitWidth_r:(width-columnSpacing*(columns-1))/columns
				Repeater{
					model:[" L Sig"," Switch"," Gate"," R Sig"]
					delegate:	Item{
						width: parent.unitWidth_r
						height: T3Styling.fontSubSub_r
						T3Text{
							textContent_s: modelData
							opacity: custom_maintainanceMode.valueratio_r>0.5?1:0.5
							Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
							anchors{
								left: parent.left
								top:parent.top
								bottom:parent.bottom
								right:parent.right
							}
							textColor_c: T3Styling.cFgSub_c
							textPixelSize_r: T3Styling.fontSubSub_r
							textBold_b: true
							textAlign_s: "left"
						}
						T3Text{
							textContent_s: {
								if(index===0){//left switch
									if(cust_leftSignalDial.ticks_nA.length===0)
										return "N/A ";
									else if(cust_leftSignalDial.currValue_n<=0)
										return "RED "
									else if(cust_leftSignalDial.currValue_n<=0.18)
										return "YELLOW "
									else if(cust_leftSignalDial.currValue_n<=0.36)
										return "GREEN "
									else return "???"
								}else if(index===1){
									if(cust_switchDial.ticks_nA.length===0)
										return "N/A ";
									else if(cust_switchDial.currValue_n<=0
											||cust_switchDial.currValue_n>=1)
										return "DOWN "
									else if(cust_switchDial.currValue_n<=0.36
											||cust_switchDial.currValue_n>=0.64)
										return "UP "
									else return "???"
								}else if(index===2){
									if(cust_gateDial.ticks_nA.length===0)
										return "N/A ";
									else if(cust_gateDial.currValue_n===0
											||cust_gateDial.currValue_n===1)
										return "DOWN "
									else if(cust_gateDial.currValue_n===0.36
											||cust_gateDial.currValue_n===0.64)
										return "UP "
									else return "???"
								}else if(index===3){//left switch
									if(cust_rightSignalDial.ticks_nA.length===0)
										return "N/A ";
									else if(cust_rightSignalDial.currValue_n===1)
										return "RED "
									else if(cust_rightSignalDial.currValue_n===0.82)
										return "YELLOW "
									else if(cust_rightSignalDial.currValue_n===0.64)
										return "GREEN "
									else return "???"
								}
								return "???";
							}
							opacity: custom_maintainanceMode.valueratio_r>0.5?1:0.5
							Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
							anchors{
								left: parent.left
								top:parent.top
								bottom:parent.bottom
								right:parent.right
							}
							textColor_c: {
								if(textContent_s==="RED ") return T3Styling.cRed_c
								else if(textContent_s==="YELLOW ") return T3Styling.cYellow_c
								else if(textContent_s==="GREEN ") return T3Styling.cGreen_c
								else return T3Styling.cFgMain_c
							}
							textPixelSize_r: T3Styling.fontSubSub_r
							textBold_b: true
							textAlign_s: "right"
						}

					}
				}



			}

			Grid{
				id:grid_dials
				rows:1
				columns: 4
				height: unitWidth_r
				width: parent.width
				columnSpacing: 0
				property real unitWidth_r:(width-columnSpacing*(columns-1))/columns

				T3NCDial{
					id:cust_leftSignalDial
					height: parent.unitWidth_r
					width: parent.unitWidth_r
					//anchors.horizontalCenter: parent.horizontalCenter
					ticks_nA:direction_s.toLowerCase()!=="right"?[0,0.18,0.36]:[]
					enabled_b:custom_maintainanceMode.valueratio_r>0.5
				}
				T3NCDial{
					id:cust_switchDial
					height: parent.unitWidth_r
					width: parent.unitWidth_r
					//anchors.horizontalCenter: parent.horizontalCenter
					ticks_nA:{
						if(switchSide_s==="left")return [0,0.36]
						else if(switchSide_s==="right")return [0.64,1]
						else return [];
					}
					enabled_b:custom_maintainanceMode.valueratio_r>0.5
				}
				T3NCDial{
					id:cust_gateDial
					height: parent.unitWidth_r
					width: parent.unitWidth_r
					//anchors.horizontalCenter: parent.horizontalCenter
					ticks_nA: hasGate_b?[0.64,1]:[]
					enabled_b:custom_maintainanceMode.valueratio_r>0.5
				}
				T3NCDial{
					id:cust_rightSignalDial
					height: parent.unitWidth_r
					width: parent.unitWidth_r
					//anchors.horizontalCenter: parent.horizontalCenter
					ticks_nA:direction_s.toLowerCase()!=="left"?[0.64,0.82,1]:[]
					enabled_b:custom_maintainanceMode.valueratio_r>0.5
				}
			}

			Item{
				width: parent.width
				height: T3Styling.spacing_r*1.5
			}

			Item{
				width: col_column.width
				height: T3Styling.fontSubSub_r
				T3Text{
					textContent_s: " Authority"
					opacity: custom_maintainanceMode.valueratio_r>0.5?1:0.5
					Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
					anchors.fill: parent
					textColor_c: T3Styling.cFgSub_c
					textPixelSize_r: T3Styling.fontSubSub_r
					textBold_b: true
					textAlign_s: "left"
				}
				T3Text{
					textContent_s:{
						if(tInp_dispatchFrom.text==="  ") return "N/A"
						else tInp_dispatchFrom.text
					}
					opacity:custom_maintainanceMode.valueratio_r>0.5?1:0.5
					Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
					anchors.fill: parent
					textColor_c: T3Styling.cFgMain_c
					textPixelSize_r: T3Styling.fontSubSub_r
					textBold_b: true
					textAlign_s: "right"
				}

			}

			Rectangle{
				id:rect_dispatchFrom
				height: T3Styling.fontSubSub_r*2
				width: parent.width
				radius: T3Styling.margin_r
				opacity:custom_maintainanceMode.valueratio_r>0.5?1:0.5
				Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
				color: (custom_maintainanceMode.valueratio_r>0.5)?T3Styling.cBgMain_c:T3Styling.cFgSubSub_c
				Behavior on color{PropertyAnimation{}}
				T3Text{
					id:text_dispatchFromHint
					anchors{
						fill:parent
					}
					textPixelSize_r: T3Styling.fontSubSub_r
					textContent_s: "NO AUTHORITY GIVEN"
					visible: tInp_dispatchFrom.text==="  "
					textColor_c: T3Styling.cFgMain_c
				}
				TextInput{
					id:tInp_dispatchFrom
					readOnly:!custom_maintainanceMode.valueratio_r>0.5
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
					onFocusChanged: {
						if(focus||!acceptableInput) selectAll()
					}
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
			width: (rect_canvas.width-T3Styling.margin_r*2)
			buttonLabel_s: "APPLY"
			onButtonClicked: {
				//console.log(tInp_dispatchFrom.text)
				if(tInp_dispatchFrom.text==="  "){
					view2db();
					return;
				}
				let dispatchFrom_s = tInp_dispatchFrom.text.split(" ").join("_");
				for(let i = 0;i<t3databaseQml.trackConstantsObjects_QML.length;++i){
					if(t3databaseQml.trackConstantsObjects_QML[i]["blocksMap"][dispatchFrom_s]!==undefined){
						if(dispatchFrom_s.charAt(0)!==root.blockId_s.charAt(0)){
							rect_errMessage.showMessage("Block on Different Line");
							return;
						}
						view2db();
						return;
					}
				}
				rect_errMessage.showMessage("Unrecognized Block");
			}
		}



		Rectangle{
			anchors{
				bottom:parent.bottom
				left:parent.left
				right:parent.right
				margins: T3Styling.margin_r
			}
			height: T3Styling.margin_r
			id:rect_errMessage
			radius: T3Styling.margin_r
			color: Qt.darker(T3Styling.cRed_c)
			opacity:0
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
	}

	function db2view(maintanceModeIncluded_b:bool){
		if(!t3databaseQml.trackVariablesObjects_QML) return;
		//console.log(t3databaseQml.km_getTrackProperty(blockId_s,0))
		//handles authority
		let authority_s = t3databaseQml.km_getTrackProperty(blockId_s,1);
		if(!authority_s||authority_s.length===0) tInp_dispatchFrom.text = "";
		else{
			let splittedPath_sA = authority_s.split("|");
			tInp_dispatchFrom.text = splittedPath_sA[splittedPath_sA.length-1].split("_").join(" ");
		}
		//handles maintanaceMode
		if(maintanceModeIncluded_b){
			let mainanceMode_b = t3databaseQml.km_getTrackProperty(blockId_s,10);
			custom_maintainanceMode.valueratio_r = mainanceMode_b?1:0;
		}
		//handles suggested speed
		let suggestedSpeed_r = t3databaseQml.km_getTrackProperty(blockId_s,0);
		cust_suggestedSpeed.valueratio_r = suggestedSpeed_r/100;
		//handles switch position
		let switchIsUp_b = t3databaseQml.km_getTrackProperty(blockId_s,2);
		for(let i = 0;i<t3databaseQml.trackConstantsObjects_QML.length;++i){
			if(t3databaseQml.trackConstantsObjects_QML[i]["blocksMap"][root.blockId_s]!==undefined){
				let currBlockConstantObject = t3databaseQml.trackConstantsObjects_QML[i]["blocksMap"][root.blockId_s];
				let prevBlock2_s = currBlockConstantObject["prevBlock2"];
				let nextBlock2_s= currBlockConstantObject["nextBlock2"];
				if(prevBlock2_s!==""&&prevBlock2_s!=="PASSIVE"){
					root.switchSide_s = "left"
					cust_switchDial.dialDialValue_r = switchIsUp_b?0.36:0;
				}
				else if(nextBlock2_s!==""&&nextBlock2_s!=="PASSIVE"){
					root.switchSide_s = "right"
					cust_switchDial.dialDialValue_r = switchIsUp_b?0.64:1;
				}else{
					root.switchSide_s = ""
				}
			}
		}
	}

	function view2db(){
		if(!t3databaseQml.trackVariablesObjects_QML) return;
		//handles authority
		//add new authorities
		if(tInp_dispatchFrom.text==="  ")
			t3databaseQml.kc_revokeAuthority(blockId_s)
		else{
			let paths_A = t3databaseQml.ctc_getPossiblePathsFromMetaInfo(["____",blockId_s,tInp_dispatchFrom.text.split(" ").join("_"),"__:__"]);
			if(paths_A.length===0){
				rect_errMessage.showMessage("No Path Found");
				return;
			}
			paths_A.sort((a_A,b_A)=>a_A.length-b_A.length);
			t3databaseQml.kc_grantAuthority(paths_A[0]);
		}
		//handles maintanaceMode
		t3databaseQml.km_setTrackProperty(blockId_s,10,custom_maintainanceMode.valueratio_r>0.5);
		//handles suggested speed
		t3databaseQml.km_setTrackProperty(blockId_s,0,cust_suggestedSpeed.actualValue_r);
		//handles switch position
		if(switchSide_s==="left")
			t3databaseQml.km_setTrackProperty(blockId_s,2,cust_switchDial.currValue_n===0.36);
		else if(switchSide_s==="right")
			t3databaseQml.km_setTrackProperty(blockId_s,2,cust_switchDial.currValue_n===0.64);
		//console.log(custom_maintainanceMode.valueratio_r>0.5)
	}


}
