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
import sys
import socket
import tempfile
import bottle

MONITOR_PORT = 61616
BOTTLE_PORT = 8080
BOTTLE_HOST = 'localhost'


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
        pid = cmd_sock.recv(64).decode().strip('\0')
        cmd_sock.recv(64)
        if ':' in pid:
            edges.append({'data': {'id': '%s-%s' % (pid.split(':')[-1], n['data']['id']),
                                   'source': pid.split(':')[-1], 'target': n['data']['id']}})
    cmd_sock.close()
    return edges


@bottle.route("/graph")
def graph():
    nodes = query_nodes()
    if nodes:
        edges = query_edges(nodes)
        return {'elements': nodes + edges}
    return bottle.HTTPResponse(status=500)


@bottle.route('/static/<filename>')
def static_assets(filename):
    return bottle.static_file(filename, root='./static')


@bottle.route('/')
def index():
    return bottle.template('cytoscape.html')


if __name__ == '__main__':
    try:
        query_edges(query_nodes())
    except socket.timeout:
        print('No response received form StackLine/AirLine\nIs Whitefield running?', file=sys.stderr)
        exit(-1)
    print('Listening on http://%s:%d/' % (BOTTLE_HOST, BOTTLE_PORT))
    bottle.run(host=BOTTLE_HOST, port=BOTTLE_PORT, quiet=True)
