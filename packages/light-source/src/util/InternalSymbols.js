/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { symbolFor } from './index'

// Symbols used by internal classes for private/protected property access.

export const $adapter = symbolFor('adapter')
export const $scene = symbolFor('scene')
export const $events = symbolFor('events')
export const $emit = symbolFor('emit')
export const $setResourcePath = symbolFor('setResourcePath')
export const $width = symbolFor('width')
export const $height = symbolFor('height')
export const $fullscreen = symbolFor('fullscreen')
export const $stage = symbolFor('stage')
export const $root = symbolFor('root')
export const $destroy = symbolFor('destroy')
export const $displayIndex = symbolFor('displayIndex')
export const $activeNode = symbolFor('activeNode')
export const $destroying = symbolFor('destroying')
export const $mainLoopHandle = symbolFor('mainLoopHandle')
export const $fps = symbolFor('fps')
export const $attach = symbolFor('attach')
export const $detach = symbolFor('detach')
export const $frame = symbolFor('frame')
export const $displays = symbolFor('displays')
export const $exitListener = symbolFor('exitListener')
export const $quitRequested = symbolFor('quitRequested')
export const $input = symbolFor('input')
export const $audio = symbolFor('audio')
export const $source = symbolFor('source')
export const $hasFocus = symbolFor('hasFocus')
export const $audioSourceMap = symbolFor('audioSourceMap')
export const $state = symbolFor('state')
export const $init = symbolFor('init')
export const $options = symbolFor('options')
export const $buffer = symbolFor('buffer')
export const $asyncId = symbolFor('asyncId')
export const $owner = symbolFor('owner')
export const $resourcePath = symbolFor('resourcePath')
export const $destination = symbolFor('destination')
export const $setNativeKeyboard = symbolFor('setNativeKeyboard')
export const $sendKeyEvent = symbolFor('sendKeyEvent')
export const $processEvents = symbolFor('processEvents')
export const $image = symbolFor('image')
export const $plugin = symbolFor('plugin')
