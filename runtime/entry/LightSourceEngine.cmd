@echo off

SET V_SHARE_HOME=%~dp0\..\share\lse
SET LSE_PATH=%V_SHARE_HOME%\builtin
SET LSE_FONT_PATH=%V_SHARE_HOME%\assets
export NODE_PATH="%LSE_PATH%;%NODE_PATH%"
SET LSE_ENV=runtime

{{#if install_game_controller_db}}
IF "%LSE_GAME_CONTROLLER_DB%"=="" (SET LSE_GAME_CONTROLLER_DB=%V_SHARE_HOME%\assets\gamecontrollerdb.txt)
{{/if}}

IF "%LSE_DISABLE_DEFAULT_LOADER%"=="1" (
  START "" "%V_SHARE_HOME%\node\{{node_version}}\node.exe" %*
) ELSE (
  START "" "%V_SHARE_HOME%\node\{{node_version}}\node.exe" --loader "file://%V_SHARE_HOME%/builtin/@lse/loader/index.mjs" %*
)
