export const createEmulatorController = () => {
    const loadRom = (filename) => {
        try {
            const encoder = new TextEncoder();
            const romPath = `roms/${filename}\0`;
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

    const setQuirk = (method, isEnabled) => {
        Module.ccall(method, null, ["number"], [isEnabled ? 1 : 0]);
    }
    
    const startEmulator = (rom) => {
        loadRom(rom.filename);
        setSpeed(rom.speed);
        setQuirk("setShiftQuirk", rom.shiftQuirk);
        setQuirk("setClipQuirk", rom.clipQuirk);
    
        Module.ccall("main", "null", [], []);
    };
    
    const stopEmulator = () => {
        Module.ccall("stop", "null", [], []);
    };
    
    const pauseEmulator = () => {
        Module.ccall("pause", "null", [], []);
    };
    
    const resumeEmulator = () => {
        Module.ccall("resume", "null", [], []);
    };

    return {
        loadRom,
        setSpeed,
        setQuirk,
        startEmulator,
        stopEmulator,
        pauseEmulator,
        resumeEmulator,
    }
}