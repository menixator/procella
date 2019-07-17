#!/usr/bin/env bash


DRIVE=$(readlink --canonicalize /dev/disk/by-label/MICROBIT)

udisksctl mount -b $DRIVE

cp build/bbc-microbit-classic-gcc/src/*-combined.hex /run/media/$(whoami)/MICROBIT/
