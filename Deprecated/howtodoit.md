Great! Here's how to **integrate `@admin_required`** with your existing authentication system using **Flask-Login** and your `User` model.

---

## ✅ Step-by-Step Integration

---

### 1. **Flask-Login Setup (Already Part of Your Project)**

You likely already have this part in place:

```python
from flask_login import LoginManager, UserMixin, login_user, current_user, login_required

login_manager = LoginManager(app)
login_manager.login_view = 'login'

@login_manager.user_loader
def load_user(user_id):
    return User.query.get(int(user_id))
```

✅ This allows `current_user` to be used globally (e.g., in `@admin_required`).

---

### 2. **Update `User` Model** (Add `is_admin` if not already there)

```python
class User(db.Model, UserMixin):
    id = db.Column(db.Integer, primary_key=True)
    name = db.Column(db.String(100))
    email = db.Column(db.String(120), unique=True)
    password = db.Column(db.String(128))
    is_admin = db.Column(db.Boolean, default=False)  # 👈 Admin flag
```

* Use `is_admin=True` for admin accounts.

---

### 3. **Create `@admin_required` Decorator**

Add this somewhere in your `utils.py` or top of your `routes.py`:

```python
from functools import wraps
from flask import redirect, url_for, flash
from flask_login import current_user

def admin_required(f):
    @wraps(f)
    def decorated_function(*args, **kwargs):
        if not current_user.is_authenticated:
            flash("Please log in first.", "warning")
            return redirect(url_for("login"))
        if not current_user.is_admin:
            flash("You do not have permission to access this page.", "danger")
            return redirect(url_for("dashboard"))  # or any non-admin page
        return f(*args, **kwargs)
    return decorated_function
```

---

### 4. **Protect Admin Routes**

Apply the decorator to your backup/restore routes:

```python
@app.route('/admin/backup')
@admin_required
def manual_backup():
    backup_data()
    flash('Manual backup completed successfully.')
    return redirect(url_for('admin_dashboard'))

@app.route('/admin/restore', methods=['GET', 'POST'])
@admin_required
def restore():
    ...
```

---

### 5. **Admin Login Flow**

When logging in users (e.g., in `POST /login` route), you can use:

```python
if user and check_password_hash(user.password, password):
    login_user(user)
    if user.is_admin:
        return redirect(url_for('admin_dashboard'))
    else:
        return redirect(url_for('user_dashboard'))
```

---

## 🧪 Testing Tips

* Mark one user in your database as admin:

```python
admin_user = User.query.filter_by(email='admin@example.com').first()
admin_user.is_admin = True
db.session.commit()
```

* Test:

  * Admin can access `/admin/backup`
  * Regular user is blocked with a flash message and redirected

---

