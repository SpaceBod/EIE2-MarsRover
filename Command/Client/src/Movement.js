import React, { Component } from 'react';
import { useGamepads } from 'react-gamepads';
import $ from "jquery";
import axios from 'axios';
import './movement.css';
import Stats from './stats';
import Map from './map';

// TODO: CHANGE CORNER ABCD
class ChangePos extends React.Component {

  changeCorner(){
  }
  
  render() {
    return (
      <button onClick={() => this.changeCorner()} className="btnCorner">Change Corner</button>
    );
  }
}

// ANGLE MANUAL INPUT
class AngleInput extends React.Component {

  constructor(props) {
    super(props);
    this.sayHello = this.sendData.bind(this);
  }

  // SEND DATA USING BUTTON
  sendData(movement){
    var doSend = false;
    const  colour = getComputedStyle(document.querySelector('.statusLED')).backgroundColor;
    if (colour === "rgb(255, 0, 0)" || colour === ""){
      doSend =  false;
    }
    else{
      doSend = true;
    }

    if(doSend){

      const data = { body: movement };
      // axios.post('http://ec2-18-133-76-249.eu-west-2.compute.amazonaws.com:3003/displaymovement', data)
      // axios.post('http://localhost:3001', data)
      axios.post('http://146.169.216.172:3003/movement', data)
      .then((response) => {
        console.log(response);
      });
      const endData = { body: {distance: "0cm", angle: "0°", power: "0"} };
      // axios.post('http://ec2-18-133-76-249.eu-west-2.compute.amazonaws.com:3003/displaymovement', endData)
      // axios.post('http://localhost:3001', endData)
      axios.post('http://146.169.216.172:3003/movement', endData)
      
      .then((response) => {
        console.log(response);
    });
    }
  }

  componentDidMount(){
    // SLIDER CODE
    const ajax_script = document.createElement("script");
    ajax_script.async = true;
    ajax_script.src = "//cdnjs.cloudflare.com/ajax/libs/jquery/2.1.3/jquery.min.js";
    this.div.appendChild(ajax_script);
    var rangeSliderAngle = function(){
      var slider = $('.range2-slider'),
          range = $('.range2-slider__range'),
          value = $('.range2-slider__value');
        
      slider.each(function(){
    
        value.each(function(){
          var value = $(this).prev().attr('value');
          $(this).html(value);
        });
        range.on('input', function(){
          $(this).next(value).html(this.value + "°");
        });
      });
    };
    rangeSliderAngle();
  }

  render() {
    return(
      <div>
        <div className="Range" ref={el => (this.div = el)}></div>
        <div className="range2-slider">
          <input className="range2-slider__range" type="range" defaultValue={0+ "°"} min={-180} max={180} step={45} />
          <span className="range2-slider__value">0</span>
        </div>
        <button className='btn' onClick= {() => this.sendData({distance: document.getElementsByClassName("range-slider__value")[0].innerHTML, angle: document.getElementsByClassName("range2-slider__value")[0].innerHTML, power:"50"})} >Send Instruction</button>
        <ChangePos />
        <div className = "status"> Remote Control
          <div className = "statusLED"></div>
        </div>
      </div>
    );
  }
}

// DISTANCE MANUAL INPUT
class DistInput extends React.Component {

  componentDidMount(){
    // SLIDER CODE
    const ajax_script = document.createElement("script");
    ajax_script.async = true;
    ajax_script.src = "//cdnjs.cloudflare.com/ajax/libs/jquery/2.1.3/jquery.min.js";
    this.div.appendChild(ajax_script);
    var rangeSliderDist = function(){
      var slider = $('.range-slider'),
          range = $('.range-slider__range'),
          value = $('.range-slider__value');
        
      slider.each(function(){
    
        value.each(function(){
          var value = $(this).prev().attr('value');
          $(this).html(value);
        });
        range.on('input', function(){
          $(this).next(value).html(this.value + "cm");
        });
      });
    };
    rangeSliderDist();

  }
  render() {
    return(
      <div>
        <div className="Range" ref={el => (this.div = el)}></div>
        <div className="range-slider">
          <input className="range-slider__range" type="range" defaultValue={0+ "cm"} min={-200} max={200} step={5} />
          <span className="range-slider__value">0</span>
        </div>
        <AngleInput />
      </div>
    );
  }
}


class ArrowMovement extends React.Component {

