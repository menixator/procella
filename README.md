# Morseus

Encrypted message passing between to BBC Micro:bits over GPIO pins.

## Getting Started

These instructions will get you a copy of the project up and running.

### Prerequisites

1. ( Yotta )[http://docs.yottabuild.org/#installing]
2. ( Git )[https://git-scm.com/]  (Only if you are cloning the repository)


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

## License

This project is licensed under the GNU GPLv3 License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

*	David Wheeler, Roger Needham - XXTEA implementation
