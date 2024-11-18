import os
import re
import sqlite3
from typing import Any, List, Tuple, Union


class Model:
    def __init__(self, db_name: str):
        self.db_name = db_name
        self.setup()

    def setup(self):
        self.name: str = ""
        self.fields: List[str] = []
        self.create_table_query: str = ""
        self.create_samples: Tuple[str, List[List[Any]]] = ("", [])


class Users(Model):
    def setup(self):
        self.name = "Users"
        self.fields = [
            "user_id INTEGER PRIMARY KEY AUTOINCREMENT",
            "username TEXT NOT NULL",
            "password TEXT NOT NULL"
        ]
        self.create_table_query = f"""
            CREATE TABLE IF NOT EXISTS {self.name} (
                {", ".join(self.fields)}
            );
        """
        self.create_samples = (
            f"INSERT INTO {self.name} (username, password) VALUES (?, ?);",
            [["user1", "pass1"], ["user2", "pass2"]]
        )


class Nodes(Model):
    def setup(self):
        self.name = "Nodes"
        self.fields = [
            "node_id INTEGER PRIMARY KEY AUTOINCREMENT",
            "node_name TEXT NOT NULL",
            "path_to_store TEXT NOT NULL"
        ]
        self.create_table_query = f"""
            CREATE TABLE IF NOT EXISTS {self.name} (
                {", ".join(self.fields)}
            );
        """
        self.create_samples = (
            f"INSERT INTO {self.name} (node_name, path_to_store) VALUES (?, ?);",
            [["Node1", os.path.join(os.path.dirname(__file__), "../storage/Node1")], ["Node2", os.path.join(os.path.dirname(__file__), "../storage/Node2")]]
        )


class Files(Model):
    def setup(self):
        self.name = "Files"
        self.fields = [
            "file_id INTEGER PRIMARY KEY AUTOINCREMENT",
            "file_name TEXT NOT NULL",
            "file_size INTEGER NOT NULL",
            "user_id INTEGER",
            "node_id INTEGER"
        ]
        self.create_table_query = f"""
            CREATE TABLE IF NOT EXISTS {self.name} (
                {", ".join(self.fields)}
            );
        """
        self.create_samples = (
            f"INSERT INTO {self.name} (file_name, file_size, user_id, node_id) VALUES (?, ?, ?, ?);",
            [["file1", 100, 1, 1], ["file2", 200, 2, 2]]
        )


class Cache(Model):
    def setup(self):
        self.name = "Cache"
        self.fields = [
            "query_id INTEGER PRIMARY KEY AUTOINCREMENT",
            "file_id INTEGER",
            "file_range text", # format bytes=a-b
            "user_id INTEGER",
            "node_id INTEGER",
            "user_action TEXT",  # if user_action = download -> node_action = upload and vice versa
            "node_action TEXT",
            "done_by_user BOOLEAN",
            "done_by_node BOOLEAN"
        ]
        
        self.create_table_query = f"""
            CREATE TABLE IF NOT EXISTS {self.name} (
                {", ".join(self.fields)}
            );
        """
        
        # Adjust the insert query and sample data to include user_action and node_action
        self.create_samples = (
            f"INSERT INTO {self.name} (file_id, file_range, user_id, node_id, user_action, node_action, done_by_user, done_by_node) "
            f"VALUES (?, ?, ?, ?, ?, ?, ?, ?);",
            [
                [1, "bytes=0-9", 1, 1, "download", "upload", True, True],
                [2, "bytes=0-9", 2, 2, "upload", "download", True, True]
            ]
        )

