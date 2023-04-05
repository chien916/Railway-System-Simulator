import QtQuick 2.12

Item {
	id:root
	signal applyClicked();
	property string blockId_s:""
	readonly property string switchSide_s : "left"
	onBlockId_sChanged: {db2view(true)}
	readonly property variant configModel_nA:[
		//[textonly,twostate,labal,unit]
		"F_T_Maintance Mode_"
		,"F_F_Suggested Speed_mph"
	]
	Rectangle{
		id:rect_canvas
		radius: T3Styling.margin_r
		color: T3Styling.cBgSub_c
		anchors.fill: parent
//		Column{
//			anchors{
//				margins: T3Styling.margin_r
//				fill: parent
//			}
//			id:colu_allConfigs
//			Text{
//				text:"eet"
//			}
//		}
		Column{
			id:col_column
			anchors{
				top:parent.top
				left:parent.left
				right:parent.right
				bottom:parent.bottom
			}
			clip:true
			height: parent.height*0.3
			anchors{
				topMargin: T3Styling.margin_r
				leftMargin: T3Styling.margin_r
				rightMargin: T3Styling.margin_r
				bottomMargin: T3Styling.margin_r*3
			}
//			clip: true
			spacing: T3Styling.spacing_r
			T3ParamUnit{
				id:custom_maintainanceMode
				height:T3Styling.fontMain_r*1
				maxValue_r: 1;
				minValue_r: 0
				fixedPoint_i: 2
				readOnly_b:false
				paramConfig_A:"F_T_Maintance Mode_"
				width: col_column.width
			}
			T3ParamUnit{
				id:cust_suggestedSpeed
				opacity:1
				height:T3Styling.fontMain_r*1
				maxValue_r: 100;
				minValue_r: 0
				fixedPoint_i: 2
				readOnly_b: false
				paramConfig_A:"F_F_Suggested Speed_mph"
				width: col_column.width
			}
			Item{
				width: parent.width
				height: T3Styling.spacing_r
			}
			Item{
				width: col_column.width
				height: T3Styling.fontSubSub_r
				T3Text{
					textContent_s: " Switch Position"
					opacity: custom_maintainanceMode.valueratio_r>0.5?1:0.5
					Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
					anchors.fill: parent
					textColor_c: T3Styling.cFgSub_c
					textPixelSize_r: T3Styling.fontSubSub_r
					textBold_b: true
					textAlign_s: "left"
				}
				T3Text{
					textContent_s:{
						if(switchSide_s==="") return "N/A"
						else if(cust_switchDial.currValue_n===0) return "BOTTOM"
						else if(cust_switchDial.currValue_n===0.36) return "TOP"
						else if(cust_switchDial.currValue_n===0.64) return "TOP"
						else if(cust_switchDial.currValue_n===1) return "BOTTOM"
					}
					opacity:custom_maintainanceMode.valueratio_r>0.5?1:0.5
					Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
					anchors.fill: parent
					textColor_c: T3Styling.cFgMain_c
					textPixelSize_r: T3Styling.fontSubSub_r
					textBold_b: true
					textAlign_s: "right"
				}

			}

			T3NCDial{
				id:cust_switchDial
				height: T3Styling.fontSubSub_r*12
				anchors.horizontalCenter: parent.horizontalCenter
				ticks_nA: [0,0.36]
				enabled_b:custom_maintainanceMode.valueratio_r>0.5
			}
			Item{
				width: parent.width
				height: T3Styling.spacing_r*1.5
			}

			Item{
				width: col_column.width
				height: T3Styling.fontSubSub_r
				T3Text{
					textContent_s: " Authority"
					opacity: custom_maintainanceMode.valueratio_r>0.5?1:0.5
					Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
					anchors.fill: parent
					textColor_c: T3Styling.cFgSub_c
					textPixelSize_r: T3Styling.fontSubSub_r
					textBold_b: true
					textAlign_s: "left"
				}
				T3Text{
					textContent_s:{
						if(tInp_dispatchFrom.text==="  ") return "N/A"
						else tInp_dispatchFrom.text
					}
					opacity:custom_maintainanceMode.valueratio_r>0.5?1:0.5
					Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
					anchors.fill: parent
					textColor_c: T3Styling.cFgMain_c
					textPixelSize_r: T3Styling.fontSubSub_r
					textBold_b: true
					textAlign_s: "right"
				}

			}

			Rectangle{
				id:rect_dispatchFrom
				height: T3Styling.fontSubSub_r*2
				width: parent.width
				radius: T3Styling.margin_r
				opacity:1
				Behavior on opacity{PropertyAnimation{easing.type: Easing.OutCirc}}
				color:T3Styling.cBgMain_c
				Behavior on color{PropertyAnimation{}}
				T3Text{
					id:text_dispatchFromHint
					anchors{
						fill:parent
					}
					textPixelSize_r: T3Styling.fontSubSub_r
					textContent_s: "NO AUTHORITY GIVEN"
					visible: tInp_dispatchFrom.text==="  "
					textColor_c: T3Styling.cFgMain_c
				}
				TextInput{
					id:tInp_dispatchFrom
					readOnly:false
					anchors{
						fill:parent
					}
					cursorDelegate: Item{}
					width: T3Styling.fontSubSub_r*6
					font.bold: acceptableInput
					color: acceptableInput?T3Styling.cFgMain_c:text==="  "?T3Styling.cFgMain_c:T3Styling.cYellow_c
					verticalAlignment: Text.AlignVCenter
					horizontalAlignment: Text.AlignHCenter
					inputMask: ">A A 900"
					text:""
					font.family: "Inter"
					font.pixelSize: T3Styling.fontSubSub_r
					onFocusChanged: {
						if(focus||!acceptableInput) selectAll()
					}
				}
			}
		}


		T3Button{
			anchors{
				bottom:rect_canvas.bottom
				left:rect_canvas.left
//				right:rect_canvas.right
				margins: T3Styling.margin_r
			}
			height: T3Styling.margin_r
			width: (rect_canvas.width-T3Styling.margin_r*3)/2
			buttonLabel_s: "APPLY"
			onButtonClicked: {
				//console.log(tInp_dispatchFrom.text)
				if(tInp_dispatchFrom.text==="  "){
					view2db();
					db2view(true);
					applyClicked();
					return;
				}
				let dispatchFrom_s = tInp_dispatchFrom.text.split(" ").join("_");
				for(let i = 0;i<t3databaseQml.trackConstantsObjects_QML.length;++i){
					if(t3databaseQml.trackConstantsObjects_QML[i]["blocksMap"][dispatchFrom_s]!==undefined){
						if(dispatchFrom_s.charAt(0)!==root.blockId_s.charAt(0)){
							rect_errMessage.showMessage("Block on Different Line");
							return;
						}
						view2db();
						db2view(true);
						applyClicked();
						return;
					}
				}
				rect_errMessage.showMessage("Unrecognized Block");
			}
		}

		T3Button{
			anchors{
				bottom:rect_canvas.bottom
//				left:rect_canvas.left
				right:rect_canvas.right
				margins: T3Styling.margin_r
			}
			height: T3Styling.margin_r
			width: (rect_canvas.width-T3Styling.margin_r*3)/2
			buttonLabel_s: "CANCEL"
			onButtonClicked: applyClicked()
		}

		Rectangle{
			anchors{
				bottom:parent.bottom
				left:parent.left
				right:parent.right
				margins: T3Styling.margin_r
			}
			height: T3Styling.margin_r
			id:rect_errMessage
			radius: T3Styling.margin_r
			color: Qt.darker(T3Styling.cRed_c)
			opacity:0
			SequentialAnimation{
				id:anim_showAndDisappear
				alwaysRunToEnd: true
				PropertyAnimation{
					target:rect_errMessage
					property:"opacity"
					from:0.0
					to:1.0
					duration: 500
				}
				PropertyAnimation{
					target:rect_errMessage
					property:"opacity"
					from:1.0
					to:1.0
					duration: 1000
				}
				PropertyAnimation{
					target:rect_errMessage
					property:"opacity"
					from:1.0
					to:0.0
					duration: 500
				}
			}
			T3Text{
				id:text_errMessage
				anchors.fill: parent
				textContent_s: "ERR MESSAGE"
				textColor_c: T3Styling.cFgMain_c
				textPixelSize_r: T3Styling.fontSubSub_r
			}
			function showMessage(errMsg:string){
				text_errMessage.textContent_s = errMsg;
				anim_showAndDisappear.start()
			}
		}
	}


	function db2view(maintanceModeIncluded_b:bool){
		if(blockId_s==="")return;
		let metaInfo_A = t3databaseQml.ctc_readPlcInputFromMetaInfo(blockId_s);
		if(maintanceModeIncluded_b)
			custom_maintainanceMode.valueratio_r = metaInfo_A[0]?1:0;
		cust_suggestedSpeed.valueratio_r = metaInfo_A[1]/100;
		cust_switchDial.dialDialValue_r = metaInfo_A[2]?0.36:0;
		tInp_dispatchFrom.text = metaInfo_A[3];
	}

	function view2db(){
		if(blockId_s==="")return;
		let metaInfo_A = [
				custom_maintainanceMode.valueratio_r>0.5,
				cust_suggestedSpeed.actualValue_r,
				cust_switchDial.currValue_n===0.36,
				tInp_dispatchFrom.text.split(" ").join("_")
			]
		t3databaseQml.ctc_writeToPlcInputFromMetaInfo(blockId_s,metaInfo_A);
	}

	//for testing only
	Component.onCompleted: {
		db2view(true);
	}

	implicitWidth: 350
	implicitHeight: 450
}
