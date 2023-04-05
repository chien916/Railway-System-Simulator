import QtQuick 2.12
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4
Item {
	id:root
	width: 1200
	height: 500

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
		if(trainId_s===""||!reap_trainConstantsDisplay||!reap_trainConstantsDisplay2) return;
		let metaInfo_A = t3databaseQml.nm_getStringsFromMetaInfo(trainId_s);
		text_leftDoorStatus.textContent_s = metaInfo_A[0];
		text_rightDoorStatus.textContent_s = metaInfo_A[1];
		text_serviceBrakeStatus.textContent_s = metaInfo_A[2];
		text_emergencyBrakeStatus.textContent_s = metaInfo_A[3];
		cust_speedLimitAndSignal.currSpeedLimit_i = metaInfo_A[5];
		cust_speedLimitAndSignal.currSignal_s = metaInfo_A[6];
		cGau_power.currValue_n = metaInfo_A[7];
		cGau_velocity.currValue_n = metaInfo_A[8];
		cGau_acceleration.currValue_n = metaInfo_A[9];
		reap_trainConstantsDisplay.itemAt(0).val_s = metaInfo_A[10];
		reap_trainConstantsDisplay.itemAt(1).val_s = metaInfo_A[11];
		reap_trainConstantsDisplay.itemAt(2).val_s = metaInfo_A[12];
		reap_trainConstantsDisplay.itemAt(3).val_s = metaInfo_A[13];
		reap_trainConstantsDisplay.itemAt(4).val_s = metaInfo_A[14];
		reap_trainConstantsDisplay2.itemAt(0).val_s = metaInfo_A[16];
		reap_trainConstantsDisplay2.itemAt(1).val_s = metaInfo_A[17];
		reap_trainConstantsDisplay2.itemAt(2).val_s = metaInfo_A[18];
		reap_trainConstantsDisplay2.itemAt(3).val_s = metaInfo_A[20];
		reap_trainConstantsDisplay2.itemAt(4).val_s = metaInfo_A[22];
		text_trainTemperature.textContent_s = metaInfo_A[24];
		gaug_trainTemperature.value = metaInfo_A[25];
		text_crewCount.textContent_s = metaInfo_A[26];
		text_passangerCount.textContent_s = metaInfo_A[27];
		text_authorityGiven.textContent_s = metaInfo_A[28];
		text_authorityCount.textContent_s = metaInfo_A[29];
		text_stationInfo.textContent_s = metaInfo_A[30];
		if(includeIO_b){
			butt_brokenRail.currState_b = metaInfo_A[19];
			butt_trackCircuit.currState_b = metaInfo_A[21];
			butt_emergencyBrake.currState_b = metaInfo_A[4];
			butt_powerFailure.currState_b = metaInfo_A[23];
			cust_trainTemperature.valueratio_r = metaInfo_A[25]/cust_trainTemperature.maxValue_r;
		}
	}
	Rectangle{
		id:rect_canvas
		color: T3Styling.cBgSub_c
		anchors.fill: root
		//radius: T3Styling.margin_r
		T3Text{
			anchors.bottom: parent.bottom
			anchors.right: parent.right
			anchors.rightMargin: T3Styling.margin_r
			textPixelSize_r: T3Styling.fontSubSub_r
			height: T3Styling.margin_r
			width: parent.width
			textContent_s: "T3 Train Model | Jared Kinneer"
			textAlign_s: "right"
			textBold_b: true
		}
	}
	T3TrackSelectorBlock{
		id:cust_trainSelector
		anchors{
			top:parent.top
			//top:sDis_segDisplay.bottom
			//topMargin: T3Styling.margin_r
			left:parent.left
			bottom:parent.bottom
			margins: T3Styling.margin_r
		}
		isTrack_b: false
		model_A: t3databaseQml?t3databaseQml.trainIds_QML:[[""]]
		width: root.width*0.05
	}



	Rectangle{
		id:rect_leftScreen
		anchors{
			left:cust_trainSelector.right
			top:parent.top
			bottom:cust_trainTemperature.top
			leftMargin: T3Styling.margin_r
			rightMargin: T3Styling.margin_r
			topMargin:T3Styling.margin_r
			bottomMargin:T3Styling.spacing_r
		}
		width: root.width*0.35
		border.width: T3Styling.lineWidth_r
		border.color: T3Styling.cFgSubSub_c
		radius: T3Styling.spacing_r
		color:T3Styling.cBgMain_c
		Column{
			id:colu_leftdoorInfo
			spacing:T3Styling.spacing_r*1.5
			anchors{
				left:cVie_carView.left
				right: parent.horizontalCenter
				rightMargin: T3Styling.spacing_r*2
				//top:rect_seperator1.bottom
				//topMargin: T3Styling.spacing_r*2
				top: parent.top
				topMargin: T3Styling.margin_r
			}
			Rectangle{
				height: T3Styling.fontSubSub_r
				width: parent.width
				color:"transparent"
				T3Text{
					width:parent.width
					height:T3Styling.fontSubSub_r
					textPixelSize_r: T3Styling.fontSubSub_r
					textContent_s:"Left Door"
					textColor_c: T3Styling.cFgSub_c
					textAlign_s: "left"
				}
				T3Text{
					id: 	text_rightDoorStatus
					width:parent.width
					height:T3Styling.fontSubSub_r
					textPixelSize_r: T3Styling.fontSubSub_r
					textContent_s: "N/A"
					textColor_c:{
						if(textContent_s==="OPEN") return T3Styling.cYellow_c
						else return  T3Styling.cGreen_c
					}
					textAlign_s: "right"
				}
			}

		}
		Column{
			id:colu_rightdoorInfo
			spacing:T3Styling.spacing_r*1.5
			anchors{
				right:cVie_carView.right
				left: parent.horizontalCenter
				leftMargin: T3Styling.spacing_r*2
				//top:rect_seperator1.bottom
				//topMargin: T3Styling.spacing_r*2
				top: parent.top
				topMargin: T3Styling.margin_r
			}
			Rectangle{
				height: T3Styling.fontSubSub_r
				width: parent.width
				color:"transparent"
				T3Text{
					width:parent.width
					height:T3Styling.fontSubSub_r
					textPixelSize_r: T3Styling.fontSubSub_r
					textContent_s: "Right Door"
					textColor_c: T3Styling.cFgSub_c
					textAlign_s: "left"
				}
				T3Text{
					id:text_leftDoorStatus
					width:parent.width
					height:T3Styling.fontSubSub_r
					textPixelSize_r: T3Styling.fontSubSub_r
					textContent_s: "N/A"
					textColor_c:{
						if(textContent_s==="OPEN") return T3Styling.cYellow_c
						else return  T3Styling.cGreen_c
					}
					textAlign_s: "right"
				}
			}

		}
		T3NMCarView{
			id:cVie_carView
			anchors{
				top:colu_leftdoorInfo.bottom
				topMargin: T3Styling.spacing_r*1.5
				left:rect_leftScreen.left
				right:rect_leftScreen.right
				leftMargin: T3Styling.margin_r
				rightMargin: T3Styling.margin_r
				bottom: colu_serviceBrakeInfo.top
				bottomMargin:  T3Styling.spacing_r*1.5
			}
			leftDoorClosed_b: text_rightDoorStatus.textContent_s === "CLOSED"
			rightDoorClosed_b: text_leftDoorStatus.textContent_s === "CLOSED"
			brakeReleased_b: text_serviceBrakeStatus.textContent_s === "RELEASED"
			brakeEmergency_b: text_emergencyBrakeStatus.textContent_s==="APPLIED"
			//height: rect_leftScreen.height*0.35
		}

		Column{
			id:colu_serviceBrakeInfo
			spacing:T3Styling.spacing_r*1.5
			anchors{
				left:cVie_carView.left
				right: parent.horizontalCenter
				rightMargin: T3Styling.spacing_r*2
				//top: cVie_carView.bottom
				//topMargin: T3Styling.spacing_r*1.5
				bottom:  rect_seperator2.top
				bottomMargin: T3Styling.spacing_r*2
			}
			Rectangle{
				height: T3Styling.fontSubSub_r
				width: parent.width
				color:"transparent"
				T3Text{
					width:parent.width
					height:T3Styling.fontSubSub_r
					textPixelSize_r: T3Styling.fontSubSub_r
					textContent_s: "Service Brake"
					textColor_c: T3Styling.cFgSub_c
					textAlign_s: "left"
				}
				T3Text{
					id:text_serviceBrakeStatus
					width:parent.width
					height:T3Styling.fontSubSub_r
					textPixelSize_r: T3Styling.fontSubSub_r
					textContent_s: "N/A"
					textColor_c:{
						if(textContent_s==="APPLIED") T3Styling.cYellow_c
						else return T3Styling.cGreen_c
					}
					textAlign_s: "right"
				}
			}

		}
		Column{
			id:colu_emergencyBrakeInfo
			spacing:T3Styling.spacing_r*1.5
			anchors{
				right:cVie_carView.right
				left: parent.horizontalCenter
				leftMargin: T3Styling.spacing_r*2
				//top:rect_seperator1.bottom
				//topMargin: T3Styling.spacing_r*2
				//top: cVie_carView.bottom
				//topMargin: T3Styling.spacing_r*1.5
				bottom:  rect_seperator2.top
				bottomMargin: T3Styling.spacing_r*2
			}

			Rectangle{
				height: T3Styling.fontSubSub_r
				width: parent.width
				color:"transparent"
				T3Text{
					width:parent.width
					height:T3Styling.fontSubSub_r
					textPixelSize_r: T3Styling.fontSubSub_r
					textContent_s: "Emergency Brake"
					textColor_c: T3Styling.cFgSub_c
					textAlign_s: "left"
				}
				T3Text{
					id:text_emergencyBrakeStatus
					width:parent.width
					height:T3Styling.fontSubSub_r
					textPixelSize_r: T3Styling.fontSubSub_r
					textContent_s: "N/A"
					textColor_c:{
						if(textContent_s=="APPLIED") T3Styling.cRed_c
						else return T3Styling.cGreen_c
					}
					textAlign_s: "right"
				}
			}

		}
		Rectangle{
			id:rect_seperator2
			anchors{
				left:cVie_carView.left
				right: cVie_carView.right
				bottom:rect_trainTemperatureContainer.top
				bottomMargin: T3Styling.spacing_r*1.5
			}
			visible: true
			height: T3Styling.lineWidth_r
			radius: height
			color: T3Styling.cFgSubSub_c
		}
		Gauge {
			id:gaug_trainTemperature
			anchors{
				bottom:rect_passangerCount.top
				bottomMargin: T3Styling.spacing_r*4
				left:rect_seperator2.left
				right:rect_seperator2.right
			}
			visible: true
			minimumValue: 0
			value: 50
			maximumValue: 100
			style: GaugeStyle {
				valueBar: Rectangle {
					implicitWidth: 16
					color: T3Styling.cGreen_c
					radius: T3Styling.spacing_r
				}
				foreground:Item{}
				background:
					Rectangle {
					implicitWidth: 16
					color: T3Styling.cBgSubSub_c
					radius: T3Styling.spacing_r
				}
			}
			orientation:Qt.Horizontal
		}

		Rectangle{
			id:rect_trainTemperatureContainer
			height: T3Styling.fontSubSub_r
			anchors{
				bottom:gaug_trainTemperature.top
				bottomMargin: T3Styling.spacing_r*1.5
				left:rect_seperator2.left
				right:rect_seperator2.right
			}
			color:"transparent"
			T3Text{
				width:parent.width
				height:T3Styling.fontSubSub_r
				textPixelSize_r: T3Styling.fontSubSub_r
				textContent_s: "Internal Train Temperature"
				textColor_c: T3Styling.cFgSub_c
				textAlign_s: "left"
			}
			T3Text{
				id:text_trainTemperature
				width:parent.width
				height:T3Styling.fontSubSub_r
				textPixelSize_r: T3Styling.fontSubSub_r
				textContent_s: "N/A"
				textColor_c:{
					if(textContent_s=="APPLIED") T3Styling.cRed_c
					else return T3Styling.cGreen_c
				}
				textAlign_s: "right"
			}
		}
		Rectangle{
			id:rect_seperator3
			anchors{
				left:cVie_carView.left
				right: cVie_carView.right
				bottom:rect_passangerCount.top
				bottomMargin: T3Styling.spacing_r*1.5
			}
			visible: true
			height: T3Styling.lineWidth_r
			radius: height
			color: T3Styling.cFgSubSub_c
		}
		Rectangle{
			height: T3Styling.fontSubSub_r
			anchors{
				bottom: rect_seperator4.top
				bottomMargin: T3Styling.spacing_r*1.5
				left:rect_seperator2.left
				right:parent.horizontalCenter
				rightMargin: T3Styling.spacing_r*2
			}
			color:"transparent"
			T3Text{
				width:parent.width
				height:T3Styling.fontSubSub_r
				textPixelSize_r: T3Styling.fontSubSub_r
				textContent_s: "Crew Count"
				textColor_c: T3Styling.cFgSub_c
				textAlign_s: "left"
			}
			T3Text{
				id:text_crewCount
				width:parent.width
				height:T3Styling.fontSubSub_r
				textPixelSize_r: T3Styling.fontSubSub_r
				textContent_s: "N/A"
				textColor_c:{
					 T3Styling.cFgMain_c
				}
				textAlign_s: "right"
			}
		}
		Rectangle{
			id:rect_passangerCount
			height: T3Styling.fontSubSub_r
			anchors{
				bottom: rect_seperator4.top
				bottomMargin: T3Styling.spacing_r*1.5
				right:rect_seperator2.right
				left:parent.horizontalCenter
				leftMargin: T3Styling.spacing_r*2
			}
			color:"transparent"
			T3Text{
				width:parent.width
				height:T3Styling.fontSubSub_r
				textPixelSize_r: T3Styling.fontSubSub_r
				textContent_s: "Passanger Count"
				textColor_c: T3Styling.cFgSub_c
				textAlign_s: "left"
			}
			T3Text{
				id:text_passangerCount
				width:parent.width
				height:T3Styling.fontSubSub_r
				textPixelSize_r: T3Styling.fontSubSub_r
				textContent_s: "N/A"
				textColor_c:{
					 T3Styling.cFgMain_c
				}
				textAlign_s: "right"
			}
		}
		Rectangle{
			id:rect_seperator4
			anchors{
				left:cVie_carView.left
				right: cVie_carView.right
				bottom: text_authorityGranted.top
				bottomMargin: T3Styling.spacing_r*1.5
			}
			visible: true
			height: T3Styling.lineWidth_r
			radius: height
			color: T3Styling.cFgSubSub_c
		}
		Rectangle{
			id:text_authorityGranted
			height: T3Styling.fontSubSub_r
			anchors{
				bottom: parent.bottom
				bottomMargin: T3Styling.margin_r
				left:rect_seperator2.left
				right:parent.horizontalCenter
				rightMargin: T3Styling.spacing_r*2
			}
			color:"transparent"
			T3Text{
				width:parent.width
				height:T3Styling.fontSubSub_r
				textPixelSize_r: T3Styling.fontSubSub_r
				textContent_s: "Authority Granted"
				textColor_c: T3Styling.cFgSub_c
				textAlign_s: "left"
			}
			T3Text{
				id:text_authorityGiven
				width:parent.width
				height:T3Styling.fontSubSub_r
				textPixelSize_r: T3Styling.fontSubSub_r
				textContent_s: "N/A"
				textColor_c:{
					if(textContent_s=="NO") T3Styling.cRed_c
					else return  T3Styling.cFgMain_c
				}
				textAlign_s: "right"
			}
		}
		Rectangle{
			id:text_authorityBlockNumbers
			height: T3Styling.fontSubSub_r
			anchors{
				bottom: parent.bottom
				bottomMargin: T3Styling.margin_r
				right:rect_seperator2.right
				left:parent.horizontalCenter
				leftMargin: T3Styling.spacing_r*2
			}
			color:"transparent"
			T3Text{
				width:parent.width
				height:T3Styling.fontSubSub_r
				textPixelSize_r: T3Styling.fontSubSub_r
				textContent_s: "Authorized Blocks"
				textColor_c: T3Styling.cFgSub_c
				textAlign_s: "left"
			}
			T3Text{
				id:text_authorityCount
				width:parent.width
				height:T3Styling.fontSubSub_r
				textPixelSize_r: T3Styling.fontSubSub_r
				textContent_s: "N/A"
				textColor_c:{
					if(textContent_s=="0 BLOCKS") T3Styling.cRed_c
					else return T3Styling.cFgMain_c
				}
				textAlign_s: "right"
			}
		}



	}
	T3NMSpeedLimitAndSignal{
		id:cust_speedLimitAndSignal
		currSignal_s:""
		//currSpeedLimit_i:
		anchors{
			//top:sDis_segDisplay.bottom
			top:parent.top
			topMargin: T3Styling.margin_r
			left: rect_leftScreen.right
			leftMargin: T3Styling.spacing_r
			right:rect_rightScreen.left
			rightMargin:T3Styling.spacing_r
			bottom: grid_buttons.top
			bottomMargin: T3Styling.spacing_r
		}
	}
	Rectangle{
		id:sDis_segDisplay
		anchors{
			left:rect_rightScreen.left
			right:rect_rightScreen.right
			//			right:root.right
			top:root.top
			topMargin: T3Styling.margin_r
//			margins: T3Styling.margin_r
		}
		height: (cust_speedLimitAndSignal.height-T3Styling.spacing_r*9)/10*2+T3Styling.spacing_r
		color: T3Styling.cBgMain_c
		radius: T3Styling.spacing_r
		border.color: T3Styling.cFgSubSub_c
		border.width: T3Styling.lineWidth_r
		PropertyAnimation{
			target:rect_moveVisualizer
			property:"pos_r"
			from:0.0
			to:1
			running:cGau_velocity.currValue_n>0
			loops:Animation.Infinite
			duration: Math.max(4000-Math.pow((cGau_velocity.currValue_n),2),100)
			easing.type: Easing.InOutCubic
		}
		T3Text{
			id:text_stationInfo
			anchors.fill: parent
			//anchors.bottomMargin: -T3Styling.fontSubSub_r
			//anchors.topMargin: T3Styling.fontSubSub_r
			textContent_s: ""
			textColor_c: T3Styling.cFgMain_c
			textPixelSize_r: T3Styling.fontSubSub_r
			SequentialAnimation{
				id:sAnim_animationStation
				running:text_stationInfo.textContent_s!=="stat"
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
		Rectangle{
			id:rect_moveVisualizer
			anchors.fill: parent
			border.color: T3Styling.cFgSubSub_c
			border.width: T3Styling.lineWidth_r
			radius: T3Styling.spacing_r
			opacity: {
				if(text_stationInfo.textContent_s!=="") return 0;
				else if(pos_r<0.4) return pos_r/0.4;
				else if(pos_r>0.6) return (1-pos_r)/0.4;
				else return 1;
			}
			//Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
			property real pos_r:0.5
			onPos_rChanged: gs_changeGs.position = pos_r
			gradient: Gradient {
				orientation:Gradient.Horizontal
				GradientStop {  id:gs_changeGs0
					position: 0; color: T3Styling.cBgMain_c }
				GradientStop {
					id:gs_changeGs
					color: T3Styling.cFgSub_c

				}

				GradientStop { id:gs_changeGs2
					position:1 ; color:T3Styling.cBgMain_c}
			}
		}

	}
	Rectangle{

		id:rect_rightScreen
		anchors{
			right:root.right
			top:sDis_segDisplay.bottom
			topMargin: T3Styling.spacing_r
			bottom:grid_buttons.top
			//l//eftMargin: T3Styling.margin_r
			rightMargin: T3Styling.margin_r
			bottomMargin:T3Styling.spacing_r
		}
		width: root.width*0.4
		border.width: T3Styling.lineWidth_r
		border.color: T3Styling.cFgSubSub_c
		radius: T3Styling.spacing_r
		color:T3Styling.cBgMain_c
		Rectangle{
			anchors{
				fill:parent
				margins: T3Styling.margin_r}
			color: "transparent"
			T3NMCircularGauge{
				id:cGau_power
				anchors{
					top:parent.top
					left: parent.left
					bottom: text_powerLabel.top
					topMargin: -T3Styling.spacing_r
					bottomMargin: -T3Styling.spacing_r
				}
				currValue_n:0
				fixedPoint_i: 0
				stepsize_r: 0.1
				tickmarkStepsize_r: 15
				gaugeUnit_s: "kW"
				maxValue_n:150
				textOnly_b: false
				toggle_b: false
				width:(parent.width- T3Styling.spacing_r*2)/10*2.5
				height: width
			}
			T3Text{
				id:text_powerLabel
				anchors{
					horizontalCenter: cGau_power.horizontalCenter
					bottom: rect_seperator1.top
					bottomMargin: T3Styling.lineWidth_r

				}

				height:T3Styling.fontSub_r
				textPixelSize_r: T3Styling.fontSubSub_r
				textContent_s: "Power"
				textColor_c: T3Styling.cFgMain_c
			}
			T3NMCircularGauge{
				id:cGau_velocity
				anchors{
					top:parent.top
					horizontalCenter: parent.horizontalCenter
					left: cGau_power.right
					leftMargin: T3Styling.spacing_r
					bottom: text_velocityLabel.top
					topMargin: -T3Styling.spacing_r
					bottomMargin: -T3Styling.spacing_r
				}
				stepsize_r: 0.1
				fixedPoint_i: 0
				currValue_n: 0
				tickmarkStepsize_r: 5
				maxValue_n: 50
				textOnly_b: true
				toggle_b: false
				width:(parent.width- T3Styling.spacing_r*2)/3
				//height: 100
				//height: width
			}
			T3Text{
				id:text_velocityLabel
				anchors{
					horizontalCenter: cGau_velocity.horizontalCenter
					bottom: rect_seperator1.top
					bottomMargin: T3Styling.lineWidth_r

				}

				height:T3Styling.fontSub_r
				textPixelSize_r: T3Styling.fontSubSub_r
				textContent_s: "Velocity"
				textColor_c: T3Styling.cFgMain_c
			}
			T3NMCircularGauge{
				id:cGau_acceleration
				anchors{
					top:parent.top
					left: cGau_velocity.right
					leftMargin: T3Styling.spacing_r
					bottom: text_accelerationLabel.top
					topMargin: -T3Styling.spacing_r
					bottomMargin: -T3Styling.spacing_r
				}
				fixedPoint_i: 1
				currValue_n: 0
				stepsize_r: 0.01
				tickmarkStepsize_r: 0.4
				gaugeUnit_s: "ft/s²"
				textOnly_b: false
				minValue_n: -2
				maxValue_n: 2
				toggle_b: false
				width:(parent.width- T3Styling.spacing_r*2)/10*2.5
				height: width
			}
			T3Text{
				id:text_accelerationLabel
				anchors{
					horizontalCenter: cGau_acceleration.horizontalCenter
					bottom: rect_seperator1.top
					bottomMargin: T3Styling.lineWidth_r
				}
				height:T3Styling.fontSub_r
				textPixelSize_r: T3Styling.fontSubSub_r
				textContent_s: "Acceleration"
				textColor_c: T3Styling.cFgMain_c
			}
			Rectangle{
				id:rect_seperator1
				anchors{
					left:parent.left
					right: parent.right
					bottom: colu_trainInfo.top
					bottomMargin: T3Styling.spacing_r*2
				}

				height: T3Styling.lineWidth_r
				radius: height
				color: T3Styling.cFgSubSub_c
			}
			Column{
				id:colu_trainInfo
				spacing:T3Styling.spacing_r*1.5
				anchors{
					left:parent.left
					right: cGau_velocity.horizontalCenter
					rightMargin: T3Styling.spacing_r*2
					//top:rect_seperator1.bottom
					//topMargin: T3Styling.spacing_r*2
					bottom: parent.bottom
				}
				Repeater{
					id:reap_trainConstantsDisplay
					model:["Train Length"
						,"Train Height"
						,"Train Width"
						,"Train Mass"
						,"Train Heater"
					]
					delegate:Rectangle{
						height: T3Styling.fontSubSub_r
						width: parent.width
						color:"transparent"
						property string val_s:"N/A"
						T3Text{
							width:parent.width
							height:T3Styling.fontSubSub_r
							textPixelSize_r: T3Styling.fontSubSub_r
							textContent_s: modelData
							textColor_c: T3Styling.cFgSub_c
							textAlign_s: "left"
						}
						T3Text{
							width:parent.width
							height:T3Styling.fontSubSub_r
							textPixelSize_r: T3Styling.fontSubSub_r
							textContent_s: parent.val_s
							textColor_c: T3Styling.cFgMain_c
							textAlign_s: "right"
						}
					}
				}
			}
			Column{
				spacing:T3Styling.spacing_r*1.5
				anchors{
					right:parent.right
					left: cGau_velocity.horizontalCenter
					leftMargin: T3Styling.spacing_r*2
					//					top:rect_seperator1.bottom
					//					topMargin: T3Styling.spacing_r*2
					bottom: parent.bottom
				}
				Repeater{
					id:reap_trainConstantsDisplay2
					model:["Exterior Light"
						,"Interior Light"
						,"Train Engine"
						,"Signal Pickup"
						,"Brake Status"
					]
					delegate:Rectangle{
						height: T3Styling.fontSubSub_r
						width: parent.width
						color:"transparent"
						property string val_s:"N/A"
						T3Text{
							width:parent.width
							height:T3Styling.fontSubSub_r
							textPixelSize_r: T3Styling.fontSubSub_r
							textContent_s: modelData
							textColor_c: T3Styling.cFgSub_c
							textAlign_s: "left"
						}
						T3Text{
							width:parent.width
							height:T3Styling.fontSubSub_r
							textPixelSize_r: T3Styling.fontSubSub_r
							textContent_s: parent.val_s
							textColor_c: {
								if(textContent_s==="OK")
									return T3Styling.cGreen_c
								else if(textContent_s==="ERROR")
									return T3Styling.cRed_c
								else if(textContent_s==="ON")
									return T3Styling.cYellow_c
								else if(textContent_s==="OFF")
									return T3Styling.cFgMain_c
								else
									return T3Styling.cFgMain_c
							}
							textAlign_s: "right"
						}
					}
				}
			}

		}
	}
	T3ParamUnit{
		id:cust_trainTemperature
		anchors{
			bottom: grid_buttons.top
			left:grid_buttons.left
			right:rect_leftScreen.right
			rightMargin: T3Styling.margin_r+T3Styling.spacing_r*2
			bottomMargin: T3Styling.spacing_r
		}
		height: T3Styling.margin_r*2
		maxValue_r: 100
		isWhole_b: true
		paramConfig_A: "F_F_Internal Train Temperature_F";
	}
	T3Button{
		id:butt_applyTrainTemperature
		anchors{
			top:cust_trainTemperature.top
			bottom: cust_trainTemperature.bottom
			left:cust_trainTemperature.right
			right:rect_leftScreen.right
		}
		buttonLabel_s: "√"
		onButtonClicked: {
			if(trainId_s==="") return;
			t3databaseQml.nm_setTrainTemperature(trainId_s,
												 cust_trainTemperature.actualValue_r);
		}

	}
	Grid{
		id:grid_buttons
		anchors{
			left: rect_leftScreen.left
			right:rect_rightScreen.right
			bottom:parent.bottom
			bottomMargin: T3Styling.margin_r
		}
		height: T3Styling.margin_r*1
		//		width: parent.width
		//		height: parent.unitHeight_r
		rows:1
		columns: 4
		columnSpacing: T3Styling.spacing_r
		property real unitWidth_r: (width-(columns-1)*columnSpacing)/columns
		T3Button{
			id:butt_brokenRail
			property bool currState_b: false
			width: parent.unitWidth_r
			height:parent.height
			buttonLabel_s: (currState_b?"Revoke":"Invoke")+" Train Engine Failure"
			releasedColor_c: currState_b?Qt.darker(T3Styling.cRed_c):T3Styling.cFgSubSub_c
			onButtonClicked: {
				currState_b = !currState_b;
				if(trainId_s==="") return;
				t3databaseQml.nm_setFailureOrBrake(trainId_s,0,currState_b);
				db2view(true);
			}
		}
		T3Button{
			id:butt_trackCircuit
			property bool currState_b: false
			width: parent.unitWidth_r
			height:parent.height
			buttonLabel_s: (currState_b?"Revoke":"Invoke")+" Signal Pickup Failure"
			releasedColor_c: currState_b?Qt.darker(T3Styling.cRed_c):T3Styling.cFgSubSub_c
			onButtonClicked: {
				currState_b = !currState_b;
				if(trainId_s==="") return;
				t3databaseQml.nm_setFailureOrBrake(trainId_s,1,currState_b);
				db2view(true);
			}
		}
		T3Button{
			id:butt_powerFailure
			property bool currState_b: false
			width: parent.unitWidth_r
			height:parent.height
			buttonLabel_s: (currState_b?"Revoke":"Invoke")+" Brake Failure"
			releasedColor_c: currState_b?Qt.darker(T3Styling.cRed_c):T3Styling.cFgSubSub_c
			onButtonClicked: {
				currState_b = !currState_b;
				if(trainId_s==="") return;
				t3databaseQml.nm_setFailureOrBrake(trainId_s,2,currState_b);
				db2view(true);
			}
		}
		T3Button{
			id:butt_emergencyBrake
			property bool currState_b: false
			width: parent.unitWidth_r
			height:parent.height
			buttonLabel_s: (currState_b?"Revoke":"Invoke")+" Emergency Brake"
			releasedColor_c: currState_b?Qt.darker(T3Styling.cRed_c):T3Styling.cFgSubSub_c
			onButtonClicked: {
				currState_b = !currState_b;
				if(trainId_s==="") return;
				t3databaseQml.nm_setFailureOrBrake(trainId_s,03,currState_b);
				db2view(true);
			}
		}

	}

	Rectangle{
		id:rect_frontHelper
		anchors{
			left:cust_trainSelector.right
			margins: T3Styling.margin_r
			top:parent.top
			bottom: parent.bottom

			right: parent.right
		}
		color:T3Styling.cBgSub_c
		property bool initState_b:true
		opacity: 1
//		visible: false
		border.width: T3Styling.lineWidth_r
		border.color: T3Styling.cFgSubSub_c
		radius: T3Styling.lineWidth_r
		T3Text{
			textContent_s: rect_frontHelper.initState_b?"T3 | TRAIN MODEL\n\n\n":""
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
	//	T3Button{
	//		id:butt_emergencyBrake
	//		anchors{
	//			left: rect_leftScreen.left
	//			right:rect_leftScreen.right
	//			bottom:parent.bottom
	//			bottomMargin: T3Styling.margin_r

	//		}
	//		height: T3Styling.margin_r*2
	//		buttonLabel_s: "EMERGENCY BRAKE"
	//		buttonTextPixelSize_r: T3Styling.fontMain_r*3
	//		pressedColor_c: T3Styling.cRed_c
	//		releasedColor_c:Qt.darker(T3Styling.cRed_c)
	//		delayButton_b: true
	//	}
}
