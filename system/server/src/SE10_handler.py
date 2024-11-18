import os
import re
import json
from typing import List
from SE10_dbms import MyDBMS
from SE10_crypto import MyCrypto

class GeneralHandler:
    STATUS_CODES = {
        200: "OK",
        400: "Bad Request",
        404: "Not Found",
        405: "Method Not Allowed",
        500: "Internal Server Error"
    }

    def __init__(self, dbms, crypto):
        self.dbms:MyDBMS = dbms
        self.crypto:MyCrypto = crypto        

    def handleHeadRequest(self, rfile):
        lines: List = []
        while True:
            line = rfile.readline().decode('utf-8')
            if line == '\r\n' or line == '\n':
                break
            lines.append(line.strip())

        method, path, version = lines[0].split()
        attributes = {
            key.strip(): value.strip() 
            for line in lines[1:] 
            for key, value in (line.split(':', 1),)
        }

        return ((method, path, version), attributes)
    
    def receiveMessage(self, rfile, content_length: int) -> bytes:
        """
        Reads the message body based on the given content length.
        """
        try:
            message = rfile.read(content_length)
            return message
        except Exception as e:
            print(f"Failed to read message body: {str(e)}")
            return b''
        
    def sendResponse(self, wfile, status_code: int, response: bytes):
        # Send header
        status_message = self.STATUS_CODES.get(status_code, "Unknown Status")
        header = f"HTTP/1.1 {status_code} {status_message}\r\nContent-Length: {len(response)}\r\n\r\n"
        wfile.write(header.encode('utf-8'))

        # Send response
        wfile.write(response)

class GETHandler:
    def __init__(self, general_handler):
        self.general_handler: GeneralHandler = general_handler

    def user_ready(self, rfile, wfile, attributes:dict, user_id:int, node_id:int):
        result = self.general_handler.dbms.user_ready(user_id, node_id)
        
        # Check if the user is ready
        if result[0]:
            response = b'{"status": "success", "message": "Ready"}'
            status_code = 200
        else:
            response = f'{{"status": "error", "message": "{result[1]}"}}'.encode('utf-8')
            status_code = 400

        # Send response
        self.general_handler.sendResponse(wfile, status_code, response)

    def node_ready(self, rfile, wfile, attributes, node_id):
        result = self.general_handler.dbms.node_ready(node_id)
        status_code = 200
        response = result[1].encode('utf-8')

        # Send response
        self.general_handler.sendResponse(wfile, status_code, response)

    def node_download(self, rfile, wfile, attributes:dict, node_id:int):
        result = self.general_handler.dbms.node_download(attributes, node_id)

        # Check if the result is a success
        if result[0]:
            response = result[1].encode('utf-8')
            status_code = 200
            
        else:
            # Handle error, if the file was not found or any other issue
            response = f'{{"status": "error", "message": "{result[1]}"}}'.encode('utf-8')
            status_code = 404  # Not Found

        # Send response
        self.general_handler.sendResponse(wfile, status_code, response)
        if result[0]:
            self.general_handler.dbms.node_marked_action(attributes, node_id)

    def user_download(self, rfile, wfile, attributes: dict, user_id: int, node_id: int):
        # Call to DBMS method to get the file details
        result = self.general_handler.dbms.user_download_get(attributes, user_id, node_id)
        # Check if the result is a success
        if result[0]:
            response = result[1].encode('utf-8')
            status_code = 200
            
        else:
            # Handle error, if the file was not found or any other issue
            response = f'{{"status": "error", "message": "{result[1]}"}}'.encode('utf-8')
            status_code = 404  # Not Found

        # Send the response to the user
        self.general_handler.sendResponse(wfile, status_code, response)
        if result[0]:
            result = self.general_handler.dbms.user_marked_action(attributes, user_id, node_id)

    def user_files(self, rfile, wfile, attributes:dict, user_id:int, node_id):
        result = self.general_handler.dbms.user_files(attributes, user_id, node_id)
        # Check if the result is a success
        if result[0]:
            response = result[1].encode('utf-8')
            status_code = 200
            
        else:
            # Handle error, if the file was not found or any other issue
            response = f'{{"status": "error", "message": "{result[1]}"}}'.encode('utf-8')
            status_code = 404  # Not Found

        # Send the response to the user
        self.general_handler.sendResponse(wfile, status_code, response)