class MyDBMS:
    def __init__(self, db_name: str):
        self.db_name = db_name
        self.models = [Users(db_name), Nodes(db_name), Files(db_name), Cache(db_name)]
        self.tables = [model.name for model in self.models]
        self.migrate_data = True

    def execute_query(self, query: str, params: Tuple[Any, ...] = ()) -> Union[bool, List[Tuple]]:
        try:
            with sqlite3.connect(self.db_name) as connection:
                cursor = connection.cursor()

                if query.strip().lower().startswith("select"):
                    cursor.execute(query, params)
                    return cursor.fetchall()

                elif query.strip().lower().startswith(("drop table if exists", "create table if not exists")):
                    cursor.execute(query, params)
                    return True

                elif query.strip().lower().startswith(("insert", "delete", "update")):
                    cursor.execute(query, params)
                    connection.commit()
                    return True

                elif query.strip().lower().startswith("pragma"):
                    cursor.execute(query, params)
                    return cursor.fetchall()

                else:
                    print(f"Unsupported query type: {query}")
                    return False

        except sqlite3.Error as e:
            print(f"Query failed: {query.strip()}. Error: {e}")
            if query.strip().lower().startswith("select"):
                return []  # Return an empty list for SELECT queries on failure
            return False

    def node_init(self, attributes: dict, node_id: int, data: str) -> Tuple[bool, str]:
        """
        Initialize a node and populate related data.

        :param attributes: A dictionary with keys `Nodename` and `PathToSave`.
        :param node_id: The ID of the node.
        :param data: String containing information about files in the format
                    "file_size/user<user_id>_file_name\n".
        :return: Tuple indicating success and a status message.
        """
        try:
            # Extract attributes
            node_name = attributes.get("Nodename")
            path_to_save = attributes.get("PathToSave")

            if not node_name or not path_to_save:
                return False, "Invalid attributes: Nodename and PathToSave are required."

            # Insert into Nodes table
            insert_node_query = """
                INSERT INTO Nodes (node_id, node_name, path_to_store)
                VALUES (?, ?, ?);
            """
            node_inserted = self.execute_query(insert_node_query, (node_id, node_name, path_to_save))
            if not node_inserted:
                return False, f"Failed to insert node with ID {node_id}."

            # Regex pattern to match the format: file_size/user<user_id>_file_name
            pattern = r"^(\d+)/user(\d+)_([\w\d_]+)$"  # file_size/user<user_id>_file_name

            # Parse and process the data string
            lines = data.strip().split('\n')
            for line in lines:
                # Validate line format using regex
                match = re.match(pattern, line)
                if match:
                    file_size = int(match.group(1))
                    user_id = int(match.group(2))
                    file_name = match.group(3)

                    # Insert into Files table
                    insert_file_query = """
                        INSERT INTO Files (file_name, file_size, user_id, node_id)
                        VALUES (?, ?, ?, ?);
                    """
                    file_inserted = self.execute_query(insert_file_query, (file_name, file_size, user_id, node_id))
                    if not file_inserted:
                        print(f"Failed to insert file: {file_name}")
                else:
                    print(f"Invalid format for line: {line}")

            return True, f"Node {node_id} and related files initialized successfully."

        except Exception as e:
            return False, f"An error occurred: {e}"


    def user_files(self, attributes: dict, user_id: int, node_id: int) -> Tuple[bool, str]:
        file_name = attributes.get("Filename")
        
        # Construct the query to fetch file information based on user_id and node_id
        query = """
            SELECT file_name, file_size
            FROM Files
            WHERE user_id = ? AND node_id = ?
        """
        
        # Execute the query with user_id and node_id as parameters
        result = self.execute_query(query, (user_id, node_id))
        
        if not result:
            return False, "No files found for the given user_id and node_id."

        # Format the result into a response string
        response = ""
        for file_name, file_size in result:
            response += f"{file_name}/{file_size}/bytes\n"
        
        return True, response

    def user_marked_action(self, attributes: dict, user_id: int, node_id: int) -> Tuple[bool, str]:
        file_name = attributes.get("Filename")
        if not file_name:
            return False, "Filename attribute is missing."
        
        # Step 1: Retrieve file_id from Files table
        query = """
            SELECT file_id 
            FROM Files 
            WHERE file_name = ? AND user_id = ? AND node_id = ?
        """
        result = self.execute_query(query, (file_name, user_id, node_id))

        if not result:
            return False, "File not found for the given user and node."

        file_id = result[0][0]  # Assuming result is a list of tuples [(file_id,)]

        # Step 2: Update the Cache table to set done_by_user to 1
        update_query = """
            UPDATE Cache
            SET done_by_user = 1
            WHERE user_id = ? AND node_id = ? AND file_id = ? AND done_by_user = 0
        """
        update_result = self.execute_query(update_query, (user_id, node_id, file_id))

        if update_result:
            return True, "Action marked as done by user."
        else:
            return False, "Failed to update Cache table."

    def user_download_get(self, attributes: dict, user_id: int, node_id: int) -> Tuple[bool, str]:
        file_name = attributes.get("Filename")
        if not file_name:
            return False, "Filename attribute is missing."
        
        # Fetch the path to store for the given node_id
        query = "SELECT path_to_store FROM Nodes WHERE node_id = ?"
        result = self.execute_query(query, (node_id,))
        
        if not result:
            return False, f"Node with ID {node_id} not found."
        
        path_to_store = result[0][0]  # The path is the first column of the result
        
        # Construct the full path to the file
        file_path = os.path.join(path_to_store, "user"+ str(user_id) + '_' + file_name)

        # Check if the file exists
        if not os.path.exists(file_path):
            return False, f"File '{file_name}' not found in the specified node directory."
        
        # Read the file content
        try:
            with open(file_path, "r") as file:
                data = file.read()
            return True, data  # Return the file data if the file exists and is read successfully
        except Exception as e:
            return False, f"Error reading the file: {str(e)}"

    def user_download_post(self, user_id: int, node_id: int, data: dict) -> Tuple[bool, str]:
        # Step 1: Check if the file exists in the Files table
        query = "SELECT file_id FROM Files WHERE user_id = ? AND node_id = ? AND file_name = ?"
        result = self.execute_query(query, (user_id, node_id, data['file_name']))
        
        if not result:
            # File not found, return False with a message
            return False, "File not found for the given user and node."
        
        # Extract file_id from the result
        file_id = result[0][0]
        
        # Step 2: Insert the action into the Cache table
        insert_query = """
            INSERT INTO Cache (file_id, file_range, user_id, node_id, user_action, node_action, done_by_user, done_by_node)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        """
        insert_params = (
            file_id, 
            data['file_range'], 
            user_id, 
            node_id, 
            'download',  # User action
            'upload',    # Node action
            False,       # done_by_user initially False
            False        # done_by_node initially False
        )
        
        insert_result = self.execute_query(insert_query, insert_params)
        
        if insert_result:
            return True, "Download action added to the cache."
        else:
            return False, "Failed to add the download action to the cache."

    def node_upload(self, attributes: dict, node_id: int, data: str) -> Tuple[bool, str]:
        # Step 1: Select the path to save the file from the Nodes table
        select_query = "SELECT path_to_store FROM Nodes WHERE node_id = ?"
        result = self.execute_query(select_query, (node_id,))
        
        if not result:
            return False, f"Node with id {node_id} not found."
        
        # Extract the path where the file should be stored
        path_to_save = result[0][0]
        
        # Step 2: Write the file to the specified path
        try:
            # Ensure the directory exists
            os.makedirs(path_to_save, exist_ok=True)
            
            # Write the file with 'bin' and 'truncate' mode
            file_path = os.path.join(path_to_save, attributes["Filename"])
            with open(file_path, 'wb') as file:
                if data:
                    file.write(data.encode('utf-8'))
                else:
                    return False, "No file content provided in the request."
            
            # Step 3: Mark the action as completed in the Cache table
            node_action_result, message = self.node_marked_action(attributes, node_id)
            
            if node_action_result:
                return True, f"File uploaded and action marked as done for node '{node_id}': {message}"
            else:
                return False, message
        
        except Exception as e:
            return False, f"Error writing file to disk: {str(e)}"

    def node_marked_action(self, attributes: dict, node_id: str) -> Tuple[bool, str]:
        # Extract filename and user_id from the provided attributes
        filename = attributes.get("Filename")  # "user1_your_file_name"
        filerange = attributes.get("Range")  # If this exists, update file_range in cache too
        
        # Split the filename into user and file_name
        user, file_name = filename.split('_', 1)  # Split into user and file_name
        user_id = user[4:]  # Extract the user_id from the 'user' string, assuming it's in the format 'userX'

        # Query to select the file_id from the Files table based on user_id, node_id, and file_name
        select_query = """
            SELECT file_id
            FROM Files
            WHERE user_id = ? AND node_id = ? AND file_name = ?
        """
        result = self.execute_query(select_query, (user_id, node_id, file_name))

        if result:  # If the query returns a result, we proceed
            file_id = result[0][0]  # Extract the file_id from the result

            # If filerange exists, update the file_range in Cache
            if filerange:
                update_range_query = """
                    UPDATE Cache
                    SET file_range = ?
                    WHERE user_id = ? AND node_id = ? AND file_id = ? AND done_by_node = ?
                """
                update_range_result = self.execute_query(update_range_query, (filerange, user_id, node_id, file_id, False))
                
                if not update_range_result:  # Check if the range update failed
                    return False, "Failed to update file range in Cache."

            # Update the Files table to set done_by_node = True
            update_query = """
                UPDATE Cache
                SET done_by_node = ?
                WHERE user_id = ? AND node_id = ? AND file_id = ? AND done_by_node = ?
            """
            update_result = self.execute_query(update_query, (True, user_id, node_id, file_id, False))

            if update_result:  # If the update query is successful
                return True, f"Action marked as done for file '{file_name}' by node '{node_id}'."
            else:
                return False, "Failed to update the file action status."
        else:
            return False, "File not found in the database."

    def node_download(self, attributes: dict, node_id: int) -> Tuple[bool, str]:
        # Query to get the path_to_store for the given node_id
        query = """
            SELECT path_to_store
            FROM Nodes
            WHERE node_id = ?;
        """
        result = self.execute_query(query, (node_id,))
        
        # If no result is returned, the node_id does not exist
        if not result:
            return False, f"Node with ID {node_id} not found."

        path_to_store = result[0][0]  # Get the path_to_store from the query result

        # Construct the file path
        file_name = attributes.get("Filename")
        if not file_name:
            return False, "Filename attribute is missing."

        file_path = os.path.join(path_to_store, file_name)

        # Check if the file exists at the path
        if not os.path.exists(file_path):
            return False, f"File {file_name} not found at {file_path}."

        # Read the file data
        try:
            with open(file_path, "r") as file:
                data = file.read()
            return True, data  # Return the file data if read successfully
        except Exception as e:
            return False, f"Error reading file: {e}"

    def node_ready(self, node_id: int) -> Tuple[bool, str]:
        # Step 1: Define the query to select the user_id and file_name for unfinished cache entries
        query = """
            SELECT Cache.user_id, Cache.node_action, Cache.file_range, Files.file_name
            FROM Cache
            LEFT JOIN Files ON Cache.file_id = Files.file_id
            WHERE Cache.node_id = ? AND Cache.done_by_node = 0
        """
        
        # Step 2: Execute the query and fetch results
        result = self.execute_query(query, (node_id,))
        
        # Step 3: Process the result and format the response
        response = ""
        if result:
            for item in result:
                user_id, node_action, file_range, file_name = item  # Unpack the result properly
                print (item)
                start_byte, end_byte = file_range.split('=')[1].split('-')
                response += f"{node_action}/{start_byte}/user{user_id}_{file_name}\n"  # Include node_action in the response

        # Step 4: Return the result as a tuple
        return True, response

    def user_upload(self, attributes:dict, user_id:int, node_id:int, data:str) -> Tuple[bool, str]:
        # Extract filename and range details from attributes
        file_name = attributes["Filename"]
        file_range = attributes["Range"]
        file_size = int(attributes["Content-Length"])

        # 1. Precheck if the file already exists in the Files table
        query = "SELECT file_id FROM Files WHERE user_id = ? AND node_id = ? AND file_name = ?"
        result_files = self.execute_query(query, (user_id, node_id, file_name))

        if not result_files:  # File doesn't exist, insert a new file record
            query = """
                INSERT INTO Files (file_name, file_size, user_id, node_id) 
                VALUES (?, ?, ?, ?)
            """
            self.execute_query(query, (file_name, 0, user_id, node_id))  # Initial file size is 0

            # Retrieve the newly inserted file_id
            result_files = self.execute_query("SELECT file_id FROM Files WHERE user_id = ? AND node_id = ? AND file_name = ?", (user_id, node_id, file_name))

        file_id = result_files[0][0]

        # 2. Precheck if the node exists and get the path to store
        query = "SELECT path_to_store FROM Nodes WHERE node_id = ?"
        result_nodes = self.execute_query(query, (node_id,))

        if not result_nodes:  # No node found
            return False, "Node not found"

        node_path = result_nodes[0][0]

        # 3. Save the uploaded data to a file on the node's storage
        file_path = os.path.join(node_path, f"user{user_id}_{file_name}")
        try:
            with open(file_path, 'wb') as file:
                file.write(data.encode('utf-8'))  # Saving data in binary mode
        except Exception as e:
            return False, f"Error saving file: {e}"

        # 4. Update the file size in the Files table
        start_byte, end_byte = map(int, file_range.split('=')[1].split('-'))
        new_size = end_byte + 1

        query = "UPDATE Files SET file_size = file_size + ? WHERE file_id = ?"
        update_result = self.execute_query(query, (new_size, file_id))

        if not update_result:
            return False, "Failed to update file size"

        # 5. Insert a record into the Cache table for this upload operation
        query = """
            INSERT INTO Cache (file_id, file_range, user_id, node_id, user_action, node_action, done_by_user, done_by_node)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        """
        insert_result = self.execute_query(query, (file_id, file_range, user_id, node_id, "upload", "download", True, False))

        if not insert_result:
            return False, "Failed to record cache entry"

        # 6. Return success
        return True, "Upload successful"
    
    def user_ready(self, user_id: int, node_id: int) -> Tuple[bool, str]:
        """
        Check if the user is ready based on whether the associated actions in the Cache table
        have been completed by both the user and the node.
        Returns:
            - False with a message if action is not completed by user or node
            - True, "Ready" if actions are completed by both
        """
        query = f"""
        SELECT * 
        FROM Cache
        WHERE user_id = ? AND node_id = ? AND (done_by_user = 0 OR done_by_node = 0);
        """
        
        # Execute the query with user_id as a parameter
        result = self.execute_query(query, (user_id, node_id, ))

        if result:
            # If there's any result, it means the action is not completed by either the user or node
            return False, "Action not completed by user or node."
        else:
            # No results found, indicating the user and node have completed their actions
            return True, "Ready"

    def user_login(self, username: str, password: str) -> Tuple[bool, str]:
        """
        Verifies user login credentials.

        Args:
            username (str): The username provided by the user.
            password (str): The password provided by the user.

        Returns:
            Tuple[bool, str]: A tuple containing a boolean for success and a message.
        """
        query = "SELECT * FROM Users WHERE username = ? AND password = ?;"
        result = self.execute_query(query, (username, password))

        if result:
            return True, "Login successful."
        else:
            return False, "Invalid username or password."

    def setup(self):
        for model in self.models:
            # Drop table if it exists
            self.execute_query(f"DROP TABLE IF EXISTS {model.name};")

            # Create table
            self.execute_query(model.create_table_query)

            # Insert sample data
            if self.migrate_data:
                query, records = model.create_samples
                for record in records:
                    self.execute_query(query, tuple(record))

            # Create directories based on Nodes table paths
            if model.name == "Nodes":
                nodes = self.execute_query(f"SELECT path_to_store FROM {model.name}")
                for node in nodes:
                    node_path = node[0]
                    # Check if the directory exists, if not, create it
                    if not os.path.exists(node_path):
                        try:
                            os.makedirs(node_path)
                            print(f"Directory created: {node_path}")
                        except Exception as e:
                            print(f"Failed to create directory {node_path}. Error: {e}")

    def show(self, table_name: str):
        if table_name not in self.tables:
            print(f"Error: Invalid table name. Available tables: {self.tables}")
            return

        query = f"SELECT * FROM {table_name};"
        result = self.execute_query(query)
        print(table_name, " Table:")
        for row in result:
            print(row)

    def showSchema(self):
        print("Table Schemas:")
        for table in self.tables:
            query = f"PRAGMA table_info({table});"
            schema = self.execute_query(query)
            if schema:
                print(f"Schema for table {table}:")
                print(f"{'Column':<30}{'Type':<15}{'Not Null':<12}{'Default':<15}{'Primary Key':<12}")
                print("-" * 84)
                for column in schema:
                    _, column_name, column_type, not_null, default, primary_key = column  # Unpack all six fields
                    print(f"{column_name:<30}{column_type:<15}{not_null:<12}{default if default else 'None':<15}{primary_key:<12}")
                print()  # Add a blank line after each table's schema




def main():
    database_path = os.path.join(os.path.dirname(__file__), "../storage/test.db")
    my_dbms = MyDBMS(database_path)
    my_dbms.setup()

    # Show data from the Users table
    my_dbms.show("Users")

    # Show schema for all tables
    my_dbms.showSchema()


if __name__ == "__main__":
    main()
