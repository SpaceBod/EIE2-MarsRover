import React, { Component } from 'react';
import $ from "jquery";
import "./map.css";
import "./stats.css";

class Map extends React.Component {
  
  updateMap(){
    // CHECK WINDOW RESIZE
    if(window.innerWidth < 600) {
      var windowSize = "displayScroll";
    }
    else {
      windowSize = "display";
    }

    // GET ALIEN POS
    const a0Pos = document.getElementById("a0").getElementsByClassName("pos")[0].innerHTML.split(',');
    const a1Pos = document.getElementById("a1").getElementsByClassName("pos")[0].innerHTML.split(',');
    const a2Pos = document.getElementById("a2").getElementsByClassName("pos")[0].innerHTML.split(',');
    const a3Pos = document.getElementById("a3").getElementsByClassName("pos")[0].innerHTML.split(',');
    const a4Pos = document.getElementById("a4").getElementsByClassName("pos")[0].innerHTML.split(',');
    const a5Pos = document.getElementById("a5").getElementsByClassName("pos")[0].innerHTML.split(',');
    
    var alienPos = [a0Pos, a1Pos, a2Pos, a3Pos, a4Pos, a5Pos];
    var aliens = ["a0", "a1", "a2", "a3", "a4", "a5"];

    // GET ROVER & FAN STATS
    const rPos = document.getElementById("rover").getElementsByClassName("pos")[0].innerHTML.split(',');
    const fPos = document.getElementById("fan").getElementsByClassName("pos")[0].innerHTML.split(',');
    const angle = parseInt(document.getElementById("angleValue").innerHTML);

    var corner = document.getElementsByClassName("btnCorner")[0].innerHTML.split(": ").pop();
    var xmult = 1;
    var ymult = 1;
    var xoffset = 0;
    var yoffset = 0;

    // CREATE EMPTY MAP
    var canvas = document.getElementById(windowSize);
    var ctx = canvas.getContext("2d");
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    // CORNER SQUARES
    ctx.fillStyle = "rgb(54, 54, 54)";
    ctx.strokeRect(0, 0, 40, 40);
    ctx.rect(0, 0, 40, 40);
    

    ctx.fillStyle = "rgb(54, 54, 54)";
    ctx.strokeRect(canvas.width - 40, 0, 40, 40);
    ctx.rect(canvas.width - 40, 0, 40, 40);

    ctx.fillStyle = "rgb(54, 54, 54)";
    ctx.strokeRect(0, canvas.height - 40, 40, 40);
    ctx.rect(0, canvas.height -  40, 40, 40);

    ctx.fillStyle = "rgb(54, 54, 54)";
    ctx.strokeRect(canvas.width - 40, canvas.height - 40, 40, 40);
    ctx.rect(canvas.width - 40, canvas.height - 40, 40, 40);
    ctx.fill();

    // LETTERS AT CORNERS
    ctx.beginPath();
    ctx.font = '20px Sans-serif';
    ctx.textAlign = "center";
    ctx.textBaseline = "middle";
    if (corner[0] === "A"){
      ctx.fillStyle = "rgb(246,190,0)";
      ctx.fillText("A", 20, 20);
      ctx.fillStyle = "rgb(94, 94, 94)";
      ctx.fillText("B", canvas.width - 20, 20);
      ctx.fillStyle = "rgb(94,94,94)";
      ctx.fillText("C", 20, canvas.height - 20);
      ctx.fillStyle = "rgb(94,94,94)";
      ctx.fillText("D", canvas.width - 20, canvas.height - 20);
      ctx.fill();
      xmult = -1;
      ymult = 1;
      xoffset = 0;
      yoffset = 0;
      // CUSTOM ROVER POS OFFSET
      rPos[0] = parseInt(rPos[0])-10;
      rPos[1] = parseInt(rPos[1])+5;
    }
    else if (corner[0] === "B"){
      ctx.fillStyle = "rgb(246,190,0)";
      ctx.fillText("B", canvas.width - 20, 20);
      ctx.fillStyle = "rgb(94, 94, 94)";
      ctx.fillText("A", 20, 20);
      ctx.fillStyle = "rgb(94,94,94)";
      ctx.fillText("C", 20, canvas.height - 20);
      ctx.fillStyle = "rgb(94,94,94)";
      ctx.fillText("D", canvas.width - 20, canvas.height - 20);
      xoffset = canvas.width;
      xmult = -1;
      // CUSTOM ROVER POS OFFSET
      rPos[0] = parseInt(rPos[0])+10;
      rPos[1] = parseInt(rPos[1])+5;
    }
    else if (corner[0] === "C"){
      ctx.fillStyle = "rgb(94, 94, 94)";
      ctx.fillText("A", 20, 20);
      ctx.fillStyle = "rgb(94, 94, 94)";
      ctx.fillText("B", canvas.width - 20, 20);
      ctx.fillStyle = "rgb(246,190,0)";
      ctx.fillText("C", 20, canvas.height - 20);
      ctx.fillStyle = "rgb(94,94,94)";
      ctx.fillText("D", canvas.width - 20, canvas.height - 20);
      ymult = -1;
      yoffset = canvas.height;
      // CUSTOM ROVER POS OFFSET
      rPos[0] = parseInt(rPos[0])+10;
      rPos[1] = parseInt(rPos[1])+15;
    }
    else if (corner[0] === "D"){
      ctx.fillStyle = "rgb(94, 94, 94)";
      ctx.fillText("A", 20, 20);
      ctx.fillStyle = "rgb(94, 94, 94)";
      ctx.fillText("B", canvas.width - 20, 20);
      ctx.fillStyle = "rgb(94, 94, 94)";
      ctx.fillText("C", 20, canvas.height - 20);
      ctx.fillStyle = "rgb(246,190,0)";
      ctx.fillText("D", canvas.width - 20, canvas.height - 20);
      xmult = 1;
      xoffset = canvas.width;
      ymult = -1;
      yoffset = canvas.height;
      // CUSTOM ROVER POS OFFSET
      rPos[0] = parseInt(rPos[0])-15;
      rPos[1] = parseInt(rPos[1])+15;
    }
    ctx.fill();

    // APPLY CORNER CORRECTION
    
    a0Pos[0] = xoffset + xmult*parseInt(a0Pos[0]);
    a0Pos[1] = yoffset + ymult*parseInt(a0Pos[1]);
    console.log(a0Pos[0], a0Pos[1])
    a1Pos[0] = xoffset + xmult*parseInt(a1Pos[0]);
    a1Pos[1] = yoffset + ymult*parseInt(a1Pos[1]);
    a2Pos[0] = xoffset + xmult*parseInt(a2Pos[0]);
    a2Pos[1] = yoffset + ymult*parseInt(a2Pos[1]);
    a3Pos[0] = xoffset + xmult*parseInt(a3Pos[0]);
    a3Pos[1] = yoffset + ymult*parseInt(a3Pos[1]);
    a4Pos[0] = xoffset + xmult*parseInt(a4Pos[0]);
    a4Pos[1] = yoffset + ymult*parseInt(a4Pos[1]);
    a5Pos[0] = xoffset + xmult*parseInt(a5Pos[0]);
    a5Pos[1] = yoffset + ymult*parseInt(a5Pos[1]);
    
    rPos[0] = (xoffset + xmult*parseInt(rPos[0])).toString();
    rPos[1] = (yoffset + ymult*parseInt(rPos[1])).toString();
    fPos[0] = (xoffset + xmult*parseInt(fPos[0])).toString();
    fPos[1] = (yoffset + ymult*parseInt(fPos[1])).toString();

    // DRAW ALL ALIENS
    for (var x in alienPos){
      ctx.beginPath();
      ctx.lineWidth = 1;
      ctx.fillStyle = getComputedStyle(document.getElementById.bind(document)(aliens[x]).getElementsByClassName("dot")[0]).color;
      ctx.arc(alienPos[x][0], alienPos[x][1], 10, 0, 2 * Math.PI);
      ctx.fill();
      ctx.fillStyle = "white";
      ctx.arc(alienPos[x][0], alienPos[x][1], 2, 0, 2 * Math.PI);
    }

    for (var i in alienPos){
      ctx.beginPath();
      ctx.lineWidth = 1;
      ctx.fillStyle = "white";
      ctx.arc(alienPos[i][0]-4, alienPos[i][1]-4, 2, 0, 2 * Math.PI);
      ctx.fill();
    }
    
    // DRAW ROVER
    ctx.beginPath();
    ctx.lineWidth = 2;
    ctx.fillStyle = getComputedStyle(document.getElementById.bind(document)("rover").getElementsByClassName("dot")[0]).color;
    ctx.arc(rPos[0], rPos[1], 10, 0.4, Math.PI);
    ctx.moveTo(parseInt(rPos[0])+5, parseInt(rPos[1])+10);
    ctx.arc(parseInt(rPos[0])+5, parseInt(rPos[1])+10, 3,0, 2 * Math.PI);
    ctx.moveTo(parseInt(rPos[0])-5, parseInt(rPos[1])+10);
    ctx.arc(parseInt(rPos[0])-5, parseInt(rPos[1])+10, 3,0, 2 * Math.PI);
    ctx.rect(parseInt(rPos[0])-4, parseInt(rPos[1])-4, 0.1, 5);
    ctx.strokeStyle = "rgb(255,255,255)";
    ctx.stroke();
    ctx.fill();

    // DRAW DIRECTION ARROW
    ctx.save();
    ctx.font = '12px Sans-serif';
    ctx.strokeStyle = 'red';
    ctx.textAlign = "center";
    ctx.textBaseline = "bottom";
    ctx.translate(parseInt(rPos[0]), parseInt(rPos[1]) + 8);
    ctx.rotate(angle * Math.PI/180);
    ctx.strokeText("↑", 0, 0);
    ctx.fillStyle = "rgb(255, 0, 0)";
    ctx.fillText("↑", 0, 0);
    ctx.restore();

    // DRAW FAN
    ctx.beginPath();
    ctx.lineWidth = 1;
    ctx.fillStyle = getComputedStyle(document.getElementById.bind(document)("fan").getElementsByClassName("dot")[0]).color;
    ctx.moveTo(fPos[0], fPos[1]);
    ctx.lineTo(parseInt(fPos[0])+7, parseInt(fPos[1])-14);
    ctx.lineTo(parseInt(fPos[0])-7, parseInt(fPos[1])-14);
    ctx.lineTo(parseInt(fPos[0])+7, parseInt(fPos[1])+14);
    ctx.lineTo(parseInt(fPos[0])-7, parseInt(fPos[1])+14);
    ctx.moveTo(fPos[0], fPos[1]);
    ctx.lineTo(parseInt(fPos[0])+14, parseInt(fPos[1])+7);
    ctx.lineTo(parseInt(fPos[0])+14, parseInt(fPos[1])-7);
    ctx.lineTo(parseInt(fPos[0])-14, parseInt(fPos[1])+7);
    ctx.lineTo(parseInt(fPos[0])-14, parseInt(fPos[1])-7);
    ctx.strokeStyle = getComputedStyle(document.getElementById.bind(document)("fan").getElementsByClassName("dot")[0]).color;
    ctx.stroke();
    ctx.fill();
    ctx.beginPath();
    ctx.arc(fPos[0], fPos[1], 3, 0, 2 * Math.PI);
    ctx.fill();
    ctx.lineWidth = 4;
    ctx.rect(fPos[0]-14, fPos[1]-14, 28, 28);
    ctx.stroke();
  }

