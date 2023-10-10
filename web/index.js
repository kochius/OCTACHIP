// get rid of size methods
import { createMonitor } from "./scripts/monitor.js";
import { createKeypad } from "./scripts/keypad.js";
import { fetchRomsMetadata } from "./scripts/utils.js";
import { createEmulatorController } from "./scripts/emulator_controller.js";
import { createUI } from "./scripts/ui.js";

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

Module["onRuntimeInitialized"] = async () => {
    const emulatorController = createEmulatorController();
    const hwMonitor = createMonitor();
    const keypad = createKeypad();
    const ui = createUI();

    const roms = await fetchRomsMetadata();

    ui.buildRomDropdown(roms);

    const romSelector = document.querySelector("#rom-select");
    romSelector.addEventListener("change", (event) => {
        const selectedRom = roms[event.target.value];
        ui.setRomDescription(selectedRom.description);

        if (Module.running) {
            emulatorController.loadRom(selectedRom.filename);
            emulatorController.setSpeed(selectedRom.speed);
        }
    });

    const startButton = document.querySelector("#start-button");
    startButton.addEventListener("click", () => {
        if (Module.running) {
            emulatorController.stopEmulator();
            hwMonitor.updateMonitoringInfo();
        }
        else {
            const selectedRom = roms[romSelector.value];
            emulatorController.startEmulator(selectedRom);
            hwMonitor.startMonitoring();
        }
    });

    const pauseButton = document.querySelector("#pause-button");
    pauseButton.addEventListener("click", () => {
        if (Module.paused) {
            emulatorController.resumeEmulator();
            hwMonitor.startMonitoring();
        }
        else {
            emulatorController.pauseEmulator();
            hwMonitor.updateMonitoringInfo();
        }
    });

    const settingsButton = document.querySelector("#settings-button");
    const settingsMenu = document.querySelector("#settings-menu");
    settingsButton.addEventListener("click", () => {
        settingsMenu.showModal();
    });

    const settingsMenuCloseButton = document.querySelector("#settings-menu-close-button");
    settingsMenuCloseButton.addEventListener("click", () => {
        ui.closeModal(settingsMenu);
    });

    settingsMenu.addEventListener("click", (event) => {
        if (event.target.id === "settings-menu") {
            ui.closeModal(settingsMenu);
        }
    });

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
    ui.setRomDescription(initialRom.description);
    hwMonitor.updateMonitoringInfo();
};