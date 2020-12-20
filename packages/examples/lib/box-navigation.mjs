/*
 * Light Source Engine
 * Copyright (C) 2019-2020 Daniel Anderson <dan.anderson.oss@gmail.com>.
 *
 * This source code is licensed under the MIT License.
 *
 * License Text:    https://github.com/lightsourceengine/LightSourceEngine/blob/master/LICENSE
 * Original Source: https://github.com/lightsourceengine/LightSourceEngine
 */

import { createStyleSheet } from '@lse/core';

import { letThereBeLight } from '@lse/react';

import React, { useEffect, forwardRef, useState, createRef } from 'react';

const sheet = createStyleSheet({
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
});

const ListItem = forwardRef((props, ref) => {
    const [listItemStyle, setListItemStyle] = useState(sheet.listItem);
    return React.createElement('box', {
        focusable: true,
        ref,
        style: listItemStyle,
        onFocus: () => setListItemStyle(sheet.listItemFocused),
        onBlur: () => setListItemStyle(sheet.listItem)
    });
});

const NavigationApp = () => {
    const ref = createRef();
    useEffect(() => {
        ref.current.node.focus();
    });
    return React.createElement('box', {
        class: sheet.body,
        waypoint: 'horizontal'
    }, React.createElement(ListItem, {
        ref
    }), React.createElement(ListItem, null), React.createElement(ListItem, null), React.createElement(ListItem, null), React.createElement(ListItem, null));
};

letThereBeLight(React.createElement(NavigationApp, null), {
    fullscreen: false
});