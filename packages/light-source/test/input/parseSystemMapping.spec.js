/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { assert } from 'chai'
import { parseSystemMapping } from '../../src/input/parseSystemMapping'
import { MappingType } from '../../src/input/MappingType'

const csv = '03000000c82d00002038000000000000,8bitdo,a:b1,b:b0,back:b10,dpdown:h0.4,dpleft:h0.8,dpright:h0.2,dpup:h0.1,' +
  'guide:b2,leftshoulder:b6,leftstick:b13,lefttrigger:b8,leftx:a0,lefty:a1,rightshoulder:b7,rightstick:b14,' +
  'righttrigger:b9,rightx:a3,righty:a4,start:b11,x:b4,y:b3,platform:Windows,'

describe('parseSystemMapping()', () => {
  it('should parse game controller csv', () => {
    const [mapping, uuid] = parseSystemMapping(csv)

    assert.equal(uuid, '03000000c82d00002038000000000000')
    assert.equal(mapping.name, MappingType.Standard)
  })
  it('should reject invalid csv strings', () => {
    for (const input of ['', ',', '03000000c82d00002038000000000000,', '03000000c82d00002038000000000000,8bitdo']) {
      assert.throws(() => parseSystemMapping(input))
    }
  })
  it('should reject invalid key name', () => {
    assert.throws(() => parseSystemMapping('03000000c82d00002038000000000000,8bitdo,invalid:b1'))
  })
  it('should reject invalid key value', () => {
    assert.throws(() => parseSystemMapping('03000000c82d00002038000000000000,8bitdo,a:0'))
  })
})
