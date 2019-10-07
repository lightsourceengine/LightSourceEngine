{
  "conditions": [
    [
      "OS==\"linux\"", {
        "libraries": [
          "-Wl,--enable-new-dtags,-rpath,'$$ORIGIN/../lib'"
        ]
      }
    ]
  ],  
}
