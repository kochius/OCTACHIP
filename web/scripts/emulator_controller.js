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
    
        const startButton = document.querySelector("#start-button");
        startButton.textContent = "Stop";
    
        const pauseButton = document.querySelector("#pause-button");
        pauseButton.disabled = false;
    };
    
    const stopEmulator = () => {
        Module.ccall("stop", "null", [], []);
        Module.running = false;
        Module.paused = false;
    
        const startButton = document.querySelector("#start-button");
        startButton.textContent = "Start";
    
        const pauseButton = document.querySelector("#pause-button");
        pauseButton.textContent = "Pause";
        pauseButton.disabled = true;
    };
    
    const pauseEmulator = () => {
        Module.ccall("pause", "null", [], []);
        Module.paused = true;
    
        const pauseButton = document.querySelector("#pause-button");
        pauseButton.textContent = "Resume";
    };
    
    const resumeEmulator = () => {
        Module.ccall("resume", "null", [], []);
        Module.paused = false;
    
        const pauseButton = document.querySelector("#pause-button");
        pauseButton.textContent = "Pause";
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