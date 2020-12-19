@echo off

SET LSE_PATH=%~dp0\builtin
SET LSE_ENV=lse-node

START "" "%~dp0\node.exe" --loader "file://%~dp0\builtin\@lse\loader\index.mjs" %*
