# Ethernet Modem for IP Over Avian Carriers

Created for CMPT 433, and based loosely on RFC 1149.

## Components

### pigeond

Create a "pigeon0" network device using the tun/tap interface. Send and receive data over this device using the avian carrier link layer protocol. This should include a systemd unit file.

Currently we work with a hard-coded persistent network device. It is best to create and configure this device in advance:

    sudo ip tuntap add pigeon0 mode tap
    sudo ip link set pigeon0 up
    sudo ip addr add 10.0.0.1/24 dev pigeon0

Finally, run pigeond and it will connect to the pigeon0 device.

### pigeon-admin

NodeJS-based web server that displays status information and provides some tools to manage pigeond.

### System setup

Please install the Debian package, which includes a systemd init script for pigeond. Unfortunately, the rest of the configuration is manual...

First, we need to enable the BB-BONE-AUDI-02 and BB-I2CI capes at startup. Edit /etc/default/capemgr and add this line:

    CAPE=BB-BONE-AUDI-02,BB-I2C1

If you need to use the file transfer mode, we should set an environment variable for pigeond with a location where a device will *reliably* appear. Edit /etc/default/pigeond with something like this:

    PIGEOND_FILES_TX=/media/PGN_Ada
    PIGEOND_FILES_RX=/media/PGN_Grace

Note: TX and RX must point to the base directory of two separate USB storage devices.

To enable the printer module, include `PIGEOND_PRINTER_TX=yes`. To enable the camera module, include `PIGEOND_CAMERA_RX=yes`.

For our bridge interface to work, we need to stop eth0 from being assigned an IP address. To do this, disable connman's ownership of the eth0 interface. In /etc/connman/main.conf, change this line:

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

Reboot the device and it should behave like a layer 2 hub (with some quirks). You can connect a device to the ethernet port, and any messages going over that port will be sent to our carrier pigeon interface. Any device we connect will need to configure its own network settings appropriately so it can communicate with whatever device is on the other end. For example, each device should have a static IP address like `192.168.10.2`, and a netmask like `255.255.255.0`. You should set an MTU of 170.

Note that our device will drop IPv6 frames as well as IP broadcast frames. These were too noisy for our carrier pigeons to manage. Some applications may behave unexpectedly as a result.

### Monitoring devices

Let's install udevil for its devmon tool. (This sucks, but trust me, the other options here are worse).

    apt install udevil

Now, create a service file for devmon at /etc/systemd/system/devmon.service:

    [Unit]
    Description=Device monitoring daemon
    
    [Service]
    ExecStart=/usr/bin/devmon
    Restart=on-failure
    
    [Install]
    WantedBy=multi-user.target

Now start and enable it...

    systemctl start devmon
    systemctl enable devmon

## Authors

 * Adam Shambrook (ashambro@sfu.ca)
 * Dylan McCall (dmccall@sfu.ca)
 * Sunny Singh (psa71@sfu.ca)
