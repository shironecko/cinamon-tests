@echo off

if not exist ..\build mkdir ..\build

cl cinamon-tests.c ^
	/Fe..\build\ /Fd..\build\ /Fo..\build\ ^
	/nologo /Od /Zi /MTd ^
	/link /subsystem:console lua53.lib /nodefaultlib:libcmt
