export const hexFormat = (value, numDigits) => {
    return `0x${value.toString(16).toUpperCase().padStart(numDigits, "0")}`;
}