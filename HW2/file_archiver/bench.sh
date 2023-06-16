#!/bin/bash 
sudo insmod ./pmuon.ko
sudo sync && sudo sysctl -w vm.drop_caches=3
taskset -c 0 $@
sudo rmmod pmuon
dmesg | tail -15
