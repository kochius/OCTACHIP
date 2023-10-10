// get rid of size methods
import { createMonitor } from "./scripts/monitor.js";
import { createKeypad } from "./scripts/keypad.js";
import { fetchRomsMetadata } from "./scripts/utils.js";
import { createEmulatorControls } from "./scripts/emulator_controls.js";

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

Module["onRuntimeInitialized"] = async () => {
    const hwMonitor = createMonitor();
    const emulatorControls = createEmulatorControls();

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
            emulatorControls.loadRom(selectedRom.filename);
            emulatorControls.setSpeed(selectedRom.speed);
        }
    });

    startStopButton.addEventListener("click", () => {
        if (Module.running) {
            emulatorControls.stopEmulator();
            hwMonitor.updateMonitoringInfo();
        }
        else {
            const selectedRom = roms[romSelector.value];
            emulatorControls.startEmulator(selectedRom);
            hwMonitor.startMonitoring();
        }
    });

    pauseResumeButton.addEventListener("click", () => {
        if (Module.paused) {
            emulatorControls.resumeEmulator();
            hwMonitor.startMonitoring();
        }
        else {
            emulatorControls.pauseEmulator();
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

    const keypad = createKeypad();
    const keypadToggle = document.querySelector("#keypad-toggle");
    keypadToggle.addEventListener("change", () => {
        if (keypadToggle.checked) {
            keypad.addKeypad();
        }
        else {
            keypad.removeKeypad();
        }
    });

    const initialRom = roms[romSelector.value];
    setRomDescription(initialRom.description);
    hwMonitor.updateMonitoringInfo();
};