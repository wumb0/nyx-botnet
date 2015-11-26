'''
__init__.py - Initialize the application, logins, and its views
'''
from flask import Flask
from flask.ext.sqlalchemy import SQLAlchemy
from flask.ext.admin import Admin
from flask_admin.base import MenuLink
from flask.ext.security import Security, SQLAlchemyUserDatastore
from flask_wtf.csrf import CsrfProtect
from threading import Thread
import signal
from sys import exit

# Initialize the app and database, import the config
app = Flask(__name__)
app.config.from_object('config')
db = SQLAlchemy(app)
CsrfProtect(app)
app.killswitch = 0

#this starts the app
from app import models
userstore = SQLAlchemyUserDatastore(db, models.User, None)
sec = Security(app, userstore)
db.create_all()
try:
    userstore.create_user(email="root@localhost", password="pwnsauce")
    db.session.commit()
except: pass
from app.views import main, admin

#admin setup
_admin = Admin(app, 'NYX Admin', template_mode='bootstrap3',
              index_view=admin.ProtectedIndexView())
_admin.add_link(MenuLink(name='Back to Site', url='/'))
_admin.add_view(admin.UserModelView(models.User, db.session))

from app import master

master_t = Thread(target=master.main)
master_t.start()

def exitfunc(a, b):
    print("Yes")
    master.EXIT = 1
    master_t.join()
    exit(0)


signal.signal(signal.SIGINT, exitfunc)
