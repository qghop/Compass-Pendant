# Compass-Pendant
WIP - Creating a Pendant with LEDs and a Compass Module on a custom PCB

## Design Goals and Inspiration

The main design goal from the beginning was to create a unique piece of jewlery, from a Custom PCB with real-world functional components. 

Several purposes were ideated for this purpose. I initially wanted to create a GPS Logging Device to track my position, an idea I was first enamored by with [BOTW's Hero's Path Mode](https://www.youtube.com/watch?v=YSooSC6LfSU), and without giving my position data away to, or relying upon, a huge company like Google Maps' Timeline feature requires. I couldn't figure out a way to keep all the components small for this idea, so it was shelved, though I implemented a similar feature in my [Apple Photos Analysis Project](https://github.com/qghop/Apple-Photos-Analysis).

Different sensors, chips, and general designs were considered, but eventually using a Coin Cell Battery, an ATmega328P, and an I2C Magnetometer were chosen for ease of implementing, in both wiring and programming. 

The whole project was heavily inspired by the work of [Mitxela](https://mitxela.com/), especially his [UV Protection Amulet](https://www.youtube.com/watch?v=JhNMeJZ8Ll0) and [Fluid Simulation Pendant](https://www.youtube.com/watch?v=jis1MC5Tm8k). The header for chip programming was taken from the [KeyChainino](https://www.keychainino.com/) project, thanks to [upir](https://www.youtube.com/watch?v=Zsum_6dWfVU) for their video on the device. Additional thanks to the hundreds of KiCad tutorials used to build the PCB.

## Media

### V1

[![Working Compass Video](https://img.youtube.com/vi/5i10F70vsIM/0.jpg)](https://www.youtube.com/watch?v=5i10F70vsIM)

![Top View](<Media/IRL Top View.jpg>)

![Bottom View](<Media/IRL Bottom View.jpg>)

![Top View, Cut](<Media/IRL Top View Cut.jpg>)

![Side View](<Media/IRL Side View 2.jpg>)

![Schematic](<Media/v1 Schematic.png>)

![PCB](<Media/v1 PCB.png>)

![3D Model](<Media/v1 3D.png>)

[![Programming Video](https://img.youtube.com/vi/YMhp7EAuIT4/0.jpg)](https://www.youtube.com/watch?v=YMhp7EAuIT4)

## V1 Successes

Honestly, I was suprised by how not difficult this project was. Obviously I have a very basic scope, but to have my PCB design work, essentially, first try, is remarkable as someone with only the base electrical experience required of a Mechanical Engineering grad. KiCad especially proved to be quite easy to learn.

A functional V1 device was created!

## V1 Failures

- A switch was initially considered, but was removed to more easily wire connections with the large battery pads taking most of the limited space. This was a mistake, a switch is really required for actual use. Removing the battery from the chosen holder is not easy, and a case for the PCB (which could hide the LEDs) would add a ton of complexity, and would cover up an, in my opinion, great looking bare PCB.

- The battery is oddly inconsistent in delivering power to the magnetometer. This problem needs to be properly diagnosed, but there is no issue in running the debug "BasicSpinningLights.ino" sketch, but there is sometimes issues with the Compass program loaded.

- PCBA is too expensive to justify for revisions, so V2 should be designed so I could assemble myself (mostly meaning larger components).

- The small, red LEDs aren't as appealing as envisioned. Could be fixed with a diffuser, a larger or variable resistor, or a cooler (blue, green) color.

- The snap-off header could be designed to be easier to cut off, or designed to be integrated into the jewlery, as say the part that connects to the necklace chain.

## Programming

I primarily referenced the [KeyChainino Guide](https://www.keychainino.com/how-to-program-keychainino-v144-via-isp/) for programming. Below is a summarized version, with some tweaks.

1. Install the MiniCore Library to the Arduino IDE.

2. Load the example sketch "ArduinoISP" to the Arduino UNO.

3. Remove the battery from the Pendant, and don't put it back in until finished. Physically connect the Pendant programming header to the proper pins of the UNO. 

    This can be done via wiring:

    ![Default Wiring Programming Method](<Media/Default Wiring Arduino UNO.png>)

    Or via the ISP header of the UNO. The RESET pin of the ISP pins cannot be used though, so there must still be one manual connection to Digital Pin 10, acting as a digital reset:

    ![Header Programming 1](<Media/Programming Setup.jpg>)

    ![Header Programming 2](<Media/Programming Setup 2.jpg>)

4. Switch to the ATmega328 Board, with the following settings:

![Upload Settings](<Media/Upload Settings.png>)

5. If first time programming the chip, run "Burn Bootloader" from Tools

6. Upload the sketch. If errors occour, wires are likely not connected properly. Tilting the pendant or the wires can help ensure connection.
