/*
 * Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
 * an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations under the License.
 */

/**
 * Applies an overlay file, containing publishing specific changes, to a package.json.
 *
 * Usage: node apply-package-json-overlay.js <package.json> <overlay.json5> <out.json>
 *
 * The overlay file is a JSON5 file containing modifications to be applied to the passed in package.json file. The
 * modifications can include replacement of keys, removing keys and inserting publishing version. The options are
 * limited to keep processing simple.
 *
 * Examples
 *
 * { "dependencies": { ... } }
 * - Replace all dependencies from package.json
 *
 * { "files": "!" }
 * - Remove files from package.json
 *
 * { "version": "@version" }
 * - Set publishing version.
 *
 * { "scripts": { "install": "@package.json" } }
 * - Replace all scripts, except the install script.
 */

const { readFileSync, writeFileSync } = require('fs')
const { join } = require('path')
const JSON5 = require('json5')

const readJsonFile = file => JSON5.parse(readFileSync(file, "utf8"))
const version = readJsonFile(join(__dirname, '..', 'publishing', 'version.json')).version

let packageJson
let packageJsonOverlay

try {
  packageJson = readJsonFile(process.argv[2])
} catch (e) {
  throw Error('First argument must be a package.json')
}

try {
  packageJsonOverlay = readJsonFile(process.argv[3])
} catch (e) {
  throw Error("Second argument must be a package-overlay.json")
}

const target = process.argv[4]

if (!target) {
  throw Error("Third argument must be an output package.json filename")
}

for (const key of Object.keys(packageJsonOverlay)) {
  let overlayValue = packageJsonOverlay[key]

  if (overlayValue === '!') {
    delete packageJson[key]
    continue
  }

  if (overlayValue === '@version') {
    overlayValue = version
  } else if (typeof overlayValue === 'object') {
    for (const subKey of Object.keys(overlayValue)) {
      if (overlayValue[subKey] === '@package.json') {
        overlayValue[subKey] = packageJson[key][subKey]
      } else if (overlayValue[subKey] === '@version') {
        overlayValue[subKey] = version
      }
    }
  }

  packageJson[key] = overlayValue
}

writeFileSync(target, JSON.stringify(packageJson, null, 2) + '\n');
