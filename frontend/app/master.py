from threading import Thread, Lock
import socket
from select import select
from functools import wraps

EXIT = 0
clients = {}
mutex = Lock()

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
    while not EXIT:
        inp = select([s], [], [], 1)
        if inp[0]:
            (csock, addr) = s.accept()
            print("Conn: "+addr[0])
            handle_client(csock, addr)

@thread
def handle_client(sock, addr):
    print clients
    if not addr[0] in clients.keys():
        clients[addr[0]] = list()
    inp = select([sock], [], [], 1)
    if inp[0]:
        print(sock.recv(2048))
    if len(clients[addr[0]]) != 0:
        sock.send(clients[addr[0]].pop())
    sock.close()
