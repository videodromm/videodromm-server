var mocha = require('mocha');
var chai = require('chai');
var wss = require('../../src/ws.js');
var WebSocket = require('ws');

var expect = chai.expect;

describe('Websocket initialization', function(){
    this.timeout(10 * 1000);
    var testMsg = 'A test message';
    before(function(done) {
        var http = require('http').Server();
        var winston = require('winston');
        http.listen(8088, function() {
            console.log('HTTP server initialized!');
        });
        var logger = {info:function(){}};
        new wss(http, winston, function(err){
            console.log('WS server initialized!');
            done();
        });
    });

    it('Accept incoming connections', function(done){
        var ws = new WebSocket('ws://localhost:8088');
        ws.on('open', function(){
            ws.close();
            done();
        });
    });

    it('Send messages', function(done){
        var ws = new WebSocket('ws://localhost:8088');
        ws.on('open', function(){
           ws.send(testMsg);
           ws.close();
           done();
        });
    });

    it('Broadcast messages', function(done){
        var ws = new WebSocket('ws://localhost:8088');
        ws.on('message', function(msg){
            expect(msg).to.be.equal(testMsg);
            ws.close();
            ws2.close();
            done();
        });
        var ws2 = new WebSocket('ws://localhost:8088');
        ws2.on('open', function(){
           ws2.send(testMsg);
        });
    });
});
