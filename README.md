# weasley-clock

I recreated the Weasley Family Clock (whereabouts clock) from Harry Potter using an embedded system. I am able to display the locations of two people on the face of the clock. I created virtual locations to display how the clock works. When a person enters an area the clock hands move in response to the change. The clock face also lights up when it's dark.

![pin diagram](https://github.com/zfatmi13/weasley-clock/blob/master/Final_Pin_Assignments.jpg)

The embedded system behind the magical clock consists of the following components which were integrated seamlessly.

- The microprocessor used was an FRDM-KL43Z board with an ARM Cortex-M0+. The code, written in C, controls the peripheral sensors and actuators.

- Light emitting diodes (LEDs) and pull-up resistors are situated around the circumference of the clock. The program either turns on or off depending on the value received from the light dependent resistor (LDR).

- The LDR is an analog sensor connected to the microcontroller using a voltage divider configuration. The input signal drops when the light is restricted and allows higher voltages as the intensity of the light increases. The values of the LDR are accessed through the onboard analog-to-digital converter (ADC).

- The HC-SR04 ultrasonic sensors are digital sensors used to determine the location of a person. Using a 10-microsecond delay I send out trigger pulses to the sensors at intervals. When receiving a trigger pulse, the sensor sends out 8 sonic bursts and listens for the echo. Using interrupts and the onboard crystal, I time the duration between the sonic bursts and their echo (i.e. until the sound reflects back), in order to measure the distance.

- The two continuous rotation servo motors, with built-in h-bridge and feedback control, rotate the clock hands to the correct positions. The motors are run with a pulse width modulated signal.

- A 5V power source

![weasley clock](https://github.com/zfatmi13/weasley-clock/blob/master/Clock.png)
