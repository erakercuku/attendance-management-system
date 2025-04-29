import os
import cv2
import time
import numpy as np

from db_utils import (
    save_embedding_to_db,
    fetch_all_embeddings
)

from face_utils import (
    locate_faces,
    extract_face_embedding,
    verify_identity
)


def draw_text(frame, text, position, color=(0, 255, 255), font_scale=0.7, thickness=2):
    font = cv2.FONT_HERSHEY_SIMPLEX
    cv2.putText(frame, text, position, font, font_scale, color, thickness, cv2.LINE_AA)


def register_new_face():
    try:
        cap = cv2.VideoCapture(0)
        if not cap.isOpened():
            print("Unable to access the webcam.")
            return

        cv2.namedWindow("Face Detection", cv2.WND_PROP_FULLSCREEN)
        cv2.setWindowProperty("Face Detection", cv2.WND_PROP_TOPMOST, 1)

        start_time = time.time()

        while True:
            ret, frame = cap.read()
            if not ret:
                print("Camera read failure.")
                break

            faces = locate_faces(frame)
            elapsed_time = int(time.time() - start_time)
            countdown = 5 - elapsed_time

            if len(faces) > 0:
                (x, y, w, h) = faces[0]
                center = (x + w // 2, y + h // 2)
                radius = w // 2

                cv2.circle(frame, center, radius, (0, 255, 0), 2)
                draw_text(frame, f"Face detected. Hold still! Capturing in {countdown}s...", (10, 30))

                if countdown <= 0:
                    face_embedding = extract_face_embedding(frame, faces[0])
                    cap.release()
                    cv2.destroyAllWindows()

                    person_name = input("Please enter the name to associate with this face: ")
                    if person_name:
                        try:
                            save_embedding_to_db(person_name, face_embedding)
                            print(f"{person_name}'s face has been successfully registered.")
                        except Exception as e:
                            print(f"Database error while saving: {str(e)}")
                    else:
                        print("Registration cancelled. No name provided.")

                    input("Press Enter to return to the main menu...")
                    os.system('cls' if os.name == 'nt' else 'clear')
                    break

            draw_text(frame, "Scanning for face...", (10, 30))
            draw_text(frame, f"Photo capture in {countdown}s...", (10, 60))
            cv2.imshow("Face Detection", frame)

            if cv2.waitKey(1) & 0xFF == ord('q') or countdown <= 0:
                break

        cap.release()
        cv2.destroyAllWindows()

    except Exception as e:
        print(f"An unexpected error occurred: {str(e)}")


def verify_face_from_camera():
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("Webcam could not be accessed.")
        return

    cv2.namedWindow("Face Detection", cv2.WND_PROP_FULLSCREEN)
    cv2.setWindowProperty("Face Detection", cv2.WND_PROP_TOPMOST, 1)

    start_time = time.time()

    while True:
        ret, frame = cap.read()
        if not ret:
            print("Camera feed could not be retrieved.")
            break

        faces = locate_faces(frame)
        elapsed_time = int(time.time() - start_time)
        countdown = 5 - elapsed_time

        if len(faces) > 0:
            (x, y, w, h) = faces[0]
            center = (x + w // 2, y + h // 2)
            radius = w // 2

            cv2.circle(frame, center, radius, (0, 255, 0), 2)
            draw_text(frame, f"Face detected. Hold still! Capturing in {countdown}s...", (10, 30))

            if countdown <= 0:
                face_embedding = extract_face_embedding(frame, faces[0])
                known_faces = fetch_all_embeddings()

                cap.release()
                cv2.destroyAllWindows()

                found_match = False
                for name, known_embedding in known_faces:
                    if verify_identity(face_embedding, known_embedding):
                        print(f"Identity verified! Welcome, {name}.")
                        found_match = True
                        break

                if not found_match:
                    print("Face not recognized in the system.")

                input("Press Enter to return to the main menu...")
                os.system('cls' if os.name == 'nt' else 'clear')
                break

        draw_text(frame, "Scanning for face...", (10, 30))
        draw_text(frame, f"Photo capture in {countdown}s...", (10, 60))
        cv2.imshow("Face Detection", frame)

        if cv2.waitKey(1) & 0xFF == ord('q') or countdown <= 0:
            break

    cap.release()
    cv2.destroyAllWindows()


def show_menu():
    print("\n" + "=" * 40)
    print("   Face Recognition Control Panel   ")
    print("=" * 40 + "\n")
    print("  1. Register new face via camera")
    print("  2. Verify identity via camera")
    print("  3. Exit")
    print("\n" + "=" * 40)
    return input("Select an option (1, 2, or 3): ").strip()


def main():
    while True:
        choice = show_menu()
        if choice == '1':
            register_new_face()
        elif choice == '2':
            verify_face_from_camera()
        elif choice == '3':
            print("Closing application. Goodbye!")
            break
        else:
            print("Invalid selection. Please enter 1, 2, or 3.")


if __name__ == '__main__':
    main()
