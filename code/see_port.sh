#
ARG='-n -l -p -e -F'
netstat $ARG | grep gateserver
netstat $ARG | grep gameserver
netstat $ARG | grep centerser
