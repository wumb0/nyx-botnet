NYX Botnet
==========
We wrote a botnet for class. C&C xD<br>
For this to run you need to create a file called app.vars and put anything on the first line. That's the secret key for the app. Then put something else on the second line. That's the password salt used for password hashing. Then on the third line put the initial user's user name. Use the fourth line for the initial user's password<br>
We recommend using a virtualenv... (in this directory) sudo pip install virtualenv;virtualenv flask;source flask/bin/activate;pip install -r requirements.txt<br>
If you don't use a virtualenv just run sudo pip install -r requirements.txt<br>
You can run the server by running python runp.py or by using gunicorn (which is installed in the requirements): sudo gunicorn -b :80 -u www-data -g www-data app:app<br>
This gunicorn command expects that the frontend files will be accessible by the www-data user. I don't need to say this, but don't run the master as root. That's just asking for trouble.<br>
Have fun.
