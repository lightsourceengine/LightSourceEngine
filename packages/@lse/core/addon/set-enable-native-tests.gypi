{
  "variables": {
    "enable_native_tests": "<(ls_enable_native_tests)"
  },
  "conditions": [
    [ "enable_native_tests==1", { "defines": [ "LS_ENABLE_NATIVE_TESTS" ] } ]
  ]
}
