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

import { jsx, jsxs } from '@lse/react/jsx-runtime';

import { Style } from '@lse/core';

import { letThereBeLight } from '@lse/react';

import { useEffect, forwardRef, useState, createRef } from 'react';

const sheet = Style.createStyleSheet({
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
