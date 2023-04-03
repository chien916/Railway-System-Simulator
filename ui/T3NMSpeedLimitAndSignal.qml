import QtQuick 2.12

Item {
	id:root

	property int currSpeedLimit_i:30
	//"clear"|"approach"|"stop"
	property string currSignal_s: "stop"
	readonly property variant speedLimitIntervals_iA: [80,70,65,60,55,45,40,30,20,15]
	readonly property int roundedSpeedLimit_i:{
		for(const currSpeedLimitIntervalPoint_i of speedLimitIntervals_iA){
			if(currSpeedLimit_i>=currSpeedLimitIntervalPoint_i)
				return currSpeedLimitIntervalPoint_i;
		}
	}

	Rectangle{
		anchors.fill: parent
		color: T3Styling.cBgSub_c
	}
	Rectangle{
		id:rect_leftPortion
		readonly property real squareSideLength_r: (root.height-T3Styling.spacing_r*9)/10;
		width: squareSideLength_r
		height: root.height
		color:"transparent"
		Column{
			id:column_speedLimitSquares
			spacing: T3Styling.spacing_r
			width: rect_leftPortion.squareSideLength_r
			height: root .height
			readonly property real squareSideLength_r: (root .height-T3Styling.spacing_r*9)/10;
			Repeater{
				model:root.speedLimitIntervals_iA
				delegate:	Rectangle{
					readonly property int copiedModelData_i: modelData
					width: rect_leftPortion.squareSideLength_r
					height:  rect_leftPortion.squareSideLength_r
					radius: T3Styling.lineWidth_r
					color: {
						if(modelData===15)
							if(root.roundedSpeedLimit_i===modelData)
								if(root.currSpeedLimit_i!==root.roundedSpeedLimit_i)
									return T3Styling.cYellow_c;
								else
									return T3Styling.cFgMain_c;
							else
								return T3Styling.cFgSubSub_c;
						else
							return "transparent"
					}
					Behavior on color{PropertyAnimation{}}
					border.color: {
						if(modelData===15)
							return T3Styling.cBgSub_c;
						else
							if(root.currSpeedLimit_i<=80&&root.roundedSpeedLimit_i===copiedModelData_i)
								if(root.currSpeedLimit_i!==root.roundedSpeedLimit_i)
									return T3Styling.cYellow_c;
								else
									return T3Styling.cFgMain_c;
							else
								return T3Styling.cFgSubSub_c;
					}
					Behavior on border.color{PropertyAnimation{}}
					border.width: modelData===15?0:T3Styling.lineWidth_r
					Text{
						text:parent.copiedModelData_i.toString()
						color: parent.border.color

						anchors.fill: parent
						anchors.margins: T3Styling.spacing_r
						font.family: "Inter"

						font.pixelSize: T3Styling.fontSub_r
						fontSizeMode: Text.Fit
						horizontalAlignment: Text.AlignHCenter
						verticalAlignment: Text.AlignVCenter

					}
				}
			}
		}
	}


	Rectangle{
		id:rectangle_commandedSpeedValue
		//			x:column_speedLimitSquares.width
		anchors{
			left:rect_leftPortion.right
			leftMargin: T3Styling.spacing_r
			right:root.right
			top: root.top
		}
		color: "transparent"
		height: rect_leftPortion.squareSideLength_r*2+T3Styling.spacing_r
		//				width: item_commandedSpeedLabel.width
		//				height: column_speedLimitSquares.squareSideLength_r*2+T3Styling.spacing_r
		//				x:item_commandedSpeedLabel.x
		//				y: item_commandedSpeedLabel.y/*+item_commandedSpeedLabel.height+T3Styling.spacing_r*/
		border.width: T3Styling.lineWidth_r
		border.color: T3Styling.cFgSubSub_c
		radius: T3Styling.spacing_r
		Text{
			id:text_commandedSpeedValueBack
			anchors.fill: parent
			anchors.margins: T3Styling.lineWidth_r
			text:"~~~"
			font.family: "DSEG14 Classic"
			font.pixelSize: T3Styling.fontSub_r*0.8
			fontSizeMode: Text.Fit
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			color: T3Styling.cFgSubSub_c
		}
		Text{
			id:text_commandedSpeedValue
			anchors.fill: parent
			anchors.margins: T3Styling.lineWidth_r
			text:root.currSpeedLimit_i.toString().padStart(3,"!").substring(0,3)
			font.family: "DSEG14 Classic"
			font.pixelSize: T3Styling.fontSub_r*0.8
			fontSizeMode: Text.Fit
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			color: {
				if(root.currSpeedLimit_i!==root.roundedSpeedLimit_i)
					if(root.currSpeedLimit_i>80)
						return T3Styling.cRed_c;
					else
						return T3Styling.cYellow_c;
				else
					return T3Styling.cFgMain_c;
			}
		}
	}
	Rectangle{
		id:rectangle_signalValue
		anchors{
			left:rect_leftPortion.right
			leftMargin: T3Styling.spacing_r
			right:root.right
			bottom: root.bottom
		}
		height:rect_leftPortion.squareSideLength_r*8+T3Styling.spacing_r*7
		border.width: T3Styling.lineWidth_r
		color: "transparent"
		border.color: T3Styling.cFgSubSub_c
		radius: T3Styling.lineWidth_r
		Column{
			id:column_signalOrganizer
			spacing: T3Styling.spacing_r
			anchors{
				//						horizontalCenter: rectangle_signalValue.horizontalCenter
				//						verticalCenter: rectangle_signalValue.verticalCenter
				margins: T3Styling.spacing_r
				fill:parent
			}
			Repeater{
				model:3
				delegate:Rectangle{
					height: (column_signalOrganizer.height-column_signalOrganizer.spacing*2)/3
					width: column_signalOrganizer.width
					color: "transparent"
					Rectangle{
						anchors{
							verticalCenter: parent.verticalCenter
							horizontalCenter: parent.horizontalCenter
						}
						width: text_commandedSpeedValueBack.paintedWidth
						height: width

//						anchors.fill: parent
						anchors.margins: T3Styling.lineWidth_r
						radius: width*0.5
						color: {
							if(index===0&&root.currSignal_s==="green")
								return T3Styling.cGreen_c
							else if(index===1&&root.currSignal_s==="yellow")
								return T3Styling.cYellow_c
							else if(index===2&&root.currSignal_s==="red")
								return T3Styling.cRed_c
							else
								return T3Styling.cFgSubSub_c
						}
						Behavior on color {PropertyAnimation{}}
					}
				}



			}
		}
	}


	implicitHeight: 500
	implicitWidth: 100
	//for testing only

}
