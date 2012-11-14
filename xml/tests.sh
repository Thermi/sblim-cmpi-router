#!/bin/bash

# THIS FILE IS PROVIDED UNDER THE TERMS OF THE ECLIPSE PUBLIC LICENSE
# ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
# CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
#
# You can obtain a current copy of the Eclipse Public License from
# http://www.opensource.org/licenses/eclipse-1.0.php
#
# Author: Federico Martin Casares <warptrosse@gmail.com>


TEST_ORDER=(
    # ComputerSystem -> NextHopIPRoute
    CSHostedRoute_assoc
    CSHostedRoute_enumerateInstances

    # NextHopIPRoute
    NextHopIPRoute_getclass
    NextHopIPRoute_enumerateInstanceNames
    NextHopIPRoute_enumerateInstances
    NextHopIPRoute_getInstance
    NextHopIPRoute_setInstance
    NextHopIPRoute_createInstance
    NextHopIPRoute_deleteInstance

    # NextHopIPRoute -> LANEndpoint
    RouteUsesEndPoint_assoc_lan
    RouteUsesEndPoint_assoc_routes
    RouteUsesEndPoint_assocNames_lan
    RouteUsesEndPoint_assocNames_routes
    RouteUsesEndPoint_refer_lan
    RouteUsesEndPoint_refer_routes
    RouteUsesEndPoint_referNames_lan
    RouteUsesEndPoint_referNames_routes

    # LANEndpoint
    LANEndpoint_enumerateInstanceNames
    LANEndpoint_enumerateInstances
    LANEndpoint_getclass
    LANEndpoint_getInstance
    LANEndpoint_invokeMethod_off
    LANEndpoint_invokeMethod_on
    )


ip link set eth0 down
ip link set eth0 up
ip route add 192.168.0.0/24 dev eth0
ip route add 192.168.1.0/24 dev eth0
ip route add 192.168.2.0/24 dev eth0

for fileName in ${TEST_ORDER[@]}
do
    echo -e "============================================================"
    echo -e "== $fileName"
    echo -e "============================================================"
    xmltest $fileName".xml"
    echo -e "============================================================"
    echo -e ""
done

ip route del 192.168.1.0/24 dev eth0

echo -e ""
ip link lst
echo -e ""
ip route lst
echo -e ""
