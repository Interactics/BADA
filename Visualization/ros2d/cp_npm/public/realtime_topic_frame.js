var canvas;
var ctx;
var start;
var oldTimeStamp;
var x;
var y;
var theta;
var str;
var data;
var time;
var sig_name;
var viewtime;
var PrintHistory = "Connection";

const today = new Date();
today.setTime(0);

var hidx = 0;
const FRAMES_PER_SECOND = 10;  // Valid values are 60,30,20,15,10...
const FRAME_MIN_TIME = (1000 / 60) * (60 / FRAMES_PER_SECOND) - (1000 / 60) * 0.5;
var lastFrameTime = 0;  // the last frame time






// Connecting to ROS
// -----------------
var ros = new ROSLIB.Ros();

// If there is an error on the backend, an 'error' emit will be emitted.
ros.on('error', function (error) {
    console.log(error);
});

// Find out exactly when we made a connection.
ros.on('connection', function () {
    console.log('Connection made!');
});

ros.on('close', function () {
    console.log('Connection closed.');
});

// Create a connection to the rosbridge WebSocket server.
ros.connect('ws://localhost:9090');

// Like when publishing a topic, we first create a Topic object with details of the topic's name
// and message type. Note that we can call publish or subscribe on the same topic object.
var listener = new ROSLIB.Topic({
    ros: ros,
    name: '/listener',
    messageType: 'std_msgs/String'
});

// Then we add a callback to be called every time a message is published on this topic.
listener.subscribe(function (message) {
    // console.log('Received message on ' + listener.name + ': ' + message.data);

    // If desired, we can unsubscribe from the topic as well.
    listener.unsubscribe();
});




var ros = new ROSLIB.Ros({
    url: 'ws://localhost:9090'
});

var audio_topic = new ROSLIB.Topic({
    ros: ros,
    name:'/audio',
    // name: '/bada_audio/audio',
    messageType: 'std_msgs/String'
});

audio_topic.subscribe(function (m){

    str = m.data;

    for (var i = 0; i < 100; i++) str = str.replace("\"", "");
    str = str.substring(1);
    str = str.substring(1);
    str = str.slice(0, -1);
    str = str.slice(0, -1);
    for (var i = 0; i < 100; i++) str = str.replace(",", "");
    for (var i = 0; i < 100; i++) str = str.replace("[", "");
	str=str.split("]");

    document.getElementById("first_topic").innerHTML = str[0];
    document.getElementById("second_topic").innerHTML = str[1];
    document.getElementById("third_topic").innerHTML = str[2];
    document.getElementById("fourth_topic").innerHTML = str[3];
    document.getElementById("fifth_topic").innerHTML = str[4];
    document.getElementById("sixth_topic").innerHTML = str[5];
    document.getElementById("seventh_topic").innerHTML = str[6];
    document.getElementById("eighth_topic").innerHTML = str[7];
    document.getElementById("ninth_topic").innerHTML = str[8];
    document.getElementById("tenth_topic").innerHTML = str[9];
});

