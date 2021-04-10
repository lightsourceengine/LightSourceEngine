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

const background = 'resource/pexels-emiliano-arano-1295138.jpg';

const sheet = Style.createStyleSheet({
    filterNone: {
        '@extend': '%cell'
    },
    filterFlipH: {
        filter: Style.flipH(),
        '@extend': '%cell'
    },
    filterFlipV: {
        filter: Style.flipV(),
        '@extend': '%cell'
    },
    filterTint: {
        filter: Style.tint('red'),
        '@extend': '%cell'
    },
    body: {
        padding: '4vh',
        flexDirection: 'row',
        flexWrap: 'wrap',
        justifyContent: 'space-between',
        backgroundColor: '#264653',
        '@extend': '%absoluteFill'
    },
    header: {
        padding: '2vh',
        color: '#264653',
        backgroundColor: 'white',
        fontSize: '2.5vh'
    },
    container: {
        width: '49%',
        marginBottom: '4vh',
        border: 2,
        borderColor: 'white'
    },
    '%cell': {
        objectFit: 'contain',
        width: '100%',
        height: '20vh'
    }
});

const Column = ({exampleClass, children}) => jsxs('box', {
    class: sheet.container,
    children: [ jsx('text', {
        class: sheet.header,
        children
    }), jsx('img', {
        src: background,
        class: exampleClass
    }) ]
});

const ImageFilterApp = () => jsxs('box', {
    class: sheet.body,
    children: [ jsx(Column, {
        exampleClass: sheet.filterNone,
        children: 'filter: none'
    }), jsx(Column, {
        exampleClass: sheet.filterFlipH,
        children: 'filter: flipH()'
    }), jsx(Column, {
        exampleClass: sheet.filterFlipV,
        children: 'filter: flipV()'
    }), jsx(Column, {
        exampleClass: sheet.filterTint,
        children: 'filter: tint(\'red\')'
    }) ]
});

letThereBeLight(jsx(ImageFilterApp, {}), {
    scene: {
        fullscreen: false
    }
});
