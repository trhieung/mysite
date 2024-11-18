import os
import argparse  # Fixed module name
from SE10_dbms import MyDBMS
from SE10_crypto import MyCrypto
from SE10_handler import Handler
from SE10_socketserver import StreamRequestHandler, ThreadingMixIn, TCPServer


class HTTPRequestHandler(StreamRequestHandler):
    def __init__(self, request, client_address, server):
        self.handler = Handler(server.my_dbms, server.my_crypto)
        super().__init__(request, client_address, server)

    def handle(self):
        self.handler.handle_request(self.rfile, self.wfile)


class ThreadingTCPServer(ThreadingMixIn, TCPServer):
    def __init__(self, server_address, RequestHandlerClass, start: bool, bind_and_activate=True):
        super().__init__(server_address, RequestHandlerClass, bind_and_activate)
        self.start = start
        self.setupDB()
        self.setupOther()

    def setupDB(self):
        self.my_dbms = MyDBMS(os.path.join(os.path.dirname(__file__), "../storage/test.db"))
        if self.start:
            self.my_dbms.setup()
        else:
            self.my_dbms.show("Cache")

    def setupOther(self):
        self.my_crypto = MyCrypto()  # Fixed typo


def main():
    parser = argparse.ArgumentParser(description="Simple HTTP server")  # Fixed module name
    parser.add_argument("-a", "--address", type=str, default="0.0.0.0", help="Server address (default: 0.0.0.0)")
    parser.add_argument("-p", "--port", type=int, default=8080, help="Server port (default: 8080)")
    parser.add_argument("-i", "--init", action="store_true", help="Initialize/reset database")

    args = parser.parse_args()

    HOST, PORT, START = args.address, args.port, args.init
    server = ThreadingTCPServer((HOST, PORT), HTTPRequestHandler, START)  # Fixed argument unpacking
    print(f"Serving HTTP on {HOST}:{PORT}")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("Shutting down the server")
        server.shutdown()


if __name__ == "__main__":  # Fixed __main__ block
    main()
