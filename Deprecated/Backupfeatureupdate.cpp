import os
import csv
import logging
from datetime import datetime
from flask import Flask, send_file, request, redirect, url_for, flash, render_template
from werkzeug.utils import secure_filename
from apscheduler.schedulers.background import BackgroundScheduler
from functools import wraps

# === Setup ===
app = Flask(__name__)
app.secret_key = 'your-secret-key'  # Needed for flashing messages

# Logging setup
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

# Database models assumed to be imported
# from models import db, User, Attendance

BACKUP_FOLDER = 'backups'
os.makedirs(BACKUP_FOLDER, exist_ok=True)

# === Admin Authentication Decorator ===
def admin_required(f):
    @wraps(f)
    def decorated_function(*args, **kwargs):
        # Replace with actual admin check logic
        if not request.cookies.get('is_admin') == 'true':
            flash('Admin access required.')
            return redirect(url_for('login'))
        return f(*args, **kwargs)
    return decorated_function

# === Backup Function ===
def backup_data():
    timestamp_str = datetime.now().strftime('%Y-%m-%d-%H%M%S')
    users_file = os.path.join(BACKUP_FOLDER, f'users-{timestamp_str}.csv')
    attendance_file = os.path.join(BACKUP_FOLDER, f'attendance-{timestamp_str}.csv')

    try:
        with open(users_file, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['id', 'name', 'email'])
            for user in User.query.all():
                writer.writerow([user.id, user.name, user.email])

        with open(attendance_file, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['id', 'user_id', 'date', 'status'])
            for record in Attendance.query.all():
                writer.writerow([record.id, record.user_id, record.date, record.status])

        logger.info(f"Backup created: {users_file}, {attendance_file}")
    except Exception as e:
        logger.error("Backup failed", exc_info=True)

# === Restore Function ===
def restore_data(users_csv=None, attendance_csv=None):
    try:
        if users_csv:
            with open(users_csv, 'r') as f:
                reader = csv.DictReader(f)
                User.query.delete()
                for row in reader:
                    db.session.add(User(id=int(row['id']), name=row['name'], email=row['email']))

        if attendance_csv:
            with open(attendance_csv, 'r') as f:
                reader = csv.DictReader(f)
                Attendance.query.delete()
                for row in reader:
                    db.session.add(Attendance(
                        id=int(row['id']),
                        user_id=int(row['user_id']),
                        date=row['date'],
                        status=row['status']
                    ))

        db.session.commit()
        logger.info("Restore completed successfully")
    except Exception as e:
        db.session.rollback()
        logger.error("Restore failed", exc_info=True)
        raise

# === Routes ===
@app.route('/admin/backup')
@admin_required
def manual_backup():
    backup_data()
    flash('Manual backup completed successfully.')
    return redirect(url_for('admin_dashboard'))

@app.route('/admin/restore', methods=['GET', 'POST'])
@admin_required
def restore():
    if request.method == 'POST':
        try:
            users_file = request.files.get('users_file')
            attendance_file = request.files.get('attendance_file')

            users_path = os.path.join(BACKUP_FOLDER, secure_filename(users_file.filename)) if users_file else None
            attendance_path = os.path.join(BACKUP_FOLDER, secure_filename(attendance_file.filename)) if attendance_file else None

            if users_file:
                users_file.save(users_path)
            if attendance_file:
                attendance_file.save(attendance_path)

            restore_data(users_path, attendance_path)
            flash('System restored from backup.')
        except Exception as e:
            flash(f'Restore failed: {str(e)}', 'error')
    return render_template('restore.html')

@app.route('/admin/download/users')
@admin_required
def download_users_csv():
    backup_data()
    latest_file = sorted([f for f in os.listdir(BACKUP_FOLDER) if f.startswith('users-')], reverse=True)[0]
    path = os.path.join(BACKUP_FOLDER, latest_file)
    return send_file(path, as_attachment=True)

@app.route('/admin/download/attendance')
@admin_required
def download_attendance_csv():
    backup_data()
    latest_file = sorted([f for f in os.listdir(BACKUP_FOLDER) if f.startswith('attendance-')], reverse=True)[0]
    path = os.path.join(BACKUP_FOLDER, latest_file)
    return send_file(path, as_attachment=True)

# === Scheduled Daily Backup ===
scheduler = BackgroundScheduler()
scheduler.add_job(backup_data, 'interval', days=1)
scheduler.start()

# === Admin Menu Integration ===
# HTML Template Snippet:
'''
<ul>
  <li><a href="{{ url_for('manual_backup') }}">Manual Backup</a></li>
  <li><a href="{{ url_for('restore') }}">Restore from Backup</a></li>
  <li><a href="{{ url_for('download_users_csv') }}">Download Users CSV</a></li>
  <li><a href="{{ url_for('download_attendance_csv') }}">Download Attendance CSV</a></li>
</ul>
'''
