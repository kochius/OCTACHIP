// get rid of size methods
import { createMonitor } from "./scripts/monitor.js";

window.Module = {
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
const romDescription = document.querySelector("#rom-description");
const startStopButton = document.querySelector("#start-stop-button");
const pauseResumeButton = document.querySelector("#pause-resume-button");
const settingsButton = document.querySelector("#settings-button");
const settingsMenu = document.querySelector("#settings-menu");
const settingsMenuCloseButton = document.querySelector("#settings-menu-close-button");

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

const closeModalCallback = (event) => {
    const modal = event.target;
    modal.close();
    modal.removeEventListener("animationend", closeModalCallback);
    modal.classList.remove("close");
};

const closeModal = (modal) => {
    modal.addEventListener("animationend", closeModalCallback);
    modal.classList.add("close");
};

let pressedKeys = new Array(16).fill(false);

const onKeyEvent = (key, down) => {
    const keyMap = new Map([
        ["0", "x"],
        ["1", "1"],
        ["2", "2"],
        ["3", "3"],
        ["4", "q"],
        ["5", "w"],
        ["6", "e"],
        ["7", "a"],
        ["8", "s"],
        ["9", "d"],
        ["A", "z"],
        ["B", "c"],
        ["C", "4"],
        ["D", "r"],
        ["E", "f"],
        ["F", "v"]
    ]);
    const keyCharCode = keyMap.get(key).charCodeAt(0);
    if (down) {
        Module.ccall("pushKeyDownEvent", null, ["number"], [keyCharCode]);
    }
    else {
        Module.ccall("pushKeyUpEvent", null, ["number"], [keyCharCode]);
    }
};

const onKeyDown = (event) => {
    event.preventDefault();
    onKeyEvent(event.target.value, true);
    event.target.classList.add("active");

    pressedKeys[event.target.id] = true;
};

const onKeyUp = (event) => {
    event.preventDefault();
    if (pressedKeys[event.target.id]) {
        onKeyEvent(event.target.value, false);
        event.target.classList.remove("active");

        pressedKeys[event.target.id] = false;
    }
};

const addKeypad = () => {
    const keypad = document.createElement("div");
    keypad.id = "keypad";

    keypad.addEventListener("touchstart", (event) => {event.preventDefault()});
    keypad.addEventListener("contextmenu", (event) => {event.preventDefault()});

    const keys = ["1", "2", "3", "C",
                  "4", "5", "6", "D",
                  "7", "8", "9", "E",
                  "A", "0", "B", "F"];
    keys.forEach((key, index) => {
        const keypadButton = document.createElement("button");
        keypadButton.textContent = key;
        keypadButton.id = index;
        keypadButton.classList.add("keypad-button");
        keypadButton.setAttribute("type", "button");
        keypadButton.setAttribute("autocomplete", "off");
        keypadButton.setAttribute("value", key);

        keypadButton.addEventListener("touchstart", onKeyDown);
        keypadButton.addEventListener("touchend", onKeyUp);

        keypadButton.addEventListener("mousedown", onKeyDown);
        keypadButton.addEventListener("mouseup", onKeyUp);
        keypadButton.addEventListener("mouseleave", onKeyUp);
        
        keypadButton.addEventListener("contextmenu", (event) => {event.preventDefault()});

        keypad.appendChild(keypadButton);
    });
    const controlsCard = document.querySelector("#controls-card");
    const mainGrid = document.querySelector(".main-grid");
    mainGrid.insertBefore(keypad, controlsCard);
    mainGrid.classList.add("keypad-enabled");
};

const removeKeypad = () => {
    const keypad = document.querySelector("#keypad");
    if (keypad) {
        keypad.remove();
        const mainGrid = document.querySelector(".main-grid");
        mainGrid.classList.remove("keypad-enabled");
    }
};

Module["onRuntimeInitialized"] = async () => {
    const hwMonitor = createMonitor();

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
            hwMonitor.updateMonitoringInfo();
        }
        else {
            const selectedRom = roms[romSelector.value];
            startEmulator(selectedRom);
            hwMonitor.startMonitoring();
        }
    });

    pauseResumeButton.addEventListener("click", () => {
        if (Module.paused) {
            resumeEmulator();
            hwMonitor.startMonitoring();
        }
        else {
            pauseEmulator();
            hwMonitor.updateMonitoringInfo();
        }
    });

    settingsButton.addEventListener("click", () => {
        settingsMenu.showModal();
    });

    settingsMenuCloseButton.addEventListener("click", () => {
        closeModal(settingsMenu);
    });

    settingsMenu.addEventListener("click", (event) => {
        if (event.target.id === "settings-menu") {
            closeModal(settingsMenu);
        }
    });

    const keypadToggle = document.querySelector("#keypad-toggle");
    keypadToggle.addEventListener("change", () => {
        if (keypadToggle.checked) {
            addKeypad();
        }
        else {
            removeKeypad();
        }
    });

    const initialRom = roms[romSelector.value];
    setRomDescription(initialRom.description);
    hwMonitor.updateMonitoringInfo();
};