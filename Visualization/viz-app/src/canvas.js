"use strict";
import ROSLIB from 'roslib';
// const ROSLIB=require('roslib');
var canvas;
var ctx;
var start;
var oldTimeStamp;

// window.onload = init;

const FRAMES_PER_SECOND = 30;  // Valid values are 60,30,20,15,10...
// set the mim time to render the next frame
const FRAME_MIN_TIME = (1000/60) * (60 / FRAMES_PER_SECOND) - (1000/60) * 0.5;
var lastFrameTime = 0;  // the last frame time

export function init() {
  canvas = document.getElementById('canvas');
  console.log(canvas);
  ctx = canvas.getContext('2d');

  // Start the first frame request
  window.requestAnimationFrame(loop);
  
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
  ros.connect('ws://localhost:9090');

  // Like when publishing a topic, we first create a Topic object with details of the topic's name
  // and message type. Note that we can call publish or subscribe on the same topic object.
  var listener = new ROSLIB.Topic({
    ros : ros,
    name : '/listener',
    messageType : 'std_msgs/String'
  });

  // Then we add a callback to be called every time a message is published on this topic.
  listener.subscribe(function(message) {
    console.log('Received message on ' + listener.name + ': ' + message.data);

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
    console.log(message);
    console.log('Received message on ' + turtle1.name + ': ' + message.data);

    // If desired, we can unsubscribe from the topic as well.
    if(count>50){
      turtle1.unsubscribe();
    }
  });
}

function loop(timeStamp) {
  
  // Keep requesting new frames
  if(timeStamp-lastFrameTime < FRAME_MIN_TIME){ //skip the frame if the call is too early
    window.requestAnimationFrame(loop);
    return; // return as there is nothing to do
  }
  lastFrameTime = timeStamp; // remember the time of the rendered frame

  // console.log('drawing');

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
  if (!start) start = timestamp;
  var progress = timestamp - start;

  const cx = 200;
  const cy = 40;
  const radius = progress / 10;
  const startangle = 3/2*Math.PI;
  const endangle = 1/2*Math.PI;
  ctx.fillStyle = 'rgba(255,100,50,0.5)';
  ctx.moveTo(cx, cy);
  ctx.arc(cx, cy, radius, startangle, endangle);
  ctx.lineTo(cx, cy);    
  ctx.fill(); // or context.fill()
}