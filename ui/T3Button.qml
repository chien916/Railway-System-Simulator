import QtQuick 2.12
import QtQuick.Controls 2.15
Item {
	id:root
	implicitHeight: T3Styling.fontSubSub_r+T3Styling.spacing_r*2
	implicitWidth: 200
	property string buttonLabel_s :"buttonText"
	property color pressedColor_c:T3Styling.cBgMain_c
	property color releasedColor_c:T3Styling.cFgSubSub_c
	property color buttonTextColor_c:T3Styling.cFgMain_c
	property real buttonTextPixelSize_r:T3Styling.fontSubSub_r
	property bool delayButton_b:false
	signal buttonClicked()
	Rectangle{
		id:rect_buttonBackground
		color:{
			if(dBut_delayButton.hovered&&!dBut_delayButton.pressed)
				return pressedColor_c
			else
				return releasedColor_c
		}
		Behavior on color {PropertyAnimation{duration:100}}
		radius:T3Styling.margin_r
		anchors{
			fill: parent
		}
		Rectangle{
			anchors{
				top:parent.top
				left:parent.left
				bottom:parent.bottom
			}
			color: pressedColor_c
			radius:T3Styling.margin_r
			width: {
				let rawWidth_n = parent.width*dBut_delayButton.progress;
				if(rawWidth_n<radius*2) return radius*2;
				else return rawWidth_n;
			}

			visible:  dBut_delayButton.progress>0&&delayButton_b
		}
		T3Text{
			textContent_s: buttonLabel_s
			textPixelSize_r: root.buttonTextPixelSize_r
			textColor_c: root.buttonTextColor_c
			anchors{
				margins: T3Styling.spacing_r
				fill:parent
			}
		}
	}
//	Timer{
//		id:time_timer
//		interval: 300
//		onTriggered: dBut_delayButton.progress = 0;
//	}

	DelayButton{
		id:dBut_delayButton
		anchors.fill: rect_buttonBackground
		//		visible: delayButton_b
		opacity: 0
		delay:1000
		Behavior on progress {PropertyAnimation{duration:100}}
		hoverEnabled: true
		onActivated: {
			if(!delayButton_b) return;
			buttonClicked();
		}
		onClicked: {
			if(delayButton_b) return;
			buttonClicked();
		}
		//		MouseArea{
		//			id:mAre_mouseArea
		//			hoverEnabled: true
		//			anchors.fill: parent
		//			onClicked: {
		//				if(delayButton_b) return;
		//				buttonClicked();
		//				console.log("ho")
		//			}
		//		}
	}

}
