@echo off

START "" "%dp0\__node.exe" --loader "%dp0\..\lib\builtin\loader.mjs" %*
