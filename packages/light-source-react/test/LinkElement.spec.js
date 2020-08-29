/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import React from 'react'
import { renderAsync, container, afterEachTestCase, beforeEachTestCase } from './test-env'
import { render } from '../src/renderer'

const kImage = 'test/resource/image-1280x720.png'
let root

describe('LinkElement', () => {
  beforeEach(() => {
    beforeEachTestCase()
    root = container()
  })
  afterEach(async () => {
    root = null
    await afterEachTestCase()
  })
  it('should set href, as and rel properties', async () => {
    await renderAsync(<link href={kImage} as='image' rel='preload' />)

    assert.equal(root.children[0].href, kImage)
    assert.equal(root.children[0].as, 'image')
    assert.equal(root.children[0].rel, 'preload')
  })
  it('should return default values for href, as and rel properties', async () => {
    await renderAsync(<link />)

    assert.equal(root.children[0].href, '')
    assert.equal(root.children[0].as, 'auto')
    assert.equal(root.children[0].rel, 'preload')
  })
  it('should call onLoad for image', async () => {
    const summary = await linkOnLoadAsync(kImage)

    assert.equal(root.children[0].href, kImage)
    assert.equal(summary.width, 1280)
    assert.equal(summary.height, 720)
  })
  it('should call onLoad for file not found', async () => {
    await linkOnErrorAsync('does not exist')
  })
})

const linkOnLoadAsync = async (href) => {
  return new Promise((resolve, reject) => {
    render(
      root,
      <link
        href={href}
        onLoad={(...args) => resolve(args[1])}
        onError={(...args) => reject(Error(args[1]))}
      />)
  })
}

const linkOnErrorAsync = async (href) => {
  return new Promise((resolve, reject) => {
    render(
      root,
      <link
        href={href}
        onLoad={() => reject(Error('Unexpected onError'))}
        onError={(...args) => resolve(args[1])}
      />)
  })
}
