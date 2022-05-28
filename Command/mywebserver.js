var express = require('express'); var server = express();
server.get('/', function(req, res) {
res.sendFile('/Users/admin/Desktop/MarsRover/Command/index.html'); });

server.get('/Users/admin/Desktop/MarsRover/Command/movement.js', function(req, res) {
res.sendFile('/Users/admin/Desktop/MarsRover/Command/movement.js'); });

server.get('/Users/admin/Desktop/MarsRover/Command/grid.js', function(req, res) {
res.sendFile('/Users/admin/Desktop/MarsRover/Command/grid.js'); });

console.log('Server is running on port 3000'); server.listen(3000,'0.0.0.0');