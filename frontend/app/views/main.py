from app import app
from flask import render_template, g, abort
from flask.ext.security import current_user, login_required

@app.before_request
def before_request():
    if app.killswitch:
        abort(500)
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
