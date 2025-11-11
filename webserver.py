from flask import Flask, Response
from flask_cors import CORS

app = Flask(__name__)
cors = CORS(app)

@app.route('/', methods=['GET'])
def getclient():
    resp = Response(open("cmake-build-debug-emsdk//MineWeb.html").read())
    return resp

@app.route('/MineWeb.js', methods=['GET'])
def getjs():
    resp = Response(open("cmake-build-debug-emsdk//MineWeb.js").read(), mimetype="application/javascript")
    return resp

@app.route('/MineWeb.wasm', methods=['GET'])
def getwasm():
    resp = Response(open("cmake-build-debug-emsdk//MineWeb.wasm", "rb").read(), mimetype="application/wasm")
    return resp

@app.route('/MineWeb.data', methods=['GET'])
def getdata():
    resp = Response(open("cmake-build-debug-emsdk//MineWeb.data", "rb").read(), mimetype="application/octet-stream")
    return resp

app.run(host="localhost", port=3000, threaded=False, debug=True)