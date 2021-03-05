/*
 * Light Source Engine
 * Copyright (C) 2019-2021 Daniel Anderson <dan.anderson.oss@gmail.com>.
 *
 * This source code is licensed under the MIT License.
 *
 * License Text:    https://github.com/lightsourceengine/LightSourceEngine/blob/master/LICENSE
 * Original Source: https://github.com/lightsourceengine/LightSourceEngine
 */

import { jsx, jsxs } from '@lse/react/jsx-runtime';

import { createStyleSheet } from '@lse/core';

import { letThereBeLight } from '@lse/react';

import { useEffect, forwardRef, useState, createRef } from 'react';

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
    return jsx('box', {
        focusable: true,
        ref,
        class: listItemStyle,
        onFocus: () => setListItemStyle(sheet.listItemFocused),
        onBlur: () => setListItemStyle(sheet.listItem)
    });
});

const NavigationApp = () => {
    const ref = createRef();
    useEffect(() => {
        ref.current.node.focus();
    }, []);
    return jsxs('box', {
        class: sheet.body,
        waypoint: 'horizontal',
        children: [ jsx(ListItem, {
            ref
        }), jsx(ListItem, {}), jsx(ListItem, {}), jsx(ListItem, {}), jsx(ListItem, {}) ]
    });
};

letThereBeLight(jsx(NavigationApp, {}), {
    scene: {
        fullscreen: false
    }
});
