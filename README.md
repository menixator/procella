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

If the microbit was initialized as a Sender, you should see an arrow climbing
up, indicating your selection.  You will see an animation from the Receiver as
well, except the arrow will point downwards.


To send data between the microbits, you'll have to hold down the Button A on the
Sender for a designated period and then.  When you are done entering the data
for the character, you can press the Button B on the Sender to transmit the data
  over to the Receiver.

- To add a dot/dit to the buffer, you can press the Button A and immediately let
  go. The screen will display a dot during this period.
- When the screen changes to a dash/dah you can let go to add a dash to the
  buffer.

When Button B is pressed on the Sender, the screen will show a small animation
indicating that the data is being transmitted.  After ~2 seconds(the details
about the time will be discussed further below) the Receiver will display the
transmitted character on the LED display.


## Internal Workings
The internal buffer of the Sender can hold a limited amount of dits/dahs. This
is to make sure that the value being trasmitted fits into a single byte(less
than 256).  If the internal buffer is on the verge going over the limit, the
Sender will initiate a transmission forcefully.  To convert the morse code
signal to a integer value, a binary tree representation of the morse code
alphabet is used.

This is the binary tree that represents the morse corde standard.  Each left
branch represents a dot and each right branch represnts a dash.

![Morse Code Binary Tree](https://upload.wikimedia.org/wikipedia/commons/c/ca/Morse_code_tree3.png)

This can be flattend according to a Breadth First Traversal. The exclamation
marks represent special characters.

```
!!etianmsurwdkgohvf!l!pjbxcyzq!!54!3!!!2!!+!!!!16=/!!!(!7!!!8!90!!!!?!"!!.@!'!!-!)!,:!!!!!
```

While it is not necessary to, I have chosen to use the position a character is
in the above string instead its ASCII value to represent it during transmission.

After the more code signals that the user had entered have been converted into
an integer value, a simple [Caesar Cipher](https://en.wikipedia.org/wiki/Caesar_cipher) is applied to the value.
The shift value being [42](https://en.wikipedia.org/wiki/Phrases_from_The_Hitchhiker%27s_Guide_to_the_Galaxy#Answer_to_the_Ultimate_Question_of_Life,_the_Universe,_and_Everything_(42).


## Packet Construction
There are 8 bytes in total in the packet payload itself. It is of fixed length.
The packet is rather simple to constuct. 

Here are the details of each byte:
```
morse::ESC: Which is a special value in the morse alphabet I have displayed above. It takes the value 0x13(19).
shifted_value: This will be the value the user entered, shifted to the right by 42 places
parity: Whether or not the shifted value has even number of bits

The following bytes are made random so that no two packets holding the same
value looks the same.
rand_1:
rand_2:
rand_3:
rand_4:

morse::EOW: another special value which corresponds to: 0x49(69)
```

## Encryption
While the Caesar Cipher serves as a simple obfuscation method, it is simple to
unravel.  Therefore, a more reliable form of hiding the data is required. Thus
an encryption cipher was required. I looked into several ciphers of the same
family:
1. TEA - Tiny Encryption Algorithm
2. XTEA - eXtended TEA
3. XXTEA - Corrected TEA

These were 3 very simple encryption algorithms that did not require too much
processing to be done and did not require too much memory. These 3 algorithms
were tiny as the name implies. I chose XXTEA because it is corrected version of
its predecessors.

XXTEA however, comes with a price. XXTEA can only work on blocks that are a
multiple of 32bits that are greater than or equal to 64bits. This presented a
problem. Since we are only sending a byte(8 bits), 56 of those bits would be
completely wasted. I have chosen to repupose some byte for presentational
purposes(header and footer) and  one more byte for parity checks.

The remaining 4 bytes are just garbage data that is generated randomly within
the microbit. This would actually help mitigate an attack where the attacker has
access to a microbit running the program and reading the output on the other end
of the GPIO pin.

If these random bytes were not filled, the values will always be a static value,
lets assume 0. Therefore, the packet for A will always be the same(even after
encryption) no matter how many times you send it. The attacker can use this
information to map out all the possible outputs on the gpio pin for all the
combinations and essentially create a device that can talk to a Receiver without
even having to know the packet format.

However, if these 4 bytes are filled with random values, the chances of two
packets for A being the same are 1 in 4294967296. Well, almost. Because the
random number generator is only so random the chances are probably higher than
that.

## Transmission
Transmission of the data proved to be quite a challenge. Initially an attempt
was made with sending HI signals for different durations to signify a LO and a
HI. Using this method, to send all HI 64 bits, it would take at least `(64*2n)`
milliseconds(`2n` because there should be a LO signal segregating the HIs) where
`n` is the minimum duration that the gpio pin can be HI or LO and be noticable
on the microbit on the other end. 

Through various tests, I have narrowed down the value to about `35-40`ms.  So,
by using this method, it would take about `4480` milliseconds. Which obviously
did not sit well with me.  Of course this value is not realistic because n is
the minimum time it would take. Since we have to send LOs as well, the duration
that the GPIO pin is high for LO's should be MORE than n to differentiate them.
Therefore, the duration would actually be more than 4.4seconds.

The next method I tried, and the method that was ultimately used in this
project, is using n(which I had found earlier) milliseconds of GPIO signals,
either HI or LO to send HIs and LOs respectively. Using this method,
theoratically you could send 64 bits over in `2240`ms. All of it!

The transmission worked as follows:
```
0xAB: A sort of palate cleanser for the GPIO pins. It assures that every transmission begins with a HI.
[8 BYTES OF ENCRYPTED DATA]
0xAB: Same as the header but assures that every transmission ENDS in a low.
```

## Receiving
The reciever would read the data, assure that the first byte is a 'marker byte'
by comparing the value to `0xAB`. If not, the Reciever will choose to reset
itself. 

The Reciever reads all 10 bytes of the data, checks if the marker bytes are
encapsulating the packet.  Decryption occurs here as well, but only on the 8
bytes encapsulated within the marker bytes. After decryption, the first and last
bytes are compared with the respective values. The obfuscated value is grabbed
and its parity is compared with the parity byte present in the packet. Finally,
the obfuscated value is then passed back through the Ceaser cipher to get the
original value.

The character corresponding to this value in the binary tree is then displayed
on the screen. If any of the checks failed, a cross is displayed indicating that
something went wrong.


## License

This project is licensed under the GNU GPLv3 License - see the LICENSE file for details

## Acknowledgments

*	David Wheeler, Roger Needham - XXTEA implementation
