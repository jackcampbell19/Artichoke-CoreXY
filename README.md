# Artichoke-CoreXY Controller
Controller module for the Artichoke Core XY.

## I2C Commands
Below are the avaliable commands that can be send to the controller over I2C.

Each command consists of one `command` byte followed by 0 or more `data` bytes. The `command` byte consists of 4 `code` bits and 4 `flag` bits. The code bits are extracted using the bit mask `0b11110000` and the `flag` bits are extracted using the bit mask `0b00001111`. The `data` bytes are unique to each command.

### `Home`
* Code: `0b0000` (aka. `0`)
* Flag:
   * All axis: `0b0000`
   * X axis only: `0b0001`
   * Y axis only: `0b0010`
   * Z axis only: `0b0011`
   * X and Y axis: `0b0100`
* Data bytes: `none`
* Total bytes: `1`

### `Move Line`
* Code: `0b0001` (aka. `1`)
* Flag:
   * Machine space: `0b0____`
   * Subspace: `0b1____`
   * Axis filter: `0b_XX_`
      * X only: `01`
      * Y only: `10`
      * Z only: `11`
* Data bytes:
    * X position: `2 bytes`
    * Y position: `2 bytes`
    * Z position: `2 bytes`
* Total bytes: `7`

### `Exchange Tool`
* Code: `0b0010` (aka. `2`)
* Flag: `Tool index (4 bits)`
* Data bytes: `none`
* Total bytes: `1`

### `Cup Position`
* Code: `0b0011` (aka. `3`)
* Flag:
   * Cup holder position: `0b_XXX`
   * Move tool: `0b1___`
* Data bytes: `none`
* Total bytes: `1`

### `Dispense Paint`
* Code: `0b0100` (aka. `4`)
* Flag: `none`
* Data bytes: `1 byte per color`
* Total bytes: `21`

### `Configure`
* Code: `0b0101` (aka. `5`)
* Flag: `none`
* Data bytes:
   * Parameter ID: `1 byte`
      * Configure speed: `0b00000000` (aka. `0`): ramp, min, max (each 2 bytes)
   * Value bytes: `24 bytes`
* Total bytes: `26`

### `Measure`
* Code: `0b0110` (aka. `6`)
* Flag:
   * X: `0b0000`
   * Y: `0b0001`
   * Z: `0b0010`
* Data bytes: `none`
* Total bytes: `1`

### `Move Arc`
* Code: `0b0111` (aka. `7`)
* Flag:
   * Default speed ramp: `0b_000`
   * Fast speed ramp: `0b_001`
   * Clockwise: `0b0____`
   * Counter-clockwise: `0b1___`
* Data bytes:
    * Center X position: `2 bytes`
    * Center Y position: `2 bytes`
    * Center Z position: `2 bytes`
    * Radius X position: `2 bytes`
    * Radius Y position: `2 bytes`
    * Radius Z position: `2 bytes`
    * Rotation Degrees: `2 bytes`
* Total bytes: `15`

### `Wash Tool`
* Code: `0b1000` (aka. `8`)
* Flag: `none`
* Data bytes: `none`
* Total bytes: `1`