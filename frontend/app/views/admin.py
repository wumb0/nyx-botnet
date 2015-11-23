from app import app, db, models
from flask_admin.contrib.fileadmin import FileAdmin
from flask.ext.admin import AdminIndexView, BaseView
from flask.ext.admin.contrib.sqla.view import ModelView
from flask.ext.login import current_user as user
from flask import redirect, url_for, flash
from app.models import Post

class ProtectedBaseView(BaseView):
    def is_accessible(self):
        if user.is_authenticated and user.is_admin():
            return True
        return False

    def _handle_view(self, name, **kwargs):
        if not self.is_accessible():
            flash("You don't have permission to go there")
            return redirect(url_for('index'))

class ProtectedModelView(ModelView, ProtectedBaseView):
    pass

class ProtectedIndexView(AdminIndexView, ProtectedBaseView):
    pass

class ProtectedFileAdmin(FileAdmin, ProtectedBaseView):
    pass

class UserModelView(ProtectedModelView):
    pass
