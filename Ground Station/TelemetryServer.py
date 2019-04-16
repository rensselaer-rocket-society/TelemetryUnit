from flask import Flask, request, jsonify
from flask_socketio import SocketIO

from PacketDecoder import DecoderThread
from LogManager import TelemetryManager

app = Flask(__name__,
			static_url_path='', 
            static_folder='static')

socketio = SocketIO(app, logger=True, engineio_logger=True)
telemetry = TelemetryManager(socketio)

@app.route('/')
def root():
    return app.send_static_file('index.html')

@app.route('/history/<eventype>')
def lookup(eventype):
	startt = int(request.args.get("start"))
	endt = int(request.args.get("end"))
	print("Getting " + eventype + " events from time " + str(startt) + " to " + str(endt))
	return jsonify([e.data for e in telemetry.serviceRequest(eventype, startt, endt)])


if __name__ == '__main__':

	decoder = DecoderThread(telemetry)
	decoder.start()

	socketio.run(app, port=8000)