import QtQuick 2.12

Item {
	id:root
	property bool leftDoorClosed_b: true
	property bool rightDoorClosed_b: false
	property bool brakeReleased_b: true
	property bool brakeEmergency_b: false
	Rectangle{
		anchors{
			fill: parent
		}
//		radius: T3Styling.margin_r
		color: "transparent"
		//color:T3Styling.cBgMain_c
		radius: T3Styling.spacing_r
//		Text{
//			anchors{
//				top:rect_top.bottom
//				bottom: rect_bottom.top
//				left:parent.left
//				right:parent.right
//			}
////			width: parent.width
////			height: parent.height*0.2
////			x:parent.x
////			y:parent.y+parent.height*0.5-height/2
//			text:"Door Status :<font color=\"#ffff00\">Open</font> /
//<font color=\"#00ff00\">Closed</font>
//<br/>Brake Status :
//<font color=\"#ffff00\">Applied</font> /
//<font color=\"#00ff00\">Released</font> /
//<font color=\"#ff0000\">Emergency</font>"
//			font.family: "Inter"
//			font.pixelSize: T3Styling.fontSubSub_r
//			fontSizeMode: Text.Fit
////			font.letterSpacing: T3Styling.lineWidth_r
//			lineHeight: T3Styling.lineWidth_r*0.5
//			horizontalAlignment: Text.AlignHCenter
//			verticalAlignment: Text.AlignVCenter
//			color: T3Styling.cFgSub_c
//		}
		Rectangle{
			id:rect_top
			height: (root.height-T3Styling.spacing_r)/2
			width: root.width
			color: "transparent"
			Repeater{
				//number of door sides in this car
				model:[1,2,0,2,1]
				delegate:Rectangle{
					border.width: T3Styling.lineWidth_r*0.5
					border.color: T3Styling.cFgSub_c
					color: "transparent"
					height: rect_top.height
					width: (parent.width-4*T3Styling.spacing_r)/5
					radius: T3Styling.spacing_r
					x: parent.x+(width+T3Styling.spacing_r)*index
					y: 0/*parent.y+parent.height*0.25-height*0.5*/
					Grid{
						id:grid_doorStatusCarView
						rows:2
						visible: modelData>0
						columns: modelData
						rowSpacing:T3Styling.spacing_r
						columnSpacing: T3Styling.spacing_r
						anchors{
							fill: parent
							leftMargin: columnSpacing
							topMargin: rowSpacing
						}
						Repeater{
							model:grid_doorStatusCarView.rows*grid_doorStatusCarView.columns
							Rectangle{
	//							border.width: T3Styling.lineWidth_r
	//							border.color: T3Styling.cFgSub_c
								color: (index<grid_doorStatusCarView.columns?leftDoorClosed_b:rightDoorClosed_b)?T3Styling.cGreen_c:T3Styling.cYellow_c
								Behavior on color {PropertyAnimation{}}
								width: (grid_doorStatusCarView.width-grid_doorStatusCarView.columnSpacing*(grid_doorStatusCarView.columns))/grid_doorStatusCarView.columns
								height: (grid_doorStatusCarView.height-grid_doorStatusCarView.rowSpacing*(grid_doorStatusCarView.rows))/grid_doorStatusCarView.rows
								radius: T3Styling.spacing_r
							}
						}

					}
				}
			}
		}
		Rectangle{
			id:rect_bottom
			anchors.bottom: parent.bottom
			anchors.left: parent.left
			anchors.right: parent.right
			height: (root.height-T3Styling.spacing_r)/2
			color: "transparent"
			Repeater{
				//number of brake sides in this car
				model:[2,0,2,0,2]
				delegate:Rectangle{
					border.width: T3Styling.lineWidth_r*0.5
					border.color: T3Styling.cFgSub_c
					color: "transparent"
					height: rect_bottom.height
					width: (parent.width-4*T3Styling.spacing_r)/5
					radius:T3Styling.spacing_r
					x: parent.x+(width+T3Styling.spacing_r)*index
					y: parent.height-height
					Grid{
						id:grid_brakeStatusCarView
						rows:2
						visible: modelData>0
						columns: modelData
						rowSpacing:T3Styling.spacing_r
						columnSpacing: T3Styling.spacing_r
						anchors{
							fill: parent
							leftMargin: columnSpacing
							topMargin: rowSpacing
						}
						Repeater{
							model:grid_brakeStatusCarView.rows*grid_brakeStatusCarView.columns
							Rectangle{
	//							border.width: T3Styling.lineWidth_r
	//							border.color: T3Styling.cFgSub_c
								color: {
									if(root.brakeEmergency_b)
										return T3Styling.cRed_c;
									else if(brakeReleased_b)
										return T3Styling.cGreen_c;
									else
										return T3Styling.cYellow_c;
								}
								Behavior on color {ColorAnimation{}}
								width: (grid_brakeStatusCarView.width-grid_brakeStatusCarView.columnSpacing*(grid_brakeStatusCarView.columns))/grid_brakeStatusCarView.columns
								height: (grid_brakeStatusCarView.height-grid_brakeStatusCarView.rowSpacing*(grid_brakeStatusCarView.rows))/grid_brakeStatusCarView.rows
								radius: T3Styling.spacing_r
							}
						}

					}
				}
			}
		}

	}
	implicitHeight: 400
	implicitWidth: 700
}
