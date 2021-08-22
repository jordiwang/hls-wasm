let video = document.querySelector('.js_video');
let input = document.querySelector('.js_input');

let sourceBuffer = null;

const fileReader = new FileReader();

input.addEventListener('change', () => {
    let file = input.files[0];

    fileReader.addEventListener('load', () => {
        sourceBuffer.appendBuffer(fileReader.result);
    });

    fileReader.readAsArrayBuffer(file);
});

let mimeCodec = 'video/mp4; codecs="avc1.4D401F, mp4a.40.2"';

if ('MediaSource' in window && MediaSource.isTypeSupported(mimeCodec)) {
    let mediaSource = new MediaSource();
    //console.log(mediaSource.readyState); // closed
    video.src = URL.createObjectURL(mediaSource);

    mediaSource.addEventListener('sourceclose', evt => {
        console.log('0000000000', evt);
    });

    mediaSource.addEventListener('sourceopen', () => {
        console.log('==========', mediaSource.readyState);

        sourceBuffer = mediaSource.addSourceBuffer(mimeCodec);

        sourceBuffer.addEventListener('updateend', function (evt) {
            console.log(';;;;;;;;;;;', mediaSource.readyState);

            // mediaSource.endOfStream();
            video.play();
            //console.log(mediaSource.readyState); // ended
        });
    });
} else {
    console.error('Unsupported MIME type or codec: ', mimeCodec);
}
