import QtQuick 2.12
import QtQuick.Controls 2.15
Item {
	id:root
	implicitHeight: 200
	implicitWidth: 200
	property variant ticks_nA: [0.64,0.82,1]
	property bool enabled_b:true
	property alias dialDialValue_r:dial_dial.value
	property real currValue_n :{
		if(ticks_nA.length===0) return 0 ;
		//		return 0;
		let ticksCopy_nA = [...ticks_nA];
		ticksCopy_nA.sort((a_n,b_n)=>{
							  return Math.abs(dial_dial.value-a_n)
							  -Math.abs(dial_dial.value-b_n);
						  });
		return ticksCopy_nA[0];
	}
	Rectangle{
		anchors.fill: parent
		color:T3Styling.cBgSub_c
	}
	Dial{
		id:dial_dial
		width: root.width
		height: root.height
		enabled: ticks_nA.length>0&&enabled_b
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		stepSize:0.5
		background: Rectangle {
			id:rect_dialBackground
			x: dial_dial.width / 2 - width / 2
			y: dial_dial.height / 2 - height / 2
			width:dial_dial.width*0.7
			height: width
			color: "transparent"
			radius: width / 2
			border.width: T3Styling.spacing_r
			border.color: dial_dial.pressed
						  ? T3Styling.cFgSub_c
						  : T3Styling.cFgSubSub_c
			Behavior on border.color{PropertyAnimation{}}
			opacity: dial_dial.enabled ? 1 : 0.3
		}
		handle:Item{}
		onPressedChanged: {
			if(pressed) return;
			dial_dial.value = currValue_n;
		}
	}
	Rectangle{
		anchors{
			fill: dial_dial
		}
		color:"blue"
		opacity:0.0
	}
	Rectangle{
		id:rect_throttleBar
		x:rect_center.x+rect_center.width/2
		y:rect_center.y+rect_center.height/2-height/2
		color: dial_dial.enabled?T3Styling.cFgSub_c:T3Styling.cFgSubSub_c
		Behavior on color{PropertyAnimation{easing.type: Easing.OutCirc}}
		height: dial_dial.width*0.15
		width: dial_dial.width*0.475
		radius: height
		opacity: root.ticks_nA.length>0?1:0
		transform: Rotation{
			origin.y:rect_throttleBar.height/2
			angle:130+280*currValue_n
			Behavior on angle{PropertyAnimation{}}
		}
	}
	Rectangle{
		id:rect_center
		anchors{
			horizontalCenter: dial_dial.horizontalCenter
			verticalCenter: dial_dial.verticalCenter
		}
		color: dial_dial.enabled?T3Styling.cFgMain_c:T3Styling.cFgSub_c
		Behavior on color{PropertyAnimation{easing.type: Easing.OutCirc}}
		width: dial_dial.width*0.25
		height:dial_dial.height*0.25
		radius: width
		T3Text{
			id:text_rotation
			textContent_s: "↻"
			anchors.fill: parent
	//			anchors.verticalCenter: parent.verticalCenter
	//			anchors.horizontalCenter: parent.horizontalCenter
			textColor_c: T3Styling.cFgSubSub_c
			textPixelSize_r: T3Styling.fontSub_r
			opacity:root.ticks_nA.length>0?1:0
			transform: Rotation{
				origin.x:text_rotation.width/2
				origin.y:text_rotation.height/2
				angle: 130+280*currValue_n
				Behavior on angle{PropertyAnimation{easing.type: Easing.InCubic}}
			}
		}
	}
//	Component{
//		id:comp_tickLabel
//		Rectangle{
//			id:rect_tickLabel
//			property string tickLabelText_s:"24"
//			x:dial_dial.x+dial_dial.height/2-height/2
//			y:dial_dial.y+dial_dial.height/2-height/2
//			width: dial_dial.width/2+T3Styling.spacing_r*2
//			radius: width
//			height: T3Styling.fontSub_r
//			transform: Rotation{
//				origin.x:parent.height/2
//				origin.y:parent.height/2
//				angle: 130+280*currValue_n
//				Behavior on angle{PropertyAnimation{easing.type: Easing.InCubic}}
//			}
//			color:"transparent"
//			Rectangle{
//				x:parent.height/2+rect_center.width/2+T3Styling.spacing_r
//				anchors.verticalCenter: parent.verticalCenter
//				width:rect_dialBackground.width/2- rect_center.width/2-T3Styling.spacing_r*3
//				height: T3Styling.lineWidth_r*1.5
//				color: T3Styling.cFgSub_c
//				radius: height
//			}
//			T3Text{
//				id:text_tickLabel
//				textContent_s: parent.tickLabelText_s
//				textColor_c: T3Styling.cFgSub_c
//				anchors{
//					top:parent.top
//					right:parent.right
//					bottom:parent.bottom
//				}
//				width: parent.height
//				transform:Rotation{
//					origin.x:text_tickLabel.width/2
//					origin.y:text_tickLabel.height/2
//					angle: -(130+280*currValue_n)
//				}
//			}
//		}
//	}
}
