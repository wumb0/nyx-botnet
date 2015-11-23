from app import app
from flask import render_template, g
from flask.ext.security import current_user

@app.before_request
def before_request():
    g.user = current_user

@app.route('/')
@app.route('/home')
@app.route('/index')
def index():
    return render_template("index.html", title="Home")
