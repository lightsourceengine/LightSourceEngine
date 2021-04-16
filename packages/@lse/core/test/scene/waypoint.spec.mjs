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
import { waypoint } from '../../src/scene/waypoint.mjs'

const { assert } = chai

describe('waypoint()', () => {
  it('should return a waypoint when given a "horizontal" tag', () => {
    assert.equal(waypoint('horizontal').tag, 'horizontal')
  })
  it('should return a waypoint when given a "vertical" tag', () => {
    assert.equal(waypoint('vertical').tag, 'vertical')
  })
  it('should throw Error when tag is not "vertical" or "horizontal"', () => {
    for (const input of ['test', '', null, undefined, {}, 3]) {
      assert.throws(() => waypoint(input))
    }
  })
})
