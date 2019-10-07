@ECHO OFF
TASKLIST|FIND "CenterServer.exe"
IF ERRORLEVEL 1 (
ECHO "not find and start..."
) ELSE (
ECHO "find and kill..."
TASKKILL /F /IM CenterServer.exe /T
)

TASKLIST|FIND "LogicServer.exe"
IF ERRORLEVEL 1 (
ECHO "not find and start..."
) ELSE (
ECHO "find and kill..."
TASKKILL /F /IM LogicServer.exe /T
)

TASKLIST|FIND "LogicDB.exe"
IF ERRORLEVEL 1 (
ECHO "not find and start..."
) ELSE (
ECHO "find and kill..."
TASKKILL /F /IM LogicDB.exe /T
)

TASKLIST|FIND "GateServer.exe"
IF ERRORLEVEL 1 (
ECHO "not find and start..."
) ELSE (
ECHO "find and kill..."
TASKKILL /F /IM GateServer.exe /T
)

TASKLIST|FIND "LoginServer.exe"
IF ERRORLEVEL 1 (
ECHO "not find and start..."
) ELSE (
ECHO "find and kill..."
TASKKILL /F /IM LoginServer.exe /T
)

ECHO "kill statr..."