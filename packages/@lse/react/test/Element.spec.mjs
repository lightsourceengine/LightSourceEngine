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

import chai from 'chai'
import { Style } from '@lse/core'
import React from 'react'
import { renderAsync, beforeEachTestCase, afterEachTestCase } from './test-env.mjs'

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
      const styleClass = Style.createStyleClass({ backgroundColor: 'blue', zIndex: 5 })
      const ref = React.createRef()

      await renderAsync(<box ref={ref} style={{ backgroundColor: 'red' }} class={styleClass} />)

      assert.equal(ref.current.node.style.backgroundColor, 0xFFFF0000)
      assert.equal(ref.current.node.style.zIndex, 5)
    })
  })
  describe('prop: class', () => {
    it('should set class with StyleClass object', async () => {
      const backgroundRed = Style.createStyleClass({ backgroundColor: 'red' })
      const ref = React.createRef()

      await renderAsync(<box ref={ref} class={backgroundRed} />)

      assert.equal(ref.current.node.style.backgroundColor, 0xFFFF0000)
    })
    it('should clear class', async () => {
      const backgroundRed = Style.createStyleClass({ backgroundColor: 'red' })
      const ref = React.createRef()

      await renderAsync(<box ref={ref} class={backgroundRed} />)

      assert.equal(ref.current.node.style.backgroundColor, 0xFFFF0000)

      await renderAsync(<box ref={ref} class={null} />)

      assert.isUndefined(ref.current.node.style.backgroundColor)
    })
  })
})
