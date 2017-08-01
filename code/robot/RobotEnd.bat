@for /f "skip=1 delims=" %%i in ('tasklist /FI "imagename eq Robot.exe"') do @call :kill %%i


:kill
@rem echo %1
@set this=%1
@set killed=Robot.exe

@if "%this%" EQU "%killed%" taskkill /f /im %this%

