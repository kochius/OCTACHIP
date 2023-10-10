import { createKeypad } from "./keypad.js"

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

    const showKeypad = () => {
        keypad.addKeypad();
    };

    const hideKeypad = () => {
        keypad.removeKeypad();
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

    return {
        buildRomDropdown,
        setRomDescription,
        showKeypad,
        hideKeypad,
        closeModal,
    }
}