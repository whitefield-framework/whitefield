#!/bin/bash

. config.inc

cd `dirname $NS3PATH`
./waf configure && make

