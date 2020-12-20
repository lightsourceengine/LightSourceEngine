/*
 * Light Source Engine
 * Copyright (C) 2019-2020 Daniel Anderson <dan.anderson.oss@gmail.com>.
 *
 * This source code is licensed under the MIT License.
 *
 * License Text:    https://github.com/lightsourceengine/LightSourceEngine/blob/master/LICENSE
 * Original Source: https://github.com/lightsourceengine/LightSourceEngine
 */

import { createStyleSheet, stage, ScanCode, Key } from '@lse/core';

import { letThereBeLight } from '@lse/react';

import { EventEmitter } from 'events';

import { cpus } from 'os';

import React from 'react';

const styles = createStyleSheet({
    body: {
        backgroundColor: 'lightgray',
        flexDirection: 'column',
        paddingLeft: '5vw',
        paddingRight: '5vw',
        paddingTop: '5vh',
        paddingBottom: '5vh',
        '@extend': '%absoluteFill'
    },
    leftColumn: {
        paddingTop: '2.5vh',
        flexDirection: 'column'
    },
    rightColumn: {
        paddingTop: '2.5vh',
        paddingLeft: '4vw',
        paddingRight: '4vw',
        backgroundColor: '#CCC',
        height: '100%',
        flex: 1
    },
    tabButton: {
        border: '0.5vh',
        padding: '0.75vh'
    },
    tabButtonFocused: {
        borderColor: '#00CED1',
        backgroundColor: 'white',
        '@extend': 'tabButton'
    },
    tabButtonText: {
        fontSize: '2.5vh',
        paddingLeft: '0.75vw',
        paddingTop: '0.75vw',
        paddingBottom: '0.75vw',
        borderLeft: '0.40vw',
        maxLines: 1,
        '@extend': '%primaryFont'
    },
    tabButtonTextFocused: {
        borderColor: 'dodgerblue',
        '@extend': [ 'tabButtonText', '%highlightFont' ]
    },
    buttonText: {
        fontSize: '2.5vh',
        marginRight: 'auto',
        paddingTop: '1vh',
        paddingBottom: '1vh',
        paddingLeft: '1vh',
        textOverflow: 'ellipsis',
        maxLines: 1,
        '@extend': '%primaryFont'
    },
    buttonValueText: {
        fontSize: '2.5vh',
        paddingTop: '1vh',
        paddingBottom: '1vh',
        paddingRight: '1vh',
        maxLines: 1,
        '@extend': '%highlightFont'
    },
    buttonContainer: {
        border: '0.5vh',
        flexDirection: 'row'
    },
    buttonContainerFocused: {
        border: '0.5vh',
        borderColor: '#00CED1',
        backgroundColor: 'white',
        '@extend': 'buttonContainer'
    },
    listItem: {
        borderTop: 1,
        borderColor: '#AAA'
    },
    listItemEnd: {
        borderBottom: 1,
        '@extend': 'listItem'
    },
    hintText: {
        fontSize: '2.25vh',
        paddingLeft: '1vh',
        paddingTop: '1vh',
        paddingBottom: '1.5vh',
        '@extend': '%hintFont'
    },
    h1: {
        fontSize: '3vh',
        '@extend': '%primaryFont'
    },
    h2: {
        fontSize: '2.5vh',
        '@extend': '%primaryFont'
    },
    headline: {
        borderBottom: 1,
        borderColor: '#444',
        paddingBottom: '1.5vh'
    },
    content: {
        flexDirection: 'row',
        flex: 1
    },
    footer: {
        borderTop: 1,
        borderColor: '#444',
        paddingTop: '1.5vh',
        flexDirection: 'row',
        justifyContent: 'flex-end'
    },
    '%primaryFont': {
        color: '#444'
    },
    '%hintFont': {
        color: '#999'
    },
    '%highlightFont': {
        color: 'dodgerblue'
    }
});

