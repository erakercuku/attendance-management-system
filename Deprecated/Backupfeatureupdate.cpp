import os
import csv
from datetime import datetime
from flask import Flask, send_file, request, redirect, url_for, flash, render_template
from werkzeug.utils import secure_filename
from apscheduler.schedulers.background import BackgroundScheduler

# Assume 'db' is the database session and 'User' and 'Attendance' are the models
# Assume 'app' is your Flask app

BACKUP_FOLDER = 'backups'
os.makedirs(BACKUP_FOLDER, exist_ok=True)

# === Backup Function ===
def backup_data():
    today_str = datetime.now().strftime('%Y-%m-%d')

    users_file = os.path.join(BACKUP_FOLDER, f'users-{today_str}.csv')
    attendance_file = os.path.join(BACKUP_FOLDER, f'attendance-{today_str}.csv')

    # Export users
    with open(users_file, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['id', 'name', 'email'])  # Adjust columns as per model
        for user in User.query.all():
            writer.writerow([user.id, user.name, user.email])

    # Export attendance
    with open(attendance_file, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['id', 'user_id', 'date', 'status'])
        for record in Attendance.query.all():
            writer.writerow([record.id, record.user_id, record.date, record.status])

# === Restore Function ===
def restore_data(users_csv=None, attendance_csv=None):
    if users_csv:
        with open(users_csv, 'r') as f:
            reader = csv.DictReader(f)
            User.query.delete()
            for row in reader:
                db.session.add(User(**row))

    if attendance_csv:
        with open(attendance_csv, 'r') as f:
            reader = csv.DictReader(f)
            Attendance.query.delete()
            for row in reader:
                db.session.add(Attendance(**row))

    db.session.commit()

# === Routes ===
@app.route('/admin/backup')
def manual_backup():
    backup_data()
    flash('Manual backup completed successfully.')
    return redirect(url_for('admin_dashboard'))

@app.route('/admin/restore', methods=['GET', 'POST'])
def restore():
    if request.method == 'POST':
        users_file = request.files.get('users_file')
        attendance_file = request.files.get('attendance_file')

        if users_file:
            users_path = os.path.join(BACKUP_FOLDER, secure_filename(users_file.filename))
            users_file.save(users_path)
        else:
            users_path = None

        if attendance_file:
            attendance_path = os.path.join(BACKUP_FOLDER, secure_filename(attendance_file.filename))
            attendance_file.save(attendance_path)
        else:
            attendance_path = None

        restore_data(users_path, attendance_path)
        flash('System restored from backup.')
        return redirect(url_for('admin_dashboard'))

    return render_template('restore.html')

@app.route('/admin/download/users')
def download_users_csv():
    today_str = datetime.now().strftime('%Y-%m-%d')
    path = os.path.join(BACKUP_FOLDER, f'users-{today_str}.csv')
    if not os.path.exists(path):
        backup_data()  # Ensure latest backup exists
    return send_file(path, as_attachment=True)

@app.route('/admin/download/attendance')
def download_attendance_csv():
    today_str = datetime.now().strftime('%Y-%m-%d')
    path = os.path.join(BACKUP_FOLDER, f'attendance-{today_str}.csv')
    if not os.path.exists(path):
        backup_data()  # Ensure latest backup exists
    return send_file(path, as_attachment=True)

# === Scheduled Daily Backup ===
scheduler = BackgroundScheduler()
scheduler.add_job(backup_data, 'interval', days=1)
scheduler.start()

# === Menu Integration (example snippet to add to templates) ===
# Insert the following HTML snippet in the admin and manager dashboard templates (e.g., dashboard.html):

'''
<ul>
  <li><a href="{{ url_for('manual_backup') }}">Manual Backup</a></li>
  <li><a href="{{ url_for('restore') }}">Restore from Backup</a></li>
  <li><a href="{{ url_for('download_users_csv') }}">Download Users CSV</a></li>
  <li><a href="{{ url_for('download_attendance_csv') }}">Download Attendance CSV</a></li>
</ul>
'''

# Place this in a block where admin or manager-specific actions are listed.
