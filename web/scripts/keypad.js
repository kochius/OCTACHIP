export const createKeypad = () => {
    const KEYS = [
        "1", "2", "3", "C",
        "4", "5", "6", "D",
        "7", "8", "9", "E",
        "A", "0", "B", "F"
    ];

    const KEY_MAP = new Map([
        ["0", "x"],
        ["1", "1"],
        ["2", "2"],
        ["3", "3"],
        ["4", "q"],
        ["5", "w"],
        ["6", "e"],
        ["7", "a"],
        ["8", "s"],
        ["9", "d"],
        ["A", "z"],
        ["B", "c"],
        ["C", "4"],
        ["D", "r"],
        ["E", "f"],
        ["F", "v"]
    ]);

    let pressedKeys = new Array(16).fill(false);

    const onKeyEvent = (key, down) => {
        const keyCharCode = KEY_MAP.get(key).charCodeAt(0);
        const method = down ? "pushKeyDownEvent" : "pushKeyUpEvent"
        Module.ccall(method, null, ["number"], [keyCharCode]);
    };

    const onKeyDown = (event) => {
        event.preventDefault();
        onKeyEvent(event.target.value, true);
        event.target.classList.add("active");
        pressedKeys[event.target.id] = true;
    };

    const onKeyUp = (event) => {
        event.preventDefault();
        if (pressedKeys[event.target.id]) {
            onKeyEvent(event.target.value, false);
            event.target.classList.remove("active");
            pressedKeys[event.target.id] = false;
        }
    };

    const createButton = (keyText, index) => {
        const btn = document.createElement("button");
        btn.textContent = keyText;
        btn.id = index;
        btn.classList.add("keypad-button");
        btn.setAttribute("type", "button");
        btn.setAttribute("autocomplete", "off");
        btn.setAttribute("value", keyText);
        btn.addEventListener("contextmenu", (event) => {event.preventDefault()});

        const touchEvents = ["touchstart", "touchend"];
        touchEvents.forEach((event) => {
            btn.addEventListener(event, event === "touchstart" ? onKeyDown : onKeyDown);
        })

        const mouseEvents = ["mousedown", "mouseup", "mouseleave"]
        mouseEvents.forEach((event) => {
            btn.addEventListener(event, event === "mousedown" ? onKeyDown : onKeyUp)
        })

        return btn;
    };

    const addKeypad = () => {
        const keypad = document.createElement("div");
        keypad.id = "keypad";
        keypad.addEventListener("touchstart", (event) => {event.preventDefault()});
        keypad.addEventListener("contextmenu", (event) => {event.preventDefault()});

        KEYS.forEach((keyText, index) => {
            keypad.appendChild(createButton(keyText, index));
        });
        
        const controlsCard = document.querySelector("#controls-card");
        const mainGrid = document.querySelector(".main-grid");
        mainGrid.insertBefore(keypad, controlsCard);
        mainGrid.classList.add("keypad-enabled");
    };

    const removeKeypad = () => {
        const keypad = document.querySelector("#keypad");
        if (keypad) {
            keypad.remove();
            const mainGrid = document.querySelector(".main-grid");
            mainGrid.classList.remove("keypad-enabled");
        }
    };

    return {
        addKeypad,
        removeKeypad
    };
};