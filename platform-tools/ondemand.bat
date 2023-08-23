@ECHO off
cd /d %~dp0
echo.
echo Waiting for device…
adb wait-for-device
echo.
adb -d shell stop
adb push mycwmfix.zip /sdcard/mycwmfix.zip
adb reboot recovery