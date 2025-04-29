import cv2
import numpy as np
import onnxruntime as ort
from numpy.linalg import norm

# Load ArcFace ONNX model and Haar Cascade
arcface_model_path = 'model/arcface.onnx'
arcface_session = ort.InferenceSession(arcface_model_path)

haar_cascade_path = r"haarcascade_frontalface_default.xml"
face_detector = cv2.CascadeClassifier(haar_cascade_path)

# Detect faces from an image using Haar cascades
def locate_faces(frame):
    grayscale = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    face_boxes = face_detector.detectMultiScale(grayscale, scaleFactor=1.1, minNeighbors=5)
    return face_boxes

# Generate normalized embedding using ArcFace
def encode_face(face_image):
    preprocessed = face_image.astype('float32')
    preprocessed = cv2.resize(preprocessed, (112, 112))
    preprocessed = np.expand_dims(preprocessed, axis=0)
    preprocessed = np.transpose(preprocessed, (0, 3, 1, 2))
    preprocessed = (preprocessed - 127.5) / 128.0

    input_name = arcface_session.get_inputs()[0].name
    embedding = arcface_session.run(None, {input_name: preprocessed})[0]
    return embedding[0]

# Extract face region and return its embedding
def extract_face_embedding(frame, face_coords):
    (x, y, w, h) = face_coords
    cropped_face = frame[y:y+h, x:x+w]
    return encode_face(cropped_face)

# Compare two face embeddings and check for match
def verify_identity(embedding_a, embedding_b, threshold=0.8):
    norm_a = embedding_a / norm(embedding_a)
    norm_b = embedding_b / norm(embedding_b)
    similarity = norm(norm_a - norm_b)
    print(f"Distance = {similarity}, Threshold = {threshold}")
    return similarity < threshold
