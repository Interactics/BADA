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


//Kakao.init("b886eede39b9d47bc9d3cb6e91483799");   // 사용할 앱의 JavaScript 키를 설정

function tryConnectWebsocket() {

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

  listener.subscribe(function (message) {
    listener.unsubscribe();
  });





  var odom=new ROSLIB.Topic({
    ros:ros,
    name : '/t265/odom/sample/',
    messageType : 'nav_msgs/Odometry'
  });



  odom.subscribe(function (message) {
    // if (count) {
    //   console.log(message);
    // }
    x=message.pose.pose.position.x
    y=message.pose.pose.position.y
    //console.log(message.pose.pose.position);
    console.log(x,y);
    x=(x*55)+220;
    y=((y*55)+380);

    // X=(x*150) +280
     // 280, 230 = X*590
    //x=x*400;
    //y=y*400;

    var quaternion = new THREE.Quaternion(message.pose.pose.orientation.x, message.pose.pose.orientation.y, message.pose.pose.orientation.z, message.pose.pose.orientation.w);
    var euler= new THREE.Euler();
    euler.setFromQuaternion(quaternion,'XYZ');
    theta=euler.z;
//   console.log(theta);
  });
};




///////////////////////////
var ros = new ROSLIB.Ros({
  url: 'ws://localhost:9090'
});

function init() {
  canvas = document.getElementById('canvas');
  console.log(canvas);
  ctx = canvas.getContext('2d');

  // Start the first frame request
  window.requestAnimationFrame(loop);
  tryConnectWebsocket();
};

function loop(timeStamp) {
  // Keep requesting new frames
  if (timeStamp - lastFrameTime < FRAME_MIN_TIME) { //skip the frame if the call is too early
    window.requestAnimationFrame(loop);
    return; // return as there is nothing to do
  }
  lastFrameTime = timeStamp; // remember the time of the rendered frame
  
  ctx.clearRect(0, 0, canvas.width, canvas.height);

  var secondsPassed = (timeStamp - oldTimeStamp) / 1000;
  var elapsedSeconds = (timeStamp - start) / 1000;
  oldTimeStamp = timeStamp;

  //Calculate fps
  var fps = Math.round(10 / secondsPassed) / 10;

  //Draw number to the screen
  //ctx.fillStyle = 'white';3
  //ctx.fillRect(0, 0, 200, 100);
  //ctx.font = '25px Arial';
  //ctx.fillStyle = 'black';
  //tx.fillText("FPS: " + fps, 10, 30);

  draw(timeStamp);

  if (elapsedSeconds > 5) {
    return;
  } else {
    window.requestAnimationFrame(loop);
  }
}

function draw(timestamp) {
  // if (!start) start = timestamp;
  // var progress = timestamp - start;

  //Draw number to the screen
 
  
  const radius = 1500;
  const startangle = theta - 1 / 16 * Math.PI;
  const endangle = theta + 1 / 16 * Math.PI;
  ctx.fillStyle = 'rgba(248,206,105,0.7)';



  ctx.beginPath();//ADD THIS LINE!<<<<<<<<<<<<<
  ctx.moveTo(x, y);
  ctx.arc(x, y, radius, startangle, endangle);
  ctx.lineTo(x, y);
  ctx.fill(); // or context.fill()

  ctx.beginPath();//ADD THIS 
  ctx.fillStyle = '#d45d54';//#DC143C
  ctx.strokeStyle="#black";
  ctx.lineWidth="10";
  ctx.arc(x, y, 7, 0, 2 * Math.PI);
  ctx.stroke();
  ctx.fill(); // or context.fill()*/
}

 //canvas initiatio

