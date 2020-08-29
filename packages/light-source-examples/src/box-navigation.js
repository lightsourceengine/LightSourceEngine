/*
 * Copyright (C) 2020 Daniel Anderson
 *
 * This source code is licensed under the MIT license found in the LICENSE file in the root directory of this source tree.
 */

import { createStyleSheet } from 'light-source'
import { letThereBeLight } from 'light-source-react'
import React, { createRef, useEffect, forwardRef, useState } from 'react'

// Demonstrates keyboard or gamepad focus navigation.

const sheet = createStyleSheet({
  // https://coolors.co/e63946-f1faee-a8dadc-457b9d-1d3557
  body: {
    backgroundColor: '#457b9d',
    flexWrap: 'wrap',
    flexDirection: 'row',
    justifyContent: 'space-between',
    padding: '5vh',
    '@extend': '%absoluteFill'
  },
  listItem: {
    '@size': '25vh',
    border: '1vh',
    backgroundColor: '#f1faee',
    borderColor: '#1d3557'
  },
  listItemFocused: {
    borderColor: '#e63946',
    '@extend': 'listItem'
  }
})

const ListItem = forwardRef((props, ref) => {
  const [listItemStyle, setListItemStyle] = useState(sheet.listItem)

  return (
    <box
      focusable
      ref={ref}
      style={listItemStyle}
      onFocus={() => setListItemStyle(sheet.listItemFocused)}
      onBlur={() => setListItemStyle(sheet.listItem)}
    />)
})

const NavigationApp = () => {
  const ref = createRef()

  useEffect(() => {
    ref.current.node.focus()
  })

  return (
    <box style={sheet.body} waypoint='horizontal'>
      <ListItem ref={ref} />
      <ListItem />
      <ListItem />
      <ListItem />
      <ListItem />
    </box>
  )
}

letThereBeLight(<NavigationApp />, { fullscreen: false })
