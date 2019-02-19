from flask import Flask
from flask_socketio import SocketIO

from PacketDecoder import DecoderThread

app = Flask(__name__,
			static_url_path='', 
            static_folder='static')

socketio = SocketIO(app, logger=True, engineio_logger=True)

@app.route('/')
def root():
    return app.send_static_file('index.html')


if __name__ == '__main__':

	decoder = DecoderThread(socketio)
	decoder.start()

	socketio.run(app, port=8000)