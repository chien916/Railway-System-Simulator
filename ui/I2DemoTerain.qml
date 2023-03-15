import QtQuick 2.12
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
Rectangle {
	id:root
	height:700
	width: 1200
	//height:700
	//width: 1200
	property int vind:0
	color: T3Styling.cBgMain_c
	T3CTC{
		visible: vind===4
		id:ctc
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
	}
	T3NM{
		visible: vind===0
		velocity_r: root.velocity_r
		id:nm
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
	}
	T3NC{
		visible: vind===1
		id:nc
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
		onOnControlButtonClicked: {
			if(metaData==="spaceHeater_on"){
				if(root.cabinTemperature_r>20) --nm.cabinTemperature_r;
			}
			else if(metaData==="spaceHeater_off") {
				if(root.cabinTemperature_r<30) ++nm.cabinTemperature_r;
			}
			else if(metaData==="leftDoors_on") nm.leftDoorOpened_b = true;
			else if(metaData==="leftDoors_off")nm.leftDoorOpened_b = false;
			else if(metaData==="rightDoors_on")nm.rightDoorOpened_b = true;
			else if(metaData==="rightDoors_off")nm.rightDoorOpened_b = false;
			else if(metaData==="exteriorLights_on")nm.exteriorLightsOn_b = true;
			else if(metaData==="exteriorLights_off")nm.exteriorLightsOn_b = false;
			else if(metaData==="interiorLights_on")nm.interiorLightsOn_b = true;
			else if(metaData==="interiorLights_off")nm.exteriorLightsOn_b = false;
			else if(metaData==="atc_toggle") return;
			else if(metaData==="serviceBrake_toggle")nm.serviceBrakeApplied_b = !nm.serviceBrakeApplied_b
			else if(metaData==="emergencyBrake_toggle")nm.emergencyBrakeApplied_b = !nm.emergencyBrakeApplied_b
		}
	}
	T3KM{
		visible: vind===2
		id:km
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
	}
	T3KC{
		visible: vind===3
		id:kc
		anchors.horizontalCenter: parent.horizontalCenter
		anchors.verticalCenter: parent.verticalCenter
	}

	Timer{
		interval:1000
		running:true
		repeat: true
		onTriggered: {
			if(nc.direction_s==="fwd"){
				if(nc.power_s==="p1"){
					nm.normalBrakeRatio_r=0.0;
					if(nm.velocity_r>70) return;
					else nm.velocity_r+=1;
				}else if(nc.power_s==="p2"){
					nm.normalBrakeRatio_r=0.0;
					if(nm.velocity_r>70) return;
					else nm.velocity_r+=4;
				}else if(nc.power_s==="b1"){
					nm.normalBrakeRatio_r=0.2
					if(nm.velocity_r<=0) nm.velocity_r=0;
					else nm.velocity_r-=1;
				}else if(nc.power_s==="b2"){
					nm.normalBrakeRatio_r=0.4
					if(nm.velocity_r<=0) nm.velocity_r=0;
					else nm.velocity_r-=4;
				}else if(nc.power_s==="n"){
					nm.normalBrakeRatio_r=0.0
				}
			}
		}

	}
}
