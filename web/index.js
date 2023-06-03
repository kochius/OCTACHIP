var Module = {
    noInitialRun: true,
    canvas: (() => {
        var canvas = document.querySelector("#canvas");
        
        canvas.addEventListener("webglcontextlost", (e) => {
            alert("WebGL context lost. You will need to reload the page.");
            e.preventDefault();
        }, false);

        return canvas;
    })(),
    running: false,
    paused: false,
};

const romSelector = document.querySelector("#rom-select");
const startStopButton = document.querySelector("#start-stop-button");
const pauseResumeButton = document.querySelector("#pause-resume-button");
const romDescription = document.querySelector("#rom-description");

const fetchRomsMetadata = async () => {
    try {
        const response = await fetch("roms.json");

        if (!response.ok) {
            throw new Error(`HTTP status ${response.status}`);
        }

        return await response.json();
    }
    catch (error) {
        console.error(`Failed to fetch roms.json: ${error.message}`);
        return [];
    }
};

const loadRom = (rom) => {
    try {
        const encoder = new TextEncoder();
        const romPath = `roms/${rom.filename}\0`;
        const encodedPath = encoder.encode(romPath);
        Module.ccall("loadRom", null, ["array"], [encodedPath]);
    }
    catch (error) {
        console.error(`Failed to load ROM: ${error.message}`);
    }
};

const setSpeed = (emulationSpeed) => {
    Module.ccall("setSpeed", null, ["number"], [emulationSpeed]);
};

const startEmulator = (rom) => {
    loadRom(rom);
    setSpeed(rom.speed);

    Module.ccall("main", "null", [], []);
    Module.running = true;
    startStopButton.textContent = "Stop";

    pauseResumeButton.disabled = false;
};

const stopEmulator = () => {
    Module.ccall("stop", "null", [], []);
    Module.running = false;
    startStopButton.textContent = "Start";

    Module.paused = false;
    pauseResumeButton.textContent = "Pause";
    pauseResumeButton.disabled = true;
};

const pauseEmulator = () => {
    Module.ccall("pause", "null", [], []);
    Module.paused = true;
    pauseResumeButton.textContent = "Resume";
};

const resumeEmulator = () => {
    Module.ccall("resume", "null", [], []);
    Module.paused = false;
    pauseResumeButton.textContent = "Pause";
};

const setRomDescription = (description) => {
    romDescription.textContent = "";

    const lines = description.split("\n");

    lines.forEach((line, index) => {
        const lineText = document.createTextNode(line);
        romDescription.appendChild(lineText);

        if (index < lines.length - 1) {
            const lineBreak = document.createElement("br");
            romDescription.appendChild(lineBreak);
        }
    });
};

Module["onRuntimeInitialized"] = async () => {
    const roms = await fetchRomsMetadata();

    roms.forEach((rom, index) => {
        const romOption = document.createElement("option");
        romOption.textContent = rom.title;
        romOption.value = index;
        romSelector.add(romOption);
    });

    romSelector.addEventListener("change", (event) => {
        const selectedRom = roms[event.target.value];
        setRomDescription(selectedRom.description);

        if (Module.running) {
            loadRom(selectedRom);
            setSpeed(selectedRom.speed);
        }
    });

    startStopButton.addEventListener("click", () => {
        if (Module.running) {
            stopEmulator();
        }
        else {
            const selectedRom = roms[romSelector.value];
            startEmulator(selectedRom);
        }
    });

    pauseResumeButton.addEventListener("click", () => {
        if (Module.paused) {
            resumeEmulator();
        }
        else {
            pauseEmulator();
        }
    });

    const initialRom = roms[romSelector.value];
    setRomDescription(initialRom.description);
};