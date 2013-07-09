ECHO %CD%
for /R %CD%\ %%f in (*.h) do copy %%f %CD%\include\CommunityLib