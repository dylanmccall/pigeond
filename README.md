# Ethernet Modem for IP Over Avian Carriers

Created for CMPT 433, and based loosely on RFC 1149.

## Components

### pigeond

Create a "pigeonN" network device using the tun/tap interface. Send and receive data over this device using the avian carrier link layer protocol. This should include a systemd unit file.

Currently we work with a persistent network device with the hard-coded name "pigeon0". It is best to create and configure this device in advance:

    sudo ip tuntap add pigeon0 mode tap
    sudo ip link set pigeon0 up
    sudo ip addr add 10.0.0.1/24 dev pigeon0

Finally, run pigeond and it will connect to the pigeon0 device.

### pigeon-admin

NodeJS-based web server that displays status information and provides some tools to manage pigeond.

### System setup

Please install the Debian package, which includes systemd configuration for our virtual network device.

In addition, you will need to disable connman's ownership of the eth0 interface. In /etc/connman/main.conf, change this line:

    NetworkInterfaceBlacklist=usb0,SoftAp0

... to include eth0:

    NetworkInterfaceBlacklist=usb0,SoftAp0,eth0

## Authors

 * Adam Shambrook (ashambro@sfu.ca)
 * Dylan McCall (dmccall@sfu.ca)
 * Sunny Singh (psa71@sfu.ca)
