/*
 * Light Source Engine
 * Copyright (C) 2019-2020 Daniel Anderson <dan.anderson.oss@gmail.com>.
 *
 * This source code is licensed under the MIT License.
 *
 * License Text:    https://github.com/lightsourceengine/LightSourceEngine/blob/master/LICENSE
 * Original Source: https://github.com/lightsourceengine/LightSourceEngine
 */

import { createStyle } from '@lse/core';

import { letThereBeLight } from '@lse/react';

import React from 'react';

const body = createStyle({
    backgroundImage: 'resource/pattern-1004855.jpg',
    backgroundRepeat: 'repeat',
    '@extend': '%absoluteFill'
});

letThereBeLight(React.createElement('box', {
    class: body
}), {
    fullscreen: false
});
