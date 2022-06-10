const express = require('express');
const server = express();
const fs = require('fs')
const _ = require('lodash')
const { addAlien, getAliens, addBuilding, getBuildings, addFan, getFans, addRover, getRover, countdeleteRover } = require('./dynamo'); //get in all dynamo functions
const {querydb, senddb} = require('./localdatabase') //get in sql functions
const cors = require('cors');
const bodyParser = require('body-parser');
const {response} = require('express');
const { vary } = require('express/lib/response');
const { toNumber } = require('lodash');

server.use(cors())
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
        let x = element.x_direction
        let y = element.y_direction
        let coord = x+''+', '+y+''
        let colour = element.colour+''

        local.push({coord, colour})
    });
       
    for(let i in items){
        aws.push({coord: items[i].coords, colour: items[i].colour})
    }
   
    let diff = _.differenceWith(local, aws, _.isEqual);    
    console.log("differences: ", diff)

    diff.forEach((element)=>{
        addAlien(element)
    })

    let newaws = await getAliens()
    if(diff.length != 0){
    res.send("aliens added")
    }
    else{
        res.send("nothing added g")
    }
})

server.get("/getbuildings", async(req,res)=>{

    let buildings = await getBuildings()
    let items = buildings.Items;
    let resp = await querydb("buildings") 
    let useableresp = JSON.parse(JSON.stringify(resp));

    let local = []
    let aws = []

    useableresp.forEach(element => { //buildings only identified by coord?
        let x = element.x_direction
        let y = element.y_direction
        let coord = x+''+', '+y+''

        local.push({coord})
    });

    for(let i in items){ //add in aws items
        aws.push({coord: items[i].coords})
    }

    let diff = _.differenceWith(local, aws, _.isEqual);
    console.log("differences: ", diff)

    
    diff.forEach((element)=>{
        addBuilding(element)
    })

    let newaws = getBuildings()
    //res.send(newaws)
    if(diff.length != 0){
        res.send("rover(s) added")
        }
        else{
            res.send("nothing added g")
        }
    

   
})


server.get("/getrover", async(req,res) =>{

    let resp = await querydb("rover") //get sql stuff
    let useableresp = JSON.parse(JSON.stringify(resp));

    local = []
    useableresp.forEach(element => { //angle, battery, coords needed
        let x = element.x_direction
        let y = element.y_direction
        let coord = x+''+', '+y+''
        let angle = element.angle+''
        let battery = element.battery_life+''

        local.push({coord, angle, battery})
    });

    if(resp != "error"){ //dont add stuff if there is an error in fetching - also dont want to delete items as addrover does that
        await countdeleteRover()//get rid of everything rn

        local.forEach((element)=>{
            console.log("adding: ", element)
            addRover(element).then()
     })
    }   

    let newestrover = await getRover("all")
    
    res.send(newestrover.Items.at(-1))//return last (newest) item

})


server.get("/getfan", async(req,res)=> {

    let fan = await getFans()
    let items = fan.Items;
    let resp = await querydb("fan") //add this to querydb func
    let useableresp = JSON.parse(JSON.stringify(resp));

    let local = []
    let aws = []

    useableresp.forEach(element => { //just coords needed, maybe add confidence param?
        let x = element.x_direction
        let y = element.y_direction
        let coord = x+''+', '+y+''

        local.push({coord})
    });

    for(let i in items){ //push in aws items
        aws.push({coord: items[i].coords})
    }

    let diff = _.differenceWith(local, aws, _.isEqual);
    console.log("differences: ", diff)

    
    diff.forEach((element)=>{
        addFan(element)
    })

    let newaws = getFans()
    //res.send(newaws.Items)
    if(diff.length != 0){
        res.send("fan(s) added")
        }
        else{
            res.send("nothing added g")
        }
})


server.post("/movement", (req, res)=>{ //for remote movement

    let body = req.body;
    let instr = body.body;

    senddb(instr)
   // let queue = []
   // queue.push(instr)
   // queue.forEach((element)=>{
   //     await senddb(element) //send obj with dist, ang in it
   //     queue.shift()//remove oldest (1st) item as just sent
   // })
  
    res.send("recieved")
})


server.post("/togglemovement", (req,res)=>{

    let body = req.body;
    let toggle = body.body;
    
    senddb(toggle);

    res.send(JSON.stringify("Remote set to: ", toggle))
})
 
