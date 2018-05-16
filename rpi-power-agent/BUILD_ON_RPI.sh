#!/bin/sh

RASPBERRY="pi@192.168.1.200"

cd ..
ssh $RASPBERRY "rm -fr rpi-power-agent"
scp -r rpi-power-agent $RASPBERRY:
ssh $RASPBERRY "cd rpi-power-agent; make"
