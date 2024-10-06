from mysocketserver import ThreadingMixIn, TCPServer, StreamRequestHandler
import os
from datetime import datetime

class HTTPRequestHandler(StreamRequestHandler):
    # Class-level configuration for allowed methods and status codes
    ALLOWED_METHODS = ('GET', 'HEAD')  # You can extend this with other methods like 'POST', 'PUT', etc.
    STATUS_CODES = {
        200: "OK",
        400: "Bad Request",
        404: "Not Found",
        405: "Method Not Allowed",
    }

    def __init__(self, request, client_address, server, root_directory=None, allowed_methods=None, status_codes=None):
        # Allow custom configurations through constructor
        self.root_directory = root_directory or os.getcwd()  # Default to current working directory
        self.allowed_methods = allowed_methods or self.ALLOWED_METHODS
        self.status_codes = status_codes or self.STATUS_CODES
        
        super().__init__(request, client_address, server)

    def handle(self):
        # Read the request line
        request_line = self.rfile.readline().decode('utf-8').strip()
        print(f"Received request: {request_line}")

        # Parse the request line
        try:
            method, path, _ = request_line.split()
        except ValueError:
            self.send_response(400)
            return

        # Check if the method is allowed
        if method in self.allowed_methods:
            self.handle_request(method, path)
        else:
            self.send_response(405)

    def handle_request(self, method, path):
        # Resolve the requested path relative to the root directory
        requested_path = os.path.normpath(os.path.join(self.root_directory, path.lstrip('/')))

        # If the path is a directory
        if os.path.isdir(requested_path):
            if method == 'HEAD':
                self.send_response(200, content_length=0)  # No body for HEAD requests
            else:
                directory_message = f"You requested the directory: {requested_path}\n"
                self.send_response(200, body=directory_message.encode('utf-8'), content_length=len(directory_message.encode('utf-8')))
        elif os.path.isfile(requested_path):
            # If the path is a file
            with open(requested_path, 'rb') as f:
                file_content = f.read()
                content_length = len(file_content)

                # Send the file content for GET requests
                content_disposition = f'attachment; filename="{os.path.basename(requested_path)}"'
                self.send_response(200, body=file_content if method == 'GET' else None, content_length=content_length, content_disposition=content_disposition)
        else:
            self.send_response(404)

    def send_response(self, code, body=None, content_length=None, content_disposition=None):
        """Send an HTTP response to the client."""
        message = self.status_codes.get(code, "Unknown Status")
        self.wfile.write(f"HTTP/1.1 {code} {message}\r\n".encode('utf-8'))
        self.wfile.write(f"Date: {self.get_current_time()}\r\n".encode('utf-8'))
        self.wfile.write(b"Server: HFS (HTTP File Server)\r\n")

        if content_length is not None:
            self.wfile.write(f"Content-Length: {content_length}\r\n".encode('utf-8'))

        if content_disposition is not None:
            self.wfile.write(f"Content-Disposition: {content_disposition}\r\n".encode('utf-8'))

        # End of headers
        self.wfile.write(b"\r\n")

        # Write body content if available
        if body:
            self.wfile.write(body)

    @staticmethod
    def get_current_time():
        """Return the current date and time in the HTTP-date format."""
        return datetime.utcnow().strftime("%a, %d %b %Y %H:%M:%S GMT")


class ThreadingTCPServer(ThreadingMixIn, TCPServer):
    """Handle requests in a separate thread."""
    def __init__(self, server_address, RequestHandlerClass, bind_and_activate=True):
        super().__init__(server_address, RequestHandlerClass, bind_and_activate)


if __name__ == "__main__":
    HOST, PORT = "127.0.0.1", 8080

    # Create the server with default configurations
    server = ThreadingTCPServer((HOST, PORT), HTTPRequestHandler)

    print(f"Serving HTTP on {HOST}:{PORT}")
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        print("Shutting down the server.")
        server.shutdown()
