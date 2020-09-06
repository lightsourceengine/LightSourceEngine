/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { createStyle } from 'light-source'
import React from 'react'
import { renderAsync, beforeEachTestCase, afterEachTestCase } from './test-env.js'

const { assert } = chai

describe('Element', () => {
  beforeEach(() => {
    beforeEachTestCase()
  })
  afterEach(async () => {
    await afterEachTestCase()
  })
  describe('prop: style', async () => {
    it('should set style with Object', async () => {
      const ref = React.createRef()

      await renderAsync(<box ref={ref} style={{ backgroundColor: 'red' }} />)

      assert.equal(ref.current.node.style.backgroundColor, 0xFFFF0000)
    })
    it('should override class backgroundColor', async () => {
      const styleClass = createStyle({ backgroundColor: 'blue', zIndex: 5 })
      const ref = React.createRef()

      await renderAsync(<box ref={ref} style={{ backgroundColor: 'red' }} class={styleClass} />)

      assert.equal(ref.current.node.style.backgroundColor, 0xFFFF0000)
      assert.equal(ref.current.node.style.zIndex, 5)
    })
  })
  describe('prop: class', () => {
    it('should set class with StyleClass object', async () => {
      const backgroundRed = createStyle({ backgroundColor: 'red' })
      const ref = React.createRef()

      await renderAsync(<box ref={ref} class={backgroundRed} />)

      assert.equal(ref.current.node.style.backgroundColor, 0xFFFF0000)
    })
    it('should clear class', async () => {
      const backgroundRed = createStyle({ backgroundColor: 'red' })
      const ref = React.createRef()

      await renderAsync(<box ref={ref} class={backgroundRed} />)

      assert.equal(ref.current.node.style.backgroundColor, 0xFFFF0000)

      await renderAsync(<box ref={ref} class={null} />)

      assert.isUndefined(ref.current.node.style.backgroundColor)
    })
  })
})
