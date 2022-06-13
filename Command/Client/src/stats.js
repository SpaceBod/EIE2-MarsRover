import React, { Component } from 'react';
import $, { map } from "jquery"
import axios from 'axios';
import Map from './map';
import Styles from './stats.css';
import updateMap from './map';

class Stats extends React.Component {

  constructor(props) {
    super(props);
    // EMPTY STATE VALUES
    this.state = {
      pos0: "null",
      pos1: "null",
      pos2: "null",
      pos3: "null",
      pos4: "null",
      pos5: "null",
      pos6: "null",
      pos7: "null"
    }
    this.state = {
      posr: "null",
      posf: "null",
      battery: "--",
      angle: "0"
    }
  }

  // GET URL AND SAVE RESPONSE DATA
  getAliens () {
    return axios.get("http://ec2-18-133-76-249.eu-west-2.compute.amazonaws.com:3003/getaliens")
        .then(response => {
          this.response = response.data
          return this.response
        })
    }
  // GET URL AND SAVE RESPONSE DATA
  getRover () {
    return axios.get("http://ec2-18-133-76-249.eu-west-2.compute.amazonaws.com:3003/getrover")
        .then(response => {
          this.response = response.data
          return this.response
        })
    }

  updateContent = () => {
    const date = new Date();
    let time = date.toLocaleTimeString()

    // PARSE ALIENS FOR COORDS & COLOUR
    this.getAliens().then(data => {
      for(var i in data){
        var xpos = data[i]["coords"].split(', ')[0];
        var ypos = data[i]["coords"].split(', ')[1];
        var coords = xpos.padStart(3, "0") + ", " +  ypos.padStart(3, "0");
        var alienID = "a"+i.toString();
        try{
          // ADD ALIEN COLOUR
          document.getElementById(alienID).getElementsByClassName("dot")[0].style.color = data[i]["colour"];
        }
        catch (e){}
        var alienNumber = "pos"+i;
        // ADD ALIEN COORDS
        this.setState({
          [alienNumber]: coords
        });
      }
      
    });

    // PARSE ROVER FOR COORDS & ANGLE & BATTERY
    this.getRover().then(data => {
      this.setState({
        posr: data[0]["coords"],
        battery: data[0]["battery"],
        angle: data[0]["angle"]
      })
      // SET ANGLE ELEMENT FROM DATA
      document.getElementById("angle").style.transform = "rotateZ("  + data[0]["angle"] + "deg)";
      document.getElementById("angleValue")[0].innerHTML = data[0]["angle"];
    });
  
  // UPDATE TIME
  document.getElementById("time").getElementsByClassName("value")[0].innerHTML = time;
  }

  componentDidMount(){
    this.updateContent()
    this.interval = setInterval(() => this.updateContent(), 5000);
  }
  render() {
    return (
      <div>
        <div className="title">COMMAND</div>

        <div id="block">
        <div id="a0">
        <div className="dot">Ⓐ&nbsp;</div>
        <div className="bracket">&nbsp;[&nbsp;</div>
        <div className='pos'>{ this.state.pos0 } </div>
        <div className="bracket">]</div>
        </div>
        <div id="a1">
        <div className="dot">Ⓐ&nbsp;</div>
        <div className="bracket">&nbsp;[&nbsp;</div>
        <div className='pos'>{ this.state.pos1 } </div>
        <div className="bracket">]</div>
        </div>
        </div>

        <div id="block">
        <div id="a2">
        <div className="dot">Ⓐ&nbsp;</div>
        <div className="bracket">&nbsp;[&nbsp;</div>
        <div className='pos'>{ this.state.pos2 } </div>
        <div className="bracket">]</div>
        </div>
        <div id="a3">
        <div className="dot">Ⓐ&nbsp;</div>
        <div className="bracket">&nbsp;[&nbsp;</div>
        <div className='pos'>{ this.state.pos3 } </div>
        <div className="bracket">]</div>
        </div>
        </div>

        <div id="block">
        <div id="a4">
        <div className="dot">Ⓐ&nbsp;</div>
        <div className="bracket">&nbsp;[&nbsp;</div>
        <div className='pos'>{ this.state.pos4 } </div>
        <div className="bracket">]</div>
        </div>
        <div id="a5">
        <div className="dot">Ⓐ&nbsp;</div>
        <div className="bracket">&nbsp;[&nbsp;</div>
        <div className='pos'>{ this.state.pos5 } </div>
        <div className="bracket">]</div>
        </div>
        </div>

        <div id="fan">
        <div className="dot">Ⓕ&nbsp;</div>
        <div className="bracket">&nbsp;[&nbsp;</div>
        <div className='pos'>{ this.state.posf } </div>
        <div className="bracket">]</div>
        </div>
        <div id="rover">
        <div className="dot">Ⓡ&nbsp;</div>
        <div className="bracket">&nbsp;[&nbsp;</div>
        <div className='pos'>{ this.state.posr } </div>
        <div className="bracket">]</div>
        <div id="angle">⬆&nbsp;</div>
        <div id="angleSymbol">∠</div>
        <div id='angleValue'>{ this.state.angle }</div>
        <div id="degrees">°</div>
        <div id="battery">Battery:&nbsp;
          <div className="value">{ this.state.battery }</div>
          <div className="percent">%</div>
        </div>
        </div>
        <div id="time">Last Update:&nbsp;
        <div className="value">xx:xx</div>
        </div>
        
      </div>
    );
  }
  componentWillUnmount() {
    clearInterval(this.interval);
  }
}

export default Stats;