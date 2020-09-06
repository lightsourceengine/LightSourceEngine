/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import chai from 'chai'
import { Mapping } from '../../src/input/Mapping.js'
import { Key } from '../../src/input/Key.js'
import { MappingType } from '../../src/input/MappingType.js'

const { assert } = chai
const standard = MappingType.Standard

describe('Mapping', () => {
  describe('constructor', () => {
    it('should parse button string', () => {
      const mapping = new Mapping([['b2', Key.START]])

      assert.equal(mapping.getKeyForButton(2), Key.START)
    })
    it('should parse axis string', () => {
      for (const axisValue of ['a3', '+a2', '-a2', 'a4~']) {
        const mapping = new Mapping([[axisValue, Key.START]])

        // TODO: add a real assert once axis is implemented
        assert.equal(mapping.getKeyForAxis(0, 0), -1)
      }
    })
    it('should parse hat string', () => {
      for (const hatValue of [1, 2, 4, 8]) {
        const mapping = new Mapping([[`h0.${hatValue}`, Key.START]])

        assert.equal(mapping.getKeyForHat(0, hatValue), Key.START)
      }
    })
    it('should throw Error for no arguments', () => {
      assert.throws(() => new Mapping())
    })
    it('should throw Error invalid hardware strings', () => {
      for (const input of ['', '2', 'h0.', 'b', 'a', 'h0.a']) {
        assert.throws(() => new Mapping([[input, Key.START]]))
      }
    })
    it('should throw Error for invalid entries', () => {
      for (const input of ['', null, {}, -1]) {
        assert.throws(() => new Mapping(input))
      }
    })
    it('should throw Error for invalid name', () => {
      for (const [name, entries] of [['', []], [null, []], [{}, []], [-1, []]]) {
        assert.throws(() => new Mapping(name, entries))
      }
    })
    it('should throw Error for valid name, but invalid entries', () => {
      for (const [name, entries] of [[standard, ''], [standard, null], [standard, {}], [standard, -1]]) {
        assert.throws(() => new Mapping(name, entries))
      }
    })
  })
  describe('getKeyForButton()', () => {
    it('should...', () => {

    })
  })
  describe('getKeyForHat()', () => {
    it('should...', () => {

    })
  })
  describe('getKeyForAxis()', () => {
    it('should...', () => {

    })
  })
})
