import argparse
from flask import Flask, request, jsonify, abort
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
	return jsonify([e.data for e in telemetry.serviceRequest(eventype, startt, endt)])

@app.route('/persist')
def persist():
	if not args.view_only:
		FuncThread(telemetry.saveToDisk, args.logfile).start() # Start persisting task
		return '', 204 # Success response code (204 No Content)
	else:
		abort(403) # Save of already archived log not allow (403 Forbidden)

if __name__ == '__main__':

	parser = argparse.ArgumentParser(description='MMD Ground Station Web Server')
	subparse_list = parser.add_subparsers(dest='operation', required=True)

	view_args = subparse_list.add_parser('view', help='View the complete filght data from a saved log')
	view_args.add_argument('logfile', help='Log archive to pull data from')
	view_args.set_defaults(view_only=True)

	downlink_args = subparse_list.add_parser('downlink', help='Decode and display live telemetry from a Serial Port')
	downlink_args.add_argument('serial', help='Serial device providing telemetry (COM[n])')
	downlink_args.add_argument('logfile', nargs='?', default='log.txt', help='Name of logfile to use when persisting data (request localhost:8000/persist).  Default is log.txt')
	downlink_args.set_defaults(view_only=False)

	args = parser.parse_args();

	socketio = SocketIO(app, logger=False, engineio_logger=False)
	telemetry = TelemetryManager(socketio)

	if not args.view_only:
		decoder = DecoderThread(telemetry, args.serial)
		decoder.start()
	else:
		telemetry.restoreFromDisk(args.logfile)

	socketio.run(app, port=8000)