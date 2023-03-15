import QtQuick 2.12

Item {
	id:root
	implicitHeight: 30
	implicitWidth: 100
	property real textPixelSize_r : T3Styling.fontSubSub_r
	property color textColor_c: T3Styling.cFgSubSub_c
	property real  textLetterSpacing_r: 0
	property string textContent_s: "test"
	property color bgColor_c: "transparent"
	property string textAlign_s:""
	property bool textBold_b: false
	property bool textItalic_b:false
	Rectangle{
		anchors.fill: root
		radius: T3Styling.spacing_r
		color: bgColor_c
	}
	Text{
		text:root.textContent_s
		anchors{
			fill: root
			margins: T3Styling.lineWidth_r
		}
		font.pixelSize: root.textPixelSize_r
		color: root.textColor_c
		font.letterSpacing: root.textLetterSpacing_r
		font.family: "Inter"
		font.italic: root.textItalic_b
		font.bold: root.textBold_b
		fontSizeMode: Text.Fit
		horizontalAlignment: {
			if(textAlign_s==="left")
				return Text.AlignLeft
			else if(textAlign_s==="right")
				return Text.AlignRight
			else return Text.AlignHCenter
		}
		verticalAlignment: Text.AlignVCenter
	}
}
