let video = document.querySelector('.js_video');
let input = document.querySelector('.js_input')

let sourceBuffer = null;

const fileReader = new FileReader()

input.addEventListener('change', () => {
    let file = input.files[0]

    fileReader.addEventListener('load', () => {
        console.log('ssssssss');
        console.log(fileReader.result);

        sourceBuffer.appendBuffer(fileReader.result);
    })

    fileReader.readAsArrayBuffer(file)
})

// let assetURL = 'frag_bunny.mp4';
// Need to be specific for Blink regarding codecs
// ./mp4info frag_bunny.mp4 | grep Codec
let mimeCodec = 'video/mp4; codecs="avc1.42E01E, mp4a.40.2"';

if ('MediaSource' in window &&
    MediaSource.isTypeSupported(mimeCodec)) {
    let mediaSource = new MediaSource;
    //console.log(mediaSource.readyState); // closed
    video.src = URL.createObjectURL(mediaSource);

    mediaSource.addEventListener('sourceclose', evt => {
        console.log('0000000000', evt);
    })


    mediaSource.addEventListener('sourceopen', () => {

        console.log('==========', mediaSource.readyState);

        sourceBuffer = mediaSource.addSourceBuffer(mimeCodec);

        sourceBuffer.addEventListener('updateend', function (evt) {

            console.log(';;;;;;;;;;;', mediaSource.readyState);

            // mediaSource.endOfStream();
            // video.play();
            //console.log(mediaSource.readyState); // ended
        });

    });
} else {
    console.error('Unsupported MIME type or codec: ', mimeCodec);
}

// function sourceOpen(buf) {
//     //console.log(this.readyState); // open
//     let mediaSource = e.target;

//     // fetchAB(assetURL, function (buf) {
//     sourceBuffer.addEventListener('updateend', function (_) {
//         mediaSource.endOfStream();
//         video.play();
//         //console.log(mediaSource.readyState); // ended
//     });
//     console.log("buf", buf);
//     sourceBuffer.appendBuffer(buf);
//     // });
// };

// function fetchAB(url, cb) {
//     console.log(url);
//     let xhr = new XMLHttpRequest;
//     xhr.open('get', url);
//     xhr.responseType = 'arraybuffer';
//     xhr.onload = function () {
//         cb(xhr.response);
//     };
//     xhr.send();
// };