class POSTHandler:
    def __init__(self, general_handler):
        self.general_handler: GeneralHandler = general_handler

    # def user_upload_query(self, rfile, wfile, attributes, user_id, node_id):
    #     try:
    #         content_length = int(attributes.get("Content-Length", 0))
    #         data = self.general_handler.receiveMessage(rfile, content_length).decode('utf-8')

    #         result, message = self.general_handler.dbms.?

    #     except Exception as e:
    #         error_message = f'{{"status": "error", "message": "Failed to process upload query: {str(e)}"}}'.encode('utf-8')
    #         self.general_handler.sendResponse(wfile, 400, error_message)

    def node_init(self, rfile, wfile, attributes, node_id):
        try:
            content_length = int(attributes.get("Content-Length", 0))
            data = self.general_handler.receiveMessage(rfile, content_length).decode('utf-8')

            result, message = self.general_handler.dbms.node_init(attributes, node_id, data)

            if result:
                status_code = 200
                response = f'{{"status": "success", "message": "{message}"}}'.encode('utf-8')
            else:
                status_code = 400
                response = f'{{"status": "error", "message": "{message}"}}'.encode('utf-8')
            
            # Step 4: Send the response to the client
            self.general_handler.sendResponse(wfile, status_code, response)

        except Exception as e:
            error_message = f'{{"status": "error", "message": "Failed to process upload query: {str(e)}"}}'.encode('utf-8')
            self.general_handler.sendResponse(wfile, 400, error_message)

    def user_login(self, rfile, wfile, attributes):
        try:
            content_length = int(attributes.get("Content-Length", 0))
            message = self.general_handler.receiveMessage(rfile, content_length).decode('utf-8')

            # Decode and process the message (assuming JSON format for example)
            data = json.loads(message)

            username = data.get("username")
            password = data.get("password")

            # Mock authentication
            result = self.general_handler.dbms.user_login(username, password)

            if result[0]:
                response = b'{"status": "success", "message": "Login successful"}'
                status_code = 200
            else:
                response = b'{"status": "error", "message": "Invalid credentials"}'
                status_code = 400

            # Send response
            self.general_handler.sendResponse(wfile, status_code, response)

        except Exception as e:
            error_message = f'{{"status": "error", "message": "Failed to process login: {str(e)}"}}'.encode('utf-8')
            self.general_handler.sendResponse(wfile, 400, error_message)

    def user_upload(self, rfile, wfile, attributes, user_id, node_id):
        try:
            content_length = int(attributes.get("Content-Length", 0))
            message = self.general_handler.receiveMessage(rfile, content_length).decode('utf-8')
            result = self.general_handler.dbms.user_upload(attributes, user_id, node_id, message)

            if result[0]:
                response = b'{"status": "success", "message": "success"}'
                status_code = 200
            else:
                response = b'{"status": "error", "message": "fail"}'
                status_code = 400

            # Send response
            self.general_handler.sendResponse(wfile, status_code, response)

        except Exception as e:
            error_message = f'{{"status": "error", "message": "Failed to process upload query: {str(e)}"}}'.encode('utf-8')
            self.general_handler.sendResponse(wfile, 400, error_message)

    def user_download(self, rfile, wfile, attributes, user_id, node_id):
        try:
            content_length = int(attributes.get("Content-Length", 0))
            message = self.general_handler.receiveMessage(rfile, content_length).decode('utf-8')

            # Decode and process the message (assuming JSON format for example)
            data = json.loads(message)

            # Call the user_download_post method and get the result
            success, result_message = self.general_handler.dbms.user_download_post(user_id, node_id, data)
            
            # Prepare the response based on the result
            if success:
                response = {
                    "status": "success",
                    "message": result_message
                }
                self.general_handler.sendResponse(wfile, 200, json.dumps(response).encode('utf-8'))
            else:
                response = {
                    "status": "error",
                    "message": result_message
                }
                self.general_handler.sendResponse(wfile, 400, json.dumps(response).encode('utf-8'))

        except Exception as e:
            # If an exception occurs, send an error response
            error_message = f'{{"status": "error", "message": "Failed to process download query: {str(e)}"}}'.encode('utf-8')
            self.general_handler.sendResponse(wfile, 400, error_message)

    def node_upload(self, rfile, wfile, attributes, node_id):
        try:
            # Step 1: Get the content length from the request headers and read the incoming file data
            content_length = int(attributes.get("Content-Length", 0))
            
            # Use the general handler to receive the file data
            data = self.general_handler.receiveMessage(rfile, content_length).decode('utf-8')
            
            # Step 2: Call the node_upload method to handle the file upload process
            result, message = self.general_handler.dbms.node_upload(attributes, node_id, data)
            
            # Step 3: Prepare the response based on the result of the upload
            if result:
                # If the upload was successful, send a success response
                status_code = 200
                response = f'{{"status": "success", "message": "{message}"}}'.encode('utf-8')
            else:
                # If the upload failed, send an error response
                status_code = 400
                response = f'{{"status": "error", "message": "{message}"}}'.encode('utf-8')
            
            # Step 4: Send the response to the client
            self.general_handler.sendResponse(wfile, status_code, response)

        except Exception as e:
            # In case of an exception, send an error response
            error_message = f'{{"status": "error", "message": "Failed to process upload query: {str(e)}"}}'.encode('utf-8')
            self.general_handler.sendResponse(wfile, 400, error_message)


