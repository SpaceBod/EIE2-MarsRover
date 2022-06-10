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

    // CREATE EMPTY MAP
    var canvas = document.getElementById(windowSize);
    var ctx = canvas.getContext("2d");
    ctx.clearRect(0, 0, canvas.width, canvas.height);

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

    // CUSTOM ROVER POS OFFSET
    rPos[0] = parseInt(rPos[0])+10;
    rPos[1] = parseInt(rPos[1]);

    // DRAW ALL ALIENS
    for (var x in alienPos){
      ctx.beginPath();
      ctx.lineWidth = 1;
      ctx.fillStyle = getComputedStyle(document.getElementById.bind(document)(aliens[x]).getElementsByClassName("dot")[0]).color;
      ctx.arc(alienPos[x][0], alienPos[x][1], 10, 0, 2 * Math.PI);
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