let folder_defs = [
	{
		key:"root",
		name:"RRS MMD Tracker v2.0",
		location:"ROOT",
		composition: [
			{namespace:"rrs.mmd",key:"accel"},
			{namespace:"rrs.mmd",key:"gyro"},
			{namespace:"rrs.mmd.measure",key:"gps"},
			{namespace:"rrs.mmd.measure",key:"altitude"},
			{namespace:"rrs.mmd.measure",key:"temp"},
			{namespace:"rrs.mmd.measure",key:"battery"},
			{namespace:"rrs.mmd",key:"accelhg"},
			{namespace:"rrs.mmd",key:"mag"}
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
	},
	{
		key: "accelhg",
		name : "High-G Accelerometer",
		location : "rrs.mmd:root",
		composition : [
			{namespace:"rrs.mmd.measure",key:"accelhgx"},
			{namespace:"rrs.mmd.measure",key:"accelhgy"},
			{namespace:"rrs.mmd.measure",key:"accelhgz"}
		]
	},
	{
		key : "mag",
		name : "Magnetometer",
		location : "rrs.mmd:root",
		composition : [
			{namespace:"rrs.mmd.measure",key:"magx"},
			{namespace:"rrs.mmd.measure",key:"magy"},
			{namespace:"rrs.mmd.measure",key:"magz"}
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
	            units: "m/s^2",
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
	            units: "m/s^2",
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
	            units: "m/s^2",
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
	            units: "°/s",
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
	            units: "°/s",
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
	            units: "°/s",
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
	}, {
		key: "battery",
		name: "Main Battery",
		location : "rrs.mmd:root",
        values: [
	        {
	        	key: "volts",
	            name: "Voltage",
	            units: "V",
	            format: "float",
	            hints: {
	                range: 1
	            }
	        }
        ]
	}, {
		key: "accelhgx",
		name: "Accelerometer X",
		location : "rrs.mmd:accelhg",
        values: [
	        {
	        	key: "accelhgx",
	            name: "X Acceleration",
	            units: "m/s^2",
	            format: "float",
	            hints: {
	                range: 1
	            }
	        }
        ]
	}, {
		key: "accelhgy",
		name: "Accelerometer Y",
		location : "rrs.mmd:accelhg",
        values: [
	        {
	        	key: "accelhgy",
	            name: "Y Acceleration",
	            units: "m/s^2",
	            format: "float",
	            hints: {
	                range: 1
	            }
	        }
        ]
	}, {
		key: "accelhgz",
		name: "Accelerometer Z",
		location : "rrs.mmd:accelhg",
        values: [
	        {
	        	key: "accelhgz",
	            name: "Z Acceleration",
	            units: "m/s^2",
	            format: "float",
	            hints: {
	                range: 1
	            }
	        }
        ]
	}, {
		key: "magx",
		name: "Magnetometer X",
		location : "rrs.mmd:mag",
        values: [
	        {
	        	key: "magx",
	            name: "X Magnetic Field",
	            units: "μT",
	            format: "float",
	            hints: {
	                range: 1
	            }
	        }
        ]
	}, {
		key: "magy",
		name: "Magnetometer Y",
		location : "rrs.mmd:mag",
        values: [
	        {
	        	key: "magy",
	            name: "Y Magnetic Field",
	            units: "μT",
	            format: "float",
	            hints: {
	                range: 1
	            }
	        }
        ]
	}, {
		key: "magz",
		name: "Magnetometer Z",
		location : "rrs.mmd:mag",
        values: [
	        {
	        	key: "magz",
	            name: "Z Magnetic Field",
	            units: "μT",
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