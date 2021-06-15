
echo Please, after the password prompt will be displayed save it empty

set PREDEFINED_USERNAME=Gamer

net user %PREDEFINED_USERNAME% /add /PASSWORDCHG:no
WMIC USERACCOUNT WHERE "Name='%PREDEFINED_USERNAME%'" SET PasswordExpires=FALSE