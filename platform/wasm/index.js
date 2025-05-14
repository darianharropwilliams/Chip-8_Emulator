/**
 * index.js — WebAssembly Frontend for CHIP-8 Emulator
 *
 * This file bootstraps the CHIP-8 emulator in a browser environment using
 * Emscripten-generated WebAssembly. It:
 * 
 * - Sets up audio beep control via the Web Audio API
 * - Maps physical keyboard input to CHIP-8 keypad state
 * - Renders the emulator's 64x32 framebuffer to an HTML canvas
 * - Loads ROMs via file picker or HTTP from /roms/
 * - Bridges WebAssembly exports (`wasm_init`, `wasm_cycle`, `wasm_load_rom`)
 * - Runs a frame-locked main emulation loop (approx. 700Hz)
 *
 * This is the entry point for the browser version of the emulator.
 */


// === Web Audio API Beep Control ===
let audioCtx = null;
let oscillator = null;
let gainNode = null;
let audioInitialized = false;

// Initialize audio context and gain node once on first user interaction
function initAudio() {
  if (audioInitialized) return;

  audioCtx = new (window.AudioContext || window.webkitAudioContext)();
  if (audioCtx.state === "suspended") {
    audioCtx.resume();
  }

  gainNode = audioCtx.createGain();
  gainNode.gain.value = 0.1; // Soft volume level
  gainNode.connect(audioCtx.destination);
  audioInitialized = true;
}

// Enable or disable the beep oscillator
function toggleBeep(active) {
  if (!audioInitialized) return;

  if (active && !oscillator) {
    oscillator = audioCtx.createOscillator();
    oscillator.type = "square";
    oscillator.frequency.value = 440;
    oscillator.connect(gainNode);
    oscillator.start();
  } else if (!active && oscillator) {
    oscillator.stop();
    oscillator.disconnect();
    oscillator = null;
  }
}

// Ensure audio context unlocks on first input (required by browsers)
document.addEventListener("keydown", initAudio, { once: true });
document.addEventListener("mousedown", initAudio, { once: true });
document.addEventListener("touchstart", initAudio, { once: true });

// Load and initialize the WebAssembly module (via Emscripten glue)
Chip8Emulator().then((Module) => {
  // === Platform Hooks ===
  Module.keyState = new Array(16).fill(0);     // CHIP-8 keypad state (16 keys)
  Module.toggleBeep = toggleBeep;              // Hook into CHIP-8 sound logic

  // === Keyboard Mapping ===
  const keyMap = {
    'x': 0x0, '1': 0x1, '2': 0x2, '3': 0x3,
    'q': 0x4, 'w': 0x5, 'e': 0x6, 'a': 0x7,
    's': 0x8, 'd': 0x9, 'z': 0xA, 'c': 0xB,
    '4': 0xC, 'r': 0xD, 'f': 0xE, 'v': 0xF
  };

  // Update key state on keydown
  document.addEventListener("keydown", (e) => {
    const key = e.key.toLowerCase();
    if (key in keyMap) Module.keyState[keyMap[key]] = 1;
  });

  // Reset key state on keyup
  document.addEventListener("keyup", (e) => {
    const key = e.key.toLowerCase();
    if (key in keyMap) Module.keyState[keyMap[key]] = 0;
  });

  // === Canvas Renderer ===
  Module.renderToCanvas = function(pixels) {
    const canvas = document.getElementById("screen");
    const ctx = canvas.getContext("2d");
    const scale = 10;

    ctx.fillStyle = "black";
    ctx.fillRect(0, 0, canvas.width, canvas.height);

    ctx.fillStyle = "white";
    for (let y = 0; y < 32; y++) {
      for (let x = 0; x < 64; x++) {
        if (pixels[y * 64 + x]) {
          ctx.fillRect(x * scale, y * scale, scale, scale);
        }
      }
    }
  };

  // === ROM Loader from Memory ===
  function loadROM(rom) {
    const ptr = Module._malloc(rom.length);  // Allocate memory in WASM heap
    Module.HEAPU8.set(rom, ptr);             // Copy ROM into WASM memory
    const result = Module.ccall('wasm_load_rom', 'number', ['number', 'number'], [ptr, rom.length]);
    Module._free(ptr);
    if (result !== 0) alert("ROM failed to load.");
  }

  // === ROM Loader from URL ===
  async function fetchROM(path) {
    try {
      const res = await fetch(path, { cache: "no-store" });
      if (!res.ok) throw new Error(`HTTP ${res.status}`);
      const buffer = await res.arrayBuffer();
      console.log("Fetched ROM size:", buffer.byteLength);
      loadROM(new Uint8Array(buffer));
    } catch (e) {
      console.error("ROM load error:", e);
    }
  }

  // === ROM Loader from File Picker ===
  document.getElementById("rom-picker").addEventListener("change", (e) => {
    const file = e.target.files[0];
    if (!file) return;
    const reader = new FileReader();
    reader.onload = () => loadROM(new Uint8Array(reader.result));
    reader.readAsArrayBuffer(file);
  });

  // === ROM Buttons from Index File ===
  async function populateROMButtons() {
    const container = document.getElementById("rom-links");
    try {
      const res = await fetch("roms/index.json", { cache: "no-store" });
      if (!res.ok) throw new Error(`HTTP ${res.status}`);
      const roms = (await res.json()).filter(name => name.toLowerCase() !== "index.json");

      container.innerHTML = "";
      for (const filename of roms) {
        const btn = document.createElement("button");
        btn.textContent = filename;
        btn.type = "button";
        btn.addEventListener("click", () => fetchROM(`roms/${filename}`));
        container.appendChild(btn);
      }
    } catch (err) {
      container.innerHTML = "<p>Error loading ROMs.</p>";
      console.error("Failed to load ROM list:", err);
    }
  }

  // === Main Emulation Loop ===
  let lastTime = performance.now();
  const targetHz = 700;                  // Approximate CPU frequency
  const msPerCycle = 1000 / targetHz;
  let accumulator = 0;

  function runLoop(now) {
    let delta = now - lastTime;
    lastTime = now;
    accumulator += delta;

    // Run as many cycles as needed to keep up with wall-clock time
    while (accumulator >= msPerCycle) {
      Module.ccall("wasm_cycle", null, ["number"], [1]);
      accumulator -= msPerCycle;
    }

    requestAnimationFrame(runLoop);
  }

  // === Startup Sequence ===
  Module.ccall("wasm_init");     // Initialize CHIP-8 state in WASM
  populateROMButtons();          // Load available ROM list from roms/index.json
  requestAnimationFrame(runLoop); // Begin emulation loop
});
