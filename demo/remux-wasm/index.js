let inputFile = document.querySelector('#js_input_file')

worker = new Worker('/demo/remux-wasm/worker.js')


inputFile.addEventListener('change', () => {
    const file = inputFile.files[0];

    // const fileReader = new FileReader()
    // fileReader.addEventListener('load', () => {
    //     const fileBuffer = new Uint8Array(fileReader.result)

    //     console.log(fileBuffer, syncOffset(fileBuffer));
    // })

    // fileReader.readAsArrayBuffer(file)

    worker.postMessage({
        type: 'remux',
        data: {
            file,
        }
    })



})

