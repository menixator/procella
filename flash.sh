#!/usr/bin/env bash


MY_MICROBIT=$(readlink --canonicalize /dev/disk/by-id/usb-MBED_VFS_9900000041504e4500584001000000370000000097969901-0:0)

udisksctl mount -b $MY_MICROBIT

cp build/bbc-microbit-classic-gcc/src/*-combined.hex /run/media/$(whoami)/MICROBIT/
