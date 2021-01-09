@echo off

SET LSE_PATH=%~dp0\builtin
SET LSE_ENV=lse-node

IF "%LSE_GAME_CONTROLLER_DB%"=="" (SET LSE_GAME_CONTROLLER_DB=%~dp0\assets\gamecontrollerdb.txt)
IF "%LSE_FONT_MANIFEST%"=="" (SET LSE_FONT_MANIFEST=%~dp0\assets\font.manifest)

START "" "%~dp0\node.exe" --loader "file://%~dp0\builtin\@lse\loader\index.mjs" %*
