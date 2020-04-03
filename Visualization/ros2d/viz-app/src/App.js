import React from 'react';
import logo from './logo.svg';
import './App.css';
import { init } from './canvas';

function App() {
  const canvasRef=React.useRef(null);
  const [count, setCount]=React.useState(0);

  let start;
  
  /**
   * @param {*} startPos 
   * @param {*} targetPos 
   * @param {*} theta 
   */
  function drawCircularSector(startPos, targetPos, theta=0){
    
  }

  function handleClick(e){
    e.preventDefault();
    console.log('button was clicked');
    
    // const canvas=canvasRef.current;
    // const ctx=canvas.getContext('2d');
    
    // ctx.font="40px Courier";
    // ctx.fillStyle='#FFFFFF';
    // ctx.fillRect(0,0,640,480);
    // ctx.fillStyle='#000000';
    // ctx.fillText('whahahdhwdw', 210, 75);

    // const cx=30;
    // const cy=40;
    // const radius=25.0;
    // const startangle=1;
    // const endangle=2;
    // ctx.fillStyle='rgba(255,100,50,0.5)';
    // ctx.moveTo(cx,cy);
    // ctx.arc(cx,cy,radius,startangle,endangle);
    // ctx.lineTo(cx,cy);
    // ctx.fill(); // or context.fill()

    init();

    // window.requestAnimationFrame(gameLoop);
  }

  return (
    <div className="App">
      <header className="App-header">
        <img src={logo} className="App-logo" alt="logo" />
        <p>{count}</p>
        <p>
          Edit <code>src/App.js</code> and save to reload.
        </p>
        <a
          className="App-link"
          href="https://reactjs.org"
          target="_blank"
          rel="noopener noreferrer"
        >
          Learn React
        </a>
        <button onClick={handleClick}>draw</button>
        {/* <canvas id="canvas" ref={canvasRef} width={640} height={480}>

        </canvas> */}
      </header>
    </div>
  );
}

export default App;
