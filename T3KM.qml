import QtQuick 2.12
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
Item {
	id:root
	implicitHeight: 500
	implicitWidth: 700
	property string blockId_s:"G_C_12"
	property variant  blockConstantsObject_O:t3databaseQml.trackConstantsObjects_QML[1]["blocksMap"][blockId_s]
	property variant blockVariablesObject_O:t3databaseQml.trackConstantsObjects_QML[1]["blocksMap"][blockId_s]
	property variant kmInfoModel_A :
		[["Track Data"
		  ,["T_F_Grade_%"
			,"T_F_Elevation_ft"
			,"T_F_Length_ft"
			,"T_F_Speed Limit_mph"
			,"T_F_Direction Of Travel_"
			,"T_F_Crossing_"
			,"T_F_Heaters_"
			,"T_F_Train Occupancy_"]]
		,["Station Information"
		  ,["T_F_Name_%"
			,"T_F_Side_ft"
			,"T_F_Passangers On Station_"
			,"T_F_Passangers Boarding_"
			,"T_F_Passangers Disembarking_"]
		]]
	property variant kmCtrlModel_A :
		[["Failure Mode"
		  ,["F_T_Broken Rail_"
			,"F_T_Circuit Failure_"
			,"F_T_Power Failure_"
		  ]]
		,["Environment"
		  ,["F_F_Temperature_F"]
		]]
	//	property variant currBlockDataModel_A :
	//		[["grade","%"]
	//		,["elevation","m"]
	//		,["length","m"]
	//		,["speed limit","kph"]
	//		,["direction of travel",""]
	//		,["track heaters","%"]
	//		,["train occupancy",""]
	//	];

	Rectangle{
		color: T3Styling.cBgSub_c
		anchors.fill: root
		radius: T3Styling.margin_r
		T3Text{
			anchors.bottom: parent.bottom
			anchors.right: parent.right
			anchors.rightMargin: T3Styling.margin_r
			textPixelSize_r: T3Styling.fontSubSub_r
			height: T3Styling.margin_r
			width: parent.width
			textContent_s: "T3 Track Model | Mark Roberts"
			textAlign_s: "right"
			textBold_b: true
		}
	}
	Item{
		id:item_canvas
		anchors.fill: parent
		anchors.margins: T3Styling.margin_r
//		T3SegDisplay{
//			id:segD_segD
//			width: item_canvas.width
//			idleText_s: "T3 Track Model"
//			segCharCount_i: 30
//		}

        //Entire Rail block that was previously on the CTC office and is now on the Track model
		T3CTCRailBlock{
			anchors{
				top: parent.top
				//topMargin: T3Styling.margin_r
				right:rect_config.left
				rightMargin: T3Styling.margin_r
				left:item_canvas.left
				bottom: item_canvas.bottom
			}
			blockConstantsObject_O:root.blockConstantsObject_O
			blockVariablesObject_O:root.blockVariablesObject_O
			blockId_s: "G_C_12"
		}

		//		T3CTCLineGrid{
		//			anchors{
		//				top: segD_segD.bottom
		//				topMargin: T3Styling.margin_r
		//				right:rect_config.left
		//				rightMargin: T3Styling.margin_r
		//				left:item_canvas.left
		//				bottom: item_canvas.bottom
		//			}
		//		}

		Rectangle{
			id:rect_config
			anchors{
					top: parent.top
				//topMargin: T3Styling.margin_r
				bottom: row_applyCancelButtons.top
				bottomMargin: T3Styling.margin_r
				right: item_canvas.right
			}
			width: item_canvas.width*0.3
			color: "transparent"
			ListView{
				anchors.fill: parent
				clip: true
				ScrollBar.vertical: ScrollBar {
					id:sBar_config
					active: true
				}
				spacing: T3Styling.spacing_r
				model:kmInfoModel_A.concat(kmCtrlModel_A)
				delegate: 	T3ParamUnitGroup{
					title_s: modelData[0]
					model_A: modelData[1]
					width: item_canvas.width*0.3
					height: paintedHeight_r
				}
			}
		}
		Row{
			id:row_applyCancelButtons
			anchors{
				right: item_canvas.right
				bottom: item_canvas.bottom
			}
			width: item_canvas.width*0.3
			spacing: T3Styling.margin_r
			Repeater{
				model:["Apply","Cancel"]
				delegate:T3Button{
					buttonLabel_s: modelData
					width:(row_applyCancelButtons.width
						   -row_applyCancelButtons.spacing)/2
				}
			}
		}
        // When external temp is below freezing @ 35 degrees farenheit, heaters button will be able to be used
        Rectangle{
            id:rect_configd
            anchors{
                    top: parent.top
                //topMargin: T3Styling.margin_r
                bottom: row_applyHeatersButton.top
                bottomMargin: T3Styling.margin_r
                right: item_canvas.right
            }
            width: item_canvas.width*0.3
            color: "transparent"
            ListView{
                anchors.fill: parent
                clip: true
                ScrollBar.vertical: ScrollBar {
                    id:sBar_configd
                    active: true
                }
                spacing: T3Styling.spacing_r
                model:kmInfoModel_A.concat(kmCtrlModel_A)
                delegate: 	T3ParamUnitGroup{
                    title_s: modelData[0]
                    model_A: modelData[1]
                    width: item_canvas.width*0.3
                    height: paintedHeight_r
                }
            }
        }
        Row{
            id:row_applyHeatersButton
            anchors{
                right: item_canvas.right
                bottom: item_canvas.bottom
            }
            width: item_canvas.width*0.3
            spacing: T3Styling.margin_r
            Repeater{
                model:["Apply","Heat"]
                delegate:T3Button{
                    buttonLabel_s: modelData
                    width:(row_applyHeatersButton.width
                           -row_applyHeatersButton.spacing)/2
                }
            }
        }
	}
}
