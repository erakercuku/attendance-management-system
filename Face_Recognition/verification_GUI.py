import tkinter as tk
from tkinter import ttk, messagebox
import cv2
import mysql.connector
from datetime import datetime
from app import verify_face_from_camera as vf

 Real-time QR code scanning that returns the decoded user_id or None
def scan_qr_code():
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("Unable to access the webcam.")
        return None

    detector = cv2.QRCodeDetector()
    userid = None

    while True:
        ret, frame = cap.read()
        if not ret:
            print("Camera read failure.")
            break

        data, bbox, _ = detector.detectAndDecode(frame)
        if bbox is not None and data:
            try:
                userid = data.strip()
            except ValueError:
                userid = data.strip()
            break

        cv2.imshow("Scan QR Code", frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()
    return userid

class AttendanceApp(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Attendance Checker")
        self.geometry("400x350")
        self.resizable(False, False)
        style = ttk.Style(self)
        try:
            style.theme_use('clam')
        except:
            pass
         DB connection
        self.conn = mysql.connector.connect(
            host='localhost', user='root', password='root', database='mydb'
        )
         fetch modules
        cur = self.conn.cursor()
        cur.execute("SELECT module_id FROM Modules")
        self.modules = [row[0] for row in cur.fetchall()]
        cur.close()

        self.main_frame = ttk.Frame(self, padding=20)
        self.main_frame.pack(expand=True, fill="both")
        self.create_main_menu()

    def create_main_menu(self):
        for w in self.main_frame.winfo_children():
            w.destroy()
         Module selection
        ttk.Label(self.main_frame, text="Module:").pack(pady=(0,5))
        self.mod_var = tk.StringVar()
        cb = ttk.Combobox(self.main_frame, textvariable=self.mod_var, values=self.modules, state='readonly')
        cb.pack(pady=(0,15))
        if self.modules:
            cb.current(0)
         Buttons
        ttk.Button(self.main_frame, text="Check in via Face", command=self.handle_face, width=20).pack(pady=5)
        ttk.Button(self.main_frame, text="Scan QR Code", command=self.handle_qr, width=20).pack(pady=5)

    def handle_face(self):
         returns user_id or None
        user_id = vf()
        if user_id:
            self.log_action(user_id, self.mod_var.get(), 'CHECKIN')
            self.show_message(True)
        else:
            self.show_message(False)

    def handle_qr(self):
        user_id = scan_qr_code()
        if user_id:
            self.log_action(user_id, self.mod_var.get(), 'CHECKIN')
            self.show_message(True)
        else:
            self.show_message(False)

    def log_action(self, user_id, module_id, action):
        cur = self.conn.cursor()
        now = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        try:
            cur.execute(
                "INSERT INTO UserActions (id, module_id, action, time, entry_type) VALUES (%s, %s, %s, %s, 'automatic')",
                (user_id, module_id, action, now)
            )
            self.conn.commit()
        except mysql.connector.Error as e:
            messagebox.showerror("DB Error", str(e))
        finally:
            cur.close()

    def show_message(self, success):
        for w in self.main_frame.winfo_children():
            w.destroy()
        if success:
            ttk.Label(self.main_frame, text="Hey, attendance checked", font=("Helvetica", 14)).pack(pady=20)
            ttk.Button(self.main_frame, text="OK", command=self.create_main_menu).pack()
        else:
            ttk.Label(self.main_frame, text="Check-in failed", foreground="red", font=("Helvetica", 14)).pack(pady=20)
            ttk.Button(self.main_frame, text="Retry", command=self.create_main_menu).pack()

if __name__ == "__main__":
    app = AttendanceApp()
    app.mainloop()
