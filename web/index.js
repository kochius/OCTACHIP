// get rid of size methods
import { createEmulatorController } from "./scripts/emulator_controller.js";
import { createUI } from "./scripts/ui.js";
import { createMonitor } from "./scripts/monitor.js";
import { fetchRomsMetadata } from "./scripts/utils.js";

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
    const ui = createUI();
    const monitor = createMonitor();

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
            monitor.updateAllInfo();
        }
        else {
            const selectedRom = roms[romSelector.value];
            emulatorController.startEmulator(selectedRom);
            monitor.startMonitoring();
        }
    });

    const pauseButton = document.querySelector("#pause-button");
    pauseButton.addEventListener("click", () => {
        if (Module.paused) {
            emulatorController.resumeEmulator();
            monitor.startMonitoring();
        }
        else {
            emulatorController.pauseEmulator();
            monitor.updateAllInfo();
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
            ui.showKeypad();
        }
        else {
            ui.hideKeypad();
        }
    });

    const initialRom = roms[romSelector.value];
    ui.setRomDescription(initialRom.description);
    monitor.updateAllInfo();
};