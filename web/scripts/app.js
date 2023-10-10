import { createEmulatorController } from "./emulator_controller.js";
import { createUI } from "./ui.js";
import { createMonitor } from "./monitor.js";
import { fetchRomsMetadata } from "./utils.js";

export const createApp = () => {
    const emulatorController = createEmulatorController();
    const userInterface = createUI();
    const monitor = createMonitor();

    let selectedRom;

    const handleRomChange = (roms, romIndex) => {
        selectedRom = roms[romIndex];
        userInterface.setRomDescription(selectedRom.description);
        if (Module.running) {
            emulatorController.loadRom(selectedRom.filename);
            emulatorController.setSpeed(selectedRom.speed);
        }
    };

    const handleStartButtonClick = () => {
        if (Module.running) {
            emulatorController.stopEmulator();
            monitor.updateAllInfo();
        }
        else {
            emulatorController.startEmulator(selectedRom);
            monitor.startMonitoring();
        }
    };

    const handlePauseButtonClick = () => {
        if (Module.paused) {
            emulatorController.resumeEmulator();
            monitor.startMonitoring();
        }
        else {
            emulatorController.pauseEmulator();
            monitor.updateAllInfo();
        }
    };

    const init = async () => {    
        const roms = await fetchRomsMetadata();
        userInterface.buildRomDropdown(roms);
    
        const romSelector = document.querySelector("#rom-select");
        romSelector.addEventListener("change", (event) => {
            handleRomChange(roms, event.target.value)
        });
    
        const startButton = document.querySelector("#start-button");
        startButton.addEventListener("click", handleStartButtonClick);
    
        const pauseButton = document.querySelector("#pause-button");
        pauseButton.addEventListener("click", handlePauseButtonClick);
    
        const settingsButton = document.querySelector("#settings-button");
        settingsButton.addEventListener("click", () => {
            userInterface.toggleSettingsMenu(true);
        });

        const settingsMenuCloseButton = document.querySelector("#settings-menu-close-button");
        settingsMenuCloseButton.addEventListener("click", () => {
            userInterface.toggleSettingsMenu(false);
        });

        const settingsMenu = document.querySelector("#settings-menu");
        settingsMenu.addEventListener("click", (event) => {
            if (event.target.id === "settings-menu") {
                userInterface.toggleSettingsMenu(false);
            }
        });
    
        const keypadToggle = document.querySelector("#keypad-toggle");
        keypadToggle.addEventListener("change", (event) => {
            userInterface.toggleKeypad(event.target.checked);
        });
    
        selectedRom = roms[romSelector.value];
        userInterface.setRomDescription(selectedRom.description);
        monitor.updateAllInfo();
    };

    return {
        init,
    };
};