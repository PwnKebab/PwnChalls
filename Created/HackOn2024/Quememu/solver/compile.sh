#! /bin/bash

gcc exploit.c shellcode.s -static -o exploit
