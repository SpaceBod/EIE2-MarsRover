const express = require('express');
const server = express();
const fs = require('fs')
const _ = require('lodash')
const { addAlien, getAliens, addBuilding, getBuildings, addFan, getFans, addRover, getRover, countdeleteRover, delaliens } = require('./dynamo'); //get in all dynamo functions
const {querydb, senddb} = require('./localdatabase') //get in sql functions
const cors = require('cors');
const bodyParser = require('body-parser');
const {response} = require('express');
const { vary } = require('express/lib/response');
const { toNumber } = require('lodash');
const mysql = require(`mysql-await`);// do: npm install mysql-await
const dgram = require('dgram');
const { contentDisposition } = require('express/lib/utils');

//for UDP:
const client = dgram.createSocket('udp4');
const PORT = 3004; 
const HOST = '192.168.137.167'; //put in HOST (laptop hotspot ip)

server.use(cors())
server.use(bodyParser.urlencoded({extended: true}))
server.use(bodyParser.json())

server.listen(3003, ()=> {console.log("Server started on port 3003")})

server.get("/", (req, res)=>{

    //res.render('home', {title: 'Home'}); 
    res.send("Mars Rover Server Homepage - nothing here lol")
});

server.get("/getaliens", async(req,res) =>{ //get mysql and dynamodb, compare, add new to dynamodb and return new ddb

   var alien = await getAliens(); 
   items = alien.Items
   var resp = await querydb("aliens") 

   let useableresp = JSON.parse(JSON.stringify(resp)); //useableresp[0],useableresp[1] for each individual object
   
   let local = []
   let aws = []

   useableresp.forEach(element => {

        if(element.alien_coord != null){
        let tempcoord = element.alien_coord
        let comma = tempcoord.indexOf(",");
        let coord = tempcoord.substring(0, comma+1) + " " + tempcoord.substring(comma+1) //put space into coord: 20,30 -> 20, 30

        let colour = element.alien_colour+''
        local.push({coord, colour})
        }
        else{
            
        }
    });
       
    for(let i in items){
        aws.push({coord: items[i].coords, colour: items[i].colour})
    }
   
    let diff = _.differenceWith(local, aws, _.isEqual);    
    console.log("differences: ", diff)

    diff.forEach((element)=>{
        addAlien(element)
    })

    if(diff.length != 0){
        console.log("aliens added")
        }
    else{
        console.log("nothing added")
    }

    let newaws = await getAliens()
    res.send(newaws.Items) //CHANGE TO THIS
    
})

server.get("/getbuildings", async(req,res)=>{

    let buildings = await getBuildings()
    let items = buildings.Items;
    let resp = await querydb("buildings") 
    let useableresp = JSON.parse(JSON.stringify(resp));

    let local = []
    let aws = []

    useableresp.forEach(element => { //buildings only identified by coord?
 
        if(element.building_coord != null){
        let tempcoord = element.building_coord
        let comma = tempcoord.indexOf(",");
        let coord = tempcoord.substring(0, comma+1) + " " + tempcoord.substring(comma+1) 
        local.push({coord})
        }
        else{
        }

        
    });

    for(let i in items){ //add in aws items
        aws.push({coord: items[i].coords})
    }

    let diff = _.differenceWith(local, aws, _.isEqual);
    console.log("differences: ", diff)

    
    diff.forEach((element)=>{
        addBuilding(element)
    })

    if(diff.length != 0){
        console.log("building added")
        }
    else{
        console.log("nothing added")
    }

    let newaws = await getBuildings()
    res.send(newaws.Items) //change to this
    
   
})


server.get("/getrover", async(req,res) =>{

    let resp = await querydb("rover") //get sql stuff
    let useableresp = JSON.parse(JSON.stringify(resp));

    local = []
    useableresp.forEach(element => { //angle, battery, coords needed
        
        let tempcoord = element.rover_coord
        let comma = tempcoord.indexOf(",");
        let coord = tempcoord.substring(0, comma+1) + " " + tempcoord.substring(comma+1) 
        let angle = element.rover_angle+'' //conv int to string
        let battery = element.battery_life+''

        local.push({coord, angle, battery})
    });

    if(resp != "error"){ //dont add stuff if there is an error in fetching - also dont want to delete items as addrover does that
        await countdeleteRover()//get rid of everything rn

        await local.forEach((element)=>{
            addRover(element).then()
     })
    }   

    console.log("LOCAL ",local[0])
    res.send(local[0])//return last (newest) item - dont need to query dynamo for this - rip straight off latest sql

})


server.get("/getfan", async(req,res)=> {

    let fan = await getFans()
    let items = fan.Items;
    let resp = await querydb("fan") //add this to querydb func
    let useableresp = JSON.parse(JSON.stringify(resp));

    let local = []
    let aws = []

    useableresp.forEach(element => { //just coords needed, maybe add confidence param?

        if(element.fan_coord != null){
        let tempcoord = element.fan_coord
        let comma = tempcoord.indexOf(",");
        let coord = tempcoord.substring(0, comma+1) + " " + tempcoord.substring(comma+1) 

        local.push({coord})
        }
        else{
        }
    });

    for(let i in items){ //push in aws items
        aws.push({coord: items[i].coords})
    }

    let diff = _.differenceWith(local, aws, _.isEqual);
    console.log("differences: ", diff)

    
    diff.forEach((element)=>{
        addFan(element)
    })

    if(diff.length != 0){
        console.log("fan added")
        }
    else{
        console.log("nothing added")
    }

    let newaws = await getFans()
    res.send(newaws.Items) //CHANGE TO THIS
    
})

server.post("/movement", (req,res)=>{

    let body = req.body;
    let instr = body.body;

    console.log(req.body)
    var movementData = instr.distance + " " + instr.angle + " " + instr.power;
    const message = Buffer.from(movementData, "utf8");
    client.send(message, 0, message.length, PORT, HOST, function(err, bytes) {
      if (err) {
          console.error(`UDP manual control send error:`, err);
      } else {
          console.log(`UDP manual control instr sent to ${HOST}:${PORT}`);

      }
    });
   // console.log('send UDP port:', )
    res.json({success: true});
})


server.post("/togglemovement", (req,res)=>{

    let body = req.body;
    let toggle = body.body;
    let instr = toggle+''

    const message = Buffer.from(instr, "utf8");
    client.send(message, 0, message.length, PORT, HOST, function(err, bytes) {
      if (err) {
          console.error(`UDP toggle control send error:`, err);
      } else {
          console.log(`UDP toggle control instr sent to ${HOST}:${PORT}`);

      }
    res.send(JSON.stringify("Remote set to: ", toggle))
})
})

server.get("/addalienmanual", (req,res)=>{

	res.sendFile("./alienform.html", {root: __dirname})
})
server.post("/addalienmanual", async(req,res)=>{

    let info = req.body
    let send = {coord: info.coord, colour: info.colour}
    await addAlien(send)
    res.redirect("/addalienmanual")
    console.log("alien added: ", send)
})

server.get("/addcommand", (req,res)=>{

    res.sendFile("./comform.html", {root: __dirname})
})

server.post("/addcommand", (req,res)=>{
    
    let r = req.body
    let d = r.dist
    let a = r.ang
    let inp = {distance: d+'cm', angle: a+'º'}
    console.log( r, d, a)
    senddb(inp)
    res.redirect("/addcommand")
    
})
server.get("/delaliens", async(req,res)=>{

    await delaliens()
    res.send("aliens deleted")
})


