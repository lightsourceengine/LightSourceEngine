// Light Source Engine Version 1.7.0
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

const background = 'resource/pexels-emiliano-arano-1295138.jpg';

const sheet = Style.createStyleSheet({
  body: {
    flexWrap: 'wrap',
    flexDirection: 'row',
    justifyContent: 'space-between',
    padding: '10vh',
    backgroundImage: background,
    backgroundSize: 'contain',
    backgroundPositionX: '50%',
    backgroundPositionY: '50%',
    '@extend': '%absoluteFill'
  },
  objectFitContain: {
    objectFit: 'contain',
    '@extend': '%cell'
  },
  objectFitFill: {
    objectFit: 'fill',
    '@extend': '%cell'
  },
  objectFitCover: {
    objectFit: 'cover',
    '@extend': '%cell'
  },
  objectFitScaleDown: {
    objectFit: 'scale-down',
    '@extend': '%cell'
  },
  objectFitNone: {
    objectFit: 'none',
    '@extend': '%cell'
  },
  '%cell': {
    border: '1vh',
    borderColor: 'red',
    '@size': '30vh'
  }
});

const ObjectFitApp = () => jsxs('box', {
  class: sheet.body,
  children: [ jsx('img', {
    src: background,
    class: sheet.objectFitContain
  }), jsx('img', {
    src: background,
    class: sheet.objectFitFill
  }), jsx('img', {
    src: background,
    class: sheet.objectFitCover
  }), jsx('img', {
    src: background,
    class: sheet.objectFitScaleDown
  }), jsx('img', {
    src: background,
    class: sheet.objectFitNone
  }) ]
});

letThereBeLight(jsx(ObjectFitApp, {}), {
  scene: {
    fullscreen: false
  }
});