  componentDidMount(){
    
    // FUNCTION TO AUTO RESIZE WINDOW
    function resize(){
      if(window.innerWidth < 600){

        try{
          // PLACE MAP VERTICALLY
          document.getElementById("display").id = "displayScroll";
        }
        catch(e){}
        }
      else{
        try{
          // PLACE MAP HORIZONTALLY
          document.getElementById("displayScroll").id = "display";
        }
        catch(e){}
      }
    }
    resize();

    // EVENT LISTENER TO CHECK RESIZING
    window.addEventListener('load', resize, {once: true});
    window.addEventListener('resize', resize);

    var aliens = ["a0", "a1", "a2", "a3", "a4", "a5"];

    // OBSERVES CHANGES IN ALIENS
    let observeAlien = new MutationObserver(mutationRecords => {
      this.updateMap();
      console.log("Alien stats updated..."); // console.log(the changes)
    });

    // OBSERVES CHANGES IN ROVER
    let observeRover = new MutationObserver(mutationRecords => {
      this.updateMap();
      console.log("Rover stats updated..."); // console.log(the changes)
    });

    // OBSERVES CHANGES IN CORNER ORIENTATION
    let observeCorner = new MutationObserver(mutationRecords => {
      this.updateMap();
      console.log("Corner Pos updated..."); // console.log(the changes)
    });

    // APPLIES OBSERVER TO ALIENS
    for (var x in aliens) {
      observeAlien.observe(document.getElementById(aliens[x]), {
        childList: true, // observe direct children
        subtree: true, // and lower descendants too
        characterDataOldValue: true // pass old data to callback
      });
    }

    // APPLIES OBSERVER TO ROVER
    observeRover.observe(document.getElementById("rover"), {
      childList: true,
      subtree: true, 
      characterDataOldValue: true
    });

    // APPLIES OBSERVER TO FAN
    observeAlien.observe(document.getElementById("fan"), {
      childList: true,
      subtree: true, 
      characterDataOldValue: true
    });

    // APPLIES OBSERVER TO DIRECTION
    observeRover.observe(document.getElementById("angleValue"), {
      childList: true,
      subtree: true, 
      characterDataOldValue: true
    });
    
    observeCorner.observe(document.getElementsByClassName("btnCorner")[0], {
      childList: true,
      subtree: true, 
      characterDataOldValue: true
    });

}

componentWillUnmount() {
  clearInterval(this.interval);
}

  render() {
    return (
      <div className="adjust">
        <canvas id="display" width="480" height="720" />
      </div>
    );
  }
}

export default Map;