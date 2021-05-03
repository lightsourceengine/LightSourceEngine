@echo off

SET V_SHARE_HOME=%~dp0\..\share\lse
SET LSE_PATH=%V_SHARE_HOME%\builtin
SET LSE_FONT_PATH=%V_SHARE_HOME%\assets
SET LSE_ENV=runtime

{{#if gameControllerDb}}
IF "%LSE_GAME_CONTROLLER_DB%"=="" (SET LSE_GAME_CONTROLLER_DB=%V_SHARE_HOME%\assets\gamecontrollerdb.txt)
{{/if}}

START "" "%V_SHARE_HOME%\node\{{node_version}}\node.exe" --loader "file://%V_SHARE_HOME%/builtin/@lse/loader/index.mjs" %*
