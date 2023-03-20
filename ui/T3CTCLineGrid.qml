import QtQuick 2.12
import QtGraphicalEffects 1.12
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.15
Item {
	id:root
	implicitHeight: 300
	implicitWidth: 1000
	property real unitWidth_r:30
	property variant trackConstantsObject_O:t3databaseQml.trackConstantsObjects_QML[dbIndex_i]
	property variant trackVariablesObject_O:t3databaseQml.trackVariablesObjects_QML[dbIndex_i]
	property int dbIndex_i:0
	property variant coordinates_A: solveForCoors_f(trackConstantsObject_O)
	//property variant blockHovered2_bA: []
	//property variant blockIdToCoordinatesIndMap_M: constructBlockIdToIndMap_f(coordinates_A)
	property variant pathsAvailable_sA:[]
	onPathsAvailable_sAChanged: currIndOfPathsAvailable_n = pathsAvailable_sA.length>0?0:-1
	property int currIndOfPathsAvailable_n : -1;
	signal blockClicked(blockId_s:string)

	Rectangle{
		id:rect_canvas
		property real numberOfUnitWidthNeeded_r: Math.max(...coordinates_A.map(it=>it[1][1]))+1
		color: T3Styling.cBgMain_c
		anchors{
			fill:parent
		}
		border.width: T3Styling.lineWidth_r
		border.color: T3Styling.cFgSubSub_c
		radius:T3Styling.margin_r
		clip: true
		SequentialAnimation{
			running: !load_loader.active
			loops: Animation.Infinite
			ColorAnimation{
				target: rect_canvas
				property:"color"
				from: T3Styling.cFgSubSub_c
				to: T3Styling.cRed_c
				duration: 1000
			}
			ColorAnimation{
				target: rect_canvas
				property:"color"
				from: T3Styling.cRed_c
				to: T3Styling.cFgSubSub_c
				duration: 1000
			}
		}
		Loader{
			id:load_loader
			anchors.fill: parent
			sourceComponent: comp_canvas
			active: root.trackConstantsObject_O&&trackVariablesObject_O&&root.coordinates_A.length>0&&coordinates_A!==[]
			//			onLoaded: rect_leftConnectorCanvas.adaptiveMarginCopy_r = item.heightAlias_r*0.05
		}
	}
	Component{
		id:comp_canvas
		Flickable {

			id:gVie_flickable
			//			ScrollBar.horizontal: ScrollBar {
			//					parent: gVie_flickable.parent
			//					anchors.top: gVie_flickable.top
			//					anchors.left: gVie_flickable.right
			//					anchors.bottom: gVie_flickable.bottom
			//				}
			anchors.fill: parent
			anchors{
				topMargin: T3Styling.margin_r
				leftMargin: T3Styling.margin_r
				rightMargin:T3Styling.margin_r
			}

			contentHeight:rect_blockGrid.height
			contentWidth: rect_blockGrid.width
			clip: true

			Rectangle{
				id:rect_blockGrid
				height: gVie_flickable.height
				color: "transparent"
				width: rect_canvas.numberOfUnitWidthNeeded_r*unitWidth_r+rect_leftConnectorCanvas.width+rect_rightConnectorCanvas.width
				Rectangle{
					id:rect_leftConnectorCanvas
					//					x:gVie_flickable.x
					property real adaptiveMarginCopy_r: (rect_blockGrid.height)/2*0.1
					height: rect_leftConnectorCanvas.adaptiveMarginCopy_r*10
					width: rect_leftConnectorCanvas.adaptiveMarginCopy_r*2
					color: "transparent"
					opacity:trackConstantsObject_O["startingBlock2"]===""?0:1
					Rectangle{
						y:height
						width: rect_leftConnectorCanvas.adaptiveMarginCopy_r*1.5
						height:T3Styling.spacing_r
						radius: height
						color: T3Styling.cFgSubSub_c
						antialiasing: true
					}
					Rectangle{
						y:rect_leftConnectorCanvas.height-height*2
						width: rect_leftConnectorCanvas.adaptiveMarginCopy_r*1.5
						height: T3Styling.spacing_r
						radius: height
						//				radius: rect_leftConnectorCanvas.adaptiveMarginCopy_r*0.5
						color: T3Styling.cFgSubSub_c
						antialiasing: true
					}
					Rectangle{
						y:T3Styling.spacing_r
						width: T3Styling.spacing_r
						height: rect_leftConnectorCanvas.height-1.5*T3Styling.spacing_r*1.5
						radius: height
						//				radius: rect_leftConnectorCanvas.adaptiveMarginCopy_r*0.5
						color: T3Styling.cFgSubSub_c
						antialiasing: true
					}
				}
				Rectangle{
					id:rect_rightConnectorCanvas
					x:rect_leftConnectorCanvas.x+rect_leftConnectorCanvas.width+rect_canvas.numberOfUnitWidthNeeded_r*unitWidth_r
					height: rect_leftConnectorCanvas.adaptiveMarginCopy_r*10
					width: rect_leftConnectorCanvas.adaptiveMarginCopy_r*2
					color: "transparent"
					opacity:trackConstantsObject_O["endingBlock2"]===""?0:1
					Rectangle{
						x:rect_rightConnectorCanvas.width-width
						y:height
						width: rect_leftConnectorCanvas.adaptiveMarginCopy_r*1.5
						height:T3Styling.spacing_r
						radius: height
						color: T3Styling.cFgSubSub_c
						antialiasing: true
					}
					Rectangle{
						x:rect_rightConnectorCanvas.width-width
						y:rect_rightConnectorCanvas.height-height*2
						width: rect_leftConnectorCanvas.adaptiveMarginCopy_r*1.5
						height: T3Styling.spacing_r
						radius: height
						//				radius: rect_leftConnectorCanvas.adaptiveMarginCopy_r*0.5
						color: T3Styling.cFgSubSub_c
						antialiasing: true
					}
					Rectangle{
						x:rect_rightConnectorCanvas.width-width
						y:T3Styling.spacing_r
						width: T3Styling.spacing_r
						height: rect_leftConnectorCanvas.height-1.5*T3Styling.spacing_r*1.5
						radius: height
						//				radius: rect_leftConnectorCanvas.adaptiveMarginCopy_r*0.5
						color: T3Styling.cFgSubSub_c
						antialiasing: true
					}
				}

				Repeater{
					id:reap_railBlocksRepeater
					model:coordinates_A/*[lineObjIndex_i]*/
					delegate:
						Rectangle{
						width: unitWidth_r*coordinates_A/*[lineObjIndex_i]*/[index][2]
						height: (rect_blockGrid.height)/2
						x:unitWidth_r*coordinates_A/*[lineObjIndex_i]*/[index][1][1]+rect_leftConnectorCanvas.width
						y:coordinates_A[index][1][0]*(height-height*0.1)
						color: "transparent"
						T3CTCRailBlock{
							anchors.fill: parent
							blockId_s:coordinates_A/*[lineObjIndex_i]*/[index][0]
							blockConstantsObject_O:trackConstantsObject_O["blocksMap"][blockId_s]
							blockVariablesObject_O: trackVariablesObject_O[blockId_s]
							hovered_b: mAre_railBlock.containsMouse
							hovered2_b: currIndOfPathsAvailable_n===-1?false:pathsAvailable_sA[currIndOfPathsAvailable_n]===blockId_s
							noAnimation_b: hovered2_b
							pressed_b: mAre_railBlock.containsPress
						}
						MouseArea{
							id:mAre_railBlock
							hoverEnabled: true
							anchors.fill: parent
							onClicked: blockClicked(coordinates_A[index][0])
						}
					}
				}
				Timer{
					id:time_pathSelectionBlockRollController
					interval:root.pathsAvailable_sA.length===0?1:1000/root.pathsAvailable_sA.length
					running: pathsAvailable_sA.length>0
					repeat: true
					onTriggered: {
						if(root.currIndOfPathsAvailable_n===root.pathsAvailable_sA.length)
							root.currIndOfPathsAvailable_n = 0;
						else
							root.currIndOfPathsAvailable_n += 1;
					}
				}
			}
		}
	}
	function solveForCoors_f(rootObj_O) {
		let blocksMap_O = rootObj_O["blocksMap"];
		let currTopBlockId_s = rootObj_O["startingBlock1"];
		let currBottomBlockId_s = rootObj_O["startingBlock2"];
		let currTopBlockLine_A = [];
		let currBottomBlockLine_A = [];
		let currBlockGridList_A = [];
		while (true) {
			let currIsDoubleLine_b = currBottomBlockId_s !== "";
			let currTopBlockObj_O = blocksMap_O[currTopBlockId_s];
			let currBottomBlockObj_O
				= currIsDoubleLine_b ? blocksMap_O[currBottomBlockId_s] : null;
			let spreadLength_f = (blockLine1_A, blockLine2_A) => {
				let theSmallerLength_n
				= Math.min(blockLine1_A.length, blockLine2_A.length);
				let theBiggerLength_n
				= Math.max(blockLine1_A.length, blockLine2_A.length);
				let theShorterBlockLine_O
				= blockLine1_A.length === theSmallerLength_n
				? blockLine1_A : blockLine2_A;
				let newSmallerWidth_n = theBiggerLength_n / theSmallerLength_n;
				for (let currTopBlock_A of theShorterBlockLine_O) {
					currTopBlock_A[1] = newSmallerWidth_n;
				}
			};
			if (!currIsDoubleLine_b
					&& currTopBlockId_s === rootObj_O["endingBlock1"]
					&& currBottomBlockId_s === rootObj_O["endingBlock2"]) {
				currTopBlockLine_A.push([currTopBlockId_s, 1]);
				currBlockGridList_A.push([[...currTopBlockLine_A], []]);
				break;
			} else if (currIsDoubleLine_b
					   && currTopBlockId_s === rootObj_O["endingBlock1"]
					   && currBottomBlockId_s === rootObj_O["endingBlock2"]) {
				currTopBlockLine_A.push([currTopBlockId_s, 1]);
				currBottomBlockLine_A.push([currBottomBlockId_s, 1]);
				spreadLength_f(currTopBlockLine_A, currBottomBlockLine_A);
				currBlockGridList_A.push([[...currTopBlockLine_A], [...currBottomBlockLine_A]]);
				break;
			} else if (currIsDoubleLine_b
					   && currTopBlockId_s !== currBottomBlockId_s
					   && (((currTopBlockId_s === rootObj_O["endingBlock1"]
							 || (currTopBlockObj_O["prevBlock2"] !== ""
								 && currTopBlockObj_O["prevBlock2"] !== "PASSIVE"))
							&& currBottomBlockId_s !== rootObj_O["endingBlock2"]))) {
				currBottomBlockLine_A.push([currBottomBlockId_s, 1]);
				currBottomBlockId_s = currBottomBlockObj_O["nextBlock1"];
			} else if (currIsDoubleLine_b
					   && currTopBlockId_s !== currBottomBlockId_s
					   && (((currBottomBlockId_s === rootObj_O["endingBlock2"]
							 || (currBottomBlockObj_O["prevBlock2"] !== ""
								 && currBottomBlockObj_O["prevBlock2"] !== "PASSIVE"))
							&& currTopBlockId_s !== rootObj_O["endingBlock1"]))) {
				currTopBlockLine_A.push([currTopBlockId_s, 1]);
				currTopBlockId_s = currTopBlockObj_O["nextBlock1"];
			} else if (currIsDoubleLine_b
					   && (currTopBlockObj_O["prevBlock2"] !== "" && currTopBlockObj_O["prevBlock2"] !== "PASSIVE")
					   && (currBottomBlockObj_O["prevBlock2"] !== "" && currBottomBlockObj_O["prevBlock2"] !== "PASSIVE")) {
				spreadLength_f(currTopBlockLine_A, currBottomBlockLine_A);
				currBlockGridList_A.push([[...currTopBlockLine_A], [...currBottomBlockLine_A]]);
				currTopBlockLine_A = [];
				currBottomBlockLine_A = [];
				currIsDoubleLine_b = false;
				currBlockGridList_A.push([[[currTopBlockId_s, 2]],[]]);//switch has min width 2
				// currTopBlockLine_A.push([currTopBlockId_s, 1]);
				currTopBlockId_s = currTopBlockObj_O["nextBlock1"];
				currBottomBlockId_s = currTopBlockObj_O["nextBlock2"];
			} else if (!currIsDoubleLine_b
					   && !currIsDoubleLine_b
					   && currTopBlockObj_O["nextBlock2"] !== "") {
				// currTopBlockLine_A.push([currTopBlockId_s, 1]);
				currBlockGridList_A.push([[...currTopBlockLine_A], []]);
				currBlockGridList_A.push([[[currTopBlockId_s, 2]],[]]);//switch has min width 2
				currTopBlockLine_A = [];
				currIsDoubleLine_b = true;
				currTopBlockId_s = currTopBlockObj_O["nextBlock1"];
				currBottomBlockId_s = currTopBlockObj_O["nextBlock2"];
			} else if (currIsDoubleLine_b) {
				currTopBlockLine_A.push([currTopBlockId_s, 1]);
				currBottomBlockLine_A.push([currBottomBlockId_s, 1]);
				currTopBlockId_s = currTopBlockObj_O["nextBlock1"];
				currBottomBlockId_s = currBottomBlockObj_O["nextBlock1"];
			} else if (!currIsDoubleLine_b) {
				currTopBlockLine_A.push([currTopBlockId_s, 1]);
				currTopBlockId_s = currTopBlockObj_O["nextBlock1"];
			}
		}
		let currBlockFlattenedTopList_A = [];
		let currBlockFlattenedBottomList_A = [];
		let currTopX_n = 0;
		let currBottomX_n = 0;
		for (let currBlockGrid_A of currBlockGridList_A) {
			for (let currTopBlock_A of currBlockGrid_A[0]) {
				currBlockFlattenedTopList_A.push([currTopBlock_A[0], [0, currTopX_n],currTopBlock_A[1]]);
				currTopX_n += currTopBlock_A[1];
			}
			for (let currBottomBlock_A of currBlockGrid_A[1]) {
				currBlockFlattenedBottomList_A.push([currBottomBlock_A[0], [1, currBottomX_n],currBottomBlock_A[1]]);
				currBottomX_n += currBottomBlock_A[1];
			}
			currBottomX_n = currTopX_n;
		}
		let concatedBlockFlattenedList_A
			= currBlockFlattenedTopList_A.concat(currBlockFlattenedBottomList_A);
		//console.log(concatedBlockFlattenedList_A);
		//root.blockHovered2_bA = Array(concatedBlockFlattenedList_A.length).fill(false);
		return concatedBlockFlattenedList_A;
	}
//	function constructBlockIdToIndMap_f(coord_A){
//		let blockIdToIndMap_M = new Map();
//		coord_A.forEach((it_A,ind_n)=>{
//					 blockIdToIndMap_M.set(it_A[0],ind_n);
//				 });
//		return blockIdToIndMap_M;
//	}
}
