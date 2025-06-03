import tkinter as tk
from tkinter import ttk, messagebox
from tkcalendar import DateEntry
from datetime import datetime, time
import mysql.connector

class ModulesApp(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("Module Management")
        self.geometry("850x600")
        style = ttk.Style(self)
        try:
            style.theme_use('clam')
        except:
            pass
         Database connection
        self.conn = mysql.connector.connect(
            host='localhost',
            user='root',
            password='root',
            database='mydb'
        )
        self._build_ui()

    def _build_ui(self):
        notebook = ttk.Notebook(self)
        notebook.pack(expand=True, fill='both')

        modules_tab = ttk.Frame(notebook)
        schedule_tab = ttk.Frame(notebook)
        enroll_tab = ttk.Frame(notebook)

        notebook.add(modules_tab, text='Modules')
        notebook.add(schedule_tab, text='Schedule')
        notebook.add(enroll_tab, text='Enrollment')

        self._build_modules_tab(modules_tab)
        self._build_schedule_tab(schedule_tab)
        self._build_enroll_tab(enroll_tab)

    def _build_modules_tab(self, parent):
        frame = ttk.Frame(parent, padding=10)
        frame.pack(fill='x')
        ttk.Label(frame, text='Module ID:').grid(row=0, column=0, sticky='w')
        self.mod_id_var = tk.StringVar()
        ttk.Entry(frame, textvariable=self.mod_id_var).grid(row=0, column=1, sticky='w')
        ttk.Label(frame, text='Name:').grid(row=1, column=0, sticky='w')
        self.mod_name_var = tk.StringVar()
        ttk.Entry(frame, textvariable=self.mod_name_var).grid(row=1, column=1, sticky='w')
        ttk.Label(frame, text='Type:').grid(row=2, column=0, sticky='w')
        self.mod_type_var = tk.StringVar(value='Course')
        ttk.Combobox(frame, textvariable=self.mod_type_var,
                     values=['Course', 'Work'], state='readonly').grid(row=2, column=1, sticky='w')
        ttk.Button(frame, text='Add Module', command=self.add_module).grid(row=3, column=0, columnspan=2, pady=5)
        ttk.Button(frame, text='Show Modules', command=self.show_modules).grid(row=4, column=0, columnspan=2, pady=5)

        cols = ['module_id', 'module_name', 'module_type']
        self.mod_tree = ttk.Treeview(parent, columns=cols, show='headings')
        for col in cols:
            self.mod_tree.heading(col, text=col)
        self.mod_tree.pack(expand=True, fill='both', padx=10, pady=10)

    def _build_schedule_tab(self, parent):
        frame = ttk.Frame(parent, padding=10)
        frame.pack(fill='x')
        ttk.Label(frame, text='Schedule ID:').grid(row=0, column=0, sticky='w')
        self.sch_id_var = tk.StringVar()
        ttk.Entry(frame, textvariable=self.sch_id_var).grid(row=0, column=1, sticky='w')
        ttk.Label(frame, text='Module:').grid(row=1, column=0, sticky='w')
        self.sch_mod_var = tk.StringVar()
        self.sch_mod_cb = ttk.Combobox(frame, textvariable=self.sch_mod_var, state='readonly')
        self.sch_mod_cb.grid(row=1, column=1, sticky='w')
        ttk.Button(frame, text='Refresh Modules', command=self.populate_mod_combobox).grid(row=1, column=2, padx=5)
         Date & time
        ttk.Label(frame, text='Start Date:').grid(row=2, column=0, sticky='w')
        self.start_date = DateEntry(frame, width=12)
        self.start_date.grid(row=2, column=1, sticky='w')
        ttk.Label(frame, text='Start Time:').grid(row=2, column=2, sticky='w')
        self.start_hour = tk.Spinbox(frame, from_=0, to=23, width=3, format='%02.0f')
        self.start_minute = tk.Spinbox(frame, from_=0, to=59, width=3, format='%02.0f')
        self.start_hour.grid(row=2, column=3)
        ttk.Label(frame, text=':').grid(row=2, column=4)
        self.start_minute.grid(row=2, column=5)

        ttk.Label(frame, text='End Date:').grid(row=3, column=0, sticky='w')
        self.end_date = DateEntry(frame, width=12)
        self.end_date.grid(row=3, column=1, sticky='w')
        ttk.Label(frame, text='End Time:').grid(row=3, column=2, sticky='w')
        self.end_hour = tk.Spinbox(frame, from_=0, to=23, width=3, format='%02.0f')
        self.end_minute = tk.Spinbox(frame, from_=0, to=59, width=3, format='%02.0f')
        self.end_hour.grid(row=3, column=3)
        ttk.Label(frame, text=':').grid(row=3, column=4)
        self.end_minute.grid(row=3, column=5)

        ttk.Button(frame, text='Add Session', command=self.add_session).grid(row=4, column=0, columnspan=6, pady=5)
        ttk.Button(frame, text='Show Schedule', command=self.show_schedule).grid(row=5, column=0, columnspan=6, pady=5)

        cols = ['schedule_id', 'module_id', 'start_time', 'end_time']
        self.sch_tree = ttk.Treeview(parent, columns=cols, show='headings')
        for col in cols:
            self.sch_tree.heading(col, text=col)
        self.sch_tree.pack(expand=True, fill='both', padx=10, pady=10)

    def _build_enroll_tab(self, parent):
        frame = ttk.Frame(parent, padding=10)
        frame.pack(fill='x')
        ttk.Label(frame, text='User ID:').grid(row=0, column=0, sticky='w')
        self.en_u_var = tk.StringVar()
        ttk.Entry(frame, textvariable=self.en_u_var).grid(row=0, column=1, sticky='w')
        ttk.Label(frame, text='Module:').grid(row=1, column=0, sticky='w')
        self.en_m_var = tk.StringVar()
        self.en_m_cb = ttk.Combobox(frame, textvariable=self.en_m_var, state='readonly')
        self.en_m_cb.grid(row=1, column=1, sticky='w')
        ttk.Button(frame, text='Refresh Modules', command=self.populate_mod_combobox).grid(row=1, column=2, padx=5)
        ttk.Button(frame, text='Enroll', command=self.enroll_user).grid(row=2, column=0, columnspan=2, pady=5)
        ttk.Button(frame, text='Show Enrollments', command=self.show_enrollments).grid(row=3, column=0, columnspan=2, pady=5)

        cols = ['id', 'module_id']
        self.en_tree = ttk.Treeview(parent, columns=cols, show='headings')
        for col in cols:
            self.en_tree.heading(col, text=col)
        self.en_tree.pack(expand=True, fill='both', padx=10, pady=10)
         Initialize module lists
        self.populate_mod_combobox()

    def populate_mod_combobox(self):
        cursor = self.conn.cursor()
        cursor.execute("SELECT module_id FROM Modules")
        mods = [row[0] for row in cursor.fetchall()]
        cursor.close()
        self.sch_mod_cb['values'] = mods
        self.en_m_cb['values'] = mods

    def add_module(self):
        mid = self.mod_id_var.get().strip()
        name = self.mod_name_var.get().strip()
        mtype = self.mod_type_var.get()
        if not mid or not name:
            messagebox.showwarning("Warning", "Module ID and Name required.")
            return
        cursor = self.conn.cursor()
        try:
            cursor.execute(
                "INSERT INTO Modules (module_id, module_name, module_type) VALUES (%s, %s, %s)",
                (mid, name, mtype)
            )
            self.conn.commit()
            messagebox.showinfo("Added", f"Module {mid} added.")
        except mysql.connector.Error as e:
            messagebox.showerror("Error", f"Database error: {e}")
        finally:
            cursor.close()

    def show_modules(self):
        cursor = self.conn.cursor()
        cursor.execute("SELECT module_id,module_name,module_type FROM Modules")
        rows = cursor.fetchall()
        cursor.close()
        for i in self.mod_tree.get_children():
            self.mod_tree.delete(i)
        for row in rows:
            self.mod_tree.insert('', 'end', values=row)

    def add_session(self):
        sid = self.sch_id_var.get().strip()
        mid = self.sch_mod_var.get().strip()
        start_date = self.start_date.get_date()
        end_date = self.end_date.get_date()
        try:
            start_time = time(int(self.start_hour.get()), int(self.start_minute.get()))
            end_time = time(int(self.end_hour.get()), int(self.end_minute.get()))
        except ValueError:
            messagebox.showerror("Error", "Invalid hour/minute values.")
            return
        start_dt = datetime.combine(start_date, start_time)
        end_dt = datetime.combine(end_date, end_time)
        if not sid or not mid:
            messagebox.showwarning("Warning", "Schedule ID and Module required.")
            return
        cursor = self.conn.cursor()
        try:
            cursor.execute(
                "INSERT INTO ModuleSchedule (schedule_id, module_id, start_time, end_time) VALUES (%s, %s, %s, %s)",
                (sid, mid, start_dt, end_dt)
            )
            self.conn.commit()
            messagebox.showinfo("Added", f"Session {sid} added.")
        except mysql.connector.Error as e:
            messagebox.showerror("Error", f"Database error: {e}")
        finally:
            cursor.close()

    def show_schedule(self):
        cursor = self.conn.cursor()
        cursor.execute("SELECT schedule_id, module_id, start_time, end_time FROM ModuleSchedule")
        rows = cursor.fetchall()
        cursor.close()
        for i in self.sch_tree.get_children():
            self.sch_tree.delete(i)
        for row in rows:
            self.sch_tree.insert('', 'end', values=row)

    def enroll_user(self):
        uid = self.en_u_var.get().strip()
        mid = self.en_m_var.get().strip()
        if not uid or not mid:
            messagebox.showwarning("Warning", "User ID and Module required.")
            return
        cursor = self.conn.cursor()
        try:
            cursor.execute(
                "INSERT INTO UserModules (id, module_id) VALUES (%s, %s)",
                (uid, mid)
            )
            self.conn.commit()
            messagebox.showinfo("Enrolled", f"User {uid} enrolled in {mid}.")
        except mysql.connector.Error as e:
            messagebox.showerror("Error", f"Database error: {e}")
        finally:
            cursor.close()

    def show_enrollments(self):
        cursor = self.conn.cursor()
        cursor.execute("SELECT id, module_id FROM UserModules")
        rows = cursor.fetchall()
        cursor.close()
        for i in self.en_tree.get_children():
            self.en_tree.delete(i)
        for row in rows:
            self.en_tree.insert('', 'end', values=row)

if __name__ == '__main__':
    app = ModulesApp()
    app.mainloop()