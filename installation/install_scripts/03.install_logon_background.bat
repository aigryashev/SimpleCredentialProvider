
@echo changing logon screen background to "%AllUsersProfile%\crm\logon_background.bmp"
@reg add "HKLM\SOFTWARE\Policies\Microsoft\Windows\Personalization" /v LockScreenImage /t REG_SZ /d "%AllUsersProfile%\crm\logon_background.bmp"
