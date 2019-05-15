import argparse
from flask import Flask, request, jsonify
from flask_socketio import SocketIO
from threading import Thread

class FuncThread(Thread):
	def __init__(self, target, *args):
		super(FuncThread, self).__init__()
		self._target = target
		self._args = args
 
	def run(self):
		self._target(*self._args)

from PacketDecoder import DecoderThread
from LogManager import TelemetryManager

app = Flask(__name__,
			static_url_path='', 
			static_folder='static')

@app.route('/')
def root():
	return app.send_static_file('index.html')

@app.route('/history/<eventype>')
def lookup(eventype):
	startt = float(request.args.get("start"))
	endt = float(request.args.get("end"))
	print("Getting " + eventype + " events from time " + str(startt) + " to " + str(endt))
	return jsonify([e.data for e in telemetry.serviceRequest(eventype, startt, endt)])

@app.route('/persist')
def persist():
	FuncThread(telemetry.saveToDisk, args.logfile).start() # Start persisting task
	return ('', 204)

if __name__ == '__main__':

	parser = argparse.ArgumentParser(description='MMD Ground Station Web Server')
	parser.add_argument('logfile', nargs='?', default='log.txt',
						help='File to log this flight to')
	parser.add_argument('--view-only', '-v', action='store_true',
						help='Open the specified logfile for viewing (ie. do not attempt to read new data from serial port')

	args = parser.parse_args();

	socketio = SocketIO(app, logger=True, engineio_logger=True)
	telemetry = TelemetryManager(socketio)

	if not args.view_only:
		decoder = DecoderThread(telemetry)
		decoder.start()
	else:
		telemetry.restoreFromDisk(args.logfile)

	socketio.run(app, port=8000)