  componentDidMount() {

    let last = +new Date();

    // SEND CONTROLLER OR KEY MOVEMENTS TO NODE.JS
    function sendMovement(movement){
      const now = +new Date();
        if ((now - last > 500) || ((movement['distance'] === "0cm" ) && (movement['angle'] === "0°") && (movement['power'] === "0"))) { // 5 seconds
          if ((movement['distance'] === "0cm" ) && (movement['angle'] === "0°") && (movement['power'] === "0")){
            console.log("[INPUT END]")
          }
          else{
            console.log("[INPUT SENT]");
          }
          last = now;
          const data = { body: movement };
          try{
          // axios.post('http://ec2-18-133-76-249.eu-west-2.compute.amazonaws.com:3003/displaymovement', data)
          // axios.post('http://localhost:3001', data)
          axios.post('http://146.169.216.172:3003/movement', data)
    
          .then((response) => {
            console.log(response);
          });
        }
        catch(e){}
      }
    }

    // SEND MANUAL MOVEMENTS TO NODE.JS
    function sendButton(movement){
      const now = +new Date();
        if (now - last > 500) {
          console.log("[START SEND]");
          last = now;
          const data = { body: movement };
          // axios.post('http://ec2-18-133-76-249.eu-west-2.compute.amazonaws.com:3003/displaymovement', data)
          // axios.post('http://localhost:3001', data)
          axios.post('http://146.169.216.172:3003/movement', data)
          .then((response) => {
            console.log(response);
          });
          const endData = { body: {distance: "0cm", angle: "0°"} };
          // axios.post('http://ec2-18-133-76-249.eu-west-2.compute.amazonaws.com:3003/displaymovement', endData)
          // axios.post('http://localhost:3001', endData)
          axios.post('http://146.169.216.172:3003/movement', endData)
          .then((response) => {
            console.log(response);
            console.log("[END SEND]");
          });
          }
    }
    // SEND MANUAL MOVEMENTS TO NODE.JS
    function toggleMovement(statusToggle){
      const now = +new Date();
        if (now - last > 500) {
          console.log("[START TOGGLE]");
          last = now;
          const data = { body: statusToggle };
          // axios.post('http://ec2-18-133-76-249.eu-west-2.compute.amazonaws.com:3003/displaymovement', data)
          // axios.post('http://localhost:3001', data)
          axios.post('http://146.169.216.172:3003/togglemovement', data)
          .then((response) => {
            console.log(response);
            console.log("[END TOGGLE]")
          });
        }
    }

    // BUTTON ANIMATION DEFINITIONS
    function forwards() {
      $('.up').addClass('pressed');
      $('.left').css('transform', 'translate(0, 0px)');
      $('.down').css('transform', 'translate(0, 0px)');
      $('.right').css('transform', 'translate(0, 0px)');
    }

    function forwardsReset() {
      $('.up').removeClass('pressed');
      $('.left').css('transform', 'translate(0, 0)');
      $('.down').css('transform', 'translate(0, 0)');
      $('.right').css('transform', 'translate(0, 0)');
    }

    function backwards() {
      $('.down').addClass('pressed');
      $('.down').css('transform', 'translate(0, 2px)');
    }

    function backwardsReset() {
      $('.down').removeClass('pressed');
      $('.down').css('transform', 'translate(0, 0)');
    }

    function right() {
      $('.right').addClass('pressed');
      $('.right').css('transform', 'translate(0, 2px)'); 
    }

    function rightReset() {
      $('.right').removeClass('pressed');  
      $('.right').css('transform', 'translate(0, 0)');
    }

    function left() {
      $('.left').addClass('pressed');
      $('.left').css('transform', 'translate(0, 2px)'); 
    }

    function leftReset() {
      $('.left').removeClass('pressed');  
      $('.left').css('transform', 'translate(0, 0)');
    }

    // LISTEN FOR CONNECTED CONTROLLER
    window.addEventListener('gamepadconnected', function (e) {
      console.log(
        'Gamepad connected at index %d: %s. %d buttons, %d axes.',
        e.gamepad.index,
        e.gamepad.id,
        e.gamepad.buttons.length,
        e.gamepad.axes.length
      )
    })

    // LISTEN FOR DISCONNECTED CONTROLLER
    window.addEventListener('gamepaddisconnected', function (e) {
      console.log(
        'Gamepad disconnected from index %d: %s',
        e.gamepad.index,
        e.gamepad.id
      )
    })
    var fcount = 0;
    var bcount = 0;
    var rcount = 0;
    var lcount = 0;
    var acount = 0;
    var tcount = 0;
    var togglecount = 0;
    var toggleValue = false;
    var rtPow = 0;
    var ltPow = 0;
    var send = false;

    const element = document.querySelector('.statusLED');
    const style = getComputedStyle(element);   

    // LOOP TO CHECK CONTROLLER INPUT
    setInterval(() => {
      try{
        const myGamepad = navigator.getGamepads()[0];
        const rt = myGamepad.buttons[7].pressed;
        const lt = myGamepad.buttons[6].pressed;
        const aButton = myGamepad.buttons[0].pressed;
        const axisLR = myGamepad.axes[0];
        const dpadL = myGamepad.buttons[14].pressed;
        const dpadR = myGamepad.buttons[15].pressed;
        const dpadU = myGamepad.buttons[12].pressed;
        const dpadD = myGamepad.buttons[13].pressed;
        const toggleRemote = myGamepad.buttons[9].pressed;
        const varRT = myGamepad.buttons[7];
        const varLT = myGamepad.buttons[6];

        var total = fcount + bcount + rcount + lcount;

        if (element.style.backgroundColor === "rgb(255, 0, 0)" || element.style.backgroundColor === ""){
          send = false;
        }
        else {
          send = true;
        }

        if (send === true) {
          // FORWARDS
          if (rt === true){
            fcount = 1;
            tcount = 0;
            rtPow = (Math.round(100*varRT.value)).toString();
            
            if (axisLR > 0.5){
              sendMovement({distance:"2cm", angle: "5°", power:rtPow});
            }
            if (axisLR < -0.5){
              sendMovement({distance:"2cm", angle: "-5°", power:rtPow});
            }
            else{
              sendMovement({distance:"2cm", angle: "0°", power:rtPow});
            }
            forwards();
          }
          // FORWARDS RESET
          if (rt === false && fcount === 1){
            fcount = 0;
            forwardsReset();
          }
          // BACKWARDS
          if (lt === true && fcount === 0){
            bcount = 1;
            tcount = 0;
            ltPow = (Math.round(100*varLT.value)).toString();
            if (axisLR > 0.5){
              sendMovement({distance:"-2cm", angle: "5°", power:ltPow});
            }
            if (axisLR < -0.5){
              sendMovement({distance:"-2cm", angle: "-5°", power:ltPow});
            }
            else{
              sendMovement({distance:"-2cm", angle: "0°", power:ltPow});
            }
            backwards();
          }
          // BACKWARDS RESET
          if (lt === false && bcount === 1){
            bcount = 0;
            backwardsReset();
          }
          // RIGHT ROTATE
          if (axisLR > 0.5){
            rcount = 1;
            tcount = 0;
            sendMovement({distance:"0cm", angle: "5°", power:"50"});
            right();
          }
          // RIGHT ROTATE RESET
          if (axisLR <= 0.5 && rcount === 1){
            rcount = 0;
            rightReset();
          }
          // LEFT ROTATE
          if (axisLR < -0.5){
            lcount = 1;
            tcount = 0;
            sendMovement({distance:"0cm", angle: "-5°", power:"50"});
            left();
          }
          // LEFT ROTATE RESET
          if (axisLR >= -0.5 && lcount === 1){
            lcount = 0;
            leftReset();
          }
          // RESET POSITION
          total = fcount + bcount + rcount + lcount;
          if (total === 0 && tcount !== 1){
            tcount = 1;
            sendMovement({distance:"0cm", angle: "0°", power:"0"});
          }
          // SLIDER CONTROL DPAD
          // DISTANCE POSITIVE
          if (dpadR === true){
            var distancePosR = document.getElementsByClassName('range-slider__range')[0];
            var distanceValR = parseInt(distancePosR.value);
            if (distanceValR + 5 <= 200) {
              distanceValR += 5 ;
            }
            else{
              distanceValR +=0;
            }
            distancePosR.value = distanceValR;
            document.getElementsByClassName('range-slider__value')[0].innerHTML = distanceValR + "cm";
          }
          // DISTANCE NEGATIVE
          if (dpadL === true){
            var distancePosL = document.getElementsByClassName('range-slider__range')[0];
            var distanceValL = parseInt(distancePosL.value);
            if (distanceValL - 5 >= -200) {
              distanceValL -= 5 ;
            }
            else{
              distanceValL-=0;
            }
            distancePosL.value = distanceValL;
            document.getElementsByClassName('range-slider__value')[0].innerHTML = distanceValL + "cm";
          }
          // ANGLE POSITIVE
          if (dpadU === true){
            var anglePosU = document.getElementsByClassName('range2-slider__range')[0];
            var angleValU = parseInt(anglePosU.value);
            if (angleValU + 45 <= 180) {
              angleValU+=45;
            }
            else{
              angleValU += 0;
            }
            anglePosU.value = angleValU;
            document.getElementsByClassName('range2-slider__value')[0].innerHTML = angleValU + "°";
          }
          // ANGLE NEGATIVE
          if (dpadD === true){
            var anglePosD = document.getElementsByClassName('range2-slider__range')[0];
            var angleValD = parseInt(anglePosD.value);
            if (angleValD - 45 >= -180) {
              angleValD -= 45 ;
            }
            else{
              angleValD-=0;
            }
            anglePosD.value = angleValD;
            document.getElementsByClassName('range2-slider__value')[0].innerHTML = angleValD + "°";
          }
          // BUTTON A: SEND MANUAL DISTANCE + ANGLE
          if (aButton === true && acount === 0){
            acount = 1;
            sendButton({distance: document.getElementsByClassName("range-slider__value")[0].innerHTML, angle: document.getElementsByClassName("range2-slider__value")[0].innerHTML});
            document.getElementsByClassName("btn")[0].style.top="6px";
          }
          if (aButton === false && acount === 1){
            acount = 0;
            document.getElementsByClassName("btn")[0].style.top="0px";
          }
        }

        if (toggleRemote === true && togglecount === 0){
          togglecount = 1;
          toggleValue = !toggleValue;

          if (style.backgroundColor === "rgb(0, 255, 0)") {
            element.style.backgroundColor = "rgb(255, 0, 0)";
          }
          else{
            element.style.backgroundColor = "rgb(0, 255, 0)";
          }
            toggleMovement(toggleValue.toString());
        }
        if (toggleRemote === false && togglecount === 1){
          togglecount = 0;
        }
      }
      catch(e){
      }
    }, 100)
    
    // WASD KEY MOVEMENT DOWN
    document.addEventListener('keydown', function(event) {
      if (event.key === "w" && send === true) {
        sendMovement({distance:"2cm", angle: "0°", power:"50"});
        forwards();
      }
      else if (event.key === "a" && send === true) {
        sendMovement({distance:"0cm", angle: "-5°", power:"50"});
        left();
      }
      else if (event.key === "s" && send === true) {
        sendMovement({distance:"-2cm", angle: "0°", power:"50"});
        backwards();
      }
      else if (event.key === "d" && send === true) {
        sendMovement({distance:"0cm", angle: "5°", power:"50"});
        right();
      }
    
  });

  // WASD KEY MOVEMENT UP
  document.addEventListener('keyup', function(event) {
    if (event.key === "w" && send === true) {
      sendMovement({distance:"0cm", angle: "0°", power:"0"});
      forwardsReset();
    }
    else if (event.key === "a" && send === true) {
      sendMovement({distance:"0cm", angle: "0°", power:"0"});
      leftReset();
    }
    else if (event.key === "s" && send === true) {
      sendMovement({distance:"0cm", angle: "0°", power:"0"});
      backwardsReset();
    }
    else if (event.key === "d" && send === true) {
      sendMovement({distance:"0cm", angle: "0°", power:"0"});
      rightReset();
    }
  });

  const buttonStat = document.querySelector('.statusLED')
  buttonStat.addEventListener("click", () => {
    var getColour = buttonStat.style.backgroundColor;
    if (getColour === "rgb(255, 0, 0)" || getColour === ""){
      send = true;
      buttonStat.style.backgroundColor = "rgb(0, 255, 0)";
      console.log("[START TOGGLE]");
      const data = { body: "true" };
      // axios.post('http://ec2-18-133-76-249.eu-west-2.compute.amazonaws.com:3003/displaymovement', data)
      // axios.post('http://localhost:3001', data)
      axios.post('http://146.169.216.172:3003/togglemovement', data)
      .then((response) => {
        console.log(response);
        console.log("[END TOGGLE]")
      });
    }
    else{
      send = false;
      buttonStat.style.backgroundColor = "rgb(255, 0, 0)";
      console.log("[START TOGGLE]");
      const data = { body: "false" };
      // axios.post('http://ec2-18-133-76-249.eu-west-2.compute.amazonaws.com:3003/displaymovement', data)
      // axios.post('http://localhost:3001', data)
      axios.post('http://146.169.216.172:3003/togglemovement', data)
      .then((response) => {
        console.log(response);
        console.log("[END TOGGLE]")
      });
    }
    
  })
  }

  render() {
    return (
      <div>
        <Stats />
        <div className="Movement" ref={el => (this.div = el)}></div>
        <div className="keys">
          <div className="up arr"><i className="fa fa-arrow-up" /></div>
          <br />
          <div className="left arr"><i className="fa fa-arrow-left" /></div>  
          <div className="down arr"><i className="fa fa-arrow-down" /></div>
          <div className="right arr"><i className="fa fa-arrow-right"/></div>
        </div>
        <link href="//netdna.bootstrapcdn.com/font-awesome/4.0.3/css/font-awesome.css" rel="stylesheet" />
        <br></br>
        <br></br>
        <DistInput />
      </div>
    );
  }
}

export default ArrowMovement;