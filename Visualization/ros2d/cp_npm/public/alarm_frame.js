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


var signal = new ROSLIB.Topic({
    ros : ros,
    name : '/signal',
   // name : '/bada_audio/signal',
    messageType : 'std_msgs/String'
  });
  const dic1={'Speech':'말하는 소리', 'Alarm':'알람', 'Door':'노크', 'Television':'티비소리', 'Silence':'없음', 'Water':'물소리', 'Music':'음악'};

  signal.subscribe(function(m){
    sig_name=dic1[m.data];
    document.getElementById("signal").innerHTML = sig_name;
  });