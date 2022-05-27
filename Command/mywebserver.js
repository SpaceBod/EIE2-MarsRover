var express = require('express'); var server = express();
server.get('/', function(req, res) {
res.sendFile('/Users/admin/Documents/Imperial College/Command-React/src copy/index.html'); });

server.get('/Users/admin/Downloads/movement.js', function(req, res) {
res.sendFile('/Users/admin/Downloads/movement.js'); });

server.get('/Users/admin/Downloads/grid.js', function(req, res) {
res.sendFile('/Users/admin/Downloads/grid.js'); });

console.log('Server is running on port 3000'); server.listen(3000,'0.0.0.0');