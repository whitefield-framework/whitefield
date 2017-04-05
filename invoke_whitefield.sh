#!/bin/bash

. ./config.inc

export LD_LIBRARY_PATH=$NS3PATH
$BINDIR/whitefield $*
