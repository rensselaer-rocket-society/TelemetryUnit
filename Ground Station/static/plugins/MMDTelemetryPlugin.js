let telemetry_defs = [
	{
		key: "gpslat",
		name: "GPS Latitude",
		values: [
			{
                key: "latitude",
                name: "Latitude",
                units: "°N",
                format: "float",
                min: -90,
                max: 90,
                hints: {
                    range: 1
                }
            }
		]
	}, {
		key: "gpslng",
		name: "GPS Longitude",
        values: [
	        {
	        	key: "longitude",
	            name: "Longitude",
	            units: "°E",
	            format: "float",
	            min: -180,
	            max: 180,
	            hints: {
	                range: 1
	            }
	        }
        ]
	}
]

let timeMeta = {
	key: "onboard-time",
	source: "timestamp",
	name: "Timestamp",
	format: "float",
	hints: {
		domain: 1
	}
}

let objectProvider = {
    get: function (identifier) {
    	return new Promise(function(resolve, reject) {
    		if (identifier.key === 'rocket') {
	        	comp = telemetry_defs.map(function(m) {
	        		return {namespace: 'rrs.mmd', key: m.key};
	        	});
	            resolve({
	                identifier: identifier,
	                name: "RRS MMD Tracker v1.0",
	                type: 'folder',
	                location: 'ROOT',
	                composition: comp
	            });
	        } else {
	            var measurement = telemetry_defs.filter(function (m) {
	                return m.key === identifier.key;
	            })[0];
	            var allVals = measurement.values.slice();
	            allVals.push(timeMeta);
	            resolve({
	                identifier: identifier,
	                name: measurement.name,
	                type: 'rrs.mmd',
	                telemetry: {
	                    values: allVals
	                },
	                location: 'rrs.mmd:rocket'
	            });
	        }
    	});
    }
};

function MMDTelemetryPlugin() {
	return function (openmpt) {
		openmct.objects.addRoot({
            namespace: 'rrs.mmd',
            key: 'rocket'
        });

		openmct.types.addType('rrs.mmd', {
            name: 'MMD Tracker Telemetry',
            description: 'Telemetry point streaming from an MMD Tracker Device',
            cssClass: 'icon-telemetry'
        });

        openmct.time.addTimeSystem({
		    key: 'onboard-time',
		    name: 'Onboard Time',
		    cssClass: 'icon-clock',
		    timeFormat: 'duration',
		    durationFormat: 'duration',
		    isUTCBased: false
		});

        openmct.objects.addProvider('rrs.mmd', objectProvider);

	}
}