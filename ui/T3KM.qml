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
	function db2view(){
		if(blockId_s===""||!repe_dataFields) return;
		let displayString_sA = t3databaseQml.km_getDisplayStrings(blockId_s);
		for(let i = 0;i<displayString_sA.length;++i){
			repe_dataFields.itemAt(i).stringToDisp_s = displayString_sA[i];
		}

		let metaInfo_A = t3databaseQml.km_getIOFromMetaInfo(blockId_s);
		cust_envTemp.valueratio_r = metaInfo_A[0]/cust_envTemp.maxValue_r;
		butt_brokenRail.currState_b = metaInfo_A[1];
		butt_powerFailure.currState_b = metaInfo_A[2];
		butt_trackCircuit.currState_b = metaInfo_A[3];
	}
	onBlockId_sChanged: {
		db2view();
	}
	implicitHeight: 600
	implicitWidth: 800
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
			//plcFilePath_s = fDia_fileDialog.fileUrls.toString()
			//plcStatus_s = "Loaded"
			//Qt.quit()
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
		isTrack_b:false
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
		id:rect_topScreen
		visible: true
		anchors{
			top:parent.top
			left:cust_trackSelector.right
			right:parent.right
			bottom: rect_bottomScreen.top
		}

		color: "transparent"
		anchors.margins: T3Styling.margin_r
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
			left:cust_trackSelector.right
			right:parent.right
			bottom: parent.bottom
		}
		color: "transparent"
		anchors.margins: T3Styling.margin_r
		height: T3Styling.margin_r*5+T3Styling.spacing_r
		T3ParamUnit{
			id:cust_envTemp
			width: parent.width
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
					buttonLabel_s: (currState_b?"Revoke":"Invoke")+" Broken Rail Failure"
					releasedColor_c: currState_b?Qt.darker(T3Styling.cRed_c):T3Styling.cFgSubSub_c
					onButtonClicked: {
						currState_b = !currState_b;
						view2db();
					}
				}
				T3Button{
					id:butt_trackCircuit
					property bool currState_b: false
					width: parent.unitWidth_r
					height:parent.height
					buttonLabel_s: (currState_b?"Revoke":"Invoke")+" Track Circuit Failure"
					releasedColor_c: currState_b?Qt.darker(T3Styling.cRed_c):T3Styling.cFgSubSub_c
					onButtonClicked: {
						currState_b = !currState_b;
						view2db();
					}
				}
				T3Button{
					id:butt_powerFailure
					property bool currState_b: false
					width: parent.unitWidth_r
					height:parent.height
					buttonLabel_s: (currState_b?"Revoke":"Invoke")+" Power Failure"
					releasedColor_c: currState_b?Qt.darker(T3Styling.cRed_c):T3Styling.cFgSubSub_c
					onButtonClicked: {
						currState_b = !currState_b;
						view2db();
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
						view2db();
					}
				}
				T3Button{
					id:butt_apply
					width: parent.unitWidth_r
					height:parent.height
					buttonLabel_s: "Apply"
					releasedColor_c: Qt.darker(T3Styling.cGreen_c)
					onButtonClicked: {
						view2db();
					}
				}

			}

		}

	}
	//	Rectangle{
	//		id:rect_canvas
	//		radius: T3Styling.margin_r
	//		color: T3Styling.cBgSub_c
	//		anchors{
	//			left:parent.horizontalCenter
	//			top:parent.top
	//			bottom:parent.bottom
	//			right:parent.right
	//		}
	//		//		Column{
	//		//			anchors{
	//		//				margins: T3Styling.margin_r
	//		//				fill: parent
	//		//			}
	//		//			id:colu_allConfigs
	//		//			Text{
	//		//				text:"eet"
	//		//			}
	//		//		}
	//		Column{
	//			id:col_column
	//			anchors{
	//				top:parent.top
	//				left:parent.left
	//				right:parent.right
	//				bottom:parent.bottom
	//			}
	//			clip:true
	//			height: parent.height*0.3
	//			anchors{
	//				topMargin: T3Styling.margin_r
	//				leftMargin: T3Styling.margin_r
	//				rightMargin: T3Styling.margin_r
	//				bottomMargin: T3Styling.margin_r*3
	//			}
	//			//			clip: true
	//			spacing: T3Styling.spacing_r
	//			T3ParamUnit{
	//				id:cust_maintainanceMode
	//				height:T3Styling.fontMain_r*1
	//				maxValue_r: 1;
	//				minValue_r: 0
	//				fixedPoint_i: 2
	//				readOnly_b:false
	//				paramConfig_A:"F_T_Maintance Mode_"
	//				width: col_column.width
	//			}
	//			Row{
	//				width: col_column.width
	//				height:T3Styling.fontMain_r*1
	//				spacing: T3Styling.spacing_r
	//				T3ParamUnit{
	//					id:cust_speedLimit
	//					opacity:cust_maintainanceMode.valueratio_r>0.5?0.8:0.5
	//					Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
	//					height:T3Styling.fontMain_r*1
	//					maxValue_r: 100;
	//					minValue_r: 0
	//					fixedPoint_i: 2
	//					readOnly_b: true
	//					paramConfig_A:"F_F_Speed Limit_mph"
	//					width: (parent.width-parent.spacing)/2
	//				}
	//				T3ParamUnit{
	//					id:cust_suggestedSpeed
	//					opacity:cust_maintainanceMode.valueratio_r>0.5?0.8:0.5
	//					Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
	//					height:T3Styling.fontMain_r*1
	//					maxValue_r: 100;
	//					minValue_r: 0
	//					fixedPoint_i: 2
	//					readOnly_b: true
	//					paramConfig_A:"F_F_Suggested Speed_mph"
	//					width: (parent.width-parent.spacing)/2
	//				}
	//			}


	//			T3ParamUnit{
	//				id:cust_commandedSpeed
	//				opacity:cust_maintainanceMode.valueratio_r>0.5?1:0.5
	//				Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
	//				height:T3Styling.fontMain_r*1
	//				maxValue_r: 100;
	//				minValue_r: 0
	//				fixedPoint_i: 2
	//				readOnly_b: !cust_maintainanceMode.valueratio_r>0.5
	//				paramConfig_A:"F_F_Commanded Speed_mph"
	//				width: col_column.width
	//			}
	//			Item{
	//				width: parent.width
	//				height: T3Styling.spacing_r
	//			}



	//			Grid{
	//				id:grid_dialTexts
	//				rows:1
	//				columns: 4
	//				height: T3Styling.fontSubSub_r
	//				width: parent.width
	//				columnSpacing: 0
	//				property real unitWidth_r:(width-columnSpacing*(columns-1))/columns
	//				Repeater{
	//					model:[" L Sig"," Switch"," Gate"," R Sig"]
	//					delegate:	Item{
	//						width: parent.unitWidth_r
	//						height: T3Styling.fontSubSub_r
	//						T3Text{
	//							textContent_s: modelData
	//							opacity: cust_maintainanceMode.valueratio_r>0.5?1:0.5
	//							Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
	//							anchors{
	//								left: parent.left
	//								top:parent.top
	//								bottom:parent.bottom
	//								right:parent.right
	//							}
	//							textColor_c: T3Styling.cFgSub_c
	//							textPixelSize_r: T3Styling.fontSubSub_r
	//							textBold_b: true
	//							textAlign_s: "left"
	//						}
	//						T3Text{
	//							textContent_s: {
	//								if(index===0){//left switch
	//									if(cust_leftSignalDial.ticks_nA.length===0)
	//										return "N/A ";
	//									else if(cust_leftSignalDial.currValue_n<=0)
	//										return "RED "
	//									else if(cust_leftSignalDial.currValue_n<=0.18)
	//										return "YELLOW "
	//									else if(cust_leftSignalDial.currValue_n<=0.36)
	//										return "GREEN "
	//									else return "???"
	//								}else if(index===1){
	//									if(cust_switchDial.ticks_nA.length===0)
	//										return "N/A ";
	//									else if(cust_switchDial.currValue_n<=0)
	//										return "DOWN "
	//									else if(cust_switchDial.currValue_n<=0.18)
	//										return "AUTO "
	//									else if(cust_switchDial.currValue_n<=0.36)
	//										return "UP "
	//									else return "???"
	//								}else if(index===2){
	//									if(cust_gateDial.ticks_nA.length===0)
	//										return "N/A ";
	//									else if(cust_gateDial.currValue_n===1)
	//										return "CLOSED "
	//									else if(cust_gateDial.currValue_n===0.82)
	//										return "AUTO "
	//									else if(cust_gateDial.currValue_n===0.64)
	//										return "OPEN "
	//									else return "???"
	//								}else if(index===3){//left switch
	//									if(cust_rightSignalDial.ticks_nA.length===0)
	//										return "N/A ";
	//									else if(cust_rightSignalDial.currValue_n===1)
	//										return "RED "
	//									else if(cust_rightSignalDial.currValue_n===0.82)
	//										return "YELLOW "
	//									else if(cust_rightSignalDial.currValue_n===0.64)
	//										return "GREEN "
	//									else return "???"
	//								}
	//								return "???";
	//							}
	//							opacity: cust_maintainanceMode.valueratio_r>0.5?1:0.5
	//							Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
	//							anchors{
	//								left: parent.left
	//								top:parent.top
	//								bottom:parent.bottom
	//								right:parent.right
	//							}
	//							textColor_c: {
	//								if(textContent_s==="RED "||textContent_s==="CLOSED ") return T3Styling.cRed_c
	//								else if(textContent_s==="YELLOW "||textContent_s==="DOWN "||textContent_s==="UP ") return T3Styling.cYellow_c
	//								else if(textContent_s==="GREEN "||textContent_s==="OPEN ") return T3Styling.cGreen_c
	//								else return T3Styling.cFgMain_c
	//							}
	//							textPixelSize_r: T3Styling.fontSubSub_r
	//							textBold_b: true
	//							textAlign_s: "right"
	//						}
	//					}
	//				}
	//			}

	//			Grid{
	//				id:grid_dials
	//				rows:1
	//				columns: 4
	//				height: unitWidth_r
	//				width: parent.width
	//				columnSpacing: 0
	//				property real unitWidth_r:(width-columnSpacing*(columns-1))/columns

	//				T3NCDial{
	//					id:cust_leftSignalDial
	//					height: parent.unitWidth_r
	//					width: parent.unitWidth_r
	//					//anchors.horizontalCenter: parent.horizontalCenter
	//					ticks_nA:direction_s.toLowerCase()!=="right"?[0,0.18,0.36]:[]
	//					enabled_b:cust_maintainanceMode.valueratio_r>0.5
	//				}
	//				T3NCDial{
	//					id:cust_switchDial
	//					height: parent.unitWidth_r
	//					width: parent.unitWidth_r
	//					ticks_nA:[0,0.18,0.36]
	//					enabled_b:cust_maintainanceMode.valueratio_r>0.5
	//				}
	//				T3NCDial{
	//					id:cust_gateDial
	//					height: parent.unitWidth_r
	//					width: parent.unitWidth_r
	//					ticks_nA: hasGate_b?[0.64,1]:[]
	//					enabled_b:cust_maintainanceMode.valueratio_r>0.5
	//				}
	//				T3NCDial{
	//					id:cust_rightSignalDial
	//					height: parent.unitWidth_r
	//					width: parent.unitWidth_r
	//					ticks_nA:direction_s.toLowerCase()!=="left"?[0.64,0.82,1]:[]
	//					enabled_b:cust_maintainanceMode.valueratio_r>0.5
	//				}
	//			}

	//			Item{
	//				width: parent.width
	//				height: T3Styling.spacing_r*1.5
	//			}

	//			Row{
	//				width: col_column.width
	//				height:T3Styling.fontMain_r*1
	//				spacing: T3Styling.spacing_r
	//				property real unitWidth_r: (width-spacing*3)/4
	//				T3ParamUnit{
	//					id:cust_authTravelDirection
	//					opacity:cust_maintainanceMode.valueratio_r>0.5?1:0.5
	//					Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
	//					height:T3Styling.fontMain_r*1
	//					readOnly_b: !cust_maintainanceMode.valueratio_r>0.5
	//					paramConfig_A:"F_T_Forward Auth_mph"
	//					width: parent.unitWidth_r
	//				}
	//				T3ParamUnit{
	//					id:cust_authSwitchDirection
	//					opacity:cust_maintainanceMode.valueratio_r>0.5?1:0.5
	//					Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
	//					height:T3Styling.fontMain_r*1
	//					readOnly_b: !cust_maintainanceMode.valueratio_r>0.5
	//					paramConfig_A:"F_T_Up Auth_"
	//					width: parent.unitWidth_r
	//				}
	//				T3ParamUnit{
	//					id:cust_authNumberOfBlocks
	//					opacity:cust_maintainanceMode.valueratio_r>0.5?1:0.5
	//					Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
	//					height:T3Styling.fontMain_r*1
	//					readOnly_b: !cust_maintainanceMode.valueratio_r>0.5
	//					maxValue_r: 100
	//					fixedPoint_i: 2
	//					isWhole_b: true
	//					paramConfig_A:"F_F_Number Of Blocks Authorized_"
	//					width: parent.unitWidth_r*2+parent.spacing
	//				}
	//			}
	//		}
	//		T3Button{
	//			anchors{
	//				bottom:rect_canvas.bottom
	//				left:rect_canvas.left
	//				//				right:rect_canvas.right
	//				margins: T3Styling.margin_r
	//			}
	//			height: T3Styling.margin_r
	//			width: (rect_canvas.width-T3Styling.margin_r*2)
	//			buttonLabel_s: "APPLY"
	//			onButtonClicked: {
	//				view2db();
	//				db2view();
	//			}
	//		}
	//	}

}
