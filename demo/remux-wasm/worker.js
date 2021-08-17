importScripts('/demo/remux-wasm/remux.js')

self.onmessage = function (evt) {
    const evtData = evt.data;


    console.log(';;;;;;;;');
    console.log(evtData);


    if (evtData.type == 'remux') {
        remux(evtData.data.file)
    }
}

const ffmpegRemux = Module.cwrap('remux', 'number', ['string']);

function remux(file) {
    const MOUNT_DIR = '/working';
    FS.mkdir(MOUNT_DIR);
    FS.mount(WORKERFS, { files: [file] }, MOUNT_DIR);

    let result = ffmpegRemux(`${MOUNT_DIR}/${file.name}`)

    console.log(result);
}