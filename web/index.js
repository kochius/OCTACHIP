var Module = {
    canvas: (() => {
        var canvas = document.getElementById('canvas');

        // As a default initial behavior, pop up an alert when webgl context is lost. To make your
        // application robust, you may want to override this behavior before shipping!
        // See http://www.khronos.org/registry/webgl/specs/latest/1.0/#5.15.2
        canvas.addEventListener("webglcontextlost", (e) => { alert('WebGL context lost. You will need to reload the page.'); e.preventDefault(); }, false);

        return canvas;
    })(),
    running: true,
    paused: false,
};

const stopStartButton = document.querySelector("#stop-start-button");
const pauseResumeButton = document.querySelector("#pause-resume-button");

stopStartButton.addEventListener("click", () => {
    if (Module.running) {
        Module.ccall("stop", "null", [], []);
        Module.running = false;
        stopStartButton.innerText = "Start";

        Module.paused = false;
        pauseResumeButton.innerText = "Pause";
        pauseResumeButton.disabled = true;
    }
    else {
        Module.ccall("main", "null", [], []);
        Module.running = true;
        stopStartButton.innerText = "Stop";

        pauseResumeButton.disabled = false;
    }
});

pauseResumeButton.addEventListener("click", () => {
    if (Module.paused) {
        Module.ccall("resume", "null", [], []);
        Module.paused = false;
        pauseResumeButton.innerText = "Pause";
    }
    else {
        Module.ccall("pause", "null", [], []);
        Module.paused = true;
        pauseResumeButton.innerText = "Resume";
    }
});