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

const hexFormat = (value, numDigits) => {
    return `0x${value.toString(16).toUpperCase().padStart(numDigits, "0")}`;
}

const updateMonitoringInfo = () => {
    const pcOutput = document.querySelector("#pc-output");
    const iOutput = document.querySelector("#i-output");
    const spOutput = document.querySelector("#sp-output");
    const dtOutput = document.querySelector("#dt-output");
    const stOutput = document.querySelector("#st-output");
    
    const pcValue = Module.ccall("getProgramCounterValue", "number", [], []);
    const iValue = Module.ccall("getIndexRegisterValue", "number", [], []);
    const spValue = Module.ccall("getStackPointerValue", "number", [], []);
    const dtValue = Module.ccall("getDelayTimerValue", "number", [], []);
    const stValue = Module.ccall("getSoundTimerValue", "number", [], []);

    pcOutput.textContent = hexFormat(pcValue, 4);
    iOutput.textContent = hexFormat(iValue, 4);
    spOutput.textContent = hexFormat(spValue, 2);
    dtOutput.textContent = hexFormat(dtValue, 2);
    stOutput.textContent = hexFormat(stValue, 2);

    const vRegCount = 16;
    for (let i = 0; i < vRegCount; i++) {
        const hexIndex = i.toString(16).toUpperCase();
        const vRegOutput = document.querySelector(`#v${hexIndex}-output`);
        const vRegValue = Module.ccall("getRegisterValue", "number", ["number"], 
            [i]);
        vRegOutput.textContent = hexFormat(vRegValue, 2);
    }
}

const startMonitoring = () => {
    updateMonitoringInfo();

    if (!Module.running || Module.paused) {
        return;
    }

    requestAnimationFrame(startMonitoring);
}

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
            updateMonitoringInfo();
        }
        else {
            const selectedRom = roms[romSelector.value];
            startEmulator(selectedRom);
            startMonitoring();
        }
    });

    pauseResumeButton.addEventListener("click", () => {
        if (Module.paused) {
            resumeEmulator();
            startMonitoring();
        }
        else {
            pauseEmulator();
            updateMonitoringInfo();
        }
    });

    const initialRom = roms[romSelector.value];
    setRomDescription(initialRom.description);
    updateMonitoringInfo();
};