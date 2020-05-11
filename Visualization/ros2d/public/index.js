

var canvas;
var ctx;
var start;
var oldTimeStamp;
var x;
var y;
var theta;

const FRAMES_PER_SECOND = 10;  // Valid values are 60,30,20,15,10...
const FRAME_MIN_TIME = (1000/60) * (60 / FRAMES_PER_SECOND) - (1000/60) * 0.5;
var lastFrameTime = 0;  // the last frame time


//kakao API
Kakao.init('b886eede39b9d47bc9d3cb6e91483799');
console.log(Kakao.isInitialized());


Kakao.Link.sendScrap({
  requestUrl: 'http://192.168.43.254'
});


//Tab design 
$(document).ready(function(){
  // process.. 
  $('.tab_menu_btn').on('click',function(){
    //버튼 색 제거,추가
    $('.tab_menu_btn').removeClass('on');
    $(this).addClass('on')
    
    //컨텐츠 제거 후 인덱스에 맞는 컨텐츠 노출
    var idx = $('.tab_menu_btn').index(this);
    
    $('.tab_box').hide();
    $('.tab_box').eq(idx).show();
  });
});



function tryConnectWebsocket(){

  // Connecting to ROS
  // -----------------
  var ros = new ROSLIB.Ros();

  // If there is an error on the backend, an 'error' emit will be emitted.
  ros.on('error', function(error) {
    // document.getElementById('connecting').style.display = 'none';
    // document.getElementById('connected').style.display = 'none';
    // document.getElementById('closed').style.display = 'none';
    // document.getElementById('error').style.display = 'inline';
    console.log(error);
  });

  // Find out exactly when we made a connection.
  ros.on('connection', function() {
    console.log('Connection made!');
    // document.getElementById('connecting').style.display = 'none';
    // document.getElementById('error').style.display = 'none';
    // document.getElementById('closed').style.display = 'none';
    // document.getElementById('connected').style.display = 'inline';
  });

  ros.on('close', function() {
    console.log('Connection closed.');
    // document.getElementById('connecting').style.display = 'none';
    // document.getElementById('connected').style.display = 'none';
    // document.getElementById('closed').style.display = 'inline';
  });

  // Create a connection to the rosbridge WebSocket server.
  ros.connect('ws://192.168.43.254:9090');

  // Like when publishing a topic, we first create a Topic object with details of the topic's name
  // and message type. Note that we can call publish or subscribe on the same topic object.
  var listener = new ROSLIB.Topic({
    ros : ros,
    name : '/listener',
    messageType : 'std_msgs/String'
  });

  // Then we add a callback to be called every time a message is published on this topic.
  listener.subscribe(function(message) {
    // console.log('Received message on ' + listener.name + ': ' + message.data);

    // If desired, we can unsubscribe from the topic as well.
    listener.unsubscribe();
  });

    // Like when publishing a topic, we first create a Topic object with details of the topic's name
  // and message type. Note that we can call publish or subscribe on the same topic object.
  var turtle1 = new ROSLIB.Topic({
    ros : ros,
    name : '/turtle1/pose',
    messageType : 'turtlesim/Pose'
  });

  // Then we add a callback to be called every time a message is published on this topic.
  var count=0;
  turtle1.subscribe(function(message) {
    if(count){
      console.log(message);
    }

    // console.log('received a turtlesim message');
    // console.log(message);
    x=message.x*50;
    y=message.y*50;
    theta=message.theta;
    // If desired, we can unsubscribe from the topic as well.
    // if(count>5000){
    //   turtle1.unsubscribe();
    // }
  });
}



function init() {
  canvas = document.getElementById('canvas');
  console.log(canvas);
  ctx = canvas.getContext('2d');

  // Start the first frame request
  window.requestAnimationFrame(loop);
  tryConnectWebsocket();
}

function loop(timeStamp) {
  
  // Keep requesting new frames
  if(timeStamp-lastFrameTime < FRAME_MIN_TIME){ //skip the frame if the call is too early
    window.requestAnimationFrame(loop);
    return; // return as there is nothing to do
  }
  lastFrameTime = timeStamp; // remember the time of the rendered frame

  ctx.clearRect(0, 0, canvas.width, canvas.height);
  
  var secondsPassed = (timeStamp - oldTimeStamp) / 1000;
  var elapsedSeconds = (timeStamp - start)/1000;
  oldTimeStamp = timeStamp;

  //Calculate fps
  var fps = Math.round(10 / secondsPassed)/10;
  
  //Draw number to the screen
  ctx.fillStyle = 'white';
  ctx.fillRect(0, 0, 200, 100);
  ctx.font = '25px Arial';
  ctx.fillStyle = 'black';
  ctx.fillText("FPS: " + fps, 10, 30);

  draw(timeStamp);

  if(elapsedSeconds>5){
    return;
  }else{
    window.requestAnimationFrame(loop);
  }
}

function draw(timestamp) {
  // if (!start) start = timestamp;
  // var progress = timestamp - start;

  //Draw number to the screen
  const radius = 1500;
  const startangle = theta-1/16*Math.PI;
  const endangle = theta+1/16*Math.PI;
  ctx.fillStyle = 'rgba(255,100,50,0.3)';
  
  ctx.beginPath();//ADD THIS LINE!<<<<<<<<<<<<<
  ctx.moveTo(x, y);
  ctx.arc(x, y, radius, startangle, endangle);
  ctx.lineTo(x, y);
  ctx.fill(); // or context.fill()

  ctx.beginPath();//ADD THIS 
  ctx.fillStyle = 'red';//#DC143C
  ctx.arc(x, y, 5, 0, 2*Math.PI);
  ctx.fill(); // or context.fill()
}




  // /**
  //  * Setup all visualization elements when the page is loaded.
  //  */
  // function init() {
  //   console.log('load');
  //   // Connect to ROS.
  //   var ros = new ROSLIB.Ros({
  //     url : 'ws://localhost:9090'
  //   });

  //   // Create the main viewer.
  //   var viewer = new ROS2D.Viewer({
  //     divID : 'map',
  //     width : 600,
  //     height : 500
  //   });

  //   // Setup the map client.
  //   var gridClient = new ROS2D.OccupancyGridClient({
  //     ros : ros,
  //     rootObject : viewer.scene
  //   });
  //   // Scale the canvas to fit to the map
  //   gridClient.on('change', function(){
  //     viewer.scaleToDimensions(gridClient.currentGrid.width, gridClient.currentGrid.height);
  //   });
  // }
