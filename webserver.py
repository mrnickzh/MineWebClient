from flask import Flask, Response
from flask_cors import CORS

app = Flask(__name__)
cors = CORS(app)

@app.route('/', methods=['GET'])
def getclient():
    resp = Response(open("cmake-build-debug-emsdk//MineWebClient.html").read())
    resp.headers['Cross-Origin-Embedder-Policy'] = 'require-corp'
    resp.headers['Cross-Origin-Opener-Policy'] = 'same-origin'
    return resp

@app.route('/MineWebClient.js', methods=['GET'])
def getjs():
    resp = Response(open("cmake-build-debug-emsdk//MineWebClient.js").read(), mimetype="application/javascript")
    resp.headers['Cross-Origin-Embedder-Policy'] = 'require-corp'
    resp.headers['Cross-Origin-Opener-Policy'] = 'same-origin'
    return resp

@app.route('/MineWebClient.wasm', methods=['GET'])
def getwasm():
    resp = Response(open("cmake-build-debug-emsdk//MineWebClient.wasm", "rb").read(), mimetype="application/wasm")
    resp.headers['Cross-Origin-Embedder-Policy'] = 'require-corp'
    resp.headers['Cross-Origin-Opener-Policy'] = 'same-origin'
    return resp

@app.route('/MineWebClient.data', methods=['GET'])
def getdata():
    resp = Response(open("cmake-build-debug-emsdk//MineWebClient.data", "rb").read(), mimetype="application/octet-stream")
    resp.headers['Cross-Origin-Embedder-Policy'] = 'require-corp'
    resp.headers['Cross-Origin-Opener-Policy'] = 'same-origin'
    return resp

app.run(host="localhost", port=3000, threaded=False, debug=True)