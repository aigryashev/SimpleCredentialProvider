@SET START_PATH=%cd%
@echo START_PATH=%START_PATH%

@set DISTR_PATH=%START_PATH%\..\bin
@echo DISTR_PATH=%DISTR_PATH%

@call %DISTR_PATH%\vc_redist.x64.exe
@call %DISTR_PATH%\vc_redist.x86.exe

@cd %START_PATH%

