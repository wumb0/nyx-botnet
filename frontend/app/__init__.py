'''
__init__.py - Initialize the application, logins, and its views
'''
from flask import Flask
from flask.ext.sqlalchemy import SQLAlchemy
from flask.ext.admin import Admin
from flask_admin.base import MenuLink
from flask.ext.security import Security, SQLAlchemyUserDatastore
from flask_wtf.csrf import CsrfProtect

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
