

function WebsocketTelemetryPlugin() {
	 return function (openmct) {
	 	let socket = io.connect('http://' + document.domain + ':' + location.port);
	 	var telemetry_listeners = {};

        var clock_listeners = [];
        var clock_time = 0;
        let clock = {
                key: 'WebsocketDataClock',
                cssClass: 'icon-clock',
                name: 'Lastest Data Clock',
                description: "Scroll views to track latest data",
                tick: function (newTime) {
                    clock_time = newTime;
                    clock_listeners.map(function(callback) {
                        callback(newTime);
                    });
                },
                on: function (event, callback) {
                    clock_listeners.push(callback);
                },
                off: function (event, callback) {
                    clock_listeners = clock_listeners.filter(function (l) {return l != callback;})
                },
                currentValue: function () {
                    return clock_time;
                }
            }

		socket.on('telemetry', function(data) {
            var latest = -1;
            data.map(function (measurement) {
                if(telemetry_listeners[measurement.id]){
                    telemetry_listeners[measurement.id](measurement);
                }
                if(measurement.timestamp > latest){
                    latest = measurement.timestamp;
                }
            });
            if(latest > clock_time) clock.tick(latest);
		});

		var provider = {
            supportsSubscribe: function (domainObject) {
                return domainObject.type === 'rrs.mmd';
            },
            subscribe: function (domainObject, callback) {
                telemetry_listeners[domainObject.identifier.key] = callback;
                return function unsubscribe() {
                    delete telemetry_listeners[domainObject.identifier.key];
                };
            },
            supportsRequest: function (domainObject) {
                return domainObject.type === 'rrs.mmd';
            },
            request: function (domainObject, options) {
                return new Promise(function(resolve, reject){resolve([])});
            } 
        };
        
        openmct.telemetry.addProvider(provider);
        openmct.time.addClock(clock);

	 }
}