#!/bin/bash

WF_PNAME="whitefield"
FORKER_PNAME="wf_forker"
function wf_get_pid()
{
	pgrep -u `whoami` -x $WF_PNAME
}

