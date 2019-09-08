/*
 * Copyright (C) 2019 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { Key } from './Key'
import { Mapping } from './Mapping'

const gameControllerKeyToStandardKey = new Map([
  ['a', Key.A],
  ['b', Key.B],
  ['x', Key.X],
  ['y', Key.Y],
  ['leftshoulder', Key.L1],
  ['lefttrigger', Key.L2],
  ['rightshoulder', Key.R1],
  ['righttrigger', Key.R2],
  ['start', Key.START],
  ['back', Key.SELECT],
  ['guide', Key.HOME],
  ['dpup', Key.UP],
  ['dpright', Key.RIGHT],
  ['dpdown', Key.DOWN],
  ['dpleft', Key.LEFT],
  ['rightstick', Key.RS],
  ['leftstick', Key.LS],
  // TODO: add axis support
  ['leftx', -1],
  ['lefty', -1],
  ['rightx', -1],
  ['righty', -1]
])

export const parseSystemMapping = (csv) => {
  const entries = csv.split(',')
  const uuid = entries[0]
  const mappedEntries = []
  const { length } = entries

  if (length < 3) {
    throw Error('Too few csv entries to be a valid mapping.')
  }

  if (!uuid) {
    throw Error('UUID entry is missing')
  }

  for (let i = 2; i < length; i++) {
    const [key, hardwareKey] = entries[i].split(':')

    if (key === 'platform') {
      break
    }

    if (!gameControllerKeyToStandardKey.has(key)) {
      throw Error(`unknown game controller key name: ${key}`)
    }

    mappedEntries.push([hardwareKey, gameControllerKeyToStandardKey.get(key)])
  }

  return [uuid, new Mapping(mappedEntries)]
}
