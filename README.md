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

Please install the Debian package, which includes a systemd init script for pigeond. Unfortunately, the rest of the configuration is manual...

First, you will need to disable connman's ownership of the eth0 interface. In /etc/connman/main.conf, change this line:

    NetworkInterfaceBlacklist=usb0,SoftAp0

... to include eth0:

    NetworkInterfaceBlacklist=usb0,SoftAp0,eth0

Now we need to configure a persistent pigeon0 device and a network bridge between eth0 and pigeon0. In /etc/network/interfaces, add these lines:

    iface eth0 inet manual
    
    auto pigeon0
    iface pigeon0 inet manual
        pre-up ip tuntap add pigeon0 mode tap
        post-down ip link del dev pigeon0
    
    auto br0
    iface br0 inet manual
        bridge_ports eth0 pigeon0
        post-up ip link set dev pigeon0 up
        post-up ip link set dev eth0 up
        post-down ip link set dev pigeon0 down
        post-down ip link set dev eth0 down

Now, reboot the device and it should behave like a layer 2 switch. You can connect a device to the ethernet port, and any messages going over that port will be sent to our carrier pigeon interface. Any device we connect will need to configure its own network settings appropriately. For example, a static IP address like `192.168.10.2`, and a netmask like `255.255.255.0`. You should also set an MTU around 170.

## Authors

 * Adam Shambrook (ashambro@sfu.ca)
 * Dylan McCall (dmccall@sfu.ca)
 * Sunny Singh (psa71@sfu.ca)
