var socket = io.connect('http://' + document.domain + ':' + location.port);
socket.on('telemetry', function(data) {
    console.log(data);
});