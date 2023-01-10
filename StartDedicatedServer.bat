@echo off

set UE4_EDITOR="C:\Program Files\Epic Games\UE_4.27\Engine\Binaries\Win64\UE4Editor.exe"
set SHOOOTER_UPROJECT="%CD%\ShooterMulti.uproject"
set NAME=%1

start "" %UE4_EDITOR% %SHOOOTER_UPROJECT% Server_Start?name=%NAME%?bIsLanMatch=1 -server -log -nosteam