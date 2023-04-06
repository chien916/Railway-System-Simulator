import QtQuick 2.12
import QtQuick.Dialogs 1.3
Item {
	implicitHeight: 500
	implicitWidth: 500
	property int clockRate_i:Math.floor(pUni_clockRate.actualValue_r)
	property bool automaticMode_b:pUni_automaticMode.valueratio_r>0.5
	signal menuButtonClicked(metaInfo_s:string)
	Rectangle{
		id:rect_canvas
		radius: T3Styling.margin_r
		color: T3Styling.cBgSub_c
		anchors.fill: parent
		Column{
			id:colu_column
			property int count_i :8
			readonly property real unitHeight_r
			:(rect_canvas.height-2*colu_column.anchors.margins
			  - (count_i-1)*colu_column.spacing)/count_i
			spacing: T3Styling.margin_r
			anchors.fill: parent
			anchors.margins: T3Styling.margin_r
			T3ParamUnit{
				id:pUni_clockRate
				width: colu_column.width
				height: colu_column.unitHeight_r*2+colu_column.anchors.margins
				paramConfig_A: "F_F_Clock Rate_0 x"
				readOnly_b: false
				minValue_r:1
				fixedPoint_i: 1
				valueratio_r: 0
				maxValue_r: 11
			}
			T3ParamUnit{
				id:pUni_automaticMode
				width: colu_column.width
				height: colu_column.unitHeight_r*2+colu_column.anchors.margins
				readOnly_b: false
				paramConfig_A: "F_T_CTC Master Switch_"
				onValueratio_rChanged: {
					if(valueratio_r===0) t3databaseQml.ctc_toggleConnection(false);
					else if(valueratio_r===1 ) t3databaseQml.ctc_toggleConnection(true);
					console.log(valueratio_r)
				}
			}
			T3Button{
				width: colu_column.width
				height: colu_column.unitHeight_r
				buttonLabel_s: "Request Dispatch"
				onButtonClicked: menuButtonClicked("manuallyDispatch")
			}
			T3Button{
				width: colu_column.width
				height: colu_column.unitHeight_r
				buttonLabel_s: "View Dispatch Queue"
				onButtonClicked: menuButtonClicked("viewDispatchQueue");
			}
			T3Button{
				width: colu_column.width
				height: colu_column.unitHeight_r
				buttonLabel_s: "Debugging Interface"
				onButtonClicked: {
					Qt.openUrlExternally("https://console.firebase.google.com/project/sprn2023-ece1140/database/sprn2023-ece1140-default-rtdb/data")
//					var component = Qt.createComponent("qrc:/T3CTCTestInterface.qml")
//					var window    = component.createObject(root)
//					window.show()
				}
			}

			T3Button{
				width: colu_column.width
				height: colu_column.unitHeight_r
				buttonLabel_s: "Quit Software"
				onButtonClicked: Qt.callLater(Qt.quit)
			}



		}



	}

}
