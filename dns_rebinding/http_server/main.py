from flask import Flask, render_template, send_from_directory, Markup
import sys

if len(sys.argv) != 3:
    print("Arguments error\nUsage: python3 main.py HOST PORT")

HOST = sys.argv[1]
PORT = int(sys.argv[2])

app = Flask(__name__)


@app.route('/js/<path:path>')
def send_js(path):
    return send_from_directory('js', path)

@app.route('/')
def hello_world():
    message = "Salut les copains"
    return render_template("malicious.html", message = message)

if __name__ == '__main__':
  app.run(host=HOST, port=PORT)
