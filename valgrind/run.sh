#!/bin/bash

# THIS FILE IS PROVIDED UNDER THE TERMS OF THE ECLIPSE PUBLIC LICENSE
# ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS FILE
# CONSTITUTES RECIPIENTS ACCEPTANCE OF THE AGREEMENT.
#
# You can obtain a current copy of the Eclipse Public License from
# http://www.opensource.org/licenses/eclipse-1.0.php
#
# Author: Federico Martin Casares <warptrosse@gmail.com>

export SBLIM_TRACE=4
valgrind --leak-check=full --show-reachable=yes --trace-children=yes\
 --track-fds=yes --log-file=./valgrind.log sfcbd -k -t 4096
