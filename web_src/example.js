let bloatyModule;
let currentFileData = null;
let currentFileName = "";

// Initialize the WASM module
BloatyModule().then(module => {
    bloatyModule = module;
    document.getElementById('status').innerText = 'WASM Module Loaded and Ready!';
    document.getElementById('run-btn').disabled = false;
    console.log('Bloaty WASM module loaded');
}).catch(err => {
    console.error('Failed to load Bloaty WASM module:', err);
    document.getElementById('status').innerText = 'Error loading WASM module: ' + err.message;
});

// UI elements
const dropZone = document.getElementById('drop-zone');
const fileInput = document.getElementById('file-input');
const runBtn = document.getElementById('run-btn');
const outputContainer = document.getElementById('output-container');
const dataSourceSelect = document.getElementById('data-source');

// Handle file selection
dropZone.onclick = () => fileInput.click();

fileInput.onchange = (e) => {
    if (e.target.files.length > 0) {
        handleFile(e.target.files[0]);
    }
};

dropZone.ondragover = (e) => {
    e.preventDefault();
    dropZone.classList.add('dragover');
};

dropZone.ondragleave = () => {
    dropZone.classList.remove('dragover');
};

dropZone.ondrop = (e) => {
    e.preventDefault();
    dropZone.classList.remove('dragover');
    if (e.dataTransfer.files.length > 0) {
        handleFile(e.dataTransfer.files[0]);
    }
};

function handleFile(file) {
    currentFileName = file.name;
    const reader = new FileReader();
    reader.onload = (e) => {
        currentFileData = new Uint8Array(e.target.result);
        document.getElementById('status').innerText = `File "${file.name}" loaded (${file.size} bytes)`;
        runBloaty();
    };
    reader.readAsArrayBuffer(file);
}

// Run Bloaty
runBtn.onclick = runBloaty;

function runBloaty() {
    if (!bloatyModule || !currentFileData) {
        if (!currentFileData) alert('Please select a file first');
        return;
    }

    outputContainer.innerText = 'Analyzing...';

    // We need to use the FS to make the file available to Bloaty's C++ code
    const virtualPath = '/' + currentFileName;

    try {
        // Write file to virtual file system
        bloatyModule.FS.writeFile(virtualPath, currentFileData);

        // Prepare arguments
        const dataSource = dataSourceSelect.value;
        const args = new bloatyModule.StringVector();
        args.push_back('bloaty');
        args.push_back('-d');
        args.push_back(dataSource);
        args.push_back(virtualPath);

        // Run Bloaty
        console.log(`Running bloaty -d ${dataSource} ${virtualPath}`);
        const result = bloatyModule.run_bloaty(args);

        // Display output
        outputContainer.innerText = result || 'No output from Bloaty';

        // Clean up virtual file
        bloatyModule.FS.unlink(virtualPath);
        args.delete();
    } catch (err) {
        console.error('Bloaty error:', err);
        outputContainer.innerText = 'Error running Bloaty: ' + err.message;
    }
}
