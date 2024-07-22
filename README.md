# femon_json
DVB frontend stats monitor with extra JSON formatting and UDP output.

`femon_json` is an extention to the femon toolpackaged with dvbapi tools.
femon is used for monitoring dvb frontend in non-blocking mode, I've removed the -A audio option from femon as I found it useless and added json format to tailor the status to the stdio output. In addition, added a UDP option to send status over UDP to a remote listener from those contemplating on monitoring remotely. femon reports the lock status as SCVYL flags and I needed the requirement to have the flags split out to remove the need for fancy regex filters to decipher SCVYL flags removing the need for addition regex filter code. (Less is best)

## Features

- Monitor DVB frontend status
- Output status in JSON format
- Send status over UDP
- Display human-readable output
- Display version information

## Usage

```bash
femon_json [options]
Options
-H: Human readable output
-J: JSON format output
-a number: Use given adapter (default 0)
-f number: Use given frontend (default 0)
-c number: Samples to take (default 0 = infinite)
-u ip:port: Send status over UDP to the specified IP and port
--version: Display version information  (added to determine if the tool is available on the machine) Returns V1.0.0
```
## Examples
Monitor adapter 0, frontend 0, with JSON output

```bash
femon_json -J -a 0 -f 0
```
Below is a breakdown of what is sent in JSON format with the -J option:

```bash
{
  "adapter": X,                 // int (X = the adapter to monitor. Example adapter: 0)
  "signal": int,                // scaled ((signal * 100) / 0xffff)
  "snr": int,                   // scaled ((snr * 100) / 0xffff)
  "ber": int,
  "unc": int,
  "signal_lock": true/false,     // (S)
  "carrier_lock": true/false,    // (C)
  "viterbi_lock": true/false,    // (V)
  "sync_lock": true/false,       // (Y)
  "lock": true/false             // (L)
}
```
Monitor adapter 0, frontend 0, with human-readable output
```bash
femon_json -H -a 0 -f 0
```
Monitor adapter 0, frontend 0, with JSON output and send status over UDP to 239.255.0.100:5000

```bash
femon_json -J -a 0 -f 0 -u 239.255.0.100:5000
```
Display version information
```bash
Copy code
femon_json --version
```
## Installation
To build and install femon_json, follow these steps:

## Install Dependencies
Make sure you have the necessary dependencies installed on your system. 
You will need libdvbapi-dev and libjson-c-dev and the build system GCC linker etc.

```bash
sudo apt-get update
sudo apt-get install libdvbapi-dev libjson-c-dev build-essential
```

Clone the Repository
``` bash
git clone https://github.com/TVforME/femon_json.git
cd femon_json
make
sudo make install
```
