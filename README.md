# Morseus

Encrypted message passing between to BBC Micro:bits over GPIO pins.

## Getting Started

These instructions will get you a copy of the project up and running.

### Prerequisites

1. [Yotta](http://docs.yottabuild.org/#installing)
2. [Git](https://git-scm.com/)  (Only if you are cloning the repository)


### Building
1.  Clone the repository and `cd` into the directory. (Just `cd` into it if you acquired by other means)
```
$ git clone https://gitlab.uwe.ac.uk/a2-miljau/morseus.git
$ cd morseus
```

2. Build the project with `yotta`
```
$ yt build
```

3. Copy the hex output into the Micro:bit. Replace `/run/media/$USERNAME/MICROBIT/` with the location your Micro:bit is mounted at.
```
cp build/bbc-microbit-classic-gcc/src/*-combined.hex /run/media/$USERNAME/MICROBIT/
```

### Running

When the the microbit starts up:

- On the Sender, press the Button A
- On the Receiver, press the Button B

If the microbit was initialized as a Sender, you should see an arrow climbing up, indicating your selection.
You will see an animation from the Receiver as well, except the arrow will point downwards.


To send data between the microbits, you'll have to hold down the Button A on the Sender for a designated period and then.
When you are done entering the data for the character, you can press the Button B on the Sender to transmit the data over to the Receiver.

- To add a dot/dit to the buffer, you can press the Button A and immediately let go. The screen will display a dot during this period.
- When the screen changes to a dash/dah you can let go to add a dash to the buffer.

When Button B is pressed on the Sender, the screen will show a small animation indicating that the data is being transmitted.
After ~2 seconds(the details about the time will be discussed further below) the Receiver will display the transmitted character on the LED display.


## Internal Workings
- The internal buffer of the Sender can hold a limited amount of dits/dahs. This is to make sure that the value being trasmitted fits into a single byte(less than 256).
- If the internal buffer is on the verge going over the limit, the Sender will initiate a transmission forcefully.
- To convert the morse code signal to a integer value, a binary tree representation of the morse code alphabet is used.

![Morse Code Binary Tree](https://upload.wikimedia.org/wikipedia/commons/c/ca/Morse_code_tree3.png)


## License

This project is licensed under the GNU GPLv3 License - see the LICENSE file for details

## Acknowledgments

*	David Wheeler, Roger Needham - XXTEA implementation
