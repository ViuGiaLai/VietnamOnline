import sys
import os
import urllib.parse
import pg8000.dbapi

def load_env(filepath):
    env = {}
    if not os.path.exists(filepath):
        return env
    with open(filepath, "r", encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            if "=" in line:
                key, val = line.split("=", 1)
                env[key.strip()] = val.strip()
    return env

def main():
    print("=== Vietnam Online - Apply Supabase Schema ===")
    env = load_env(".env")
    db_url = env.get("DATABASE_URL")
    if not db_url:
        print("Error: DATABASE_URL not found in .env file.")
        sys.exit(1)
    
    # Parse connection string
    # Format: postgresql://username:password@host:port/database
    try:
        parsed = urllib.parse.urlparse(db_url)
        username = parsed.username
        password = urllib.parse.unquote(parsed.password) if parsed.password else None
        host = parsed.hostname
        port = parsed.port or 5432
        database = parsed.path.lstrip('/')
    except Exception as e:
        print(f"Error parsing DATABASE_URL: {e}")
        sys.exit(1)
        
    print(f"Connecting to database {host}:{port}/{database} as {username}...")
    
    try:
        conn = pg8000.dbapi.connect(
            user=username,
            password=password,
            host=host,
            port=port,
            database=database
        )
        conn.autocommit = True
        cursor = conn.cursor()
        print("Connected successfully!")
    except Exception as e:
        print(f"Connection failed: {e}")
        sys.exit(1)
        
    schema_file = "infra/database/schema_supabase.sql"
    if not os.path.exists(schema_file):
        print(f"Error: Schema file not found at {schema_file}")
        sys.exit(1)
        
    print(f"Reading {schema_file}...")
    with open(schema_file, "r", encoding="utf-8") as f:
        sql = f.read()
        
    print("Executing SQL statements...")
    try:
        cursor.execute(sql)
        print("Schema applied successfully!")
    except Exception as e:
        print(f"Failed to apply schema: {e}")
        sys.exit(1)
    finally:
        cursor.close()
        conn.close()

if __name__ == "__main__":
    main()
