@echo off
REM Kiem tra ket noi Supabase / Redis / OpenWeather / OSM
cd /d "%~dp0..\.."
echo === Vietnam Online — Infrastructure Check ===
echo.

call tools\supabase\check_schema.bat
echo.

for /f "usebackq tokens=1,* delims==" %%a in (`findstr /B "REDIS_URL" .env`) do set REDIS_URL=%%b
echo Redis PING (can build vno_server de test day du)...
echo URL configured: %REDIS_URL:~0,30%...
echo.

for /f "usebackq tokens=1,* delims==" %%a in (`findstr /B "OPENWEATHERMAP_API_KEY" .env`) do set OWM=%%b
if "%OWM%"=="" (echo OpenWeatherMap: MISSING) else (echo OpenWeatherMap: configured)

for /f "usebackq tokens=1,* delims==" %%a in (`findstr /B "OSM_OVERPASS_URL" .env`) do set OSM=%%b
echo OSM Overpass: %OSM%

for /f "usebackq tokens=1,* delims==" %%a in (`findstr /B "CESIUM_ION_TOKEN" .env`) do set CES=%%b
if "%CES%"=="" (echo Cesium Ion: MISSING) else (echo Cesium Ion: configured — UE5 client)

echo.
echo Chay build\vno_server.exe de xem Redis + Weather + Supabase startup log.
