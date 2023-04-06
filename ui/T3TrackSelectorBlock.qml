import QtQuick 2.12
import QtQuick.Controls 2.15
Item {
	id:root
	implicitWidth: 500
	implicitHeight: 500
	property bool clickedSomewhereElse_b:false
	property string currValue_s:""
	property bool isTrack_b:true
	property variant model_A:{
		[["a","b"],["c","d"]]
	}
	property int currColumn_i:0
	Rectangle{
		anchors.fill: parent
		color: T3Styling.cBgSub_c
	}
	T3Text{
		id:text_selectTrainTrackLabel
		height: T3Styling.fontSub_r
		width:parent.width
		textPixelSize_r: T3Styling.fontSub_r
		textContent_s: "Select "+(isTrack_b?"Track":"Train")
		textColor_c: T3Styling.cFgSub_c
		textBold_b: true
	}
	ListView{
		id:lVie_listView
		anchors{
			top:text_selectTrainTrackLabel.bottom
			topMargin: T3Styling.spacing_r
			left:parent.left
			right:parent.right
			bottom:button_left.top
			bottomMargin: T3Styling.spacing_r
		}
		ScrollBar.vertical: ScrollBar {
			active: true
		}
		currentIndex: -1
		clip: true
		model:root.model_A[currColumn_i]
		delegate: Item{
			height: 20
			width: root.width
			MouseArea{
				anchors.fill: parent
				Rectangle{
					anchors.fill: parent
					color: {
						if(!clickedSomewhereElse_b&&lVie_listView.currentIndex===index) return T3Styling.cFgSub_c
						else if(parent.containsMouse) return T3Styling.cBgMain_c
						else return "transparent"
					}
					Behavior on color{PropertyAnimation{easing.type: Easing.OutCirc}}
					radius: T3Styling.spacing_r
					T3Text{
						anchors.fill: parent
						textContent_s:modelData.split("_").join(" ")
						textColor_c: (lVie_listView.currentIndex===index)
									 ?T3Styling.cBgMain_c:T3Styling.cFgSub_c
					}
				}
				hoverEnabled: true
				onClicked: {
					if(lVie_listView.currentIndex === index){
						lVie_listView.currentIndex = -1
						root.currValue_s = ""
					}else {
						lVie_listView.currentIndex = index
						root.currValue_s = modelData
						clickedSomewhereElse_b = false
					}
				}
			}
		}
		focus: true
	}
	T3Button{
		id:button_left
		anchors{
			bottom:parent.bottom
		}
		visible: root.model_A.length>1
		height: root.isTrack_b?T3Styling.margin_r:0
		width: (root.width-T3Styling.spacing_r)/2
		buttonLabel_s: "←"
		onButtonClicked: if(currColumn_i>0) {
							 --currColumn_i;
							 currValue_s = "";
							 lVie_listView.currentIndex = -1;
						 }
	}
	T3Button{
		anchors{
			bottom:parent.bottom
		}
		x:width+T3Styling.spacing_r
		visible: root.model_A.length>1
		height: root.isTrack_b?T3Styling.margin_r:0
		width: (root.width-T3Styling.spacing_r)/2
		buttonLabel_s: "→"
		onButtonClicked: if(currColumn_i<model_A.length-1){
							 ++currColumn_i;
							 currValue_s = "";
							 lVie_listView.currentIndex = -1;
						 }
	}

}
