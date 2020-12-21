{
  "variables": {
    "enable_native_tests": "<(lse_enable_native_tests)"
  },
  "conditions": [
    [ "enable_native_tests==1", { "defines": [ "LSE_ENABLE_NATIVE_TESTS" ] } ]
  ]
}
