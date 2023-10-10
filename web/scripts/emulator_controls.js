export const createEmulatorControls = () => {
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
    
        const startStopButton = document.querySelector("#start-stop-button");
        startStopButton.textContent = "Stop";
    
        const pauseResumeButton = document.querySelector("#pause-resume-button");
        pauseResumeButton.disabled = false;
    };
    
    const stopEmulator = () => {
        Module.ccall("stop", "null", [], []);
        Module.running = false;
        Module.paused = false;
    
        const startStopButton = document.querySelector("#start-stop-button");
        startStopButton.textContent = "Start";
    
        const pauseResumeButton = document.querySelector("#pause-resume-button");
        pauseResumeButton.textContent = "Pause";
        pauseResumeButton.disabled = true;
    };
    
    const pauseEmulator = () => {
        Module.ccall("pause", "null", [], []);
        Module.paused = true;
    
        const pauseResumeButton = document.querySelector("#pause-resume-button");
        pauseResumeButton.textContent = "Resume";
    };
    
    const resumeEmulator = () => {
        Module.ccall("resume", "null", [], []);
        Module.paused = false;
    
        const pauseResumeButton = document.querySelector("#pause-resume-button");
        pauseResumeButton.textContent = "Pause";
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