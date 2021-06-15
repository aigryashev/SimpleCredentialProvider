@SET START_PATH=%cd%
@echo START_PATH=%START_PATH%

@set DISTR_PATH=%START_PATH%\..\bin
@echo DISTR_PATH=%DISTR_PATH%

@%SystemDrive%
@cd %AllUsersProfile%
@mkdir crm
@xcopy %DISTR_PATH%\*.dll .\crm /Y
@xcopy %START_PATH%\logon_background.bmp .\crm /Y

@setx /M PATH "%PATH%;%AllUsersProfile%\crm"

@cd %START_PATH%

@echo Please, dont forget to reboot!



