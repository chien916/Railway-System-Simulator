import QtQuick 2.12

Item {
	id:root
	width: 1000
	height: 350

	property string line_s:"red"
	property variant destinationNames_sA :[
		"Waterfront"
		,"Downtown"
		,"Murray via Oakland"
		,"Squirrel Hill"
	]

	property bool leftDoorOpened_b:false
	property bool rightDoorOpened_b:false
	property bool serviceBrakeApplied_b:false
	property bool emergencyBrakeApplied_b:false
	property real normalBrakeRatio_r:0.0

	//signal-light-speed limit related
	property real speedLimit_r:20
	property string signalLight_s:"approach"

	//pass in those values in METRICS!!! Auto-conversion will be conducted
	property real power_r:100//unit in kwatts
	property real velocity_r:3//unit in km/h
	property real acceleration_r:0.5// unit in m/s^2
	property real cabinTemperature_r:90

	property bool exteriorLightsOn_b:true
	property bool interiorLightsOn_b:false
	property bool engineFailed_b:true
	property bool signalPickupFailed_b:false
	property bool brakeFailed:false

	readonly property real powerImperial_r:power_r
	readonly property real velocityImperial_r:0.621371*velocity_r
	readonly property real accelerationImperial_r:3.28084*acceleration_r
	readonly property real cabinTemperatureImperial_r:(cabinTemperature_r*9/5) + 32


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
		model_A: t3databaseQml?t3databaseQml.trainIds_QML:[[""]]
		width: root.width*0.05
	}

	Rectangle{
		id:sDis_segDisplay
		anchors{
			left:cust_trainSelector.right
//			right:root.right
			top:root.top
			margins: T3Styling.margin_r
		}
		height: 50
		width: root.width*0.35
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
				running:text_stationInfo.textContent_s!==""
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
		id:rect_leftScreen
		anchors{
			left:cust_trainSelector.right
			top:sDis_segDisplay.bottom
			bottom:grid_buttons.top
			leftMargin: T3Styling.margin_r
			rightMargin: T3Styling.margin_r
			topMargin:T3Styling.spacing_r
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
			Repeater{
				model:[["Left Door",root.leftDoorOpened_b]
				]
				delegate:Rectangle{
					height: T3Styling.fontSubSub_r
					width: parent.width
					color:"transparent"
					T3Text{
						width:parent.width
						height:T3Styling.fontSubSub_r
						textPixelSize_r: T3Styling.fontSubSub_r
						textContent_s: modelData[0]
						textColor_c: T3Styling.cFgSub_c
						textAlign_s: "left"
					}
					T3Text{
						width:parent.width
						height:T3Styling.fontSubSub_r
						textPixelSize_r: T3Styling.fontSubSub_r
						textContent_s: {
							if(modelData[1]) return "OPENED"
							else return "CLOSED"
						}
						textColor_c:{
							if(modelData[1]) return T3Styling.cYellow_c
							else return  T3Styling.cGreen_c
						}
						textAlign_s: "right"
					}
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
			Repeater{
				model:[["Right Door",root.rightDoorOpened_b]
				]
				delegate:Rectangle{
					height: T3Styling.fontSubSub_r
					width: parent.width
					color:"transparent"
					T3Text{
						width:parent.width
						height:T3Styling.fontSubSub_r
						textPixelSize_r: T3Styling.fontSubSub_r
						textContent_s: modelData[0]
						textColor_c: T3Styling.cFgSub_c
						textAlign_s: "left"
					}
					T3Text{
						width:parent.width
						height:T3Styling.fontSubSub_r
						textPixelSize_r: T3Styling.fontSubSub_r
						textContent_s: {
							if(modelData[1]) return "OPENED"
							else return "CLOSED"
						}
						textColor_c:{
							if(modelData[1]) return T3Styling.cYellow_c
							else return  T3Styling.cGreen_c
						}
						textAlign_s: "right"
					}
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
			leftDoorClosed_b: !root.leftDoorOpened_b
			rightDoorClosed_b: !root.rightDoorOpened_b
			brakeReleased_b: !root.serviceBrakeApplied_b
			brakeEmergency_b: root.emergencyBrakeApplied_b
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
				bottom:  parent.bottom
				bottomMargin: T3Styling.margin_r
			}
			Repeater{
				model:[["Service Brake",root.serviceBrakeApplied_b]
				]
				delegate:Rectangle{
					height: T3Styling.fontSubSub_r
					width: parent.width
					color:"transparent"
					T3Text{
						width:parent.width
						height:T3Styling.fontSubSub_r
						textPixelSize_r: T3Styling.fontSubSub_r
						textContent_s: modelData[0]
						textColor_c: T3Styling.cFgSub_c
						textAlign_s: "left"
					}
					T3Text{
						width:parent.width
						height:T3Styling.fontSubSub_r
						textPixelSize_r: T3Styling.fontSubSub_r
						textContent_s: {
							if(modelData[1]) return "APPLIED"
							else return "RELEASED"
						}
						textColor_c:{
							if(modelData[1]) T3Styling.cYellow_c
							else return T3Styling.cGreen_c
						}
						textAlign_s: "right"
					}
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
				bottom:  parent.bottom
				bottomMargin: T3Styling.margin_r
			}
			Repeater{
				model:[["Emergency Brake",root.emergencyBrakeApplied_b]
				]
				delegate:Rectangle{
					height: T3Styling.fontSubSub_r
					width: parent.width
					color:"transparent"
					T3Text{
						width:parent.width
						height:T3Styling.fontSubSub_r
						textPixelSize_r: T3Styling.fontSubSub_r
						textContent_s: modelData[0]
						textColor_c: T3Styling.cFgSub_c
						textAlign_s: "left"
					}
					T3Text{
						width:parent.width
						height:T3Styling.fontSubSub_r
						textPixelSize_r: T3Styling.fontSubSub_r
						textContent_s: {
							if(modelData[1]) return "APPLIED"
							else return "RELEASED"
						}
						textColor_c:{
							if(modelData[1]) T3Styling.cRed_c
							else return T3Styling.cGreen_c
						}
						textAlign_s: "right"
					}
				}
			}
		}
		Rectangle{
			id:rect_seperator2
			anchors{
				left:cVie_carView.left
				right: cVie_carView.right
				top:colu_emergencyBrakeInfo.bottom
				topMargin: T3Styling.spacing_r*1.5
			}
			visible: false
			height: T3Styling.lineWidth_r
			radius: height
			color: T3Styling.cFgSubSub_c
		}

	}
	T3NMSpeedLimitAndSignal{
		currSignal_s:root.signalLight_s////--------TOGGLE
		currSpeedLimit_i: root.speedLimit_r////????IMperial or metric?
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

		id:rect_rightScreen
		anchors{
			right:root.right
			top:parent.top
			topMargin: T3Styling.margin_r
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
				currValue_n: root.powerImperial_r
				fixedPoint_i: 0
				stepsize_r: 0.1
				tickmarkStepsize_r: 15
				gaugeUnit_s: "kW"
				maxValue_n:150
				textOnly_b: false
				toggle_b: false
				width:(parent.width- T3Styling.spacing_r*2)/3
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
				currValue_n: root.velocityImperial_r
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
				currValue_n: root.accelerationImperial_r
				stepsize_r: 0.01
				tickmarkStepsize_r: 0.4
				gaugeUnit_s: "ft/s²"
				textOnly_b: false
				minValue_n: -2
				maxValue_n: 2
				toggle_b: false
				width:(parent.width- T3Styling.spacing_r*2)/3
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
					model:[["Train Length","105 FT"]
						,["Train Height","11.2 FT"]
						,["Train Width","8.69 FT"]
						,["Train Mass","40.9 T"]
						,["Cabin Temperature",root.cabinTemperatureImperial_r.toFixed(1)+" F"]
					]
					delegate:Rectangle{
						height: T3Styling.fontSubSub_r
						width: parent.width
						color:"transparent"
						T3Text{
							width:parent.width
							height:T3Styling.fontSubSub_r
							textPixelSize_r: T3Styling.fontSubSub_r
							textContent_s: modelData[0]
							textColor_c: T3Styling.cFgSub_c
							textAlign_s: "left"
						}
						T3Text{
							width:parent.width
							height:T3Styling.fontSubSub_r
							textPixelSize_r: T3Styling.fontSubSub_r
							textContent_s: modelData[1]
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
					model:[["Exterior Light",root.exteriorLightsOn_b?"ON":"OFF"]
						,["Interior Light",root.interiorLightsOn_b?"ON":"OFF"]
						,["Engine Status",root.engineFailed_b?"ERROR":"OK"]
						,["Signal Pickup",root.signalPickupFailed_b?"ERROR":"OK"]
						,["Brake Status",root.brakeFailed?"ERROR":"OK"]

					]
					delegate:Rectangle{
						height: T3Styling.fontSubSub_r
						width: parent.width
						color:"transparent"
						T3Text{
							width:parent.width
							height:T3Styling.fontSubSub_r
							textPixelSize_r: T3Styling.fontSubSub_r
							textContent_s: modelData[0]
							textColor_c: T3Styling.cFgSub_c
							textAlign_s: "left"
						}
						T3Text{
							width:parent.width
							height:T3Styling.fontSubSub_r
							textPixelSize_r: T3Styling.fontSubSub_r
							textContent_s: modelData[1]
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
		T3Button{
			id:butt_emergencyBrake
			property bool currState_b: false
			width: parent.unitWidth_r
			height:parent.height
			buttonLabel_s: (currState_b?"Revoke":"Invoke")+" Emergency Brake"
			releasedColor_c: currState_b?Qt.darker(T3Styling.cRed_c):T3Styling.cFgSubSub_c
			onButtonClicked: {
				currState_b = !currState_b;
				view2db();
			}
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
