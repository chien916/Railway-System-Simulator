import QtQuick 2.12

Item {
	id:root
	implicitHeight: item_canvas.unitHeight_r*3+T3Styling.spacing_r*4
	implicitWidth: 500
	property real r_r:1.0
	property real y_r:1.0
	property real e_r:1.0
	property real p_r:1.0
	property real i_r:1.0
	property real d_r:1.0
	property real u_r:1.0
//	onData_OChanged: {
//		if(load_rInPid.status !==Loader.Ready
//				||load_eInPid.status !==Loader.Ready
//				||load_pInPid.status !==Loader.Ready
//				||load_iInPid.status !==Loader.Ready
//				||load_dInPid.status !==Loader.Ready
//				||load_uInPid.status !==Loader.Ready
//				||load_yInPid.status !==Loader.Ready)
//			return;
//		load_rInPid.item.value_s = data_O["r_r"];
//		load_eInPid.item.value_s = data_O["e_r"];
//		load_pInPid.item.value_s = data_O["p_r"];
//		load_iInPid.item.value_s = data_O["i_r"];
//		load_dInPid.item.value_s = data_O["d_r"];
//		load_uInPid.item.value_s = data_O["u_r"];
//		load_yInPid.item.value_s = data_O["y_r"];
//	}
	Rectangle{
		anchors.fill: parent
		color: T3Styling.cFgSubSub_c
		radius: T3Styling.lineWidth_r
	}
	Item{
		id:item_canvas
		anchors.fill: parent
		anchors.margins: T3Styling.spacing_r
		readonly property real circleWidth_r: T3Styling.fontSubSub_r+T3Styling.spacing_r*2
		readonly property real rectangleWidth_r:T3Styling.fontSubSub_r*6
		readonly property real unitHeight_r:T3Styling.fontSubSub_r+T3Styling.spacing_r*2
		readonly property real rowSpacing_r:(width-rectangleWidth_r*4-circleWidth_r*2)/5
		Column{
			anchors.fill: parent
			spacing: T3Styling.spacing_r
			Rectangle{
				id:rect_row1
				color: "transparent"
				height:item_canvas.unitHeight_r
				width: parent.width
				Loader{
					//id:arrow_1,1
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							1*item_canvas.circleWidth_r
							+2*item_canvas.rectangleWidth_r
							+2*item_canvas.rowSpacing_r
					}
					width: item_canvas.rowSpacing_r
					sourceComponent: comp_arrow
					onLoaded:item.textContent_s = "↗"
				}
				Loader{
					id:load_pInPid
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							1*item_canvas.circleWidth_r
							+2*item_canvas.rectangleWidth_r
							+3*item_canvas.rowSpacing_r
					}
					width: item_canvas.rectangleWidth_r
					sourceComponent: comp_rect
					onLoaded: {
						item.label_s = "P"
						item.labelColor_c = T3Styling.cGreen_c
						item.value_s = "N/A"
						item.border_b = true
					}
				}
				Loader{
					//id:arrow_1,2
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							1*item_canvas.circleWidth_r
							+3*item_canvas.rectangleWidth_r
							+3*item_canvas.rowSpacing_r
					}
					width: item_canvas.rowSpacing_r
					sourceComponent: comp_arrow
					onLoaded:item.textContent_s = "↘"
				}
			}
			Rectangle{
				id:rect_row2
				color: "transparent"
				height: item_canvas.unitHeight_r
				width: parent.width
				Loader{
					id:load_rInPid
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							0*item_canvas.circleWidth_r
							+0*item_canvas.rectangleWidth_r
							+0*item_canvas.rowSpacing_r
					}
					width: item_canvas.rectangleWidth_r
					sourceComponent: comp_rect
					onLoaded: {
						item.label_s = "R"
						item.labelColor_c = T3Styling.cFgSub_c
						item.value_s = "N/A"
						item.border_b = true
					}
				}
				Loader{
					//id:load_arraw1
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							0*item_canvas.circleWidth_r
							+1*item_canvas.rectangleWidth_r
							+0*item_canvas.rowSpacing_r
					}
					width: item_canvas.rowSpacing_r
					sourceComponent: comp_arrow
				}
				Loader{
					//id:load_leftSumInPid
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							0*item_canvas.circleWidth_r
							+1*item_canvas.rectangleWidth_r
							+1*item_canvas.rowSpacing_r
					}
					width: item_canvas.circleWidth_r
					sourceComponent: comp_circle
				}
				Loader{
					//id:load_arraw2,2
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							1*item_canvas.circleWidth_r
							+1*item_canvas.rectangleWidth_r
							+1*item_canvas.rowSpacing_r
					}
					width: item_canvas.rowSpacing_r
					sourceComponent: comp_arrow

				}
				Loader{
					id:load_eInPid
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							1*item_canvas.circleWidth_r
							+1*item_canvas.rectangleWidth_r
							+2*item_canvas.rowSpacing_r
					}
					width: item_canvas.rectangleWidth_r
					sourceComponent: comp_rect
					onLoaded: {
						item.label_s = "E"
						item.labelColor_c = T3Styling.cFgSub_c
						item.value_s = "N/A"
						item.border_b = true
					}
				}
				Loader{
					//id:load_arraw2,3
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							1*item_canvas.circleWidth_r
							+2*item_canvas.rectangleWidth_r
							+2*item_canvas.rowSpacing_r
					}
					width: item_canvas.rowSpacing_r
					sourceComponent: comp_arrow
				}

				Loader{
					id:load_iInPid
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							1*item_canvas.circleWidth_r
							+2*item_canvas.rectangleWidth_r
							+3*item_canvas.rowSpacing_r
					}
					width: item_canvas.rectangleWidth_r
					sourceComponent: comp_rect
					onLoaded: {
						item.label_s = "I"
						item.labelColor_c = T3Styling.cBlue_c
						item.value_s = "N/A"
						item.border_b = true
					}
				}

				Loader{
					//id:load_arraw2,4
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							1*item_canvas.circleWidth_r
							+3*item_canvas.rectangleWidth_r
							+3*item_canvas.rowSpacing_r
					}
					width: item_canvas.rowSpacing_r
					sourceComponent: comp_arrow
				}

				Loader{
					//id:load_leftSumInPid
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							1*item_canvas.circleWidth_r
							+3*item_canvas.rectangleWidth_r
							+4*item_canvas.rowSpacing_r
					}
					width: item_canvas.circleWidth_r
					sourceComponent: comp_circle
				}

				Loader{
					//id:load_arraw2,5
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							2*item_canvas.circleWidth_r
							+3*item_canvas.rectangleWidth_r
							+4*item_canvas.rowSpacing_r
					}
					width: item_canvas.rowSpacing_r
					sourceComponent: comp_arrow
				}

				Loader{
					id:load_uInPid
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							2*item_canvas.circleWidth_r
							+3*item_canvas.rectangleWidth_r
							+5*item_canvas.rowSpacing_r
					}
					width: item_canvas.rectangleWidth_r
					sourceComponent: comp_rect
					onLoaded: {
						item.label_s = "U"
						item.labelColor_c = T3Styling.cFgSub_c
						item.value_s = "N/A"
						item.border_b = true
					}
				}

			}
			Rectangle{
				id:rect_row3
				color: "transparent"
				height: item_canvas.unitHeight_r
				width: parent.width
				Loader{
					id:load_yInPid
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							0*item_canvas.circleWidth_r
							+0*item_canvas.rectangleWidth_r
							+0*item_canvas.rowSpacing_r
					}
					width: item_canvas.rectangleWidth_r
					sourceComponent: comp_rect
					onLoaded: {
						item.label_s = "Y"
						item.labelColor_c = T3Styling.cFgSub_c
						item.value_s = "N/A"
						item.border_b = true
					}
				}
				Loader{
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							0*item_canvas.circleWidth_r
							+1*item_canvas.rectangleWidth_r
							+0*item_canvas.rowSpacing_r
					}
					width: item_canvas.rowSpacing_r
					sourceComponent: comp_arrow
					onLoaded:item.textContent_s = "⤯"
				}
				Loader{
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							1*item_canvas.circleWidth_r
							+2*item_canvas.rectangleWidth_r
							+2*item_canvas.rowSpacing_r
					}
					width: item_canvas.rowSpacing_r
					sourceComponent: comp_arrow
					onLoaded:item.textContent_s = "↘"
				}

				Loader{
					id:load_dInPid
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							1*item_canvas.circleWidth_r
							+2*item_canvas.rectangleWidth_r
							+3*item_canvas.rowSpacing_r
					}
					width: item_canvas.rectangleWidth_r
					sourceComponent: comp_rect
					onLoaded: {
						item.label_s = "D"
						item.labelColor_c = T3Styling.cYellow_c
						item.value_s = "N/A"
						item.border_b = true
					}
				}

				Loader{
					anchors{
						top:parent.top
						bottom: parent.bottom
						left:parent.left
						leftMargin:
							1*item_canvas.circleWidth_r
							+3*item_canvas.rectangleWidth_r
							+3*item_canvas.rowSpacing_r
					}
					width: item_canvas.rowSpacing_r
					sourceComponent: comp_arrow
					onLoaded:item.textContent_s = "↗"
				}
			}
		}

		Component{
			id:comp_arrow
			T3Text{
				textContent_s: "→"
				textColor_c: T3Styling.cFgMain_c
				textPixelSize_r: T3Styling.fontSubSub_r
			}
		}

		Component{
			id:comp_rect
			Rectangle{
				property string label_s:"P"
				property color labelColor_c:T3Styling.cBlue_c
				property string value_s:"str"
				property bool border_b:true
				height: item_canvas.unitHeight_r
				width: item_canvas.rectangleWidth_r
				radius: T3Styling.lineWidth_r
				color: T3Styling.cBgMain_c
				//border.width: T3Styling.lineWidth_r*0.5
				//border.color:border_b?T3Styling.cBgSub_c:"transparent"
				T3Text{
					anchors{
						left: parent.left
						right: parent.right
						top:parent.top
						bottom:parent.bottom
						margins: T3Styling.spacing_r*2
					}
					textContent_s: parent.label_s
					textBold_b: true
					textAlign_s: "left"
					textColor_c: parent.labelColor_c
					textPixelSize_r: T3Styling.fontSubSub_r
				}
				T3Text{
					anchors{
						left: parent.left
						right: parent.right
						top:parent.top
						bottom:parent.bottom
						margins: T3Styling.spacing_r*2
					}
					textContent_s: parent.value_s
					textAlign_s: "right"
					textColor_c: T3Styling.cFgMain_c
					textPixelSize_r: T3Styling.fontSubSub_r
				}
			}

		}
		Component{
			id:comp_circle
			Rectangle{
				height: item_canvas.unitHeight_r
				width: item_canvas.circleWidth_r
				radius: height
				color: T3Styling.cBgMain_c
				//border.width: T3Styling.lineWidth_r*0.5
				//border.color:T3Styling.cBgSub_c
				T3Text{
					anchors{
						left: parent.left
						right: parent.right
						top:parent.top
						bottom:parent.bottom
						margins: T3Styling.spacing_r*2
					}
					textContent_s: "Σ"
					textColor_c: T3Styling.cFgSub_c
					textPixelSize_r: T3Styling.fontSubSub_r
				}
			}
		}

	}


	Timer{
		interval: 50
		running: true
		repeat: true
		onTriggered: {
//			console.log(load_rInPid.item.value_s)
			load_rInPid.item.value_s = r_r;
			load_eInPid.item.value_s = e_r;
			load_pInPid.item.value_s = p_r;
			load_iInPid.item.value_s = i_r;
			load_dInPid.item.value_s = d_r;
			load_uInPid.item.value_s = u_r;
			load_yInPid.item.value_s = y_r;
		}
	}
}
