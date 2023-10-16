import { hexFormat } from "./utils.js";

export const createMonitor = () => {
    const V_REG_COUNT = 16;
    const STACK_SIZE = 16;
    const PC_INDEX = 0;
    const SP_INDEX = 2;

    const createDataEntry = (selector, formatLength, getter, arg=null) => {
        return {
            element: document.querySelector(selector),
            value: null,
            formatLength,
            getter,
            arg,
        };
    };

    let specialRegisters = [
        createDataEntry("#pc-output", 4, "getProgramCounterValue"),
        createDataEntry("#i-output", 4, "getIndexRegisterValue"),
        createDataEntry("#sp-output", 2, "getStackPointerValue"),
        createDataEntry("#dt-output", 2, "getDelayTimerValue"),
        createDataEntry("#st-output", 2, "getSoundTimerValue")
    ];

    let vRegisters = [];
    for (let i = 0; i < V_REG_COUNT; i++) {
        const hexIndex = i.toString(16).toUpperCase();
        const vRegData = createDataEntry(`#v${hexIndex}-output`, 2, "getRegisterValue", i);
        vRegisters.push(vRegData);
    }

    let stack = [];
    for (let i = 0; i < STACK_SIZE; i++) {
        const stackData = createDataEntry(`#stack-output-${i}`, 4, "getStackValue", i);
        stack.push(stackData);
    }

    const displayOutputValue = (outputElement, value, formatLength) => {
        outputElement.textContent = hexFormat(value, formatLength);
    };

    const updateData = (dataArr) => {
        dataArr.forEach((item) => {
            let argTypes = item.arg !== null ? ["number"] : [];
            let args = item.arg !== null ? [item.arg] : [];
            const value = window.Module.ccall(item.getter, "number", argTypes, args);
            if (value !== item.value) {
                item.value = value;
                displayOutputValue(item.element, item.value, item.formatLength);
            }
        })
    };

    const updateStackPointer = () => {
        const previousStackTop = document.querySelector(".stack-top");
        previousStackTop?.classList.remove("stack-top");

        const spValue = specialRegisters[SP_INDEX].value;
        const stackTop = document.querySelector(`#stack-level-${spValue}`);
        stackTop.classList.add("stack-top");
    };

    const instructionsContainer = document.querySelector("#instructions-container");

    const updateCurrentInstruction = () => {
        const oldInstruction = document.querySelector(".current-instruction");
        if (oldInstruction) {
            oldInstruction.classList.remove("current-instruction");
        }

        const pcValue = specialRegisters[PC_INDEX].value;
        const newInstruction = document.querySelector(`#instruction-${pcValue}`);
        if (newInstruction) {
            newInstruction.classList.add("current-instruction");
            instructionsContainer.scrollTo(0, newInstruction.offsetTop - 
                newInstruction.parentElement.offsetTop - 60);
        }
    }
    
    const updateAllInfo = () => {
        updateData(specialRegisters);
        updateData(vRegisters);
        updateData(stack);
        updateStackPointer();
        updateCurrentInstruction();
    };
    
    let requestID;

    const startMonitoring = () => {
        updateAllInfo();
        requestID = requestAnimationFrame(startMonitoring);
    };

    const cancelMonitoring = () => {
        cancelAnimationFrame(requestID);
    };

    return {
        updateAllInfo,
        startMonitoring,
        cancelMonitoring,
    };
};