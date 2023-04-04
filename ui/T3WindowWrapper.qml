import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 1.3
ApplicationWindow {
	id:rootWindow
	minimumHeight: height_i
	maximumHeight:height_i
	minimumWidth:width_i
	maximumWidth:width_i
	property string loaderSource_s:"";
	property int height_i:20
	property int width_i:40
	opacity: 0.0


	title: loaderSource_s.replace(".qml","")
	T3Text{
		anchors.fill: parent
		textContent_s: "LOADING"
	}
	Loader {
	   id: myLoader
	   anchors.fill: parent
	   source: loaderSource_s
	   active: false
	}

		PropertyAnimation{
			id:animation
			target: rootWindow
			property:"opacity"
			from:0
			to:1
			duration:800
		}

	Component.onCompleted: {
		animation.duration = 200
		animateOpac(0,1);
		myLoader.active = true;
		animation.duration = 200
	}
	property bool closeFlag_b:false
	function animateOpac(fromWhich,toWhich){
		animation.from = fromWhich;
		animation.to = toWhich;
		animation.running = true;
	}
}
