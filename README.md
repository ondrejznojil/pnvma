# pnvm-arduino

## Description
Here is PNVM simulator for Arduino.

This was only tested on Arduino Mega 2560.
For upload it needs at least 64KB flash.
For run at least 8KB SRAM.
  - From it at least 2KB must be available just for stack.
  - The rest can be devoured by objet memory and net template code.

This is not a generic, all-featured, "rule them all" module.
It serves rather as a springboard for other more advanced and more
specific implementations. It just allows to control LED diodes, handle
input from serial line and echo back results -- hardware serial line
(the one for interfacing with PC over USB).

It uses LED diods connected to pins:
  3, 4, 5, 6, 7 - for blinking with associated *ledControl*
                  net instances
  8             - signal processing of input from serial line
                - this is handled asynchronously,
                  each accepted charecter results in change of state
                  of LED
  9             - signal processing of the main loop
                - each step of simulater toggles LED state
  12            - error LED - when this is lit up, board entered never ending
                  loop as a result of fatal error

    HIGH state of pin lights up a LED --> connect it like this:
        pin -----+
         +       |
                 |
                (O) led
                 |
                 [] resistor
                 |
                --- gnd
                 -

## Compilation and upload
  1. First make sure, that squeak plugin is up to date:
    - in parent directory run:
        `$ make src`
  2. Choose platform in platforms/ directory to be loaded into
     object memory at startup or make a new one and place it
     into platforms named as:
        `<name>.txt`
     This defaults to "platform".
     Note, that if you choose different platform since the previous build,
     you need to manually clean the working directory:
        `$ make clean`
     Before the next step.

     TODO - make it smarter, requiring little or no explanation on how
     to use or build different binaries for different platforms
  3. Compile it:
      `$ PLATFORM=<name> make`
  4. Upload it:
    - check your device and if it's not /dev/ttyACM0, export it as:
        `$ SERIALDEV=/dev/mytty make upload`
    - otherwise just run:
        `$ make upload`
      Note: check, that any program using this device is terminated before
      uploading.

## Interfacing
  1. In one terminal run:
    `$ examples/listen`    # this will echo any output from simulator
  2. In another one:
      `$ cat >/dev/mytty`
    "input as serialized tokens expected by platform"
    "as many as you want until you press ^D"
      `^D`
    Or:
      `$ echo '"same token as in previous example"' > /dev/mytty`
  3. If you run default platform, you may load *ledControl* net template
       as a toy for playing with LEDS like this:
          `$ examples/led_control`
