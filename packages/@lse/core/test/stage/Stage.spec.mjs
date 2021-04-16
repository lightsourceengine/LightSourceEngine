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
import { test } from '../test-env.mjs'
import { PluginType } from '../../src/addon/PluginType.mjs'

const { assert } = chai
let stage

describe('Stage', () => {
  beforeEach(() => {
    stage = test.stage
  })
  afterEach(() => {
    stage = null
  })
  describe('isRunning()', () => {
    it('should return true for test stage instance', () => {
      assert.isTrue(stage.isRunning())
    })
  })
  describe('isAttached()', () => {
    it('should return true for test stage instance', () => {
      assert.isTrue(stage.isAttached())
    })
  })
  describe('isConfigured()', () => {
    it('should return true for test stage instance', () => {
      assert.isTrue(stage.isConfigured())
    })
  })
  describe('wasQuitRequested()', () => {
    it('should return false for test stage instance', () => {
      assert.isFalse(stage.wasQuitRequested())
    })
  })
  describe('hasPluginType()', () => {
    it('should have platform plugin for test stage instance', () => {
      assert.isTrue(stage.hasPluginType(PluginType.PLATFORM))
    })
    it('should have audio plugin for test stage instance', () => {
      assert.isTrue(stage.hasPluginType(PluginType.AUDIO))
    })
  })
})
