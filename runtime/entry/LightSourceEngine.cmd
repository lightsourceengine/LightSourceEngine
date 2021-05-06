@echo off

SET V_SHARE_HOME=%~dp0..\share\lse
SET V_NODE_VERSION={{node_version}}
SET LSE_PATH=%V_SHARE_HOME%\builtin
SET NODE_PATH="%LSE_PATH%;%NODE_PATH%"

{{#if install_game_controller_db}}
IF "%LSE_GAME_CONTROLLER_DB%"=="" (SET LSE_GAME_CONTROLLER_DB=%V_SHARE_HOME%\assets\gamecontrollerdb.txt)
{{/if}}

IF "%LSE_DISABLE_DEFAULT_LOADER%"=="1" (
  START "" "%V_SHARE_HOME%\node\%V_NODE_VERSION%\node.exe" %*
) ELSE (
  START "" "%V_SHARE_HOME%\node\%V_NODE_VERSION%\node.exe" --loader "file://%V_SHARE_HOME%/builtin/@lse/loader/index.mjs" %*
)
