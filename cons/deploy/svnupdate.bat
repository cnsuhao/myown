goto end
set cur_dir=%cd%
set svn_dir=..
cd %svn_dir%  
svn up
cd %cur_dir%

:end
exit /b 0