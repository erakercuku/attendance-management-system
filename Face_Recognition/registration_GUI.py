import tkinter as tk
from tkinter import ttk, messagebox
import cv2
import time
import os
import csv
import qrcode
import random
from PIL import Image, ImageDraw
from face_utils import locate_faces, extract_face_embedding
from db_utils import save_embedding_to_db

 Utility to draw text on OpenCV frames
def draw_text(frame, text, position, font_scale=0.7, thickness=2):
    cv2.putText(frame, text, position, cv2.FONT_HERSHEY_SIMPLEX, font_scale, (255, 255, 255), thickness)

 Capture and register face embedding with GUI-collected data
def register_face_gui(user_id, person_name):
    try:
        cap = cv2.VideoCapture(0)
        if not cap.isOpened():
            messagebox.showerror("Error", "Unable to access the webcam.")
            return False

        cv2.namedWindow("Face Detection", cv2.WINDOW_NORMAL)
        cv2.setWindowProperty("Face Detection", cv2.WND_PROP_TOPMOST, 1)

        start_time = time.time()
        captured_emb = None

        while True:
            ret, frame = cap.read()
            if not ret:
                print("Camera read failure.")
                break

            faces = locate_faces(frame)
            elapsed = int(time.time() - start_time)
            countdown = max(0, 5 - elapsed)

            if len(faces) > 0:
                x, y, w, h = faces[0]
                center = (x + w // 2, y + h // 2)
                radius = w // 2
                cv2.circle(frame, center, radius, (0, 255, 0), 2)
                draw_text(frame, f"Capturing in {countdown}s...", (10, 30))
                if countdown <= 0:
                    captured_emb = extract_face_embedding(frame, faces[0])
                    break
            else:
                draw_text(frame, "Scanning for face...", (10, 30))
                draw_text(frame, f"Photo capture in {countdown}s...", (10, 60))

            cv2.imshow("Face Detection", frame)
            if cv2.waitKey(1) & 0xFF == ord('q') or countdown <= 0:
                break

        cap.release()
        cv2.destroyAllWindows()

        if captured_emb is None:
            messagebox.showerror("Failed", "Face capture failed.")
            return False

        save_embedding_to_db(user_id, person_name, captured_emb)
        return True

    except Exception as e:
        messagebox.showerror("Error", f"An unexpected error occurred: {e}")
        return False

 Generate a uniquely styled QR code with high error correction and noise
def create_qr_code(user_id, full_name):
    data = str(user_id)
    qr = qrcode.QRCode(
        error_correction=qrcode.constants.ERROR_CORRECT_H,
        box_size=10,
        border=4
    )
    qr.add_data(data)
    qr.make(fit=True)
    img = qr.make_image(fill_color="black", back_color="white").convert("RGB")

    draw = ImageDraw.Draw(img)
    w, h = img.size
    border_thickness = 20
    draw.rectangle([0, 0, w-1, h-1], outline=(0, 128, 255), width=border_thickness)
    for _ in range(1000):
        x = random.randint(border_thickness, w - border_thickness - 1)
        y = random.randint(border_thickness, h - border_thickness - 1)
        draw.point((x, y), fill=(0, 128, 255))

    output_path = f"qr_{user_id}.png"
    img.save(output_path)
    return output_path

 Append user details to CSV file
def save_user_info_to_csv(name, surname, course_position, password, csv_path='registrations.csv'):
    file_exists = os.path.isfile(csv_path)
    with open(csv_path, 'a', newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        if not file_exists:
            writer.writerow(['Name', 'Surname', 'Course/Position', 'Password'])
        writer.writerow([name, surname, course_position, password])

 Append credentials to CSV file
def save_credentials_to_csv(login, name, surname, course_position, password, csv_path='credentials.csv'):
    file_exists = os.path.isfile(csv_path)
    with open(csv_path, 'a', newline='', encoding='utf-8') as f:
        writer = csv.writer(f)
        if not file_exists:
            writer.writerow(['Login', 'Name', 'Surname', 'Course/Position', 'Password'])
        writer.writerow([login, name, surname, course_position, password])

class RegistrationApp(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("User Registration")
        self.geometry("400x500")
        self.resizable(False, False)
        style = ttk.Style(self)
        try:
            style.theme_use('clam')
        except:
            pass
        self.frame = ttk.Frame(self, padding=20)
        self.frame.pack(expand=True, fill="both")
        self.build_form()

    def build_form(self):
        for w in self.frame.winfo_children():
            w.destroy()
         Login
        ttk.Label(self.frame, text="Login:").grid(row=0, column=0, sticky="w", pady=5)
        self.login_var = tk.StringVar()
        ttk.Entry(self.frame, textvariable=self.login_var).grid(row=0, column=1, pady=5)
         Role
        ttk.Label(self.frame, text="Role:").grid(row=1, column=0, sticky="w", pady=5)
        self.role_var = tk.StringVar()
        ttk.Entry(self.frame, textvariable=self.role_var).grid(row=1, column=1, pady=5)
         Course/Program
        ttk.Label(self.frame, text="Course/Program:").grid(row=2, column=0, sticky="w", pady=5)
        self.course_var = tk.StringVar()
        ttk.Entry(self.frame, textvariable=self.course_var).grid(row=2, column=1, pady=5)
         First Name
        ttk.Label(self.frame, text="First Name:").grid(row=3, column=0, sticky="w", pady=5)
        self.name_var = tk.StringVar()
        ttk.Entry(self.frame, textvariable=self.name_var).grid(row=3, column=1, pady=5)
         Surname
        ttk.Label(self.frame, text="Surname:").grid(row=4, column=0, sticky="w", pady=5)
        self.surname_var = tk.StringVar()
        ttk.Entry(self.frame, textvariable=self.surname_var).grid(row=4, column=1, pady=5)
         Password
        ttk.Label(self.frame, text="Password:").grid(row=5, column=0, sticky="w", pady=5)
        self.password_var = tk.StringVar()
        ttk.Entry(self.frame, textvariable=self.password_var, show="").grid(row=5, column=1, pady=5)
         Register button
        register_btn = ttk.Button(self.frame, text="Start Registration", command=self.handle_register)
        register_btn.grid(row=6, column=0, columnspan=2, pady=20, ipadx=10)

    def handle_register(self):
        login = self.login_var.get().strip()
        role = self.role_var.get().strip()
        course = self.course_var.get().strip()
        name = self.name_var.get().strip()
        surname = self.surname_var.get().strip()
        password = self.password_var.get().strip()
        if not all([login, role, course, name, surname, password]):
            messagebox.showwarning("Missing Info", "Please fill in all fields.")
            return
         Save credentials first
        save_credentials_to_csv(login, name, surname, f"{role}, {course}", password)
         Wait for id.txt
        start = time.time()
        user_id = None
        while True:
            if time.time() - start > 20:
                messagebox.showerror("Timeout", "ID not found in time. Please retry.")
                return
             Read id.txt
            try:
                with open('id.txt', 'r') as f:
                    content = f.read().strip()
            except FileNotFoundError:
                content = ''
            if not content:
                time.sleep(1)
                continue
            if content.startswith('ERROR'):
                messagebox.showerror("ID Error", content)
                return
             valid id
            user_id = content
            break
         Proceed with face registration
        full_name = f"{name} {surname} ({role}, {course})"
        success = register_face_gui(user_id, full_name)
        if success:
            qr_path = create_qr_code(user_id, full_name)
            messagebox.showinfo("Success", f"Registration complete. QR saved to {qr_path}")
            self.build_form()
        else:
            messagebox.showerror("Error", "Registration failed. Please try again.")

if __name__ == "__main__":
    app = RegistrationApp()
    app.mainloop()