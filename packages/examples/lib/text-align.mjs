// Light Source Engine Version 1.5.0
// Copyright (c) 2021 Light Source Software, LLC. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
// the License. You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
// an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

import { Style } from '@lse/core';

import { letThereBeLight } from '@lse/react';

import { jsx, jsxs } from '@lse/react/jsx-runtime';

const sheet = Style.createStyleSheet({
    textAlignLeft: {
        textAlign: 'left',
        '@extend': '%label'
    },
    textAlignCenter: {
        textAlign: 'center',
        '@extend': '%label'
    },
    textAlignRight: {
        textAlign: 'right',
        '@extend': '%label'
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
        width: '33%',
        marginBottom: '4vh',
        border: 2,
        borderColor: 'white'
    },
    '%label': {
        fontSize: '2.5vh',
        color: 'lightgray',
        borderTop: 2,
        borderColor: 'white',
        padding: '2vh'
    }
});

const sampleText = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Nam dignissim at metus in finibus. Duis pharetra lobortis ultrices. Donec ullamcorper elementum ultricies.';

const sampleTextSmall = '   Hello, world!   ';

const sampleTextNewline = 'Line One\nLine Two\nLine Three';

const Column = ({exampleClass, children}) => jsxs('box', {
    class: sheet.container,
    children: [ jsx('text', {
        class: sheet.header,
        children
    }), jsx('text', {
        class: exampleClass,
        children: sampleTextSmall
    }), jsx('text', {
        class: exampleClass,
        children: sampleText
    }), jsx('text', {
        class: exampleClass,
        children: sampleTextNewline
    }) ]
});

const TextAlignApp = () => jsxs('box', {
    class: sheet.body,
    children: [ jsx(Column, {
        exampleClass: sheet.textAlignLeft,
        children: 'textAlign: \'left\''
    }), jsx(Column, {
        exampleClass: sheet.textAlignCenter,
        children: 'textAlign: \'center\''
    }), jsx(Column, {
        exampleClass: sheet.textAlignRight,
        children: 'textAlign: \'right\''
    }) ]
});

letThereBeLight(jsx(TextAlignApp, {}), {
    scene: {
        fullscreen: false
    }
});
