@echo off
setlocal enabledelayedexpansion

:: Find client directory
set clientdir=%APPDATA%\.minecraft
echo Client Directory: !clientdir!

:: Find program directory (zipping tool)
set program=C:\Program Files\7-Zip\7z.exe
echo Program Directory: !program!

:: Find .exe(s)
set minecraft=..\Minecraft.lnk
set instanceName=javaw.exe

:: Start Minecraft Launcher
start "" "!minecraft!"
timeout /t 3 >nul

:wait3
:: Check to see if Minecraft Launcher is running using tasklist and capture the PID of the first instance
for /f "tokens=2 delims=," %%a in ('tasklist /fo csv /nh /fi "IMAGENAME eq Minecraft.exe" 2^>NUL') do (
    if not defined pid (
        set pid=%%a
        echo Found Minecraft.exe with PID !pid!
    )
)

:wait2
:: Check to see if javaw.exe is running, if yes, go to next wait loop
tasklist /fi "IMAGENAME eq !instanceName!" 2>NUL | find /I "!instanceName!" >NUL
if "!ERRORLEVEL!"=="0" (
    goto wait_javaw
) else (
:: Check if the Minecraft Launcher is still running, if yes, continue waiting.
    timeout /t 1 >nul
	if defined pid (
    tasklist /fi "PID eq !pid!" | findstr /r /c:"!pid!" >nul
    if "!ERRORLEVEL!"=="0" (
			goto wait2
		) else (
			echo Main process has terminated.
			call client_assets_backup.bat
			exit
		)
				) else (
					echo No Minecraft.exe process found.
					pause
				)
)

:wait_javaw
:: Check to see if javaw.exe is running, if yes, wait for it to close
tasklist /fi "IMAGENAME eq !instanceName!" 2>NUL | find /I "!instanceName!" >NUL
if "!ERRORLEVEL!"=="0" (
    timeout /t 1 >nul
    goto wait_javaw
) else (
    :: Format date and time
    for /f "tokens=2 delims==" %%a in ('wmic os get localdatetime /value') do set datetime=%%a
    set filename_date=!datetime:~0,4!-!datetime:~4,2!-!datetime:~6,2!
    set filename_time=!datetime:~8,2!-!datetime:~10,2!-!datetime:~12,2!
    echo !filename_date!
    echo !filename_time!

    set executablePath=<executablePath.txt

    :: Save client world
    for /d %%a in ("!clientdir!\saves\*") do (
        set filename=%%~na_!filename_date!_!filename_time!.zip
        "!program!" a -tzip "!filename!" "%%a"
	copy "!filename!" "..\!executablePath!\!filename!"
        echo World saved.
    )
	goto wait2
)

endlocal
pause
