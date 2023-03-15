import QtQuick 2.12

Item {
	id:root
	implicitHeight: text_frontText.paintedHeight+T3Styling.margin_r
	implicitWidth: text_frontText.paintedWidth+T3Styling.margin_r
	property int segCharCount_i:20
	property string idleText_s:"IDLE TEXT HERE"
	property variant contentsQueue_sA:[]
	property bool centered_b:true
	property bool shift_b: true
	property string currContent_s: idleText_s
	property bool dark_b:true
	Rectangle{
		anchors.fill: parent
		color: T3Styling.cBgMain_c
		radius: T3Styling.spacing_r
		border.color: dark_b?T3Styling.cFgSubSub_c:T3Styling.cBgSub_c
		border.width: T3Styling.lineWidth_r
	}
	Text{
		id:text_frontText
		text:
			(centered_b?("!".repeat
						 (Math.floor((root.segCharCount_i-currContent_s.length)/2))
						 + currContent_s
						 +"!".repeat(Math.ceil((root.segCharCount_i-currContent_s.length)/2)))
					   :currContent_s.padEnd(root.segCharCount_i,"!"))
		.substring(0,root.segCharCount_i)
		.replace(/ /g,'!')
		.substring(0,root.segCharCount_i)
		font.family: "DSEG14 Classic"
		font.pixelSize: T3Styling.fontSubSub_r*1.4
		color: T3Styling.cFgMain_c
		anchors.fill: parent
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
	}
	Text{
		text:"".padStart(root.segCharCount_i,"~")
		font.family: "DSEG14 Classic"
		font.pixelSize: T3Styling.fontSubSub_r*1.4
		color: T3Styling.cFgMain_c
		anchors.fill: parent
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment: Text.AlignVCenter
		opacity: 0.2
	}
	Timer{
		property int counter_i: 0;
		id:time_charTransition
		repeat: true
		running: counter_i>0;
		interval: 500
		onTriggered: {
			counter_i
		}
	}
	Timer{
		id:time_queueUpdater
		interval: 3000
		running: true
		repeat: true
		onTriggered: {
			if(contentsQueue_sA.length>0){
				currContent_s = contentsQueue_sA.shift();
				if(!shift_b)
					contentsQueue_sA.push(currContent_s);
				text_frontText.color = T3Styling.cFgMain_c
			}else{
				if(currContent_s===idleText_s)
					currContent_s = Qt.formatDateTime(new Date(), "yyyy/mm/dd hh-mm-ss")
				else
					currContent_s = idleText_s
				text_frontText.color = T3Styling.cFgSub_c
			}
		}
	}
}
