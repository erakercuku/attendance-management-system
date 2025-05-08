import cv2
import numpy as np
import pandas as pd
from pyzbar.pyzbar import decode
from PIL import Image
import qrcode
import random
import string


# def generate_random_id(length=16):
#     characters = string.ascii_uppercase + string.digits
#     return ''.join(random.choices(characters, k=length))

def get_id_from_csv():
    try:
        with open("hashed_id.csv", "r") as f:
            id = f.readline().strip()
            if len(id) != 16:
                raise ValueError("Invalid ID length")
            return id
    except Exception as e:
        print("Something has gone wrong. Could not read hashed ID from CSV.")
        raise SystemExit(1)

def generate_qr(name, surname, course, position):
    # random_id = generate_random_id()
    random_id = get_id_from_csv()
    data = f"{random_id},{name},{surname},{course},{position}"
    qr = qrcode.QRCode(
        version=1,
        error_correction=qrcode.constants.ERROR_CORRECT_H,
        box_size=10,
        border=4,
    )
    qr.add_data(data)
    qr.make(fit=True)
    img = qr.make_image(fill_color="blue", back_color="white")
    return img, random_id


def scan_qr_from_image(image_path):
    try:
        img = Image.open(image_path)
    except Exception as e:
        print("Error opening image:", e)
        return None
    decoded_objects = decode(img)
    if not decoded_objects:
        print("No QR code found in the image.")
        return None
    data = decoded_objects[0].data.decode("utf-8")
    return data


def scan_qr_from_camera():
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("Error: Could not open camera.")
        return

    print("Starting real-time QR scanning. Press 'q' to quit.")

    valid_qr_found = False

    while True:
        ret, frame = cap.read()
        if not ret:
            print("Error: Failed to capture frame.")
            break

        decoded_objects = decode(frame)
        for obj in decoded_objects:
            points = obj.polygon
            if len(points) > 4:
                hull = cv2.convexHull(np.array([point for point in points], dtype=np.float32))
                hull = list(map(tuple, np.squeeze(hull)))
            else:
                hull = points

            n = len(hull)
            for j in range(n):
                cv2.line(frame, hull[j], hull[(j + 1) % n], (255, 0, 0), 3)

            qr_data = obj.data.decode("utf-8")
            parts = qr_data.split(",")

            if len(parts) == 5 and len(parts[0]) == 16:
                df = pd.DataFrame([{
                    'ID': parts[0],
                    'Name': parts[1],
                    'Surname': parts[2],
                    'Course/Department': parts[3],
                    'Position/Year': parts[4]
                }])
                print("Valid QR Code detected:")
                print(df)

                text = "Valid QR Code Detected"
                cv2.putText(frame, text, (obj.rect.left, obj.rect.top - 10),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)
                cv2.imshow("Real-Time QR Scanner", frame)
                cv2.waitKey(1000)  # Display for a short moment
                valid_qr_found = True
                break
            else:
                text = "Invalid QR Code"
                cv2.putText(frame, text, (obj.rect.left, obj.rect.top - 10),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 2)
                print("Invalid QR Code:", qr_data)

        cv2.imshow("Real-Time QR Scanner", frame)

        if valid_qr_found:
            break

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()


def main():
    print("Select an option:")
    print("1: Generate QR code")
    print("2: Scan QR code from image")
    print("3: Real-time QR scanning from camera")
    choice = input("Enter your choice (1, 2, or 3): ").strip()

    if choice == "1":
        name = input("Enter Name: ").strip()
        surname = input("Enter Surname: ").strip()
        course = input("Enter Course/Department: ").strip()
        position = input("Enter Position/Year of Study: ").strip()
        qr_image, random_id = generate_qr(name, surname, course, position)
        print(f"Registration ID: {random_id}")
        filename = input("Enter filename to save the QR code (e.g., qr.png): ").strip()
        filename = filename.lstrip("\u202a")
        try:
            qr_image.save(filename)
            print(f"QR code generated and saved to {filename}.")
        except Exception as e:
            print(f"Error saving QR code: {e}")

    elif choice == "2":
        image_path = input("Enter the path of the QR code image: ").strip()
        data = scan_qr_from_image(image_path)
        if data:
            parts = data.split(",")
            if len(parts) == 5:
                print("\nScanned QR Code details:")
                print("ID:                 ", parts[0])
                print("Name:               ", parts[1])
                print("Surname:            ", parts[2])
                print("Course/Department:  ", parts[3])
                print("Position/Year:      ", parts[4])
            else:
                print("Unexpected QR code format.")
                print("Data:", data)

    elif choice == "3":
        scan_qr_from_camera()

    else:
        print("Invalid choice. Please restart and choose a valid option.")


if __name__ == "__main__":
    main()
