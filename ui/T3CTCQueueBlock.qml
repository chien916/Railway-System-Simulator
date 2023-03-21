import QtQuick 2.12

Item {
	id:root
	property variant dispatchQueue_OA :t3databaseQml.dispatchQueue_QML
		/* [
		{"trainId":"1234","origin":"B_A_12","destination":"A_B_21","time":"12:20","path":[]}
		,{"trainId":"1234","origin":"B_A_12","destination":"A_B_21","time":"12:20","path":[]}
		,{"trainId":"1234","origin":"B_A_12","destination":"A_B_21","time":"12:20","path":[]}
		,{"trainId":"1234","origin":"B_A_12","destination":"A_B_21","time":"12:20","path":[]}
		,{"trainId":"1234","origin":"B_A_12","destination":"A_B_21","time":"12:20","path":[]}
		,{"trainId":"1234","origin":"B_A_12","destination":"A_B_21","time":"12:20","path":[]}
		,{"trainId":"1234","origin":"B_A_12","destination":"A_B_21","time":"12:20","path":[]}
		,{"trainId":"1234","origin":"B_A_12","destination":"A_B_21","time":"12:20","path":[]}
		,{"trainId":"1234","origin":"B_A_12","destination":"A_B_21","time":"12:20","path":[]}
		,{"trainId":"1234","origin":"B_A_12","destination":"A_B_21","time":"12:20","path":[]}
		,{"trainId":"1234","origin":"B_A_12","destination":"A_B_21","time":"12:20","path":[]}
		,{"trainId":"1234","origin":"B_A_12","destination":"A_B_21","time":"12:20","path":[]}

	]*/

	/*t3databaseQml.dispatchQueue_QML*/
	Rectangle{
		anchors.fill: parent
		color: T3Styling.cBgSub_c
		radius: T3Styling.margin_r
	}
	Grid{
		id:colu_dataGridTitle
		rows:1
		columns: 6
		anchors{
			top:parent.top
			left:parent.left
			right:parent.right
		}
		height:T3Styling.fontSub_r
		anchors.margins: T3Styling.margin_r
		columnSpacing: T3Styling.spacing_r
		readonly property real unitWidth_r:
			(width-(columns-1)*columnSpacing)/columns
		Repeater{
			model:["Number","Train ID","Origin","Destination","Time",""]
			delegate: Rectangle{
				height: 20
				width: colu_dataGridTitle.unitWidth_r
				color:"transparent"
				radius: T3Styling.lineWidth_r
				T3Text{
					anchors.fill: parent
					textContent_s: modelData
					textColor_c: T3Styling.cFgMain_c
					textBold_b: true
					textPixelSize_r: T3Styling.fontSubSub_r
					width: parent.width
				}
			}
		}
	}
	Rectangle{
		id:rect_dataGridBackground
		anchors{
			top:colu_dataGridTitle.bottom
			topMargin: T3Styling.margin_r-T3Styling.spacing_r
			left:parent.left
			leftMargin: T3Styling.margin_r-T3Styling.spacing_r
			right:parent.right
			rightMargin: T3Styling.margin_r-T3Styling.spacing_r
			//bottom: parent.bottom
		}
		height: {
			let count_n = root.dispatchQueue_OA.length;
			let processedHeight_n
				= count_n*T3Styling.fontSub_r+(count_n+1)*T3Styling.spacing_r
			let maxHeight_n
			= root.height-colu_dataGridTitle.height-T3Styling.margin_r*3+T3Styling.spacing_r;
			return Math.min(maxHeight_n,processedHeight_n);
		}
		Behavior on height {PropertyAnimation{easing.type: Easing.OutCirc}}
		radius: T3Styling.lineWidth_r
		color: T3Styling.cFgSubSub_c
	}
	ListView{
		id:lVie_dataGrid
		clip: true
		anchors{
			top:colu_dataGridTitle.bottom
			left:parent.left
			right:parent.right
			bottom: parent.bottom
		}
		spacing: T3Styling.spacing_r
		anchors.margins: T3Styling.margin_r
		model:root.dispatchQueue_OA
		delegate: 	Rectangle{
			id:rect_dataGridWrapper
			width: parent.width
			height: T3Styling.fontSub_r
			color: "transparent"
			Grid{
				id:grid_dataGrid
				rows:1
				columns: 5
				width: parent.width
				visible: true
				anchors.margins: T3Styling.margin_r
				columnSpacing: T3Styling.spacing_r
				readonly property real unitWidth_r:
					(width-(columns)*columnSpacing)/(columns+1)
				Repeater{
					model: [index+1,modelData["trainId"]
						,modelData["origin"].split("_").join(" ")
						,modelData["destination"].split("_").join(" ")
						,modelData["time"]]
					delegate: Rectangle{
						height:T3Styling.fontSub_r
						width: grid_dataGrid.unitWidth_r
						color:T3Styling.cBgSub_c
						radius: T3Styling.lineWidth_r
						//border.width: T3Styling.lineWidth_r
						//border.color: T3Styling.cFgSubSub_c
						T3Text{
							anchors.fill: parent
							textContent_s: modelData
							textColor_c: T3Styling.cFgMain_c
						}
					}
				}

			}
			T3Button{
				x:grid_dataGrid.unitWidth_r*5+T3Styling.spacing_r*5
				buttonLabel_s: "Discard"
				buttonTextColor_c: "white"
				width: grid_dataGrid.unitWidth_r
				releasedColor_c: Qt.darker(T3Styling.cRed_c)
				height: T3Styling.fontSub_r
				onButtonClicked: t3databaseQml.ctc_discardDispatchRequest(index)
			}





		}

	}








	//--TESTING ONLY
	implicitHeight: 400
	implicitWidth: 500


}
