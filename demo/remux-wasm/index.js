const inputFile = document.querySelector('#js_input_file');
const video = document.querySelector('#js_video');

const worker = new Worker('/demo/remux-wasm/worker.js');

let sourceBuffer = null

const mediaSource = new MediaSource()
video.src = URL.createObjectURL(mediaSource)



mediaSource.addEventListener('sourceopen', (evt) => {
    let mime = 'video/mp4; codecs="avc1.42E01E, mp4a.40.2"'

    sourceBuffer = mediaSource.addSourceBuffer(mime);
});


mediaSource.addEventListener('sourceended', (evt) => {
    console.log('dsadadasda');
    console.log(evt);
    video.play()
})

inputFile.addEventListener('change', () => {
    const file = inputFile.files[0];

    // const fileReader = new FileReader()
    // fileReader.addEventListener('load', () => {
    //     const fileBuffer = new Uint8Array(fileReader.result)

    //     console.log(fileBuffer, syncOffset(fileBuffer));
    // })

    // fileReader.readAsArrayBuffer(file)

    // worker.postMessage({
    //     type: 'remux',
    //     data: {
    //         file,
    //     },
    // });

    const fileReader = new FileReader()
    fileReader.addEventListener('load', () => {
        sourceBuffer.appendBuffer(fileReader.result)

    })
    fileReader.readAsArrayBuffer(file)
});



worker.addEventListener('message', evt => {
    console.log(';;;;;');
    console.log(evt);

    if (evt.data.type == 'buffer') {
        let buffer = evt.data.data.buffer;
        console.log('.......');
        console.log(buffer);

        sourceBuffer.appendBuffer(buffer)
    }
});
