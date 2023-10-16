export const fetchRomsMetadata = async () => {
  try {
    const response = await fetch("roms.json");

    if (!response.ok) {
      throw new Error(`HTTP status ${response.status}`);
    }

    return await response.json();
  } catch (error) {
    console.error(`Failed to fetch roms.json: ${error.message}`);
    return [];
  }
};

export const hexFormat = (value, numDigits) => {
  return `0x${value.toString(16).toUpperCase().padStart(numDigits, "0")}`;
};
