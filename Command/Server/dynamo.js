const { RoboMaker } = require('aws-sdk');
const AWS = require('aws-sdk')
require('dotenv').config(); 
//const uuid = require('uuid'); //allows generation of unique ids
 //id=uuid.v1(); sets id as unique id based on timestamp

AWS.config.update({ //from .env file - dont want to show this stuff off

    accessKeyId: process.env.AWS_ACCESS_KEY_ID,
    secretAccessKey: process.env.AWS_SECRET_ACCESS_KEY
})
AWS.config.update({region:'eu-west-2'}); //region needed to be done this way for ec2 for some reason idk

const dynamoClient = new AWS.DynamoDB.DocumentClient(); //.documentclient is an easier/preferred way
const TABLE_NAME = "MarsRover";

const addAlien = async (alien) => { //alien object hold coords? maybe need to manipulate to get actual coords.

    const params = {
        TableName: TABLE_NAME,
        Item: {
            Type: "Alien",
            id: ''+ Date.now(), //id based on current time - can see most recent additions
            coords: alien.coord,
            colour: alien.colour
        }
    }
    return await dynamoClient.put(params).promise()
}

const getAliens = async () => { //returns type (Alien) and coords

    const params = {
        TableName: TABLE_NAME,
        KeyConditionExpression: "#Type = :alien",
        ExpressionAttributeNames: { "#Type": "Type" },
        ExpressionAttributeValues: {
            ":alien": "Alien",
        },
        ProjectionExpression: "#Type, coords, colour, id"
        //FilterExpression: 
    
    };
    const aliens = await dynamoClient.query(params).promise()
    return aliens;
}

const addBuilding = async (building) => { //building object hold coords, colour? maybe need to manipulate to get actual coords.

    const params = {
        TableName: TABLE_NAME,
        Item: {
            Type: "Building",
            id: ''+ Date.now(), //id based on current time - can see most recent additions
            coords: building.coord,
        }
    }
    return await dynamoClient.put(params).promise()
}

const getBuildings = async () => { //returns type (Building) and coords

    const params = {
        TableName: TABLE_NAME,
        KeyConditionExpression: "#Type = :building",
        ExpressionAttributeNames: { "#Type": "Type" },
        ExpressionAttributeValues: {
            ":building": "Building",
        },
        ProjectionExpression: "#Type, coords, id"
        //FilterExpression: 
    
    };
    const buildings = await dynamoClient.query(params).promise()
    return buildings;
}

const addFan = async (fan) => { //fan object hold coords? maybe need to manipulate to get actual coords.

    const params = {
        TableName: TABLE_NAME,
        Item: {
            Type: "Fan",
            id: ''+ Date.now(), //id based on current time - can see most recent additions
            coords: fan.coord,
            
        }
    }
    return await dynamoClient.put(params).promise()
}

const getFans = async () => { //returns type (Building) and coords

    const params = {
        TableName: TABLE_NAME,
        KeyConditionExpression: "#Type = :fan",
        ExpressionAttributeNames: { "#Type": "Type" },
        ExpressionAttributeValues: {
            ":fan": "Fan",
        },
        ProjectionExpression: "#Type, coords, id"
        //FilterExpression: 
    
    };
    const fans = await dynamoClient.query(params).promise()
    return fans;
}

const addRover = async (rover) => { //rover object hold coords? maybe need to manipulate to get actual coords.

    const params = {
        TableName: TABLE_NAME,
        Item: {
            Type: "Rover",
            id: ''+ Date.now(), //id based on current time - can see most recent additions
            coords: rover.coord,
            battery: rover.battery,
            angle: rover.angle
        }
    }

    await dynamoClient.put(params).promise()
    return params.Item
}

const countdeleteRover = async() => {

    const idparams = {
        TableName: TABLE_NAME,
        KeyConditionExpression: "#Type = :rover",
        ExpressionAttributeNames: { "#Type": "Type" },
        ExpressionAttributeValues: {
            ":rover": "Rover",
        },
        ProjectionExpression: "id",
    };

    let theids = await dynamoClient.query(idparams).promise() //get all rover ids for deleting
    let id = theids.Items.id + '';
    const delparams = {
        TableName: TABLE_NAME,
        Key: {
            Type: "Rover",
            id: id
        },
    };

    theids.Items.forEach((element)=>{ //delete all rover items

        const delparams = {
            TableName: TABLE_NAME,
            Key: {
                Type: "Rover",
                id: element.id
            },
        };
         dynamoClient.delete(delparams).promise()
    })
 
    return 1
}

const getRover = async (cond) => { //returns type (Rover) and most recent coords

    if(cond == "all"){
        var params = {
            TableName: TABLE_NAME,
            KeyConditionExpression: "#Type = :rover",
            ExpressionAttributeNames: { "#Type": "Type" },
            ExpressionAttributeValues: {
                ":rover": "Rover",
            },
            ProjectionExpression: "#Type, coords, battery, angle", //maybe add battery level here
            ScanIndexForward: false,  //scans backwards
        };
    }
    else{ //probs dont need this and just make last one default
        var params = {
            TableName: TABLE_NAME,
            KeyConditionExpression: "#Type = :rover",
            ExpressionAttributeNames: { "#Type": "Type" },
            ExpressionAttributeValues: {
                ":rover": "Rover",
            },
            ProjectionExpression: "#Type, coords, battery, angle", //maybe add battery level here
            ScanIndexForward: true,  //scans backwards
            Limit: 1
        };
    }

    const rover = await dynamoClient.query(params).promise()
    return rover;
}

const delaliens = async() => {

    const idparams = {
        TableName: TABLE_NAME,
        KeyConditionExpression: "#Type = :alien",
        ExpressionAttributeNames: { "#Type": "Type" },
        ExpressionAttributeValues: {
            ":alien": "Alien",
        },
        ProjectionExpression: "id",
    };

    let id1 = await dynamoClient.query(idparams).promise()
    let count = id1.Count;
    
    for(let i = 0; i < count; i++){
        var delparams = {
            TableName: TABLE_NAME,
            Key: {
                Type: "Alien",
                id: id1.Items[i].id + ''
            }
        };
        await dynamoClient.delete(delparams).promise()
    }
    
    return 1
}

module.exports = {

    dynamoClient,
    addAlien,
    getAliens,
    addBuilding,
    getBuildings,
    addFan,
    getFans,
    addRover,
    getRover,
    delaliens,
    countdeleteRover


}
//might be useful:
//for (let [key, value] of Object.entries(p)) {
 //   console.log(`${key}: ${value}`);
//  }



