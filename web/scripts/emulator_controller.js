export const createEmulatorController = () => {
    const loadRom = (filename) => {
        try {
            const encoder = new TextEncoder();
            const romPath = `roms/${filename}\0`;
            const encodedPath = encoder.encode(romPath);
            window.Module.ccall("loadRom", null, ["array"], [encodedPath]);
        }
        catch (error) {
            console.error(`Failed to load ROM: ${error.message}`);
        }
    };

    const getDisassembledInstructions = () => {
        const instructionsText = window.Module.getDisassembledInstructions();
        return instructionsText.split("\n");
    };
    
    const setSpeed = (emulationSpeed) => {
        window.Module.ccall("setSpeed", null, ["number"], [emulationSpeed]);
    };

    const setQuirk = (method, isEnabled) => {
        window.Module.ccall(method, null, ["number"], [isEnabled ? 1 : 0]);
    }
    
    const startEmulator = (rom) => {
        loadRom(rom.filename);
        setSpeed(rom.speed);
        setQuirk("setLoadStoreQuirk", rom.loadStoreQuirk);
        setQuirk("setShiftQuirk", rom.shiftQuirk);
        setQuirk("setWrapQuirk", rom.wrapQuirk);
    
        window.Module.ccall("main", "null", [], []);
    };
    
    const stopEmulator = () => {
        window.Module.ccall("stop", "null", [], []);
    };
    
    const pauseEmulator = () => {
        window.Module.ccall("pause", "null", [], []);
    };
    
    const resumeEmulator = () => {
        window.Module.ccall("resume", "null", [], []);
    };

    return {
        loadRom,
        getDisassembledInstructions,
        setSpeed,
        setQuirk,
        startEmulator,
        stopEmulator,
        pauseEmulator,
        resumeEmulator,
    }
}