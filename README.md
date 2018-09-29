# Coop Keeper 5000
This project documents an automatic chicken coop door.

![Coop Door](https://github.com/okie1/coopDoor/blob/master/InsideViewOpen.jpg)

## Overview
Once I started raising chickens on my farm out in the country, I discovered how many different varmints liked to help themselves to my chickens. After several catastrophic loses, I developed a habit of walking out to my chicken coop at dark to make sure my chickens were shut up securely inside the coop. The next morning, I would walk back out to the coop to let the chickens back out of their coop. Rinse and repeat, day after day, week after week. As much as I loved raising chickens, the routine got old fast. So, I decided to try to develop an automatic way to do this. I ended up building an automated chicken door that would open and close based on the outside light levels. The door would shut each evening and open again the next morning. Because it used light instead of a timer, the door would stay open longer in the summers and not as long in the winters.

## Parts
* Particle Photon
* 12" Linear Actuator with internal stops
* 2 Drawer Glides
* 12" x 12" x 16 gauge metal
* 12v DC power supply
* Drok 12v to 5v converter
* 2 relays
* Double-Pole, Double-Throw Switch (DPDT)
* Photocell
* Plastic Project Box

## Circuit
todo...

## Code
The code is posted on this github feed: [https://github.com/okie1/coopDoor](https://github.com/okie1/coopDoor)

## Assembly
todo...

## Results
todo...

## Hindsight
* I initially did this project with an Arduino Uno.  It worked fine, but to update the firmware I had to bring it in the house.  Which was not that appealing once the door had been in the coop for several days.  Having over-the-air firmware updates that the Photon board supported helped greatly in improving this.
* Connecting the door to WIFI allowed me to remotely monitor the status of the door which helped me debug a few issues.
* I first connected all the wires with crimp connections.  These came loose over time and started causing shorts.  Soldering the wires was a much better approach.
* The first photocell did not have any kind of covering.  It lasted about a year before is malfunctioned and I had to replace it.  The next photocell I used I placed in a clear plastic tube that protected it from the harsh elements.  It has been much more reliable.
* The first generation of this had two pots, one to adjust the time delay once the light level transitioned, the second was for the sensitivity of the photocell.  Both I eventually pulled due to shorts and were replaced with controls done via the particle application.
