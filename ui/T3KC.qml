import QtQuick 2.12
import QtQuick.Dialogs 1.0
Item {
	id:root
	property real plcInputs_r:0xfff3ffff
	property real plcOutputs_r:13
	property string plcFilePath_s:"N/A"
	property string plcStatus_s:"Not Loaded"
	implicitHeight: 380
	implicitWidth: 500
	Rectangle{
		anchors.fill: parent
		radius: T3Styling.margin_r
		color: T3Styling.cBgSub_c
		T3Text{
			anchors.bottom: parent.bottom
			anchors.right: parent.right
			anchors.rightMargin: T3Styling.margin_r
			textPixelSize_r: T3Styling.fontSubSub_r
			height: T3Styling.margin_r
			width: parent.width
			textContent_s: "T3 Track Controller | Avi Dave"
			textAlign_s: "right"
			textBold_b: true
		}
	}
	FileDialog {
		id: fDia_fileDialog
		title: "Please choose a PLC file"
		folder: shortcuts.home
		onAccepted: {
			plcFilePath_s = fDia_fileDialog.fileUrls.toString()
			plcStatus_s = "Loaded"
			//Qt.quit()
		}
		onRejected: {
			plcFilePath_s = "N/A"
			plcStatus_s = "Not Loaded"
			//Qt.quit()
		}
		//Component.onCompleted: visible = true
	}
	Item{
		id:item_canvas
		anchors.fill: parent
		anchors.margins: T3Styling.margin_r
		Rectangle{
			id:rect_leftScreen
			anchors{
				top:parent.top
				//top:sDis_segDisplay.bottom
				//topMargin: T3Styling.margin_r
				left:item_canvas.left
				bottom:item_canvas.bottom
				bottomMargin: T3Styling.margin_r+T3Styling.spacing_r*2
			}
			width:item_canvas.width/**0.7*/
			radius: T3Styling.spacing_r
			color: T3Styling.cBgMain_c
			border.width: T3Styling.lineWidth_r
			border.color: T3Styling.cFgSubSub_c
			Item{
				id:item_leftScreenCanvas
				anchors.fill: parent
				anchors.margins: T3Styling.margin_r
				Grid{
					id:grid_label
					rows:2
					columns:1
					anchors{
						top:parent.top
						left:parent.left
						right:parent.right
					}
					height: T3Styling.fontSub_r*rows+T3Styling.spacing_r*(rows-1)
					Repeater{
						model:[["PLC File Path",root.plcFilePath_s],
						["PLC Program Status",root.plcStatus_s]]
						delegate:Item{
							width: grid_label.width
							height: T3Styling.fontSub_r
							T3Text{
								anchors.fill: parent
								textContent_s: modelData[0]
								textAlign_s: "left"
								textColor_c: T3Styling.cFgSub_c
							}
							T3Text{
								anchors.fill: parent
								textContent_s: modelData[1]
								textAlign_s: "right"
								textColor_c: T3Styling.cFgMain_c
							}
						}
					}
				}
				Rectangle{
					id:rect_seperatorInputLabel
					anchors{
						left:parent.left
						right: parent.right
						top:grid_label.bottom
						topMargin: T3Styling.spacing_r
					}
					height: T3Styling.lineWidth_r
					radius: height
					color: T3Styling.cFgSubSub_c
				}

				Item{
					id:item_inputLabels
					anchors{
						top:rect_seperatorInputLabel.bottom
						topMargin: T3Styling.spacing_r
					}
					width: grid_label.width
					height: T3Styling.fontSub_r
					T3Text{
						anchors.fill: parent
						textContent_s: "PLC Input Binaries"
						textAlign_s: "left"
						textColor_c: T3Styling.cFgSub_c
					}
					T3Text{
						anchors.fill: parent
						textContent_s: "0x"+plcInputs_r.toString(16).padStart(8,"0").toUpperCase()
						textAlign_s: "right"
						textColor_c: T3Styling.cBlue_c
					}
				}

				Grid{
					id:grid_inputBinaries
					rows:2
					columns:16
					anchors{
						top:item_inputLabels.bottom
						topMargin: T3Styling.spacing_r
						left:parent.left
						right:parent.right
					}
					columnSpacing: T3Styling.lineWidth_r
					rowSpacing: columnSpacing
					readonly property real unitWidth_r:
						(width-(columns-1)*columnSpacing)/columns


					Repeater{
						model:parent.columns*parent.rows
						delegate:	Rectangle{
							width: grid_inputBinaries.unitWidth_r
							height: width
							radius: width/4
							color: (plcInputs_r&(1<<(31-index)))!==0?T3Styling.cBlue_c:T3Styling.cFgSubSub_c
							T3Text{
								anchors.fill: parent
								textContent_s: (plcInputs_r&(1<<(31-index)))!==0?"T":"F"
								textColor_c: (plcInputs_r&(1<<(31-index)))!==0?T3Styling.cBgMain_c:T3Styling.cBgMain_c
							}
						}
					}
				}

				Rectangle{
					id:rect_seperatorOutputLabel
					anchors{
						left:parent.left
						right: parent.right
						top:grid_inputBinaries.bottom
						topMargin: T3Styling.spacing_r*2
					}
					height: T3Styling.lineWidth_r
					radius: height
					color: T3Styling.cFgSubSub_c
				}

				Item{
					id:item_outputLabels
					anchors{
						top:rect_seperatorOutputLabel.bottom
						topMargin: T3Styling.spacing_r
					}
					width: grid_label.width
					height: T3Styling.fontSub_r
					T3Text{
						anchors.fill: parent
						textContent_s: "PLC Output Binaries"
						textAlign_s: "left"
						textColor_c: T3Styling.cFgSub_c
					}
					T3Text{
						anchors.fill: parent
						textContent_s: "0x"+plcOutputs_r.toString(16).padStart(8,"0").toUpperCase()
						textAlign_s: "right"
						textColor_c: T3Styling.cGreen_c
					}
				}

				Grid{
					id:grid_outputBinaries
					rows:2
					columns:16
					anchors{
						top:item_outputLabels.bottom
						topMargin: T3Styling.spacing_r
						left:parent.left
						right:parent.right
					}
					columnSpacing: T3Styling.lineWidth_r
					rowSpacing: columnSpacing
					readonly property real unitWidth_r:
						(width-(columns-1)*columnSpacing)/columns
					Repeater{
						model:parent.columns*parent.rows
						delegate:	Rectangle{
							width: grid_inputBinaries.unitWidth_r
							height: width
							radius: width/4
							color: (plcOutputs_r&(1<<(31-index)))!==0?T3Styling.cGreen_c:T3Styling.cFgSubSub_c
							T3Text{
								anchors.fill: parent
								textContent_s: (plcOutputs_r&(1<<(31-index)))!==0?"T":"F"
								textColor_c: (plcOutputs_r&(1<<(31-index)))!==0?T3Styling.cBgMain_c:T3Styling.cBgMain_c
							}
						}
					}
				}
			}
		}
		Grid{
			id:grid_menuButtons
			anchors{
				top:rect_leftScreen.bottom
				topMargin: T3Styling.spacing_r
				bottom: parent.bottom
				left:rect_leftScreen.left
				right:rect_leftScreen.right
			}
			spacing: T3Styling.spacing_r
			rows:1
			columns:2
			Repeater{
				model:["Load Local PLC File","Toggle Manual / Automatic Mode"]
				delegate:T3Button{
					buttonLabel_s: modelData
					width: (grid_menuButtons.width-(grid_menuButtons.spacing*grid_menuButtons.columns-1))
						   /grid_menuButtons.columns
					height: grid_menuButtons.height
					pressedColor_c: index===1?T3Styling.cRed_c:T3Styling.cBgMain_c
					releasedColor_c:index===1?Qt.darker(T3Styling.cRed_c):T3Styling.cFgSubSub_c
					delayButton_b: index===1
					onButtonClicked: {
						if(index===0)
							fDia_fileDialog.open()


					}
				}
			}
		}
	}
}
