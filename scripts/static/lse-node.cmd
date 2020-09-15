@echo off

START "" "%dp0\node.exe" --loader "%dp0\builtin\loader.mjs" %*
