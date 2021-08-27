importScripts('/demo/remux-wasm/remux.js');

self.onmessage = function (evt) {
    const evtData = evt.data;

    console.log(';;;;;;;;');
    console.log(evtData);

    if (evtData.type == 'remux') {
        remux(evtData.data.file);
    }
};

const ffmpegRemux = Module.cwrap('remux', 'number', ['string']);

function remux(file) {
    const MOUNT_DIR = '/working';
    FS.mkdir(MOUNT_DIR);
    FS.mount(WORKERFS, { files: [file] }, MOUNT_DIR);

    let result = ffmpegRemux(`${MOUNT_DIR}/${file.name}`);


    let size = Module.HEAPU32[result / 4];
    let room = Module.HEAPU32[result / 4 + 1];
    let bufPtr = Module.HEAPU32[result / 4 + 2];



    let buffer = Module.HEAPU.slice(bufPtr, bufPtr + size - room);



    console.log('aaaaaaaaa');
    console.log(size, room, bufPtr);

    let evt = {
        type: 'buffer',
        data: {
            buffer,
        },
    };

    self.postMessage(evt, [evt.data.buffer.buffer]);
}
