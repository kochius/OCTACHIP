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
    
    const startEmulator = (rom) => {
        loadRom(rom.filename);
        setSpeed(rom.speed);
    
        Module.ccall("main", "null", [], []);
        Module.running = true;
    };
    
    const stopEmulator = () => {
        Module.ccall("stop", "null", [], []);
        Module.running = false;
        Module.paused = false;
    };
    
    const pauseEmulator = () => {
        Module.ccall("pause", "null", [], []);
        Module.paused = true;
    };
    
    const resumeEmulator = () => {
        Module.ccall("resume", "null", [], []);
        Module.paused = false;
    };

    return {
        loadRom,
        setSpeed,
        startEmulator,
        stopEmulator,
        pauseEmulator,
        resumeEmulator,
    }
}