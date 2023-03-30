import QtQuick 2.12
import QtQuick.Controls 2.15
import QtMultimedia 5.15
Item {
	id:root
	implicitHeight: 500
	implicitWidth: 500
	property real velocityRatio_r:0
	property real brakeRatio_r:0
	property bool onSwitch_b: false
	property bool horn_b:false

	//do not modify
	property bool stopped_b: true
	property bool lastStateIsLow_b :false
	property bool isTransitioning_b:false
	onVelocityRatio_rChanged: {
		if(stopped_b&&velocityRatio_r>0){
			sEff_engineStart.play()
			stopped_b = false;
		} else if(velocityRatio_r===0){
			sAni_bremisePulse.running = true;
			stopped_b = true;
		}

		if(0.3>velocityRatio_r){
			lastStateIsLow_b = true;
			isTransitioning_b = false;
		}
		else if(0.4<velocityRatio_r&&velocityRatio_r<0.5){
			if(!isTransitioning_b){
				if(lastStateIsLow_b){
					sEff_transitionToHigh.loops = 1;
					sEff_transitionToHigh.play();

				}
				else{
					sEff_transitionToLow.loops = 1;
					sEff_transitionToLow.play();
				}
				isTransitioning_b = true;
			}
			console.log("2")
		}
		else if(velocityRatio_r>0.6){
			lastStateIsLow_b = false;
			isTransitioning_b = false;
			console.log("3")
		}
	}


	MediaPlayer{
		id:audi_announcement
		property int numbersToPlay: 0
		playlist: 	Playlist{
			id:pLis_annQueue
		}
	}

	MediaPlayer{
		id:sEff_rail
		volume:(velocityRatio_r!==0&&!onSwitch_b)?1:0
		source : "qrc:/T3EngineSounds/Rail_2.mp3"
		playbackRate: 0
		Behavior on volume {PropertyAnimation{}}
	}

	SoundEffect{
		id:sEff_flange
		volume:(velocityRatio_r!==0&&onSwitch_b)?1:0
		Behavior on volume {PropertyAnimation{}}
		source : "qrc:/T3EngineSounds/Rail_Flange.wav"
		loops : SoundEffect.Infinite
	}

	Audio{
		id:sEff_transitionToHigh
		source : "qrc:/T3EngineSounds/Engine_ToHigh.wav"
		//		volume: 1
		//		loops : 1
//		Behavior on volume {PropertyAnimation{duration: 500}}
	}

	Audio{
		source : "qrc:/T3EngineSounds/Engine_ToLow.wav"
		id:sEff_transitionToLow
		//		volume: 1
		//		loops : 1
//		Behavior on volume {PropertyAnimation{duration: 500}}
	}


	Timer{
		id:time_rail
		interval:(1-velocityRatio_r)*2000+2000
		running: velocityRatio_r!==0
		repeat: true
		onTriggered: {
			sEff_rail.playbackRate= (velocityRatio_r)*1.2+0.4
			sEff_rail.loops = 2;
			sEff_rail.play();
		}
	}



	SoundEffect{
		id:sEff_engineStart
		source:"qrc:/T3EngineSounds/Engine_Start.wav";
	}

	SoundEffect{
		id:sEff_bremise
		loops: SoundEffect.Infinite
		source: "qrc:/T3EngineSounds/Engine_End.wav";
		volume: 0
		//		Behavior on volume {PropertyAnimation{}}
	}

	SequentialAnimation{
		id:sAni_bremisePulse
		PropertyAnimation {
			target: sEff_bremise
			property:"volume"
			from:0
			to: 1
			duration: 500
		}
		PropertyAnimation {
			target: sEff_bremise
			property:"volume"
			from:1
			to: 0.8
			duration: root.stopped_b?1000:500
		}
		PropertyAnimation {
			target: sEff_bremise
			property:"volume"
			from:0.8
			to: 0
			duration: 500
		}
	}

	SoundEffect {
		id: sEff_horn
		loops: SoundEffect.Infinite
		source:"qrc:/T3EngineSounds/Horn.wav"
		volume: horn_b?1:0
		Behavior on volume {PropertyAnimation{}}
	}

	SoundEffect  {
		id: sEff_engine1
		loops: SoundEffect.Infinite
		source: "qrc:/T3EngineSounds/Engine_1.wav"
		volume: {
			let [low_r,high_r] = [0.05,0.2]
			if(velocityRatio_r===0)
				return 0;
			if(velocityRatio_r<low_r)
				return 1;
			if(low_r<=velocityRatio_r&&velocityRatio_r<=high_r)
				return 1-(velocityRatio_r-low_r)*(high_r-low_r);
			else
				return 0;
		}
		Behavior on volume {PropertyAnimation{duration: !lastStateIsLow_b&&isTransitioning_b?100:5000}}
	}
	SoundEffect  {
		id: sEff_engine2
		loops: SoundEffect.Infinite
		//		muted: sEff_transition.playing
		source: "qrc:/T3EngineSounds/Engine_2.wav"
		volume: {
			if(isTransitioning_b&&lastStateIsLow_b)
				return 0;
			let [low_r,mid_r,high_r] = [0.05,0.5,0.55];
			let vol_r = 0;
			let vel_r = velocityRatio_r;
			if(velocityRatio_r<low_r)
				return 0;
			else if(low_r<vel_r&&vel_r<=mid_r)
				vol_r= 1;
			else if(mid_r<vel_r&&vel_r<=high_r)
				vol_r=  (vel_r-mid_r)*(high_r-low_r);
			else if(vel_r>high_r)
				vol_r= 0;
			return vol_r;
		}
		Behavior on volume {PropertyAnimation{duration: lastStateIsLow_b?1000:5000}}
	}
	SoundEffect  {

		id: sEff_engine3
		loops: SoundEffect.Infinite
		source: "qrc:/T3EngineSounds/Rolling.wav"
		volume:
		{
			if(isTransitioning_b&&!lastStateIsLow_b)
				return 0;
			let [low_r,high_r] = [0.55,0.6]
			if(velocityRatio_r<low_r){
				return 0;
			}else if(low_r<=velocityRatio_r&&velocityRatio_r<=high_r){
				return (velocityRatio_r-low_r)*(high_r-low_r);
			}
			else{
				return 1;
			}
		}
		Behavior on volume {PropertyAnimation{duration: 5000}}
	}
	SoundEffect  {
		id: sEff_brake
		loops: SoundEffect.Infinite
		source: "qrc:/T3EngineSounds/Brake_Loop.wav"
		volume: brakeRatio_r
		Behavior on volume {PropertyAnimation{}}
	}
	SoundEffect  {
		id: sEff_rolling
		loops: SoundEffect.Infinite
		source: "qrc:/T3EngineSounds/Rolling.wav"
		volume: velocityRatio_r
		Behavior on volume {PropertyAnimation{}}
	}

	Component.onCompleted: {
		sEff_engine1.play()
		sEff_engine2.play()
		sEff_engine3.play()
		sEff_rolling.play()
		sEff_brake.play()
		sEff_bremise.play()
		sEff_flange.play()
		sEff_rail.play()
		sEff_horn.play()
		//annoStation_f("red","SOUTH HILLS JUNCTION_L","POPLAR_L")
	}

	function annoStation_f(lineColor_s
						   ,destNameAndDirection_s
						   ,stationNameAndDirection_s){
		let splittedStatAndDire_sA
			= stationNameAndDirection_s.split("_");
		if(splittedStatAndDire_sA.length!==2){
			console.log("splittedStatAndDire_sA.length!==2");
			return;
		}
		let station_s = splittedStatAndDire_sA[0];
		let side_s = splittedStatAndDire_sA[1];
		let splittedDestAndDire_sA
			= destNameAndDirection_s.split("_");
		if(splittedDestAndDire_sA.length!==2){
			console.log("splittedDestAndDire_sA.length!==2");return;
		}
		let dest_s = splittedDestAndDire_sA[0];

		if(stationList_sA.indexOf(station_s)===-1){
			console.log("stationList_sA.indexOf(station_s)===-1");return;
		}
		if(stationList_sA.indexOf(dest_s)===-1) {
			console.log("stationList_sA.indexOf(dest_s)===-1");return;
		}

		let fileUrl_s = "qrc:/T3AnnSounds/"+station_s+".mp3";
		let isLeft_b = side_s.includes("L");
		let isRight_b = side_s.includes("R");
		if(!isLeft_b&&!isRight_b) {
			console.log("!isLeft_b&&!isRight_b");return;
		}
		pLis_annQueue.addItem("qrc:/T3AnnSounds/Ann_Pre.mp3")
		//This is the XX trains to
		if(lineColor_s==="red")
			pLis_annQueue.addItem("qrc:/T3AnnSounds/Ann_RedTrain.mp3")
		else if(lineColor_s==="green")
			pLis_annQueue.addItem("qrc:/T3AnnSounds/Ann_GreenTrain.mp3")
		else if(lineColor_s==="blue")
			pLis_annQueue.addItem("qrc:/T3AnnSounds/Ann_BlueTrain.mp3")
		else return;
		//XXX Station (dest)
		pLis_annQueue.addItem("qrc:/T3AnnSounds/"+dest_s+".mp3");
		//The next stop is
		pLis_annQueue.addItem("qrc:/T3AnnSounds/Ann_NextStop.mp3");
		//XXX Station (next)
		pLis_annQueue.addItem("qrc:/T3AnnSounds/"+station_s+".mp3");
		if(isLeft_b&&isRight_b)
			pLis_annQueue.addItem("qrc:/T3AnnSounds/Ann_DoorBoth.mp3");
		else if(isRight_b)
			pLis_annQueue.addItem("qrc:/T3AnnSounds/Ann_DoorRight.mp3");
		else if(isLeft_b)
			pLis_annQueue.addItem("qrc:/T3AnnSounds/Ann_DoorLeft.mp3");
		audi_announcement.play()
	}

	readonly property variant stationList_sA : ["PIONEER",
		"EDGEBROOK",
		"WHITED",
		"SOUTH BANK",
		"CENTRAL",
		"INGLEWOOD",
		"OVERBROOK",
		"GLENBURY",
		"DORMONT",
		"MT LEBANON",
		"POPLAR",
		"CASTLE SHANNON",
		"DORMONT",
		"GLENBURY",
		"OVERBROOK",
		"INGLEWOOD",
		"CENTRAL",
		"SHADYSIDE",
		"HERRON AVE",
		"SWISSVILLE",
		"PENN STATION",
		"STEEL PLAZA",
		"FIRST AVE",
		"STATION SQUARE",
		"SOUTH HILLS JUNCTION"]
}
