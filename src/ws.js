'use strict'
var uuid = require('uuid');
var WebSocketServer = require('ws').Server;

function wsCtrl ( httpListener, logger, cb ) {
    this.logger = logger;
    this.wss = new WebSocketServer({ server: httpListener });
    var that = this;
    this.wss.broadcast = function(msg, sender) {
        that.wss.clients.forEach(function(client){
            if (client.id !== sender) client.send(msg) & logger.info('Message sent to ' + client.id + ' from ' + sender);
        });
    }
    this.wss.on('connection', function(socket){
      socket.id = uuid.v4();
      logger.info('user connected');
      socket.on('disconnect', function(){
        logger.info('user disconnected');
      });
      socket.on('message', function(msg){
        logger.info('Message received:' + msg);
        that.wss.broadcast(msg, socket.id);
      });
    });
    return cb(null);
}

module.exports = wsCtrl;