const ConnectedGamepadsView = () => {
    const {gamepads} = stage.input;
    if (gamepads.length === 0) {
        return React.createElement('text', {
            class: styles.hintText
        }, 'No gamepads connected.');
    } else {
        const createListItem = gamepad => {
            const mapState = stage.input.getSystemMapping(gamepad.uuid) ? 'MAPPED' : 'UNMAPPED';
            return React.createElement(React.Fragment, null, React.createElement(Button, {
                value: mapState,
                end: true
            }, gamepad.name), React.createElement('text', {
                class: styles.hintText
            }, `UUID: ${gamepad.uuid}\nButton Count: ${gamepad.buttonCount}, Hat Count: ${gamepad.hatCount}, Axis Count: ${gamepad.axisCount}`));
        };
        return gamepads.map(createListItem);
    }
};

const SystemStatisticsView = () => {
    const createSystemStats = (cpuIdle, cpuTotal) => Object.entries(process.memoryUsage()).reduce((acc, [key, value]) => {
        acc[key] = value / 1024 / 1024;
        return acc;
    }, {
        cpuLoad: cpuTotal !== 0 ? 100 - ~~(100 * cpuIdle / cpuTotal) : 0
    });
    const createListItem = (key, name, suffix, end = false) => React.createElement(Button, {
        key,
        value: `${systemStats[key].toFixed(2)}${suffix}`,
        end
    }, `${name ?? key}`);
    const getCpuLoadSnapshot = () => {
        const cpuList = cpus();
        const result = cpuList.reduce((accumulator, cpu) => {
            accumulator.tick += Object.values(cpu.times).reduce((accumulator, time) => accumulator + time, 0);
            accumulator.idle += cpu.times.idle;
            return accumulator;
        }, {
            idle: 0,
            tick: 0
        });
        return {
            idle: result.idle / cpuList.length,
            total: result.tick / cpuList.length
        };
    };
    const [systemStats, setSystemStats] = React.useState(() => createSystemStats(0, 0));
    React.useEffect(() => {
        let start = getCpuLoadSnapshot();
        const handle = setInterval(() => {
            const end = getCpuLoadSnapshot();
            setSystemStats(createSystemStats(end.idle - start.idle, end.total - start.total));
            start = end;
        }, 1e3);
        return () => clearInterval(handle);
    });
    return [ createListItem('cpuLoad', 'Total CPU Utilization', '%'), createListItem('heapUsed', 'Heap Used', ' MB'), createListItem('heapTotal', 'Heap Total', ' MB'), createListItem('rss', 'Resident Set Size', ' MB', true) ];
};

const VideoView = () => React.createElement(Button, {
    value: `${stage.scene.width}x${stage.scene.height}`,
    end: true
}, 'Screen Resolution');

const AudioView = () => {
    const enabled = destination => destination.isAvailable() ? 'ENABLED' : 'DISABLED';
    const decoders = destination => destination.getDecoders().join(', ');
    const {sample, stream} = stage.audio;
    return React.createElement(React.Fragment, null, React.createElement(Button, {
        value: enabled(sample),
        end: true
    }, 'Sample'), React.createElement('text', {
        class: styles.hintText
    }, `Audio destination for sound effects.\nDecoders: ${decoders(sample)}`), React.createElement(Button, {
        value: enabled(stream),
        end: true
    }, 'Stream'), React.createElement('text', {
        class: styles.hintText
    }, `Audio destination for streaming.\nDecoders: ${decoders(stream)}`));
};

const InfoView = () => React.createElement(React.Fragment, null, React.createElement('text', {
    class: styles.hintText
}, 'Switchy Settings demonstrates a practical options menu application using the Light Source Engine.'), React.createElement('text', {
    class: styles.hintText
}, 'The menus can be navigated with a connected gamepad or system keyboard.'), React.createElement('text', {
    class: styles.hintText
}, 'A connected gamepad must have a system mapping installed (SDL GameController mapping) in order to be used.'), React.createElement('text', {
    class: styles.hintText
}, 'By default, the system keyboard has the arrow keys mapped to the d-pad, the Z key mapped to the B button and the X key mapping to the A button.'), React.createElement(Button, {
    onSelect: () => stage.quit(),
    end: true
}, 'Exit'));

const views = {
    InfoView,
    ConnectedGamepadsView,
    VideoView,
    AudioView,
    SystemStatisticsView
};

