@echo off
REM Kiem tra bang characters tren Supabase (can curl)
cd /d "%~dp0..\.."
for /f "usebackq tokens=1,* delims==" %%a in (`findstr /B "SUPABASE_URL SUPABASE_ANON_KEY" .env`) do (
    if "%%a"=="SUPABASE_URL" set SUPABASE_URL=%%b
    if "%%a"=="SUPABASE_ANON_KEY" set SUPABASE_ANON_KEY=%%b
)
if "%SUPABASE_URL%"=="" (
    echo Khong tim thay SUPABASE_URL trong .env
    exit /b 1
)
echo Kiem tra Supabase REST /characters ...
curl -s -o NUL -w "HTTP %%{http_code}\n" ^
  -H "apikey: %SUPABASE_ANON_KEY%" ^
  -H "Authorization: Bearer %SUPABASE_ANON_KEY%" ^
  "%SUPABASE_URL%/rest/v1/characters?select=user_id&limit=1"
echo.
echo Neu HTTP 200: bang da ton tai.
echo Neu HTTP 404/relation: chay infra/database/schema_supabase.sql trong Supabase SQL Editor.
