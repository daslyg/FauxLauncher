setlocal enabledelayedexpansion

:: Set the zipfile's name
set "zipfile=assets.zip"

:: Create the assets folder if it doesn't exist
if not exist "assets\" mkdir "assets"

:: Find client directory
set clientdir=%APPDATA%\.minecraft
echo !clientdir!

:: Find program directory (zipping tool)
set program=C:\Program Files\7-Zip\7z.exe
echo !program!

for /d %%a in (*) do (
    "!program!" a -tzip "mods.zip" "!clientdir!\mods"
    "!program!" a -tzip "mods.zip" "!clientdir!\config"
    "!program!" a -tzip "!zipfile!" "!clientdir!\resourcepacks"
    "!program!" a -tzip "!zipfile!" "!clientdir!\shaderpacks"
)

:: Remove the assets folder after the zip file has been created
rd "assets"

set executablePath=<executablePath.txt

copy /Y "mods.zip" "..\!executablePath!"
copy /Y "!zipfile!" "..\!executablePath!"

endlocal