const Button = ({children, onSelect, end = false, value = ''}) => {
    const onKeyDown = React.useCallback(e => e.key === Key.A && onSelect(), [ onSelect ]);
    const onFocus = React.useCallback(e => e.target.class = styles.buttonContainerFocused, []);
    const onBlur = React.useCallback(e => e.target.class = styles.buttonContainer, []);
    return React.createElement('box', {
        class: end ? styles.listItemEnd : styles.listItem
    }, React.createElement('box', {
        focusable: true,
        class: styles.buttonContainer,
        onKeyDown,
        onFocus,
        onBlur
    }, React.createElement('text', {
        class: styles.buttonText
    }, children), React.createElement('text', {
        class: styles.buttonValueText
    }, value)));
};

const TabButton = React.forwardRef(({viewId, children}, ref) => {
    const ctx = React.useContext(settingsContext);
    const onFocus = React.useCallback(e => {
        e.target.class = styles.tabButtonFocused;
        e.target.children[0].class = styles.tabButtonTextFocused;
        ctx.emit('changed', viewId);
    }, [ viewId ]);
    const onBlur = React.useCallback(e => {
        e.target.class = styles.tabButton;
        e.target.children[0].class = styles.tabButtonText;
    }, []);
    return React.createElement('box', {
        ref,
        focusable: true,
        class: styles.tabButton,
        onFocus,
        onBlur
    }, React.createElement('text', {
        class: styles.tabButtonText
    }, children));
});

const LeftColumn = () => {
    const ref = React.createRef();
    React.useEffect(() => ref.current.node.focus());
    return React.createElement('box', {
        class: styles.leftColumn,
        waypoint: 'vertical'
    }, React.createElement(TabButton, {
        viewId: 'InfoView',
        ref
    }, 'Info'), React.createElement(TabButton, {
        viewId: 'ConnectedGamepadsView'
    }, 'Connected Gamepads'), React.createElement(TabButton, {
        viewId: 'VideoView'
    }, 'Video'), React.createElement(TabButton, {
        viewId: 'AudioView'
    }, 'Audio'), React.createElement(TabButton, {
        viewId: 'SystemStatisticsView'
    }, 'System Statistics'));
};

const RightColumn = () => {
    const ctx = React.useContext(settingsContext);
    const [viewId, setViewId] = React.useState('InfoView');
    const View = views[viewId];
    React.useEffect(() => {
        const listener = viewId => setViewId(viewId);
        ctx.addListener('changed', listener);
        return () => ctx.removeListener('changed', listener);
    });
    return React.createElement('box', {
        waypoint: 'vertical',
        class: styles.rightColumn
    }, React.createElement(View, null));
};

class ErrorBoundary extends React.Component {
    componentDidCatch(error, errorInfo) {
        console.error(`Error: ${error.message}\nComponent Stack: ${errorInfo.componentStack}`);
        stage.quit();
    }
    render() {
        return this.props.children;
    }
}

const SwitchyApp = () => React.createElement(ErrorBoundary, null, React.createElement('box', {
    class: styles.body,
    onKeyUp: ({key}) => key === Key.B && stage.quit()
}, React.createElement('box', {
    class: styles.headline
}, React.createElement('text', {
    class: styles.h1
}, '• Switchy Settings')), React.createElement('box', {
    class: styles.content,
    waypoint: 'horizontal'
}, React.createElement(LeftColumn, null), React.createElement(RightColumn, null)), React.createElement('box', {
    class: styles.footer
}, React.createElement('box', {
    style: {
        height: '100%',
        backgroundColor: 'black',
        width: '4vw',
        marginRight: 'auto'
    }
}), React.createElement('text', {
    class: styles.h2,
    style: {
        marginRight: '2vw'
    }
}, 'B - Back'), React.createElement('text', {
    class: styles.h2
}, 'A - OK'))));

const settingsContext = React.createContext(new EventEmitter);

stage.input.on('rawkeyup', e => e.button === ScanCode.ESCAPE && stage.quit());

letThereBeLight(React.createElement(SwitchyApp, null), {
    title: 'Switchy Settings'
});
