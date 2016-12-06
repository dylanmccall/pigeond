# Ethernet Modem for IP Over Avian Carriers

Created for CMPT 433, and based loosely on RFC 1149.

## Building pigeond

The pigeond program opens a "pigeon0" network device using the tun/tap interface. It sends and receives data over this device using the IP Over Avian Carrier Protocol.

To build on Ubuntu, you will need to install some dependencies:

    sudo apt install libasound2-dev libqrencode-dev libv4l-dev libzbar-dev

Note that equivalent armhf libraries and header files (to cross-compile for BeagleBone) are included for your convenience.

To build for armhf and copy the resulting binary to ~/cmpt433/public/myApps:

    make

To build for all architectures:

    make build.all

The output will be located in ./our/default/ and ./out/armhf/. Substitute "default" or "armhf" for all to build for a specific architecture.

To remove build output:

    make clean

To create a Debian package for the current architecture, which can be installed using `dpkg -i`:

    dpkg-buildpackage -b -uc -us

This is the recommended way to install pigeond, along with its systemd service configuration, so it can be used as a system service.

## Development for pigeond

Currently we work with a hard-coded persistent network device. For testing, you can create and configure this device using the `ip` command:

    sudo ip tuntap add pigeon0 mode tap
    sudo ip link set pigeon0 up
    sudo ip addr add 10.0.0.1/24 dev pigeon0

Finally, run pigeond and it will connect to the pigeon0 device.

### Installation

Please install the Debian package, which includes a systemd init script for pigeond. Enable the pigeond service to run automatically:

    sudo systemctl enable pigeond

Unfortunately, the rest of the configuration is manual...

First, we need to enable the BB-BONE-AUDI-02 and BB-I2CI capes at startup. Edit /etc/default/capemgr and add this line:

    CAPE=BB-BONE-AUDI-02,BB-I2C1,BB-UARTS

If you need to use the file transfer mode, we should set an environment variable for pigeond with a location where a device will *reliably* appear. Edit /etc/default/pigeond with something like this:

    PIGEOND_FILES_TX=/media/PGN_Ada
    PIGEOND_FILES_RX=/media/PGN_Grace

Note: TX and RX must point to the base directory of two separate USB storage devices.

To enable the printer module, include `PIGEOND_PRINTER_TX=yes`. To enable the camera module, include `PIGEOND_CAMERA_RX=yes`.

For our bridge interface to work, we need to stop eth0 from being assigned an IP address. To do this, disable connman's ownership of the eth0 interface. In /etc/connman/main.conf, change this line:

    NetworkInterfaceBlacklist=usb0,SoftAp0

... to include eth0, pigeon0 and br0:

    NetworkInterfaceBlacklist=usb0,SoftAp0,eth0,pigeon0,br0

Now we need to configure a persistent pigeon0 device and a network bridge between eth0 and pigeon0. In /etc/network/interfaces, add these lines:

    iface eth0 inet manual
    
    auto pigeon0
    iface pigeon0 inet manual
        pre-up ip tuntap add pigeon0 mode tap
        pre-up ip link set pigeon0 multicast off
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

## Debugging

You can view a log for pigeond using journalctl:

    sudo journalctl -u pigeond -f

It is also useful to use tshark to watch the pigeon0 device:

    sudo tshark -i pigeon0

### Monitoring devices

Let's install udevil for its devmon tool. (This sucks, but trust me, the other options here are worse).

    sudo apt install udevil

Now, create a service file for devmon at /etc/systemd/system/devmon.service:

    [Unit]
    Description=Device monitoring daemon
    
    [Service]
    ExecStart=/usr/bin/devmon
    Restart=on-failure
    
    [Install]
    WantedBy=multi-user.target

Now start and enable it...

    sudo systemctl start devmon
    sudo systemctl enable devmon

## Authors

 * Adam Shambrook (ashambro@sfu.ca)
 * Dylan McCall (dmccall@sfu.ca)
 * Sunny Singh (psa71@sfu.ca)