class Handler:
    descriptions = {
        "user_login": {
            'methods': 'POST',
            'path_regex_pattern': r'^/api/user/login$',
            'function_name': 'user_login'
        },
        "user_ready": {
            'methods': 'GET',
            'path_regex_pattern': r'^/api/user-node/(\d+)-(\d+)/ready$',
            'function_name': 'user_ready'
        },
        "user_upload": {
            'methods': 'POST',
            'path_regex_pattern': r'^/api/user-node/(\d+)-(\d+)/upload$',
            'function_name': 'user_upload'
        },
        "user_download": {
            'methods': 'POST',
            'path_regex_pattern': r'^/api/user-node/(\d+)-(\d+)/download$',
            'function_name': 'user_download'
        },
        "user_download": {
            'methods': 'GET',
            'path_regex_pattern': r'^/api/user-node/(\d+)-(\d+)/download$',
            'function_name': 'user_download'
        },
        "node_ready": {
            'methods': 'GET',
            'path_regex_pattern': r'^/api/node/(\d+)/ready$',
            'function_name': 'node_ready'
        },
        "node_download": {
            'methods': 'GET',
            'path_regex_pattern': r'^/api/node/(\d+)/download$',
            'function_name': 'node_download'
        },
        "node_upload": {
            'methods': 'POST',
            'path_regex_pattern': r'^/api/node/(\d+)/upload$',
            'function_name': 'node_upload'
        },
        "user_files": {
            'methods': 'GET',
            'path_regex_pattern': r'^/api/user-node/(\d+)-(\d+)/files$',
            'function_name': 'user_files'
        },
        "node_init": {
            'methods': 'POST',
            'path_regex_pattern': r'^/api/node/(\d+)/init$',
            'function_name': 'node_init'
        },
    }

    def __init__(self, dbms, crypto):
        self.general_handler = GeneralHandler(dbms, crypto)
        self.get_handler = GETHandler(self.general_handler)
        self.post_handler = POSTHandler(self.general_handler)
        
    def handle_request(self, rfile, wfile):
        try:
            # Parse the request headers
            (method, path, version), attributes = self.general_handler.handleHeadRequest(rfile)
            print(path)

            # Handle GET requests
            if method == 'GET':
                for description in self.descriptions.values():
                    match = re.match(description['path_regex_pattern'], path)
                    if match:
                        func = getattr(self.get_handler, description['function_name'], None)
                        if func:
                            params = map(int, match.groups())  # Convert regex groups to integers
                            func(rfile, wfile, attributes, *params)
                            return
                self.general_handler.sendResponse(wfile, 404, b'{"status": "error", "message": "API not supported"}')

            # Handle POST requests
            elif method == 'POST':
                for description in self.descriptions.values():
                    match = re.match(description['path_regex_pattern'], path)
                    if match:
                        func = getattr(self.post_handler, description['function_name'], None)
                        if func:
                            params = map(int, match.groups())  # Convert regex groups to integers
                            func(rfile, wfile, attributes, *params)
                            return
                self.general_handler.sendResponse(wfile, 404, b'{"status": "error", "message": "API not supported"}')

            # If method not allowed
            else:
                self.general_handler.sendResponse(wfile, 405, b'{"status": "error", "message": "Method not supported"}')

        except Exception as e:
            # Internal Server Error
            response = f'{{"status": "error", "message": "Internal Server Error: {str(e)}"}}'.encode('utf-8')
            self.general_handler.sendResponse(wfile, 500, response)

