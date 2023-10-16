import { createKeypad } from "./keypad.js";

export const createUI = () => {
  const keypad = createKeypad();

  const buildRomDropdown = (roms) => {
    const romSelector = document.querySelector("#rom-select");
    roms.forEach((rom, index) => {
      const romOption = document.createElement("option");
      romOption.textContent = rom.title;
      romOption.value = index;
      romSelector.add(romOption);
    });
  };

  const setRomDescription = (description) => {
    const romDescription = document.querySelector("#rom-description");
    romDescription.textContent = "";

    const lines = description.split("\n");

    lines.forEach((line, index) => {
      const lineText = document.createTextNode(line);
      romDescription.appendChild(lineText);

      if (index < lines.length - 1) {
        const lineBreak = document.createElement("br");
        romDescription.appendChild(lineBreak);
      }
    });
  };

  const displayInstructions = (instructionsArr) => {
    let fragment = new DocumentFragment();
    instructionsArr.forEach((instructionText, index) => {
      let div = document.createElement("div");
      div.textContent = instructionText;
      div.id = `instruction-${0x200 + index}`;
      fragment.appendChild(div);
    });

    const instructionsContainer = document.querySelector(
      "#instructions-container",
    );
    instructionsContainer.textContent = "";
    instructionsContainer.appendChild(fragment);
  };

  const toggleStartButton = (isRunning) => {
    const startButton = document.querySelector("#start-button");
    if (isRunning) {
      startButton.textContent = "Stop";
    } else {
      startButton.textContent = "Start";
    }
  };

  const togglePauseButton = (isPaused, isRunning) => {
    const pauseButton = document.querySelector("#pause-button");
    if (isPaused) {
      pauseButton.textContent = "Resume";
    } else {
      pauseButton.textContent = "Pause";
    }
    pauseButton.disabled = !isRunning;
  };

  const toggleKeypad = (isEnabled) => {
    if (isEnabled) {
      keypad.addKeypad();
    } else {
      keypad.removeKeypad();
    }
  };

  const closeModalCallback = (event) => {
    const modal = event.target;
    modal.close();
    modal.removeEventListener("animationend", closeModalCallback);
    modal.classList.remove("close");
  };

  const closeModal = (modal) => {
    modal.addEventListener("animationend", closeModalCallback);
    modal.classList.add("close");
  };

  const toggleSettingsMenu = (isEnabled) => {
    const settingsMenu = document.querySelector("#settings-menu");
    if (isEnabled) {
      settingsMenu.showModal();
    } else {
      closeModal(settingsMenu);
    }
  };

  return {
    buildRomDropdown,
    setRomDescription,
    displayInstructions,
    toggleStartButton,
    togglePauseButton,
    toggleKeypad,
    toggleSettingsMenu,
  };
};
