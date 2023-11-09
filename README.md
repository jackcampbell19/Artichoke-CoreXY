# Artichoke-CoreXY Controller
Controller module for the Artichoke Core XY.

## I2C Commands
Below are the avaliable commands that can be send to the controller over I2C.

Each command consists of one `command` byte followed by 0 or more `data` bytes. The `command` byte consists of 4 `code` bits and 4 `flag` bits. The code bits are extracted using the bit mask `0b11110000` and the `flag` bits are extracted using the bit mask `0b00001111`. The `data` bytes are unique to each command.

### `Home`
* Code: `0b0000`
* Flag:
   * All axis: `0b0000`
   * X axis only: `0b0001`
   * Y axis only: `0b0010`
   * Z axis only: `0b0011`
   * X and Y axis: `0b0100`
* Data bytes: `none`
* Total bytes: `1`

### `Move Line`
* Code: `0b0001`
* Flag:
   * Default speed ramp: `0b0000`
   * Fast speed ramp: `0b0001`
* Data bytes:
    * X position: `2 bytes`
    * Y position: `2 bytes`
    * Z position: `2 bytes`
* Total bytes: `7`

### `Exchange Tool`
* Code: `0b0010`
* Flag: `Tool index (4 bits)`
* Data bytes: `none`
* Total bytes: `1`

### `Cup Position`
* Code: `0b0011`
* Flag: `Cup holder position (4 bits)`
* Data bytes: `none`
* Total bytes: `1`

### `Dispense Paint`
* Code: `0b0100`
* Flag: `none`
* Data bytes: `1 byte per color`
* Total bytes: `21`

### `Subspace Move`
* Code: `0b0101`
* Flag:
   * Speed ramp movement: `0b0000`
   * Linear movement: `0b0001`
* Data bytes:
    * X position: `2 bytes`
    * Y position: `2 bytes`
    * Z position: `2 bytes`
* Total bytes: `7`

### `Measure`
* Code: `0b0110`
* Flag:
   * X: `0b0000`
   * Y: `0b0001`
   * Z: `0b0010`
* Data bytes: `none`
* Total bytes: `1`

### `Move Arc`
* Code: `0b0111`
* Flag:
   * Default speed ramp: `0b0000`
   * Fast speed ramp: `0b0001`
* Data bytes:
    * Center X position: `2 bytes`
    * Center Y position: `2 bytes`
    * Center Z position: `2 bytes`
    * Radius X position: `2 bytes`
    * Radius Y position: `2 bytes`
    * Radius Z position: `2 bytes`
    * Rotation Degrees: `2 bytes`
* Total bytes: `15`
