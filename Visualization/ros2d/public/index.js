

var canvas;
var ctx;
var start;
var oldTimeStamp;
var x;
var y;
var theta;
var data;
var time;
const today = new Date();
today.setTime(0);
const FRAMES_PER_SECOND = 10;  // Valid values are 60,30,20,15,10...
const FRAME_MIN_TIME = (1000/60) * (60 / FRAMES_PER_SECOND) - (1000/60) * 0.5;
var lastFrameTime = 0;  // the last frame time
Kakao.init("b886eede39b9d47bc9d3cb6e91483799");   // 사용할 앱의 JavaScript 키를 설정
/*
//kakao API
Kakao.init('b886eede39b9d47bc9d3cb6e91483799');
console.log(Kakao.isInitialized());
*/

function shareKakaotalk(data) {
  var n=data;
  Kakao.Link.sendDefault({
        objectType:"feed"
      , content : {
            title:"이벤트 발생"   // 콘텐츠의 타이틀
          , description: n/* 콘텐츠 상세설명*/   
          , imageUrl:"/mnt/c/Users/giwon/Downloads/BADA.jpg"   // 썸네일 이미지 이거 링크로 바꿔야할듯
          , link : {
                mobileWebUrl:"http://192.168.137.1"   // 모바일 카카오톡에서 사용하는 웹 링크 URL
              , webUrl:"http://192.168.137.1" // PC버전 카카오톡에서 사용하는 웹 링크 URL
          }
      }
  });
}



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

class List{
  constructor()
  {
    this.listSize=0;
    this.pos=0;
    this.dataStore=[];
  } 

  append(element){
    this.dataStore[this.listSize++]=element;
  }

  length()
  {
    return this.listSize;
  }

  search(){
    var cnt=0;
    for(var i=0; i<this.dataStore.length;i++)
    {
      if((this.dataStore[i]-time)>1800000) //30분 이상이면 반복문탈출
      {
        break;
      }
      else{
        cnt++;
        console.log("cnt++");
      }
    }
    if(cnt>=3)
    { console.log("cnt>=3");
      return true;
    }
    else{
      return false;
    }
  }

  remove(){
    var pos=this.listSize-1;
    this.dataStore.splice(pos,1);
    --this.listSize;
  }
}

//overflow 막기위해서 10분에한번씩 water.remove실행
w_remove = setInterval(function() {
  if(water.listSize>0){
    water.remove();}
}, 600000);


var water = new List();

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
  ros.connect('ws://192.168.0.7:9090');

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


  var signal = new ROSLIB.Topic({
    ros : ros,
    name : '/signal',
    messageType : 'std_msgs/String'
  });

  signal.subscribe(function(m){
    data=m.data;
    time=today.getTime();
    if(data=='Water')
    {
      //먼저 검색해
      if(water.search())
      {
        console.log("발생한적있어.");
      //발생한적있다면
      shareKakaotalk(data);
      }
      else
      {
      //발생한적없다면
      water.append(time);
      }
  
    }
  }
  );
  
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
