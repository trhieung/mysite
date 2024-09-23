import os
from http.server import SimpleHTTPRequestHandler, HTTPServer
from socketserver import ThreadingMixIn

# Define the server's port
PORT = 8000

# Server directory path for downloads
DOWNLOAD_DIRECTORY = os.path.join(os.getcwd(), "downloads")

# Ensure the download directory exists
if not os.path.exists(DOWNLOAD_DIRECTORY):
    os.makedirs(DOWNLOAD_DIRECTORY)

# Change to the directory where files are stored
os.chdir(DOWNLOAD_DIRECTORY)

class ThreadedHTTPServer(ThreadingMixIn, HTTPServer):
    """This is a multithreaded HTTP server allowing multiple simultaneous downloads."""
    daemon_threads = True

class DownloadHandler(SimpleHTTPRequestHandler):
    """Custom handler to log file downloads and print request headers."""
    
    def do_GET(self):
        # Print the request headers
        print("Request headers:")
        for header, value in self.headers.items():
            print(f"{header}: {value}")
        
        # Call the superclass method to handle the file serving
        super().do_GET()

    def log_message(self, format, *args):
        # You can log the requests here for debugging or monitoring
        print(f"Download request: {self.address_string()} - {format % args}")

if __name__ == "__main__":
    server_address = ('', PORT)
    
    # Set up the threaded HTTP server
    httpd = ThreadedHTTPServer(server_address, DownloadHandler)
    
    print(f"Serving files on port {PORT} in directory {DOWNLOAD_DIRECTORY}...")
    
    # Run the server until interrupted
    try:
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("\nServer interrupted, shutting down...")
        httpd.server_close()
