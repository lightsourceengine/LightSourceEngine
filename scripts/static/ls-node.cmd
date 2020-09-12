@echo off

SET NODE_PATH="%dp0\..\lib\node_modules;%NODE_PATH%"

START "" "%dp0\__node.exe" %*
