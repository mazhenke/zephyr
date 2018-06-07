# Example of simple echo server
# www.solusipse.net

import socket

def listen():
    connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    connection.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    connection.bind(('0.0.0.0', 5555))
    connection.listen(1)
    while True:
        current_connection, address = connection.accept()
        print("connection from {}".format(address));
        while True:
            data = current_connection.recv(2048)
            if not data:
                break
            current_connection.sendall(data)
            print(len(data), data)
        # print("Data: %s" % ''.join(' 0x%.2x' % x for x in data))

    current_connection.close()

if __name__ == "__main__":
    try:
        listen()
    except KeyboardInterrupt:
        pass
