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