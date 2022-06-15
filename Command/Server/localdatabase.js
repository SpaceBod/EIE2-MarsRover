const mysql = require(`mysql-await`);// do: npm install mysql-await
const cors = require('cors');
const express = require('express');
var app = express();

app.use(cors())

const connection = mysql.createConnection({ //info on database + create connection
    host: "146.169.196.124",// 127.0.0.1 is me
    database:"esp32_data",  //mine is roverdata, for esp: esp32_data
    user:"savraj", // me is root, for esp: savraj
    password:"password" // me is pass, for esp: password
})

connection.connect(err=>{err? //connect to sql database
    console.log(err):
    console.log("Connected to esp32 MYSQL database")
});


const querydb = async (itemtoget)=> {

    if(itemtoget == "aliens"){
        var sql = "SELECT x_direction, y_direction, colour FROM roverdata WHERE alien_present = 'yes'" 
    }
    else if(itemtoget == "buildings"){
        var sql = "SELECT x_direction, y_direction FROM roverdata WHERE building_present = 'yes'"
    }
    else if(itemtoget == "rover"){
        var sql = "SELECT id, x_direction, y_direction, angle, battery_life FROM roverdata ORDER BY id DESC LIMIT 6 " //get last 6 rovers
    }
    else if (itemtoget == "fan"){
        var sql = "SELECT x_direction, y_direction FROM roverdata WHERE infrastructure_present = 'yes'"
    }

   let result = await connection.awaitQuery(sql, [itemtoget], (err,rows)=>{ //use itemtoget in query to db 
        if(!!err){
        console.log(err);
        return("error"); //if error, return that error
        }
        else{
        console.log("sql rows: "+JSON.stringify(rows))
        //maybe do maths here/call function to calculate coords or something idk
         //if success, return the rows
        }
    }) 
    return result;
}

const senddb = async(itemtosend) =>{
    let values = []
    let power = ''
    if(itemtosend === "true"){
        var sql = "INSERT INTO remote (remote) VALUES (?)"
         values = ['on']
    }
    else if(itemtosend === "false"){
       var sql = "INSERT INTO remote (remote) VALUES (?)"
        values = ['off']
    }
    else{
    let dist = itemtosend.distance
    let ang = itemtosend.angle

    //pad dist to 4 chars:
    if(dist.charAt(0) !== '-'){
        dist = dist.padStart(4, "0")
    }

    else if(dist.charAt(0) === '-'){
        dist =  dist.slice(1)
        dist = dist.padStart(3, "0")
        dist = '-'+dist
    }

    //pad ang to 4 chars
    if(ang.charAt(0) !== '-'){
        ang = ang.padStart(4, "0")
    }

    else if(ang.charAt(0) === '-'){
        ang =  ang.slice(1)
        ang = ang.padStart(3, "0")
        ang = '-'+ang
    }


    let power = itemtosend.power //add power later
    values = [dist, ang]; //send dist and ang, add power later 
    
    
    var sql = "INSERT INTO manualcontrol (distance, degrees) VALUES (?)"; //add power later
    }
    if((values[0] !== "0000") || (values[1]!== "0000")){ //not adding in 0's atm
    console.log("sending:...")
    console.log("Sql: ", sql, "Values: ",values)

    let result = await connection.awaitQuery(sql, [values], function (err, result) {
        if (!!err){
            console.log(err);
            return(err);
        }
        else{}
    })
    }   
    //return result;
}

module.exports = {

    querydb,
    senddb
}
