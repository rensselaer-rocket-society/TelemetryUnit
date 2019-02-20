let folder_defs = [
	{
		key:"root",
		name:"RRS MMD Tracker v1.0",
		location:"ROOT",
		composition: [
			{namespace:"rrs.mmd",key:"accel"},
			{namespace:"rrs.mmd",key:"gyro"},
			{namespace:"rrs.mmd.measure",key:"gps"},
			{namespace:"rrs.mmd.measure",key:"altitude"},
			{namespace:"rrs.mmd.measure",key:"temp"}
		]
	},
	{
		key: "accel",
		name : "Accelerometer",
		location : "rrs.mmd:root",
		composition : [
			{namespace:"rrs.mmd.measure",key:"accelx"},
			{namespace:"rrs.mmd.measure",key:"accely"},
			{namespace:"rrs.mmd.measure",key:"accelz"}
		]
	},
	{
		key : "gyro",
		name : "Gyroscope",
		location : "rrs.mmd:root",
		composition : [
			{namespace:"rrs.mmd.measure",key:"gyrox"},
			{namespace:"rrs.mmd.measure",key:"gyroy"},
			{namespace:"rrs.mmd.measure",key:"gyroz"}
		]
	}
]

let telemetry_defs = [
	{
		key: "gps",
		name: "GPS Position",
		location : "rrs.mmd:root",
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
            },
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
	}, {
		key: "accelx",
		name: "Accelerometer X",
		location : "rrs.mmd:accel",
        values: [
	        {
	        	key: "accelx",
	            name: "X Acceleration",
	            units: "g",
	            format: "float",
	            hints: {
	                range: 1
	            }
	        }
        ]
	}, {
		key: "accely",
		name: "Accelerometer Y",
		location : "rrs.mmd:accel",
        values: [
	        {
	        	key: "accely",
	            name: "Y Acceleration",
	            units: "g",
	            format: "float",
	            hints: {
	                range: 1
	            }
	        }
        ]
	}, {
		key: "accelz",
		name: "Accelerometer Z",
		location : "rrs.mmd:accel",
        values: [
	        {
	        	key: "accelz",
	            name: "Z Acceleration",
	            units: "g",
	            format: "float",
	            hints: {
	                range: 1
	            }
	        }
        ]
	}, {
		key: "gyrox",
		name: "Gyroscope X",
		location : "rrs.mmd:gyro",
        values: [
	        {
	        	key: "gyrox",
	            name: "X Rotation",
	            units: "°",
	            format: "float",
	            hints: {
	                range: 1
	            }
	        }
        ]
	}, {
		key: "gyroy",
		name: "Gyroscope Y",
		location : "rrs.mmd:gyro",
        values: [
	        {
	        	key: "gyroy",
	            name: "Y Rotation",
	            units: "°",
	            format: "float",
	            hints: {
	                range: 1
	            }
	        }
        ]
	}, {
		key: "gyroz",
		name: "Gyroscope Z",
		location : "rrs.mmd:gyro",
        values: [
	        {
	        	key: "gyroz",
	            name: "Z Rotation",
	            units: "°",
	            format: "float",
	            hints: {
	                range: 1
	            }
	        }
        ]
	}, {
		key: "altitude",
		name: "Altimeter",
		location : "rrs.mmd:root",
        values: [
	        {
	        	key: "altitude",
	            name: "Altitude (AGL)",
	            units: "m",
	            format: "float",
	            hints: {
	                range: 1
	            }
	        }
        ]
	}, {
		key: "temp",
		name: "Thermometer",
		location : "rrs.mmd:root",
        values: [
	        {
	        	key: "temp",
	            name: "Temperature",
	            units: "°C",
	            format: "float",
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
	format: "duration",
	hints: {
		domain: 1
	}
}

let folderObjectProvider = {
    get: function (identifier) {
    	return new Promise(function(resolve, reject) {
	        var folder = folder_defs.filter(function (f) {
	            return f.key === identifier.key;
	        })[0];
	        resolve({
	            identifier: identifier,
	            name: folder.name,
	            location: folder.location,
	            composition: folder.composition,
	            type: 'folder'
	        });
    	});
    }
};

let telemetryObjectProvider = {
    get: function (identifier) {
    	return new Promise(function(resolve, reject) {
	        var measurement = telemetry_defs.filter(function (m) {
	            return m.key === identifier.key;
	        })[0];
	        var allVals = measurement.values.slice();
	        allVals.push(timeMeta);
	        resolve({
	            identifier: identifier,
	            name: measurement.name,
	            location: measurement.location,
	            type: 'rrs.mmd',
	            telemetry: {
	                values: allVals
	            },
	        });
    	});
    }
};

function MMDTelemetryPlugin() {
	return function (openmpt) {
		openmct.objects.addRoot({
	        namespace: "rrs.mmd",
	        key: "root"
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

        openmct.objects.addProvider('rrs.mmd.measure', telemetryObjectProvider);
        openmct.objects.addProvider('rrs.mmd', folderObjectProvider);

	}
}