@echo off
setlocal
cd /d "%~dp0"

echo ==========================================
echo  Vietnam Online - Build Monorepo
echo ==========================================

if not exist build mkdir build
if not exist data\saves mkdir data\saves

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo Error: MSVC not found. Install Visual Studio Build Tools.
    pause
    exit /b 1
)

set INC=/Ishared\include /Iclient\prototype\include /Iserver\common\include /Iserver\gateway\include /Iserver\auth-service\include /Iserver\profile-service\include /Iserver\world-service\include /Iserver\movement-sync-service\include /Iserver\chat-service\include /Iserver\poi-service\include /Iserver\quest-service\include /Iserver\vehicle-service\include
set CXXFLAGS=/EHsc /std:c++17 %INC%

echo [1/2] Building prototype client...
cl.exe %CXXFLAGS% client\prototype\src\main.cpp client\prototype\src\GameEngine.cpp client\prototype\src\AuthManager.cpp shared\src\EnvConfig.cpp shared\src\HttpClient.cpp shared\src\SupabaseClient.cpp shared\src\RedisClient.cpp shared\src\WeatherService.cpp shared\src\OsmClient.cpp /Febuild\vno_prototype.exe
if %ERRORLEVEL% neq 0 goto :fail

echo [2/2] Building MVP server...
cl.exe %CXXFLAGS% server\src\main.cpp server\common\src\ServiceRegistry.cpp shared\src\EnvConfig.cpp shared\src\HttpClient.cpp shared\src\SupabaseClient.cpp shared\src\RedisClient.cpp shared\src\WeatherService.cpp shared\src\OsmClient.cpp ^
    server\gateway\src\GatewayService.cpp server\gateway\src\WebSocketServer.cpp server\gateway\src\SessionManager.cpp server\gateway\src\ClientSession.cpp server\auth-service\src\AuthService.cpp ^
    server\profile-service\src\ProfileService.cpp server\world-service\src\WorldService.cpp ^
    server\movement-sync-service\src\MovementSyncService.cpp server\chat-service\src\ChatService.cpp ^
    server\poi-service\src\PoiService.cpp server\quest-service\src\QuestService.cpp ^
    server\vehicle-service\src\VehicleService.cpp /Febuild\vno_server.exe

if %ERRORLEVEL% neq 0 goto :fail

echo.
echo Build OK: build\vno_prototype.exe, build\vno_server.exe
echo.
set /p RUN="Run prototype now? (y/n): "
if /i "%RUN%"=="y" build\vno_prototype.exe
goto :eof

:fail
echo Build failed!
pause
exit /b 1
