const mysql = require(`mysql-await`);// do: npm install mysql-await
const cors = require('cors');
const express = require('express');
const { toNumber } = require('lodash');
var app = express();

app.use(cors())

const connection = mysql.createConnection({ //info on database + create connection
    host: "127.0.0.1",
    database:"roverdata",  
    user:"root",
    password:"pass"
})

connection.connect(err=>{err? //connect to sql database
    console.log(err):
    console.log("Connected to local MYSQL database")
});


const querydb = async (itemtoget)=> {

    if(itemtoget == "aliens"){
        var sql = "SELECT x_direction, y_direction, colour FROM test WHERE alien_present = 'yes'" 
    }
    else if(itemtoget == "buildings"){
        var sql = "SELECT x_direction, y_direction FROM test WHERE building_present = 'yes'"
    }
    else if(itemtoget == "rover"){
        var sql = "SELECT id, x_direction, y_direction, angle, battery_life FROM test ORDER BY id DESC LIMIT 6 " //get last 7 rovers
    }
    else if (itemtoget == "fan"){
        var sql = "SELECT x_direction, y_direction FROM test WHERE infrastructure_present = 'yes'"
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
    //see brainstorm file
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
    let dist = itemtosend.distance.slice(0, -2) //remove cm, dunno if turn into string instead of num?
    let ang = itemtosend.angle.slice(0, -1) //remove º, maybe stringify instead on num
    let power = itemtosend.power
    values = [dist, ang, power];//send dist and ang, 
    
    var sql = "INSERT INTO remote (distance, angle, power) VALUES (?)"; //remote is db for remote control?
    }
    console.log("Sql: ", sql, "Values: ",values)
    let result = await connection.awaitQuery(sql, [values], function (err, result) {
        if (!!err){
            console.log(err);
            return(err);
        }
        else{
        }

    })

    
    return result;

}

module.exports = {

    querydb,
    senddb
}