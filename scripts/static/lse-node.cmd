@echo off

START "" "%~dp0\node.exe" --loader "file://%~dp0\builtin\loader.mjs" %*
