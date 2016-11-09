'use strict'

var nodemon = require('nodemon'),
winston = require('winston'),
http = require('http').createServer(),
wss = require('./src/ws.js'),
port = process.env.port || 8088;

http.listen(port, function() {
    winston.info('Application server running!');
    new wss(this, winston, function(err){
        if (!err) winston.info('Websocket listening on port ' + port);
    });
});
