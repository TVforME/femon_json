# femon_json
DVB frontend stats monitor with extra json formating and UDP output.

`femon_json` is a tool for monitoring frontend status of DVB devices, similar to `femon`.
I've added the UDP option to send over UDP to a remote listener as part of my repeater project for the core to know if the receiver 
has a valid dvb signal and in addition, extended the functionality to use with my receiver project to seperate the lock signals into individual values without having to yes 
fancy regex, filters to decypher SCVYL flags.


## Features

- Monitor DVB frontend status
- Output status in JSON format
- Send status over UDP
- Display human-readable output
- Display version information

## Usage
femon_json [options]

## Options
-H: Human readable output
-J: JSON format output
-a number: Use given adapter (default 0)
-f number: Use given frontend (default 0)
-c number: Samples to take (default 0 = infinite)
-u ip:port: Send status over UDP to the specified IP and port
--version: Display version information

## Examples

# Monitor adapter 0, frontend 0, with JSON output
femon_json -J -a 0 -f 0

Below is a breakdown on what is sent in json format with the -J option:-

adapter X     int       (X=the adaptor to monitor.  Example  adapter: 0)
signal        int       scaled ((signal * 100) / 0xffff)
snr           int       scaled ((snr * 100) / 0xffff)
ber           int
unc           int
signal_lock   True/false
carrier_lock  True/false
viterbi_lock  True/false
sync_lock     True/false
lock          True/false

# Monitor adapter 0, frontend 0, with human readable output
femon_json -H -a 0 -f 0

# Monitor adapter 0, frontend 0, with JSON output and send status over UDP to 239.255.0.100:5000
femon_json -J -a 0 -f 0 -u 239.255.0.100:5000

# Display version information
femon_json --version


## Installation
To build and install femon_json, follow these steps:

# Install Dependencies:

Make sure you have the necessary dependencies installed on your system. You will need libdvbapi and libjson-c-dev

sudo apt-get update
sudo apt-get install libdvbapi-dev libjson-c-dev build-essential

# Clone the Repository:

git clone https://github.comTVforME/femon_json.git
cd femon_json
make
sudo make install
