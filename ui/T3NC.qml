import QtQuick 2.12

Item {
	id:root
	implicitWidth: 1150
	implicitHeight: 350

	property bool engineerMode_b:false
	onEngineerMode_bChanged: db2view(true);
	property bool autoMode_b:true
	onAutoMode_bChanged: db2view(true);
	property bool stationMode_b:text_stationInfo.textContent_s!==""
	property string trainId_s:cust_trainSelector.currValue_s
	onTrainId_sChanged:{
		rect_frontHelper.runAnimation();
		db2view(true);
	}
	Connections{
		target: t3databaseQml
		function onOnTrainObjectsChanged(){
			db2view(false);
		}
	}

	function db2view(includeIO_b){
		if(trainId_s===""||!reap_paramsToggle||!reap_gain||!reap_gainValue) return;
		let metaInfo_A = t3databaseQml.nc_getMetaInfo(trainId_s);
		cGau_power.currValue_n = metaInfo_A[18]
		cGau_velocity.currValue_n = metaInfo_A[7]
		cGau_accel.currValue_n = metaInfo_A[6]//set point
		if(engineerMode_b){
			reap_gain.itemAt(0).value_r =  metaInfo_A[11]
			reap_gainValue.itemAt(0).value_r =  metaInfo_A[11]
			reap_gain.itemAt(1).value_r =  metaInfo_A[12]
			reap_gainValue.itemAt(1).value_r =  metaInfo_A[12]
			if(includeIO_b){
				cust_kpToggler.valueratio_r = metaInfo_A[11]
				cust_kiToggler.valueratio_r = metaInfo_A[12]
			}
			pid_pid.r_r = metaInfo_A[6]
			pid_pid.y_r = metaInfo_A[7]
			pid_pid.e_r = metaInfo_A[8]
			pid_pid.p_r = metaInfo_A[9]
			pid_pid.i_r = metaInfo_A[10]
			pid_pid.u_r = metaInfo_A[18]
		}else{
			if(includeIO_b){
				cust_kpToggler.valueratio_r = metaInfo_A[0]?1:0
				cust_kiToggler.valueratio_r = metaInfo_A[5]?1:0
			}
		}
		if(includeIO_b){
			reap_paramsToggle.itemAt(0).valueratio_r= metaInfo_A[1]?1:0;
			reap_paramsToggle.itemAt(1).valueratio_r= metaInfo_A[2]?1:0;
			reap_paramsToggle.itemAt(2).valueratio_r= metaInfo_A[3]?1:0;
			reap_paramsToggle.itemAt(3).valueratio_r= metaInfo_A[4]?1:0;
			cust_speedSetpoint.valueratio_r = metaInfo_A[6]/cust_speedSetpoint.maxValue_r;
		}
		//station info
		text_stationInfo.textContent_s = metaInfo_A[17];
	}
	function view2db(){
		if(trainId_s==="") return;
		if(engineerMode_b){
			let kp_r = cust_kpToggler.actualValue_r ;
			let ki_r = cust_kiToggler.actualValue_r;
			t3databaseQml.nc_setKpAndKi(trainId_s,kp_r,ki_r);
		}else if(!autoMode_b){
			if(!reap_paramsToggle) return;
			let metaInfo_A = [
					cust_kpToggler.valueratio_r>0.5,
					cust_kiToggler.valueratio_r>0.5,
					reap_paramsToggle.itemAt(0).valueratio_r>0.5,
					reap_paramsToggle.itemAt(1).valueratio_r>0.5,
					reap_paramsToggle.itemAt(2).valueratio_r>0.5,
					reap_paramsToggle.itemAt(3).valueratio_r>0.5,
					cust_speedSetpoint.actualValue_r
				];
			console.log(metaInfo_A)
			t3databaseQml.nc_setCtrlParams(trainId_s,metaInfo_A);
		}
	}
	Rectangle{
		anchors.fill: parent
		color: T3Styling.cBgSub_c
		//radius: T3Styling.margin_r
		T3Text{
			anchors.bottom: parent.bottom
			anchors.right: parent.right
			anchors.rightMargin: T3Styling.spacing_r
			textPixelSize_r: T3Styling.fontSubSub_r
			height: T3Styling.margin_r
			width: parent.width
			textContent_s: "T3 Train Controller | Aidan"
			textAlign_s: "middle"
			textBold_b: true
		}
	}
	Item{
		id:item_canvas
		anchors.fill: parent
		anchors.margins: T3Styling.margin_r
		T3TrackSelectorBlock{
			id:cust_trainSelector
			isTrack_b:false
			model_A: t3databaseQml?t3databaseQml.trainIds_QML:[[""]]
			anchors{
				top:parent.top
				//top:sDis_segDisplay.bottom
				//topMargin: T3Styling.margin_r
				left:parent.left
				bottom:parent.bottom
				//margins: T3Styling.margin_r
			}
			width: root.width*0.05
		}

		Rectangle{
			id:rect_leftScreen
			anchors{
				top:parent.top
				//top:sDis_segDisplay.bottom
				//topMargin: T3Styling.margin_r
				left:cust_trainSelector.right
				leftMargin: T3Styling.margin_r
				//				bottom:item_canvas.bottom
				//				bottomMargin: T3Styling.margin_r*3+T3Styling.spacing_r*2
			}
			height: T3Styling.margin_r*2+colu_pidLabel00.height
					+pid_pid.height+text_pidLabel1.height
			//			+T3Styling.spacing_r*4
			width:item_canvas.width*0.5
			radius: T3Styling.spacing_r
			color: T3Styling.cBgMain_c
			border.width: T3Styling.lineWidth_r
			border.color: T3Styling.cFgSubSub_c

			Grid{
				rows:1
				columns: 3
				id:item_leftScreenNonEngineer
				anchors{
					left:parent.left
					right:parent.right
					top: item_stationInfo.bottom
					bottom:parent.bottom
				}
				Behavior on height{PropertyAnimation{easing.type: Easing.OutCirc}}
				anchors.margins: T3Styling.margin_r
				opacity: !engineerMode_b?1:0
				Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
				property real unitWidth_r:(width-2*T3Styling.spacing_r)/3
				//				T3NMCircularGauge{
				//				}
				Item{
					height: parent.height
					width: parent.unitWidth_r
					T3NMCircularGauge{
						id:cGau_power
						y:-T3Styling.spacing_r
						width:parent.width
						height: parent.height
						currValue_n: 0
						fixedPoint_i: 0
						stepsize_r: 0.1
						tickmarkStepsize_r: 15
						gaugeUnit_s: "kW"
						maxValue_n:150
						textOnly_b: false
						toggle_b: false
					}
					T3Text{
						id:text_powerLabel
						width:parent.width
						anchors.bottom: cGau_power.bottom
						anchors.bottomMargin: -0.9*T3Styling.fontSub_r
						textPixelSize_r: T3Styling.fontSubSub_r
						textContent_s: "Power"
						textColor_c: T3Styling.cFgMain_c
					}
				}
				Item{
					height: parent.height
					width: parent.unitWidth_r
					T3NMCircularGauge{
						id:cGau_velocity
						y:-T3Styling.spacing_r
						width:parent.width
						height: parent.height
						stepsize_r: 0.1
						fixedPoint_i: 0
						currValue_n: 0
						tickmarkStepsize_r: 5
						maxValue_n: 50
						textOnly_b: true
						toggle_b: false
					}
					T3Text{
						id:text_velocityLabel
						width:parent.width
						anchors.bottom: cGau_velocity.bottom
						anchors.bottomMargin: -0.9*T3Styling.fontSub_r
						textPixelSize_r: T3Styling.fontSubSub_r
						textContent_s: "Velocity"
						textColor_c: T3Styling.cFgMain_c
					}
				}
				Item{
					height: parent.height
					width: parent.unitWidth_r
					T3NMCircularGauge{
						id:cGau_accel//but setpoint actually
						y:-T3Styling.spacing_r
						width:parent.width
						height: parent.height
						stepsize_r: 0.1
						fixedPoint_i: 0
						currValue_n: 0
						tickmarkStepsize_r: 5
						maxValue_n: 50
						textOnly_b: true
						toggle_b: false
					}
					T3Text{
						id:text_accelLabel
						width:parent.width
						anchors.bottom: cGau_accel.bottom
						anchors.bottomMargin: -0.9*T3Styling.fontSub_r
						textPixelSize_r: T3Styling.fontSubSub_r
						textContent_s: "Setpoint"
						textColor_c: T3Styling.cFgMain_c
					}
				}

			}
			Item{
				id:item_leftScreenCanvas
				anchors.fill: parent
				anchors.margins: T3Styling.margin_r
				opacity: engineerMode_b?1:0
				Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
				Column{
					id:colu_pidLabel00
					anchors{
						top:parent.top
						left: parent.left
						right:parent.horizontalCenter
						rightMargin:T3Styling.margin_r/2
					}
					spacing: T3Styling.lineWidth_r
					height: T3Styling.fontSub_r*2+spacing*1
					Repeater{
						id:reap_gain
						model:
							[["P - Propotional Gain",T3Styling.cGreen_c]
							,["I - Integral Gain",T3Styling.cBlue_c]]
						delegate:
							Rectangle{
							property real value_r:1.4
							width: colu_pidLabel00.width
							height: T3Styling.fontSub_r
							color: "transparent"
							T3Text{
								anchors.fill: parent
								textAlign_s: "left"
								textColor_c: modelData[1]
								textContent_s: modelData[0].slice(0,1)
							}
							T3Text{
								anchors.fill: parent
								textAlign_s: "left"
								textColor_c: T3Styling.cFgSub_c
								textContent_s:"   "+modelData[0].slice(1)
							}
							T3Text{
								anchors.fill: parent
								textAlign_s: "right"
								textColor_c: parent.value_r===0?T3Styling.cRed_c:T3Styling.cGreen_c
								textContent_s:parent.value_r===0?"OFF":"ON"
							}
						}
					}
				}
				Column{
					id:colu_pidLabel01
					anchors{
						top:parent.top
						left: parent.horizontalCenter
						right:parent.right
						leftMargin:T3Styling.margin_r/2
					}
					spacing: T3Styling.lineWidth_r
					height: T3Styling.fontSub_r*2+spacing*1
					Repeater{
						id:reap_gainValue

						model:
							["Propotional Coefficient"
							,"Integral Coefficient"]
						delegate:
							Rectangle{
							property real value_r:1.2
							width: colu_pidLabel00.width
							height: T3Styling.fontSub_r
							color: "transparent"
							T3Text{
								anchors.fill: parent
								textAlign_s: "left"
								textColor_c: T3Styling.cFgSub_c
								textContent_s:modelData
							}
							T3Text{
								anchors.fill: parent
								textAlign_s: "right"
								textColor_c: parent.value_r===0?T3Styling.cFgSub_c:T3Styling.cFgMain_c
								textContent_s:  parent.value_r===0?"N/A": parent.value_r.toFixed(2)
							}
						}

					}

				}
				T3NCPid{
					id:pid_pid
					anchors{
						top:colu_pidLabel00.bottom
						topMargin: T3Styling.spacing_r*2
						left: parent.left
						right:parent.right
					}
				}
				T3Text{
					id:text_pidLabel1
					anchors{
						top:pid_pid.bottom
						topMargin: T3Styling.spacing_r
						left: parent.left
						right:parent.right
					}
					textColor_c: T3Styling.cFgSub_c
					textContent_s: "R - Setpoint | Y - Current Velocity | E - Error Value | U - Output Power"
				}

				//				T3NMCircularGauge{
				//				}
			}
			Item{
				id:item_stationInfo
				width: parent.width
				height: sAnim_animationStation.running?T3Styling.fontSub_r:0
				Behavior on height{PropertyAnimation{easing.type: Easing.OutCirc}}
				opacity: stationMode_b&&!engineerMode_b?1:0
				Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
				T3Text{
					id:text_stationInfo
					anchors.fill: parent
					anchors.bottomMargin: -T3Styling.fontSubSub_r
					anchors.topMargin: T3Styling.fontSubSub_r
					visible: item_stationInfo.height===T3Styling.fontSub_r
					textContent_s: "APPROACHING STATION: "
					textColor_c: T3Styling.cBgMain_c
				}
				SequentialAnimation{
					id:sAnim_animationStation
					running:stationMode_b&&!engineerMode_b
					loops: Animation.Infinite
					alwaysRunToEnd: false
					PropertyAnimation{
						alwaysRunToEnd: true
						target: text_stationInfo
						property: "textColor_c"
						from: T3Styling.cBgMain_c
						to:T3Styling.cFgMain_c
						duration: 1500
					}
					PropertyAnimation{
						alwaysRunToEnd: true
						target: text_stationInfo
						property: "textColor_c"
						from: T3Styling.cFgMain_c
						to:T3Styling.cBgMain_c
						duration: 1500
					}
				}
			}

		}

		Column{
			anchors{
				topMargin: T3Styling.spacing_r
				top:rect_leftScreen.bottom
				left:rect_leftScreen.left
				right:rect_leftScreen.right
				bottom: parent.bottom
			}
			spacing: T3Styling.spacing_r
			property real unitHeight_r: (height-T3Styling.spacing_r)/2
			T3ParamUnit{
				id:cust_kpToggler
				paramConfig_A: engineerMode_b?"F_F_Kp_":"F_T_Service Brake_";
				isWhole_b: !engineerMode_b
				readOnly_b: !(engineerMode_b||!autoMode_b)
				maxValue_r: engineerMode_b?1:100
				fixedPoint_i: 3
				height: parent.unitHeight_r
				width: parent.width
				opacity: readOnly_b?0.5:1
				Behavior on opacity{PropertyAnimation{ }}

			}
			T3ParamUnit{
				id:cust_kiToggler
				paramConfig_A: engineerMode_b?"F_F_Ki_":"F_T_Emergency Brake_";
				readOnly_b: !(engineerMode_b||!autoMode_b)
				maxValue_r: engineerMode_b?1:1
				fixedPoint_i: 3
				height: parent.unitHeight_r
				width: parent.width
				opacity: readOnly_b?0.5:1
				Behavior on opacity{PropertyAnimation{ }}
			}

		}
		property int totalRow_n:8
		Grid{
			id:grid_buttons
			anchors{
				top:parent.top
				left:rect_leftScreen.right
				leftMargin: T3Styling.margin_r
				right:parent.right
				bottom:grid_buttons2.top
				bottomMargin: T3Styling.margin_r
				//				bottom:parent.bottom
			}
			spacing:T3Styling.spacing_r
			rows: 3
			columns:2
			readonly property real unitWidth_r: (width-spacing*(columns-1))/columns
			readonly property real unitHeight_r:(height-spacing*(rows-1))/rows
			//height: unitHeight_r*rows+spacing*(rows-1)//Exterior Lights On
			Repeater{
				id:reap_paramsToggle
				model:["F_T_Left Doors Opened_"
					,"F_T_Right Doors Opened_"
					,"F_T_Exterior Lights_"
					,"F_T_Interior Lights_"
				]
				delegate:T3ParamUnit{
					width: parent.unitWidth_r
					height:parent.unitHeight_r
					paramConfig_A: modelData
					opacity: readOnly_b?0.5:1
					Behavior on opacity{PropertyAnimation{ }}
					readOnly_b: autoMode_b||engineerMode_b
				}			/*T3Button{
					property bool currState_b: false
					width: parent.unitWidth_r
					height:parent.unitHeight_r
					buttonLabel_s:{
						if(!currState_b) return modelData.replace("ONOFF","ON").replace("OPENCLOSE","OPEN")
						else return modelData.replace("ONOFF","OFF").replace("OPENCLOSE","CLOSE")
					}
					releasedColor_c: !currState_b?T3Styling.cFgSubSub_c:pressedColor_c
					onButtonClicked: {
						currState_b = !currState_b
					}
				}*/
			}
		}
		T3ParamUnit{
			id:cust_speedSetpoint
			anchors.bottom: grid_buttons.bottom
			anchors.left: grid_buttons.left
			anchors.right: grid_buttons.right
			height:grid_buttons.unitHeight_r
			maxValue_r: 50
			paramConfig_A: "F_F_Speed Setpoint_Mph"
			opacity: readOnly_b?0.5:1
			Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
			readOnly_b:autoMode_b||engineerMode_b
		}


		Grid{
			id:grid_buttons2
			anchors{
				//top:grid_buttons.bottom
				left:rect_leftScreen.right
				leftMargin: T3Styling.margin_r
				//topMargin: T3Styling.spacing_r
				right:parent.right
				bottom:parent.bottom
			}
			spacing:T3Styling.spacing_r
			rows: 1
			columns:reap_buttonBottoms.model.length
			property real unitWidth_r: (width-spacing*(columns-1))/columns
			Behavior on unitWidth_r{PropertyAnimation{easing.type: Easing.OutCirc}}
			readonly property real unitHeight_r: (parent.height-spacing*(parent.totalRow_n-1))/parent.totalRow_n
			height: unitHeight_r*rows+spacing*(rows-1)
			Repeater{
				id:reap_buttonBottoms
				model:{
					(engineerMode_b||!autoMode_b)?["Engr Mode","Auto Mode","Announce","Apply"
												   ]:["Engr Mode","Auto Mode"]
				}
				delegate:			T3Button{
					width: parent.unitWidth_r
					height:parent.unitHeight_r
					buttonLabel_s: modelData.toUpperCase()
					releasedColor_c: {
						if(index===0) return engineerMode_b?T3Styling.cBgMain_c:Qt.darker(T3Styling.cBlue_c)
						if(index===1) return autoMode_b?T3Styling.cBgMain_c:Qt.darker(T3Styling.cBlue_c)
						if(index===3) return Qt.darker(T3Styling.cGreen_c)
						else return T3Styling.cFgSubSub_c
					}
					onButtonClicked: {
						if(index===0&&autoMode_b){
							root.engineerMode_b = !root.engineerMode_b ;
						}
						else if(index===1){
							if(engineerMode_b) return;
							root.autoMode_b = !root.autoMode_b;
						}
						else if(index===2) stationMode_b = !stationMode_b // test
						else if(index===3) view2db();
					}
				}
			}
		}


		Rectangle{
			id:rect_frontHelper
			anchors{
				left:cust_trainSelector.right
				leftMargin: T3Styling.margin_r
				top:parent.top
				bottom: parent.bottom
				right: parent.right
			}
			color:T3Styling.cBgSub_c
			property bool initState_b:true
			opacity: 1
			border.width: T3Styling.lineWidth_r
			border.color: T3Styling.cFgSubSub_c
			radius: T3Styling.lineWidth_r
			T3Text{
				textContent_s: rect_frontHelper.initState_b?"T3 | TRAIN CONTROLLER\n\n\n":""
				textColor_c:T3Styling.cFgMain_c
				anchors.fill: parent
				textPixelSize_r: T3Styling.fontSub_r
				textBold_b: true
				textLetterSpacing_r: T3Styling.fontSubSub_r*0.5
			}
			T3Text{
				id:text_animationLabel
				textContent_s: rect_frontHelper.initState_b?
								   "\n\n\n\n\n\n\n\n\nSelect a train on the left to start":
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
					duration:target.initState_b?0:300
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
					duration:300
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

}
