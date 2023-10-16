import { createApp } from "./scripts/app.js";

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

window.Module["onRuntimeInitialized"] = async () => {
    const octachipApp = createApp();
    octachipApp.init();
};