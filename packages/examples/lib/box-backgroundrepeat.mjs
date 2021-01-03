/*
 * Light Source Engine
 * Copyright (C) 2019-2021 Daniel Anderson <dan.anderson.oss@gmail.com>.
 *
 * This source code is licensed under the MIT License.
 *
 * License Text:    https://github.com/lightsourceengine/LightSourceEngine/blob/master/LICENSE
 * Original Source: https://github.com/lightsourceengine/LightSourceEngine
 */

import { jsx } from '@lse/react/jsx-runtime';

import { createStyle } from '@lse/core';

import { letThereBeLight } from '@lse/react';

const body = createStyle({
    backgroundImage: 'resource/pattern-1004855.jpg',
    backgroundRepeat: 'repeat',
    '@extend': '%absoluteFill'
});

letThereBeLight(jsx('box', {
    class: body
}), {
    fullscreen: false
});
