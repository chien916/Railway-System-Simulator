import QtQuick 2.12
import QtQuick.Dialogs 1.0
Item {
	id:root
	property variant bCTCPLCIO_bA:Array(32).fill(true)
	property variant bKMPLCIO_bA:Array(32).fill(false)
	property variant bKCPLCIN_bA:Array(32).fill(false)
	property variant bBCNPLCOUT_bA:Array(32).fill(false)
	property string plcFilePath_s:"N/A"
	property string plcStatus_s:"Not Loaded"
	property bool hasGate_b: true
	property string switchSide_s:"left"
	property string direction_s:"bidirectional"
	property string blockId_s:cust_trackSelector.currValue_s
	function view2db(){
		if(blockId_s==="") return;
		let metaInfo_A = [
				cust_maintainanceMode.valueratio_r>0.5,
				0,0,//not applicable
				cust_commandedSpeed.actualValue_r,
				cust_leftSignalDial.currValue_n<0.09?"red":(cust_leftSignalDial.currValue_n>0.27?"green":"yellow"),
				cust_switchDial.currValue_n<0.09?"down":(cust_switchDial.currValue_n>0.27?"up":"auto"),
				cust_gateDial.currValue_n>0.82?"closed":"open",
				cust_rightSignalDial.currValue_n>0.91?"red":(cust_rightSignalDial.currValue_n<0.73?"green":"yellow"),
				cust_authTravelDirection.valueratio_r>0.5,
				cust_authSwitchDirection.valueratio_r>0.5,
				cust_authNumberOfBlocks.actualValue_r
			];
		t3databaseQml.kc_writePlcFromMetaInfo(blockId_s,metaInfo_A);
		console.log(metaInfo_A);
	}
	function db2view(){
		if(blockId_s==="") return;
		//handles operatables
		let metaInfo_A = t3databaseQml.kc_readPlcToMetaInfo(blockId_s);
		cust_maintainanceMode.valueratio_r = metaInfo_A[0]?1:0;
		cust_speedLimit.valueratio_r = metaInfo_A[1]/100;
		cust_suggestedSpeed.valueratio_r= metaInfo_A[2]/100;
		if(metaInfo_A[3]==="red") cust_leftSignalDial.dialDialValue_r = 0
		else if(metaInfo_A[3]==="yellow") cust_leftSignalDial.dialDialValue_r = 0.18
		else if(metaInfo_A[3]==="green") cust_leftSignalDial.dialDialValue_r = 0.36
		if(metaInfo_A[4]==="down") cust_switchDial.dialDialValue_r = 0
		else if(metaInfo_A[4]==="auto") cust_switchDial.dialDialValue_r = 0.18
		else if(metaInfo_A[4]==="up") cust_switchDial.dialDialValue_r = 0.36
		if(metaInfo_A[5]==="closed") cust_gateDial.dialDialValue_r = 1
		else if(metaInfo_A[5]==="auto") cust_gateDial.dialDialValue_r = 0.82
		else if(metaInfo_A[5]==="open") cust_gateDial.dialDialValue_r = 0.64
		if(metaInfo_A[6]==="red") cust_rightSignalDial.dialDialValue_r = 1
		else if(metaInfo_A[6]==="yellow") cust_rightSignalDial.dialDialValue_r = 0.82
		else if(metaInfo_A[6]==="green") cust_rightSignalDial.dialDialValue_r = 0.64
		//handles plc binaries
		reap_plcbinaries.model = t3databaseQml.kc_getAllPlcBinaries(blockId_s)
	}
	onBlockId_sChanged: {
		db2view();
	}
	implicitHeight: 500
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
	T3TrackSelectorBlock{
		id:cust_trackSelector
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
		id:item_middleScreen
		visible: true
		anchors{
			top:parent.top
			left:cust_trackSelector.right
			right:parent.horizontalCenter
			bottom: parent.bottom
		}
		color: "transparent"
		anchors.margins: T3Styling.margin_r

		Rectangle{
			id:rect_leftScreen
			visible: true
			anchors.fill: parent
			anchors{
				bottomMargin: T3Styling.margin_r+T3Styling.spacing_r*2
			}

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
							height: T3Styling.fontSubSub_r*1.5
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
						topMargin: T3Styling.lineWidth_r
					}
					height: T3Styling.lineWidth_r
					radius: height
					color: T3Styling.cFgSubSub_c
				}

				Grid{
					id:grid_plcbinariesGrid
					rows:4
					columns: 1
					anchors{
						left:parent.left
						right: parent.right
						top:rect_seperatorInputLabel.bottom
						bottom:parent.bottom
					}
					property real unitHeight_r:
						(height-(rows-1)*rowSpacing)/rows
					Repeater{
						id:reap_plcbinaries
						model:[bCTCPLCIO_bA,bKMPLCIO_bA,bKCPLCIN_bA,bBCNPLCOUT_bA]
						delegate:Item{
							id:item_plcbinariesIndividual
							width: grid_plcbinariesGrid.width
							height: grid_plcbinariesGrid.unitHeight_r
							Item{
								id:item_inputLabels
								width: grid_label.width
								height: T3Styling.fontSub_r
								T3Text{
									anchors.fill: parent
									textContent_s: {
										if(index===0) return "CTC IO"
										else if(index===1) return "KM IO"
										else if(index===2) return "KC IN"
										else "BCN OUT"
									}
									textAlign_s: "left"
									textColor_c: T3Styling.cFgSub_c
								}
								T3Text{
									anchors.fill: parent
									textContent_s:
										"0x"+parseInt([...modelData].map((it_b)=>it_b?"1":"0").join(""),2)
									.toString(16).padStart(8,"0").toUpperCase();
									textAlign_s: "right"
									textColor_c: T3Styling.cBlue_c
								}
							}
							Grid{
								id:grid_inputBinaries
								y:item_inputLabels.height
								rows:2
								columns:16
								width: grid_plcbinariesGrid.width
								height: grid_plcbinariesGrid.unitHeight_r*0.8
								columnSpacing: T3Styling.lineWidth_r
								rowSpacing: columnSpacing
								readonly property real unitWidth_r:
									(width-(columns-1)*columnSpacing)/columns
								Repeater{
									model:modelData
									delegate:	Rectangle{
										width: grid_inputBinaries.unitWidth_r
										height: width
										radius: width/4
										color: modelData?T3Styling.cBlue_c:T3Styling.cFgSubSub_c
										T3Text{
											anchors.fill: parent
											textContent_s: modelData?"T":"F"
											textColor_c: T3Styling.cBgMain_c
										}
									}
								}
							}
						}
					}
				}
			}
		}
		Grid{
			id:grid_menuButtons
			anchors{
				bottom: parent.bottom
				left:rect_leftScreen.left
				right:rect_leftScreen.right
			}
			height: T3Styling.margin_r
			spacing: T3Styling.spacing_r
			rows:1
			columns:2
			Repeater{
				model:["Load Local PLC File","Refresh PLC Outputs"]
				delegate:T3Button{
					buttonLabel_s: modelData
					width: (grid_menuButtons.width-(grid_menuButtons.spacing*grid_menuButtons.columns-1))
						   /grid_menuButtons.columns
					height: grid_menuButtons.height
					//pressedColor_c: /*index===1?T3Styling.cRed_c:*/T3Styling.cBgMain_c
					//releasedColor_c:/*index===1?Qt.darker(T3Styling.cRed_c):*/T3Styling.cFgSubSub_c
					//delayButton_b: index===1
					onButtonClicked: {
						if(index===0)
							fDia_fileDialog.open()
						else if(index===1){
							if(blockId_s==="") return;
							t3databaseQml.kc_processPlc(blockId_s);
						}
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
				id:cust_maintainanceMode
				height:T3Styling.fontMain_r*1
				maxValue_r: 1;
				minValue_r: 0
				fixedPoint_i: 2
				readOnly_b:false
				paramConfig_A:"F_T_Maintance Mode_"
				width: col_column.width
			}
			Row{
				width: col_column.width
				height:T3Styling.fontMain_r*1
				spacing: T3Styling.spacing_r
				T3ParamUnit{
					id:cust_speedLimit
					opacity:cust_maintainanceMode.valueratio_r>0.5?0.8:0.5
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
					opacity:cust_maintainanceMode.valueratio_r>0.5?0.8:0.5
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
				opacity:cust_maintainanceMode.valueratio_r>0.5?1:0.5
				Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
				height:T3Styling.fontMain_r*1
				maxValue_r: 100;
				minValue_r: 0
				fixedPoint_i: 2
				readOnly_b: !cust_maintainanceMode.valueratio_r>0.5
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
							opacity: cust_maintainanceMode.valueratio_r>0.5?1:0.5
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
									else if(cust_switchDial.currValue_n<=0)
										return "DOWN "
									else if(cust_switchDial.currValue_n<=0.18)
										return "AUTO "
									else if(cust_switchDial.currValue_n<=0.36)
										return "UP "
									else return "???"
								}else if(index===2){
									if(cust_gateDial.ticks_nA.length===0)
										return "N/A ";
									else if(cust_gateDial.currValue_n===1)
										return "CLOSED "
									else if(cust_gateDial.currValue_n===0.82)
										return "AUTO "
									else if(cust_gateDial.currValue_n===0.64)
										return "OPEN "
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
							opacity: cust_maintainanceMode.valueratio_r>0.5?1:0.5
							Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
							anchors{
								left: parent.left
								top:parent.top
								bottom:parent.bottom
								right:parent.right
							}
							textColor_c: {
								if(textContent_s==="RED "||textContent_s==="CLOSED ") return T3Styling.cRed_c
								else if(textContent_s==="YELLOW "||textContent_s==="DOWN "||textContent_s==="UP ") return T3Styling.cYellow_c
								else if(textContent_s==="GREEN "||textContent_s==="OPEN ") return T3Styling.cGreen_c
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
					enabled_b:cust_maintainanceMode.valueratio_r>0.5
				}
				T3NCDial{
					id:cust_switchDial
					height: parent.unitWidth_r
					width: parent.unitWidth_r
					ticks_nA:[0,0.18,0.36]
					enabled_b:cust_maintainanceMode.valueratio_r>0.5
				}
				T3NCDial{
					id:cust_gateDial
					height: parent.unitWidth_r
					width: parent.unitWidth_r
					ticks_nA: hasGate_b?[0.64,1]:[]
					enabled_b:cust_maintainanceMode.valueratio_r>0.5
				}
				T3NCDial{
					id:cust_rightSignalDial
					height: parent.unitWidth_r
					width: parent.unitWidth_r
					ticks_nA:direction_s.toLowerCase()!=="left"?[0.64,0.82,1]:[]
					enabled_b:cust_maintainanceMode.valueratio_r>0.5
				}
			}

			Item{
				width: parent.width
				height: T3Styling.spacing_r*1.5
			}

			Row{
				width: col_column.width
				height:T3Styling.fontMain_r*1
				spacing: T3Styling.spacing_r
				property real unitWidth_r: (width-spacing*3)/4
				T3ParamUnit{
					id:cust_authTravelDirection
					opacity:cust_maintainanceMode.valueratio_r>0.5?1:0.5
					Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
					height:T3Styling.fontMain_r*1
					readOnly_b: !cust_maintainanceMode.valueratio_r>0.5
					paramConfig_A:"F_T_Forward Auth_mph"
					width: parent.unitWidth_r
				}
				T3ParamUnit{
					id:cust_authSwitchDirection
					opacity:cust_maintainanceMode.valueratio_r>0.5?1:0.5
					Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
					height:T3Styling.fontMain_r*1
					readOnly_b: !cust_maintainanceMode.valueratio_r>0.5
					paramConfig_A:"F_T_Up Auth_"
					width: parent.unitWidth_r
				}
				T3ParamUnit{
					id:cust_authNumberOfBlocks
					opacity:cust_maintainanceMode.valueratio_r>0.5?1:0.5
					Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
					height:T3Styling.fontMain_r*1
					readOnly_b: !cust_maintainanceMode.valueratio_r>0.5
					maxValue_r: 100
					fixedPoint_i: 2
					isWhole_b: true
					paramConfig_A:"F_F_Number Of Blocks Authorized_"
					width: parent.unitWidth_r*2+parent.spacing
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
				view2db();
				db2view();
			}
		}
	}

}
