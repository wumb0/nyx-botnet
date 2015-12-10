from flask.ext.admin import AdminIndexView, BaseView
from flask.ext.admin.contrib.sqla.view import ModelView
from flask.ext.security import current_user as user
from flask import redirect, url_for, flash

class ProtectedBaseView(BaseView):
    """Only allow logged in users to view the admin panel"""
    def is_accessible(self):
        if user.is_authenticated():
            return True
        return False

    def _handle_view(self, name, **kwargs):
        if not self.is_accessible():
            flash("You don't have permission to go there")
            return redirect(url_for('index'))

class ProtectedModelView(ModelView, ProtectedBaseView):
    """The combination class results in an authenticated model view"""
    pass

class ProtectedIndexView(AdminIndexView, ProtectedBaseView):
    """The combination class results in an authenticated indev view"""
    pass

class UserModelView(ProtectedModelView):
    """Allow user management in the admin panel"""
    column_exclude_list = ['password']
    form_excluded_columns = ['password', 'last_login_at', 'current_login_at',
                             'last_login_ip', 'current_login_ip', 'login_count']

class BotModelView(ProtectedModelView):
    """Allow bot management in the admin panel, shouldn't really be needed"""
    pass
