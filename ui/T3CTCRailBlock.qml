import QtQuick 2.12

Item {
	property string kmPlcIo_s:"00000000001011010000000011000000"
	property string bcnPlcOut_s:
		"00000000000000000000000000000000"
	property string blockId_s:"G_A_1"
	property bool noAnimation_b:false
	readonly property bool hasSwitchOnLeft_b: kmPlcIo_s.charCodeAt(24)>48
	readonly property bool hasSwitchOnRight_b: kmPlcIo_s.charCodeAt(25)>48
	readonly property bool switchIsUp_b:kmPlcIo_s.charCodeAt(18)>48
	readonly property bool stationOnLeft_b: kmPlcIo_s.charCodeAt(26)>48
	readonly property bool stationOnRight_b: kmPlcIo_s.charCodeAt(27)>48
	readonly property bool hasCrossingGate_b:kmPlcIo_s.charCodeAt(29)>48
	readonly property bool crossingGateDown_b:kmPlcIo_s.charCodeAt(19)>48
	readonly property bool failure_b: kmPlcIo_s.charCodeAt(6)>48||kmPlcIo_s.charCodeAt(7)>48
	readonly property bool authority_b: bcnPlcOut_s.substring(2,10).includes("1")
	readonly property bool isUnderground_b:kmPlcIo_s.charCodeAt(28)>48
	readonly property bool canMoveForward_b:kmPlcIo_s.charCodeAt(22)>48
	readonly property bool canMoveReversed_b:kmPlcIo_s.charCodeAt(23)>48
	readonly property bool trainOnBlock_b:kmPlcIo_s.charCodeAt(3)>48
	readonly property bool trainMovingForward_b:true
	readonly property bool maintananceMode_b:false
	readonly property int leftSignalLight_i:parseInt(bcnPlcOut_s.substring(16,18),2)
	readonly property int rightSignalLight_i:parseInt(bcnPlcOut_s.substring(20,21),2)

	 property color color_c: {//probably needs change
		if(hovered2_b)
			return T3Styling.cFgMain_c
		else if(failure_b)
			return T3Styling.cRed_c
		else{
			if(authority_b)
				return T3Styling.cGreen_c
			else
				return T3Styling.cFgSub_c
		}

	}
	//--MODIFYABLES:
	property bool hovered2_b:false
	property bool hovered_b:false
	property bool pressed_b:false
	//block identifier

	readonly property real adaptiveMargin_r :root.height*0.1
	readonly property real adaptiveFontSize_r: root.height*0.1
	readonly property real adaptiveSpacing_r: root.height*0.05
	readonly property real adaptiveLineWidth_r : root.height*0.01
	readonly property real adaptiveElemHeight_r : (rect_canvas.height-13*adaptiveLineWidth_r-adaptiveMargin_r*2)/6
	readonly property real adaptiveElemWidth_r: rect_canvas.width*((hasSwitchOnLeft_b||hasSwitchOnRight_b)?0.6:1)
	readonly property real adaptiveElemX_r: hasSwitchOnLeft_b?rect_canvas.width-adaptiveElemWidth_r:0
	property color adaptiveFgSubSub_c:{
		if(pressed_b)
			return T3Styling.cFgSub_c
		else if(hovered_b||hovered2_b)
			return T3Styling.cFgMain_c
		else
			return T3Styling.cFgSubSub_c
	}
	Behavior on adaptiveFgSubSub_c{PropertyAnimation{duration: noAnimation_b?0:500}}
	//--DO NOT MODIFY ANYTHING FROM HERE
	id:root
	Behavior on color_c{PropertyAnimation{
			easing.type: Easing.InOutQuart
			duration: noAnimation_b?0:500}
	}

	Rectangle{
		id:rect_canvas
		height: root.height
		width: root.width
		color: {
			if(pressed_b)
				return T3Styling.cBgMain_c
			else if(hovered_b||hovered2_b)
				return T3Styling.cBgSub_c
			else
				return "transparent"
		}
		clip:false
		radius: h_r/2
		readonly property real h_r: root.adaptiveMargin_r
		readonly property real ytop_r: 0;
		readonly property real ybottom_r:height-h_r;
		readonly property real ws_r:(width+h_r)*0.5
		readonly property real alpha_r: Math.atan((ybottom_r-ytop_r)/(ws_r-h_r))*180/Math.PI;
		readonly property real wl_r:Math.sqrt(Math.pow(ybottom_r-ytop_r,2)+Math.pow(ws_r-h_r,2))+h_r
		Rectangle{
			id:rect_tlBlock
			width:rect_canvas.ws_r
			height: rect_canvas.h_r
			radius: rect_canvas.h_r
			color: T3Styling.cFgSubSub_c
			x:0
			y:rect_canvas.ytop_r
		}

		Rectangle{
			id:rect_slBlock
			width: rect_canvas.wl_r
			height: rect_canvas.h_r
			radius:rect_canvas.h_r
			color: T3Styling.cFgSubSub_c
			visible:root.hasSwitchOnLeft_b
			x:0
			y:rect_canvas.ybottom_r
			transform: Rotation{
				origin.x:rect_canvas.h_r/2
				origin.y:rect_canvas.h_r/2
				angle: -rect_canvas.alpha_r
			}
		}

		Rectangle{
			id:rect_trBlock
			width: rect_canvas.ws_r
			height:rect_canvas.h_r
			radius: rect_canvas.h_r
			color:  T3Styling.cFgSubSub_c
			x:rect_canvas.ws_r-rect_canvas.h_r
			y:rect_canvas.ytop_r
			z:0
		}

		Rectangle{
			id:rect_srBlock
			width: rect_canvas.wl_r
			height:rect_canvas.h_r
			radius: rect_canvas.h_r
			color: T3Styling.cFgSubSub_c
			visible:root.hasSwitchOnRight_b
			x:rect_canvas.ws_r-rect_canvas.h_r
			y:rect_canvas.ytop_r
			z:0
			transform: Rotation{
				origin.x:rect_canvas.h_r/2
				origin.y:rect_canvas.h_r/2
				angle: rect_canvas.alpha_r
			}
		}

		Rectangle{
			id:rect_clBlock
			width:(!root.switchIsUp_b&&root.hasSwitchOnLeft_b)?rect_srBlock.width:rect_trBlock.width
			Behavior on width {PropertyAnimation {
					easing.type: Easing.InOutQuad
					duration: noAnimation_b?0:500
				}}
			height: rect_canvas.h_r
			radius: rect_canvas.h_r
			color: root.color_c
			x:rect_trBlock.x
			y:rect_trBlock.y
			z:2
			transform: Rotation{
				origin.x:rect_canvas.h_r/2
				origin.y:rect_canvas.h_r/2
				angle: ((!switchIsUp_b&&hasSwitchOnLeft_b)?180-rect_canvas.alpha_r:180)
				Behavior on angle {PropertyAnimation {
						easing.type: Easing.InOutQuad
						duration: noAnimation_b?0:500 }}
			}
		}

		Rectangle{
			id:rect_crBlock
			width:(!switchIsUp_b&&hasSwitchOnRight_b)?rect_srBlock.width:rect_trBlock.width
			Behavior on width {PropertyAnimation {
					easing.type: Easing.InOutQuad
					duration: noAnimation_b?0:500
				}}
			height: rect_canvas.h_r
			radius: rect_canvas.h_r
			color: root.color_c
			x:rect_trBlock.x
			y:rect_trBlock.y
			z:2
			transform: Rotation{
				origin.x:rect_canvas.h_r/2
				origin.y:rect_canvas.h_r/2
				angle: (hasSwitchOnRight_b&&!switchIsUp_b)?rect_canvas.alpha_r:0
				Behavior on angle {PropertyAnimation {
						easing.type: Easing.InOutQuad
						duration: noAnimation_b?0:500
					}}
			}
		}

		Rectangle{
			id:rect_train
			width: /*mAre_train.containsMouse?rect_canvas.h_r*3:*/rect_canvas.h_r
			Behavior on width {PropertyAnimation { easing.type: Easing.InOutQuad }}
			height: rect_canvas.h_r
			radius: rect_canvas.h_r/2
			visible: trainOnBlock_b
			color: /*mAre_train.containsMouse?*//*T3Styling.cBgSub_c:*/T3Styling.cBgSub_c
			border.width:root.adaptiveLineWidth_r
			border.color:T3Styling.cFgMain_c
			Behavior on color {PropertyAnimation { easing.type: Easing.InOutQuad }}
			property real trainAtPerc_r: 0.7
			readonly property real animationDuration_r : 3000.0
			readonly property variant animPosiIntervProp_rA :{
				if(trainMovingForward_b)
					return [0,1/2,1];
				else
					return [1,1/2,0];
			}
			ParallelAnimation{
				running: true
				loops: Animation.Infinite
				alwaysRunToEnd: true
				SequentialAnimation{
					alwaysRunToEnd: true
					PropertyAnimation{
						alwaysRunToEnd: true
						target: rect_train
						property:"opacity"
						from:0
						to:1
						duration:rect_train.animationDuration_r*1/5
					}
					PropertyAnimation{
						alwaysRunToEnd: true
						target: rect_train
						property:"opacity"
						from:1
						to:1
						duration:rect_train.animationDuration_r*3/5
					}
					PropertyAnimation{
						alwaysRunToEnd: true
						target: rect_train
						property:"opacity"
						from:1
						to:0
						duration:rect_train.animationDuration_r*1/5
					}
				}
				SequentialAnimation{
					alwaysRunToEnd: true
					PropertyAnimation{
						alwaysRunToEnd: true
						target: rect_train
						property:"trainAtPerc_r"
						from:rect_train.animPosiIntervProp_rA[0]
						to:rect_train.animPosiIntervProp_rA[1]
						duration: rect_train.animationDuration_r/2
						easing.type:((hasSwitchOnLeft_b||hasSwitchOnRight_b)&&!switchIsUp_b)?Easing.InOutCubic:Easing.InCubic
					}
					PropertyAnimation{
						alwaysRunToEnd: true
						target: rect_train
						property:"trainAtPerc_r"
						from:rect_train.animPosiIntervProp_rA[1]
						to:rect_train.animPosiIntervProp_rA[2]
						duration: rect_train.animationDuration_r/2
						easing.type:((hasSwitchOnLeft_b||hasSwitchOnRight_b)&&!switchIsUp_b)?Easing.InOutCubic:Easing.OutCubic
					}
				}

			}
			x:(rect_canvas.width-rect_canvas.h_r)*trainAtPerc_r
			y:{
				//				let trainAtPerc_r = parseFloat(root.trainInfo_s.split("_")[1])
				if(trainAtPerc_r<0.5&&!root.switchIsUp_b&&root.hasSwitchOnLeft_b)
					//H/B=h/b -> h=H/B*b
					return (rect_canvas.ybottom_r-rect_canvas.ytop_r)*(0.5-trainAtPerc_r)*2
				else if(trainAtPerc_r>0.5&&!root.switchIsUp_b&&root.hasSwitchOnRight_b)
					return (rect_canvas.ybottom_r-rect_canvas.ytop_r)*(-0.5+trainAtPerc_r)*2
				else
					return 0;
			}
			z:2
		}
		Rectangle{
			id:rect_underground
			x:0/*rect_canvas.h_r*/
			y:rect_canvas.h_r+root.adaptiveLineWidth_r
			width: rect_canvas.width/*-2*x*/
			height: root.adaptiveLineWidth_r*3
			//			radius: height
			color:adaptiveFgSubSub_c
			opacity: root.isUndg_b?1:0
		}

		Rectangle{
			id:rect_station
			x:0/*rect_canvas.h_r*/
			width: rect_canvas.width/*-2*x*/
			y:rect_underground.y+rect_underground.height+root.adaptiveLineWidth_r
			height: root.adaptiveElemHeight_r
			radius: root.adaptiveLineWidth_r
			color: root.adaptiveFgSubSub_c
			opacity: (stationOnLeft_b||stationOnRight_b)?1:0
			Text{
				text:stationOnLeft_b?"L":""+stationOnRight_b?"R":""
				color: T3Styling.cBgSub_c
				anchors.fill: parent
				anchors.margins: root.adaptiveLineWidth_r
				font.family: "Inter"
				font.pixelSize: root.adaptiveFontSize_r
				fontSizeMode: Text.Fit
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
			}

		}

		Rectangle{
			id:rect_leftArrow
			x:root.adaptiveElemX_r
			y:rect_station.y+rect_station.height+root.adaptiveLineWidth_r
			height:root.adaptiveElemHeight_r
			width: adaptiveElemWidth_r
			color: "transparent"
			Item{
				width: rect_leftArrow.height
				height:rect_leftArrow.height
				anchors.horizontalCenter: rect_leftArrow.horizontalCenter
				Text{
					id:text_leftArrow
					anchors.fill: parent
					text:canMoveReversed_b?"←":""
					font.family: "Inter"
					font.pixelSize: root.adaptiveFontSize_r*2
					fontSizeMode: Text.Fit
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment: Text.AlignVCenter
					color: {
						if(leftSignalLight_i===2)
							return T3Styling.cGreen_c
						else if(leftSignalLight_i===1)
							return T3Styling.cYellow_c
						else if(leftSignalLight_i===0)
							return T3Styling.cRed_c
						else
							return T3Styling.cFgSubSub_c
					}
					Behavior on color {PropertyAnimation {
							easing.type: Easing.InOutQuad
							duration: noAnimation_b?0:500
						}}
				}
				rotation: (!root.switchIsUp_b&&root.hasSwitchOnLeft_b)?-rect_canvas.alpha_r:0
				Behavior on rotation {PropertyAnimation {
						easing.type: Easing.InOutQuad
						duration: noAnimation_b?0:500
					}}

			}
		}
		Rectangle{
			id:rect_rightArrow
			x:root.adaptiveElemX_r
			y:rect_leftArrow.y+rect_leftArrow.height+root.adaptiveLineWidth_r
			height: root.adaptiveElemHeight_r
			width: adaptiveElemWidth_r
			color: "transparent"
			Item{
				width: rect_rightArrow.height
				height:rect_rightArrow.height
				anchors.horizontalCenter: rect_rightArrow.horizontalCenter
				Text{
					id:text_rightArrow
					anchors.fill: parent
					text:canMoveForward_b?"→":""
					font.family: "Inter"
					font.pixelSize: root.adaptiveFontSize_r*2
					fontSizeMode: Text.Fit
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment: Text.AlignVCenter
					color: {
						if(rightSignalLight_i===2)
							return T3Styling.cGreen_c
						else if(rightSignalLight_i===1)
							return T3Styling.cYellow_c
						else if(rightSignalLight_i===0)
							return T3Styling.cRed_c
						else
							return T3Styling.cFgSubSub_c
					}
					Behavior on color {PropertyAnimation {
							easing.type: Easing.InOutQuad
							duration: noAnimation_b?0:500
						}}
				}
				rotation: (!root.switchIsUp_b&&root.hasSwitchOnRight_b)?rect_canvas.alpha_r:0
				Behavior on rotation {PropertyAnimation {
						easing.type: Easing.InOutQuad
						duration: noAnimation_b?0:500
					}}

			}
		}

		Rectangle{
			id:rect_crossing
			x:root.adaptiveElemX_r
			y:rect_rightArrow.y+rect_rightArrow.height+root.adaptiveLineWidth_r
			height: root.adaptiveElemHeight_r
			width:adaptiveElemWidth_r
			color: "transparent"
			Item{
				width: rect_crossing.height
				height:rect_crossing.height
				anchors.horizontalCenter: rect_crossing.horizontalCenter
				Text{
					id:text_crossing
					anchors.fill: parent
					text:"×"
					font.family: "Inter"
					font.pixelSize: root.adaptiveFontSize_r*2
					fontSizeMode: Text.Fit
					horizontalAlignment: Text.AlignHCenter
					verticalAlignment: Text.AlignVCenter
					opacity:(hasCrossingGate_b)?1:0
					color: T3Styling.cFgSubSub_c
					Behavior on color {PropertyAnimation {
							easing.type: Easing.InOutQuad
							duration: noAnimation_b?0:500
						}}
					SequentialAnimation on color{
						running: hasCrossingGate_b&&crossingGateDown_b
						alwaysRunToEnd: true
						loops: Animation.Infinite
						PropertyAnimation{
							from: T3Styling.cFgSubSub_c
							to: T3Styling.cRed_c
							duration: 100
						}
						PropertyAnimation{
							from: T3Styling.cRed_c
							to: T3Styling.cFgSubSub_c
							duration: 500
						}
					}

				}
				rotation: (!root.switchIsUp_b&&root.hasSwitchOnLeft_b)?-rect_canvas.alpha_r:0
				Behavior on rotation {PropertyAnimation {
						easing.type: Easing.InOutQuad
						duration: noAnimation_b?0:500

					}}

			}
		}



		Rectangle{

			id:rect_blockInfo
			x:root.adaptiveElemX_r - (hasSwitchOnLeft_b?0: root.adaptiveLineWidth_r*0.5)
			width:adaptiveElemWidth_r + adaptiveLineWidth_r*((hasSwitchOnLeft_b||hasSwitchOnRight_b)?0.5:1)
			height: root.adaptiveElemHeight_r*2+root.adaptiveLineWidth_r/*rect_station.height*/
			color: "transparent"
			radius: root.adaptiveLineWidth_r
			border.color: /*mAre_mouseArea.containsMouse?"transparent":*/T3Styling.cFgSubSub_c;
			border.width: root.adaptiveLineWidth_r;
			//			anchors.horizontalCenter: rect_station.horizontalCenter
			y:rect_crossing.y+rect_crossing.height+root.adaptiveLineWidth_r
			Text{
				id:text_blockId
				anchors.fill: parent
				text:{
					let splittedBlockId_s = root.blockId_s.split("_");
					return splittedBlockId_s[0]+splittedBlockId_s[1]+"\n"+splittedBlockId_s[2]
				}
				color: root.adaptiveFgSubSub_c
				SequentialAnimation on color{
					running: maintananceMode_b
					alwaysRunToEnd: true
					loops: Animation.Infinite
					PropertyAnimation{
						from: root.adaptiveFgSubSub_c
						to: T3Styling.cYellow_c
						duration: 100
					}
					PropertyAnimation{
						from: T3Styling.cYellow_c
						to: root.adaptiveFgSubSub_c
						duration: 500
					}
				}
				font.family: "Inter"
				font.pixelSize: root.adaptiveFontSize_r*2
				fontSizeMode: Text.Fit
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
			}
		}
	}



	//--TESTING ONLY
	implicitHeight: 400
	implicitWidth: 400

}
