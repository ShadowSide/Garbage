set External_Libs=C:\SVN\External_Libs
set Internal_Libs=C:\SVN\Internal_libs
set TaxcomDestination=E:\Taxcom
set SingleClientSource=E:\SVN\ReferentNet
set SingleClientDestination=%TaxcomDestination%\Docliner Single\Client
set SCS1=Client\Referents\SingleUserReferent\SingleUserReferent\bin\x86\Single_Release
::set SCS2=Client\Referents\Single_Release with Debug
set SCS2=Client\Referents\Single_Release
set ServerSource=C:\SVN\ReferentNet
::set ServerSource=E:\SVN\ReferentNet2
set ServerSingleDestination=%TaxcomDestination%\Docliner Single\Server
set S1=Server\RefNetServer\Release

xcopy /y "%External_Libs%\cpprestsdk_Net\Binaries\Win32\Release\*.pdb" "%SingleClientDestination%\*.pdb"
xcopy /y "%External_Libs%\cpprestsdk_Net\Binaries\Win32\Release\*.dll" "%SingleClientDestination%\*.dll"
xcopy /y "%Internal_Libs%\TaxcomExchange\Net_Release_2017\*.dll" "%SingleClientDestination%\*.dll"
xcopy /y "%Internal_Libs%\TaxcomExchange\Net_Release_2017\*.pdb" "%SingleClientDestination%\*.pdb"
xcopy /y "%Internal_Libs%\CPCryptoSt\lib\Net_Release_2017\v141_xp\*.dll" "%SingleClientDestination%\*.dll"
xcopy /y "%Internal_Libs%\CPCryptoSt\lib\Net_Release_2017\v141_xp\*.pdb" "%SingleClientDestination%\*.pdb"

xcopy /y "%External_Libs%\cpprestsdk_Net\Binaries\Win32\Release\*.pdb" "%ServerSingleDestination%\*.pdb"
xcopy /y "%External_Libs%\cpprestsdk_Net\Binaries\Win32\Release\*.dll" "%ServerSingleDestination%\*.dll"
xcopy /y "%Internal_Libs%\TaxcomExchange\Net_Release_2017\*.dll" "%ServerSingleDestination%\*.dll"
xcopy /y "%Internal_Libs%\TaxcomExchange\Net_Release_2017\*.pdb" "%ServerSingleDestination%\*.pdb"
xcopy /y "%Internal_Libs%\CPCryptoSt\lib\Net_Release_2017\v141_xp\*.dll" "%ServerSingleDestination%\*.dll"
xcopy /y "%Internal_Libs%\CPCryptoSt\lib\Net_Release_2017\v141_xp\*.pdb" "%ServerSingleDestination%\*.pdb"

xcopy /y "%SingleClientSource%\%SCS1%\*.dll" "%SingleClientDestination%\*.dll"
xcopy /y "%SingleClientSource%\%SCS1%\*.exe" "%SingleClientDestination%\*.exe"
xcopy /y "%SingleClientSource%\%SCS1%\*.exe.config" "%SingleClientDestination%\*.exe.config"

xcopy /y "%SingleClientSource%\%SCS2%\*.dll" "%SingleClientDestination%\*.dll"
xcopy /y "%SingleClientSource%\%SCS2%\*.exe" "%SingleClientDestination%\*.exe"
xcopy /y "%SingleClientSource%\%SCS2%\*.exe.config" "%SingleClientDestination%\*.exe.config"

xcopy /y "%ServerSource%\%S1%\*.dll" "%ServerSingleDestination%\*.dll"
xcopy /y "%ServerSource%\%S1%\*.exe" "%ServerSingleDestination%\*.exe"
xcopy /y "%ServerSource%\%S1%\*.exe.config" "%ServerSingleDestination%\*.exe.config"
pause