import mysql.connector
import numpy as np

# Establish a connection to the MySQL database
def establish_connection():
    return mysql.connector.connect(
        host='localhost',
        user='root',
        password='',
        database='face_recognition_db'
    )

# Store a face embedding into the database
def save_embedding_to_db(person_name, face_vector):
    connection = establish_connection()
    cursor = connection.cursor()

    # Convert numpy array to bytes for BLOB storage
    embedding_blob = face_vector.tobytes()

    insert_query = "INSERT INTO faces (name, embedding) VALUES (%s, %s)"
    cursor.execute(insert_query, (person_name, embedding_blob))

    connection.commit()
    cursor.close()
    connection.close()

# Load all face embeddings from the database
def fetch_all_embeddings():
    connection = establish_connection()
    cursor = connection.cursor()

    cursor.execute("SELECT name, embedding FROM faces")
    records = cursor.fetchall()

    embeddings = [(name, np.frombuffer(blob, dtype=np.float32)) for name, blob in records]

    cursor.close()
    connection.close()

    return embeddings
