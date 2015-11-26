from threading import Thread
import socket
from select import select
from functools import wraps

EXIT = 0

def thread(func):
    @wraps(func)
    def async_func(*args, **kwargs):
        func_hl = Thread(target = func, args = args, kwargs = kwargs)
        func_hl.start()
        return func_hl
    return async_func

def main(port=8080):
    s = socket.socket()
    s.bind(("0.0.0.0", port))
    s.listen(100)
    while not EXIT:
        inp = select([s], [], [], 1)
        if inp[0]:
            (csock, addr) = s.accept()
            handle_client(csock)

@thread
def handle_client(sock):
    sock.send("hi dad")
    sock.close()
