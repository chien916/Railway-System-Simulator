import QtQuick 2.12
import QtQuick.Extras 1.4
import QtQuick.Controls.Styles 1.4
Item {
	id:root
	property string gaugeUnit_s: "mph"
	property int maxValue_n: 100
	property int minValue_n: 0
	property real currValue_n: 88
	property bool textOnly_b: false
	property bool toggle_b: false
	property int fixedPoint_i:1
	implicitHeight: 150
	implicitWidth: 500
	property real tickmarkStepsize_r:10
	property real stepsize_r:1
	Rectangle{
		id:rectangle_container
		color: T3Styling.cBgMain_c
//		radius: T3Styling.margin_r
		anchors{
			horizontalCenter: parent.horizontalCenter
			verticalCenter: parent.verticalCenter
			fill: parent
		}
		Item{
			id:item_ratioKeeper
			anchors{
				horizontalCenter: parent.horizontalCenter
				verticalCenter: parent.verticalCenter
			}
			width: Math.min(parent.width,parent.height)
			height: width
			CircularGauge{
				anchors.fill: parent
				anchors.margins: parent.height*0.01
				stepSize: root.stepsize_r
				style: CircularGaugeStyle {

					tickmarkStepSize: root.tickmarkStepsize_r
						needle: Rectangle {
							y: outerRadius * 0.2
							implicitWidth: outerRadius * 0.05
							implicitHeight: outerRadius * 0.9
							antialiasing: true
							color: T3Styling.cFgMain_c
							radius: outerRadius * 0.05
							//opacity: textOnly_b?0:1
							Behavior on opacity {NumberAnimation{easing.type: Easing.InOutCubic}}
						}
						foreground: Rectangle {
								width: outerRadius * 0.2
								height: width
								radius: width / 2
								color: T3Styling.cFgMain_c
								anchors.centerIn: parent
								//opacity: textOnly_b?0:1
								Behavior on opacity {NumberAnimation{easing.type: Easing.InOutCubic}}
							}
						tickmark: Rectangle {
							visible: true
							implicitWidth: outerRadius * 0.02
							antialiasing: true
							implicitHeight: outerRadius * 0.06
							color: currValue_n>=styleData.value?T3Styling.cFgMain_c:T3Styling.cFgSubSub_c
							Behavior on color {NumberAnimation{easing.type: Easing.InOutCubic}}
							radius: outerRadius * 0.02
						}
						minorTickmark: Rectangle {
							visible: true
							implicitWidth: outerRadius * 0.01
							antialiasing: true
							implicitHeight: outerRadius * 0.03
							color: currValue_n>=styleData.value?T3Styling.cFgMain_c:T3Styling.cFgSubSub_c
							Behavior on color {NumberAnimation{easing.type: Easing.InOutCubic}}
							radius: outerRadius * 0.02
						}
						tickmarkLabel:  Text {
							font.pixelSize: Math.max(6, outerRadius * 0.13)
							font.family: "Inter"
							text: Math.round(styleData.value * 10) / 10
							color: currValue_n>=styleData.value?T3Styling.cFgMain_c:T3Styling.cFgSubSub_c
							Behavior on color {NumberAnimation{easing.type: Easing.InOutCubic}}
							antialiasing: true
							//opacity: textOnly_b?0:1
							Behavior on opacity {NumberAnimation{}}
						}
					}
				value: currValue_n
				Behavior on value {NumberAnimation{}}
				maximumValue: root.maxValue_n
				minimumValue: root.minValue_n
			}
			Rectangle{
				anchors.verticalCenter: parent.verticalCenter
				anchors.horizontalCenter: parent.horizontalCenter
				width: parent.height*0.5
				height:parent.height*0.35
				color: T3Styling.cBgMain_c
				radius: width
				border.color: T3Styling.cFgSubSub_c
				border.width: T3Styling.lineWidth_r*0.5
			}
			T3Text{
				anchors.fill: parent
				anchors.margins: root.height*0.2
				textPixelSize_r: T3Styling.fontSubSub_r*1.8
				textColor_c: T3Styling.cFgMain_c
				//opacity: !textOnly_b?0:1
				textContent_s: currValue_n.toFixed(root.fixedPoint_i)
				textBold_b: true
			}
			T3Text{
				anchors.bottom: parent.bottom
				anchors.left: parent.left
				anchors.right: parent.right
				textPixelSize_r: T3Styling.fontSubSub_r
				textColor_c: T3Styling.cFgMain_c
				textContent_s: gaugeUnit_s
			}
			Text{
				anchors{
					horizontalCenter: parent.horizontalCenter
					bottom: parent.bottom
					margins: parent.height*0.05
				}
				height:  item_ratioKeeper.width*0.1
				text:"~~~"
				font.family: "DSEG14 Classic"
				visible: false
				font.pixelSize: T3Styling.fontMain_r
				fontSizeMode: Text.Fit
				color: T3Styling.cFgSub_c
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
				opacity: 0.15
			}
		}
	}
//	Timer{
//		running: toggle_b
//		repeat: true
//		onTriggered: textOnly_b = !textOnly_b;
//		interval: 3000
//	}
}
