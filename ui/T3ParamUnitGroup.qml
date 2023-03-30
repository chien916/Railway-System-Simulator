import QtQuick 2.12

Item {
	id:root
	implicitHeight: 300
	implicitWidth: 200
	readonly property real paintedHeight_r:{
		let sliderCount_i = model_A.filter(it_s=>it_s.split("_")[0]!=="T").length
		//console.log(sliderCount_i)
		let textCount_i = model_A.length;
		let spacingCount_i = model_A.length*3+sliderCount_i;
		let marginCount_i = sliderCount_i+1;
		let res_r =  textCount_i*T3Styling.fontSubSub_r
				+spacingCount_i*T3Styling.spacing_r
				+ marginCount_i*T3Styling.margin_r;
		//console.log(res_r);
		return res_r;
	}

	property string title_s:"GROUP INFO TITLE"
	property variant model_A:
		//[textonly,twostate,labal,unit]
		["T_T_Speed_Mph",
		"T_F_Speed_Mph",
		"F_T_Speed_Mph",
		"F_F_Speed_Mph"
	];


	Rectangle{
		id:rect_canvas
		anchors{
			fill: parent
		}
		color: T3Styling.cBgSub_c
		Column{
			id:colu_currBlocData
			spacing: T3Styling.spacing_r
			width: rect_canvas.width
			height: rect_canvas.height
			T3Text{
				id:t3te_title
				textContent_s: root.title_s
				height: T3Styling.margin_r
				width: parent.width
				textColor_c: T3Styling.cBgSub_c
				bgColor_c: T3Styling.cBlue_c
				textBold_b: true
			}
			Repeater{
				id:repe_config
				model:root.model_A
				T3ParamUnit{
					paramConfig_A: modelData
					width: parent.width
					readOnly_b: false
				}
			}
		}
	}


}
