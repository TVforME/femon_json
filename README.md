# femon_json
DVB frontend stats monitor with extra JSON formatting and UDP output.

`femon_json` is an extention to the goog old femon tool in the dvbapi package.
femon is for monitoring frontend in non-blocking mode for DVB frontends, I've removed the -A audio option from femon as I found it useless and refined my need to change how the tool formats the status on the stdio output. I added a UDP option to send over UDP to a remote listener from those contemplating on using for monitoring remotely. femon reports the lock status as SCVYL flags and to determine each lock status, needed to resort to fancy regex filters to decipher SCVYL flags which was slow and addition code hence, my modifications.

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
