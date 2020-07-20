#!/usr/bin/env python
#
# Canvas: Browser based node visualization for Whitefield using cytoscape.js and bottle.py
#
# This file is part of Whitefield, published under the terms of the GNU General Public License version 2.
# See the LICENSE file in the top level  directory for more details.
#
# Copyright (C) 2017 Anmol Sarma <me@anmolsarma.in>

from __future__ import print_function

import os
import socket
import sys
import tempfile

import bottle

BOTTLE_PORT = 8080
BOTTLE_HOST = '0.0.0.0'
CFGINC = {}
MONITOR_PORT = 0
ASSETS_PATH = os.path.abspath(os.path.join(os.path.dirname(__file__), 'assets'))
bottle.TEMPLATE_PATH.insert(0, ASSETS_PATH)

def get_config():
    global MONITOR_PORT
    if len(CFGINC) > 0:
        return CFGINC
    with open("config.inc") as myfile:
        for line in myfile:
            name, var = line.partition("=")[::2]
            if name.startswith('#'):
                continue
            CFGINC[name.strip()] = var.strip()
    MONITOR_PORT = int(CFGINC['MONITOR_PORT'])
    if MONITOR_PORT <= 0:
        print("MONITOR PORT not defined in config.inc")
        sys.exit()

def query_nodes():
    fd, path = tempfile.mkstemp(text=True)
    cmd_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    cmd_sock.settimeout(1)
    cmd_sock.sendto(('AL:cmd_node_position:%s' % path).encode(), ('localhost', MONITOR_PORT))
    if 'SUCCESS' != cmd_sock.recv(32).decode().strip('\0'):
        os.remove(path)
        return
    cmd_sock.close()
    nodes = []
    with os.fdopen(fd) as outfile:
        for line in outfile:
            _, node_id, _, x, y, _ = line.split()
            nodes.append({'data': {'id': node_id}, 'position': {'x': float(x), 'y': float(y)}})
    os.remove(path)
    return nodes


def query_edges(nodes):
    cmd_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    cmd_sock.settimeout(1)
    edges = []
    for n in nodes:
        cmd_sock.sendto(('SL:%s:cmd_def_route' % n['data']['id']).encode(), ('localhost', MONITOR_PORT))
        rsp = cmd_sock.recv(64).decode().strip('\0')
        cmd_sock.recv(64)
        if rsp == '[NONE]':
            continue
        pid = str(int(rsp.split(':')[-1], 16))
        edges.append({'data': {'id': '%s-%s' % (n['data']['id'], pid),
                               'source': n['data']['id'], 'target': pid}})
    cmd_sock.close()
    return edges


@bottle.route("/graph")
def graph():
    nodes = query_nodes()
    if nodes:
        edges = query_edges(nodes)
        return {'elements': nodes + edges}
    return bottle.HTTPResponse(status=500)


@bottle.route('/assets/<filename>')
def static_assets(filename):
    return bottle.static_file(filename, root=ASSETS_PATH)


@bottle.route('/')
def index():
    return bottle.template('cytoscape.html')


if __name__ == '__main__':
    try:
        get_config()
        query_edges(query_nodes())
    except socket.timeout:
        print('No response received form StackLine/AirLine\nIs Whitefield running?', file=sys.stderr)
        exit(-1)
    print('Listening on http://%s:%d/' % (BOTTLE_HOST, BOTTLE_PORT))
    bottle.run(host=BOTTLE_HOST, port=BOTTLE_PORT, quiet=True)
