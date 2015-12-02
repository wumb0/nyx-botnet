import os

#get the path that the app is running in
basedir = os.path.abspath(os.path.dirname(__file__))

#get private application resources
f = open(os.path.join(basedir, 'app.vars') , 'r')

CSRF_ENABLED = True
WTF_CSRF_ENABLED = True
SECURITY_PASSWORD_HASH='pbkdf2_sha512'
SECURITY_TRACKABLE=True

#setup database
SQLALCHEMY_DATABASE_URI = 'sqlite:///' + os.path.join(basedir, 'app.db') + '?check_same_thread=False'
SQLALCHEMY_MIGRATE_REPO = os.path.join(basedir, 'db_repository')
SQLALCHEMY_TRACK_MODIFICATIONS=False

#get constants from file
SECRET_KEY = f.readline().strip()
SECURITY_PASSWORD_SALT=f.readline().strip()
FIRST_USER_NAME=f.readline().strip()
FIRST_USER_PASS=f.readline().strip()
f.close()
