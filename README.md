# NESodore64

NESodore64 is an AVR microcontroller board that interfaces with an original NES controller pad, and outputs signals that emulate a joystick for Commodore 64's. It might be used on other computers of the era, but I haven't tested it on any and guarantee nothing!

It's creation was documented in [this YouTube video](https://youtu.be/1u0i-EAIm1I)

## Pin details

Here's some tables describing the pins and their connections found or used as part of this project.

### NES pad

The NES pad seems to have identical wire colours, as they are specifically written on the back which colour goes where, but not which wire does what, so here's that chart for ya.

| Colour | Pin | Purpose |
|--------|-----|---------|
| Brown  | 1   | Ground  |
| Red    | 2   | Clock   |
| Orange | 3   | Latch   |
| Yellow | 4   | Data    |
| White  | 7   | +5V     |

### C64 joystick

A LOT more detail about the port at https://www.c64-wiki.com/wiki/Control_Port.

In the joystick I used, however, these were the wire colours. Your joystick will PROBABLY have different colours, so unless you have the exact one used in my video, don't use this as a guide. Even then, just buzz the wires out yourself to be sure!

| Colour | Pin | Purpose   | 
|--------|-----|-----------|
| White  | 1   | Joy UP    |
| Blue   | 2   | Joy DOWN  |
| Green  | 3   | Joy LEFT  |
| Brown  | 4   | Joy RIGHT |
| Yellow | 5   | Paddle Y  |
| Orange | 6   | But FIRE  |
| Red    | 7   | +5V       |
| Black  | 8   | Ground    |
| Grey   | 9   | Paddle X  |

## Contributing
Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

Please make sure to update tests as appropriate.

## License
[MIT](https://choosealicense.com/licenses/mit/)