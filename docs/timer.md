# Handling Timers

## Timer Subsystem

The CHIP-8 system defines two 8-bit timers: a **delay timer** and a **sound timer**. Both timers decrement at a rate of 60Hz and are used for timing-sensitive operations in games and applications.

This module handles initialization, decrementing, platform beep triggering, and accessors for both timers.

---

## Header: `timer.h`

### API

```c
void timer_init(Chip8 *chip8);
void timer_update(Chip8 *chip8);

uint8_t get_delay_timer(Chip8 *chip8);
uint8_t get_sound_timer(Chip8 *chip8);

void set_delay_timer(Chip8 *chip8, uint8_t value);
void set_sound_timer(Chip8 *chip8, uint8_t value);

// Optional SDL audio hooks
void audio_init(void);
void audio_quit(void);
void audio_callback(void *userdata, uint8_t *stream, int len);
```

- `timer_init`: Resets both timers to 0
- `timer_update`: Called at 60Hz to decrement timers and control sound
- `get_*` and `set_*`: Read and write individual timer values
- `audio_*`: SDL audio integration for native sound playback

---

## Implementation: `timer.c`

### `timer_init`

```c
void timer_init(Chip8 *chip8)
```

- Sets `chip8->delay_timer = 0`
- Sets `chip8->sound_timer = 0`
- Called during `chip8_init`

---

### `timer_update`

```c
void timer_update(Chip8 *chip8)
```

- Decrements `delay_timer` if greater than 0
- Decrements `sound_timer` if greater than 0
- Triggers `platform_play_beep(true)` when sound timer is active
- Calls `platform_play_beep(false)` when sound timer reaches zero

This function must be called at a stable 60Hz interval. In this implementation, it is called once per emulation cycle inside `chip8_cycle`.

---

### Accessors

```c
uint8_t get_delay_timer(Chip8 *chip8);
uint8_t get_sound_timer(Chip8 *chip8);

void set_delay_timer(Chip8 *chip8, uint8_t value);
void set_sound_timer(Chip8 *chip8, uint8_t value);
```

Used by opcode handlers:

- `Fx07`: `Vx = delay_timer`
- `Fx15`: `delay_timer = Vx`
- `Fx18`: `sound_timer = Vx`

---

## Sound Timer

The `sound_timer` emits a tone while its value is nonzero. In SDL builds, this tone is generated using a simple square wave inside `audio_callback()` and toggled via `SDL_PauseAudioDevice`.

In WASM builds, sound is triggered via JavaScript using:

```c
Module.toggleBeep(true or false)
```

---

## Audio Integration (SDL Only)

If SDL audio is initialized (optional), the following behavior occurs:

- `audio_callback`: Generates a 440Hz square wave
- `audio_init`: Configures and opens an SDL audio device
- `audio_quit`: Cleans up audio resources
- Controlled via `platform_play_beep(bool)`

---

## Usage in Opcode Handlers

These instructions directly rely on timer values:

- `Fx07`: Read delay timer
- `Fx15`: Write delay timer
- `Fx18`: Write sound timer

---

## Notes

- Timers tick once per frame, not per opcode
- The emulator assumes a stable execution rate to maintain accurate timing
- The sound system is extensible for more advanced audio feedback if desired

---
