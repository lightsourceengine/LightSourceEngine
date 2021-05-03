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

const tar = require('tar')
const { spawn } = require('child_process')
const fetch = require('node-fetch')
const { tmpdir } = require('os')
const { copy, createWriteStream, lstat, unlink, writeFile, readFile, ensureDir } = require('fs-extra')
const { join, dirname, basename, extname } = require('path')
const Zip = require('adm-zip')
const Handlebars = require("handlebars")

const tempFolder = tmpdir()

const promise = (executor) => new Promise(executor)

const streamDone = async (stream) => promise((resolve, reject) => {
  stream.on('finish', resolve)
  stream.on('close', resolve)
  stream.on('error', reject)
})

const deleteFile = async (file) => {
  try {
    await unlink(file)
  } catch (e) {
    // ignore
  }
}

const group = (...args) => args.length ? Promise.all(args) : Promise.resolve()

const isZip = url => url.endsWith('.zip')

const isTarGz = url => url.endsWith('.tgz') || url.endsWith('.tar.gz')

const extract = async (url, workingDir, options = {}) => {
  const { files } = options

  if (url.startsWith('https://')) {
    const response = await fetch(url, { redirect: 'follow' })

    if (isTarGz(url)) {
      const stream = tar.x({ C: workingDir }, files)

      response.body.pipe(stream)

      await streamDone(stream)
    } else if (isZip(url)) {
      const tempFile = join(tempFolder, 'extract.zip')
      const stream = createWriteStream(tempFile)

      response.body.pipe(stream)

      await streamDone(stream)
      await extractZip(tempFile, workingDir, files)
      await deleteFile(tempFile)
    }
  } else if (isZip(url)) {
    await extractZip(url, workingDir, files)
  } else if (isTarGz(url)) {
    await tar.x({ C: workingDir }, files)
  } else {
    exit(`unsupported archive: ${url}`)
  }
}

const extractTemp = async (archive) => {
  if (await isDirectory(archive)) {
    return archive
  } else {
    await extract(archive, tempFolder)

    let ext

    if (archive.endsWith('.tar.gz')) {
      ext = '.tar.gz'
    } else {
      ext = extname(archive)
    }

    return join(tempFolder, basename(archive, ext))
  }
}

const extractZip = async (zipFile, workingDir, files) => {
  const zip = new Zip(zipFile)

  if (files && files.length) {
    const writes = []

    zip.getEntries()
      .filter(({entryName}) => files.includes(entryName))
      .forEach(entry => {
        const write = promise((resolve, reject) => {
          return entry.getDataAsync((buffer, err) => err ? reject(Error(err)) : resolve(buffer))
        }).then((buffer) => {
          const file = join(workingDir, entry.entryName)
          return ensureDir(dirname(file)).then(() => writeFile(file, buffer))
        })

        writes.push(write)
      })

    if (writes.length) {
      await Promise.all(writes)
    }
  } else {
    await promise((resolve, reject) => {
      zip.extractAllToAsync(workingDir, false, (err) => err ? reject(err) : resolve())
    })
  }
}

const exit = (message) => {
  console.error(message)
  process.exit(1)
}

const exec = async (program, args, options) => promise((resolve, reject) => {
  const subprocess = spawn(program, args, options)
  let out = ''

  subprocess.stdout.setEncoding('utf8').on('data', (data) => out += data.toString())
  subprocess.stderr.setEncoding('utf8').on('data', (data) => out += data.toString())
  subprocess.on('close', (code) => code !== 0 ? reject(`${args[0]} error ${code}`) : resolve())
  subprocess.on('error', reject)
})

const writeFileWithTemplate = async (source, dest, data) => {
  const contents = await readFile(source, 'utf8')
  const template = Handlebars.compile(contents)

  await writeFile(dest, template(data))
}

const copyTo = async (srcFile, destDir, destFile) => copy(srcFile, join(destDir, destFile ?? basename(srcFile)))

const createZip = async (localPath, zipPath, outFile) => {
  const z = new Zip()

  z.addLocalFolder(localPath, zipPath)

  await promise((resolve, reject) => z.writeZip(outFile, (err) => { err ? reject(err) : resolve() }))
}

const createTarGz = async (workingDir, files, outFile) => {
  await tar.create({
    file: outFile,
    gzip: true,
    C: workingDir
  }, files)
}

const isDirectory = async (path) => {
  try {
    return (await lstat(path)).isDirectory()
  } catch (err) {
    return false
  }
}

module.exports = {
  copyTo,
  createTarGz,
  createZip,
  exec,
  exit,
  extract,
  extractTemp,
  group,
  isDirectory,
  writeFileWithTemplate
}
