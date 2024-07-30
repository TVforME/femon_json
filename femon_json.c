/* femon_json -- monitor frontend status with JSON output and UDP sending
 *
 * Copyright (C) 2003 convergence GmbH
 * Johannes Stezenbach <js@convergence.de>
 * Modified by Robert Hensel <vk3dgtv@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/time.h>
#include <libdvbapi/dvbfe.h>
#include <arpa/inet.h>
#include <json-c/json.h>
#include <bits/getopt_core.h>

#define VERSION "1.0.0"

#define FE_STATUS_PARAMS (DVBFE_INFO_LOCKSTATUS|DVBFE_INFO_SIGNAL_STRENGTH|DVBFE_INFO_BER|DVBFE_INFO_SNR|DVBFE_INFO_UNCORRECTED_BLOCKS)

static char *usage_str =
    "\nusage: femon_json [options]\n"
    "     -H        : human readable output\n"
    "     -J        : JSON output\n"
    "     -a number : use given adapter (default 0)\n"
    "     -f number : use given frontend (default 0)\n"
    "     -c number : samples to take (default 0 = infinite)\n"
    "     -u ip:port: send the JSON output via UDP to the specified address\n"
    "     --version : print version information\n\n";

int sleep_time = 1000000;
char *udp_addr = NULL;
int udp_port = 0;
int udp_socket = -1;
struct sockaddr_in udp_sockaddr;

static void usage(void) {
    fprintf(stderr, "%s", usage_str);
    exit(1);
}

static void print_version(void)
{
    printf("femon_json version %s\n", VERSION);
    exit(0);
}


static int setup_udp_socket(const char *addr, int port) {
    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket < 0) {
        perror("socket");
        return -1;
    }
    memset(&udp_sockaddr, 0, sizeof(udp_sockaddr));
    udp_sockaddr.sin_family = AF_INET;
    udp_sockaddr.sin_port = htons(port);
    if (inet_aton(addr, &udp_sockaddr.sin_addr) == 0) {
        fprintf(stderr, "Invalid UDP address\n");
        return -1;
    }
    return 0;
}

static int check_frontend(struct dvbfe_handle *fe, int human_readable, unsigned int count, int adapter, int json_output) {
    struct dvbfe_info fe_info;
    unsigned int samples = 0;
    json_object *jobj = NULL;

    do {
        if (dvbfe_get_info(fe, FE_STATUS_PARAMS, &fe_info, DVBFE_INFO_QUERYTYPE_IMMEDIATE, 0) != FE_STATUS_PARAMS) {
            fprintf(stderr, "Problem retrieving frontend information: %m\n");
        }

        if (json_output) {
            jobj = json_object_new_object();
            json_object_object_add(jobj, "adapter", json_object_new_int(adapter));
            json_object_object_add(jobj, "signal", json_object_new_int((fe_info.signal_strength * 100) / 0xffff));
            json_object_object_add(jobj, "snr", json_object_new_int((fe_info.snr * 100) / 0xffff));
            json_object_object_add(jobj, "ber", json_object_new_int(fe_info.ber));
            json_object_object_add(jobj, "unc", json_object_new_int(fe_info.ucblocks));
            json_object_object_add(jobj, "signal_lock", json_object_new_boolean(fe_info.signal));
            json_object_object_add(jobj, "carrier_lock", json_object_new_boolean(fe_info.carrier));
            json_object_object_add(jobj, "viterbi_lock", json_object_new_boolean(fe_info.viterbi));
            json_object_object_add(jobj, "sync_lock", json_object_new_boolean(fe_info.sync));
            json_object_object_add(jobj, "lock", json_object_new_boolean(fe_info.lock));

            const char *json_str = json_object_to_json_string(jobj);
            printf("%s\n", json_str);

            if (udp_socket >= 0) {
                sendto(udp_socket, json_str, strlen(json_str), 0, (struct sockaddr *)&udp_sockaddr, sizeof(udp_sockaddr));
            }

            json_object_put(jobj);
        } else if (human_readable) {
            printf("status %c%c%c%c%c | signal %3u%% | snr %3u%% | ber %d | unc %d | ",
                   fe_info.signal ? 'S' : ' ',
                   fe_info.carrier ? 'C' : ' ',
                   fe_info.viterbi ? 'V' : ' ',
                   fe_info.sync ? 'Y' : ' ',
                   fe_info.lock ? 'L' : ' ',
                   (fe_info.signal_strength * 100) / 0xffff,
                   (fe_info.snr * 100) / 0xffff,
                   fe_info.ber,
                   fe_info.ucblocks);

            if (fe_info.lock)
                printf("FE_HAS_LOCK");

            printf("\n");
            fflush(stdout);
        } else {
            printf("status %c%c%c%c%c | signal %04x | snr %04x | ber %08x | unc %08x | ",
                   fe_info.signal ? 'S' : ' ',
                   fe_info.carrier ? 'C' : ' ',
                   fe_info.viterbi ? 'V' : ' ',
                   fe_info.sync ? 'Y' : ' ',
                   fe_info.lock ? 'L' : ' ',
                   fe_info.signal_strength,
                   fe_info.snr,
                   fe_info.ber,
                   fe_info.ucblocks);

            if (fe_info.lock)
                printf("FE_HAS_LOCK");

            printf("\n");
            fflush(stdout);
        }

        usleep(sleep_time);
        samples++;
    } while ((!count) || (count - samples));

    return 0;
}

static int do_mon(unsigned int adapter, unsigned int frontend, int human_readable, unsigned int count, int json_output) {
    int result;
    struct dvbfe_handle *fe;
    struct dvbfe_info fe_info;
    char *fe_type = "UNKNOWN";

    fe = dvbfe_open(adapter, frontend, 1);
    if (fe == NULL) {
        perror("opening frontend failed");
        return 0;
    }

    dvbfe_get_info(fe, 0, &fe_info, DVBFE_INFO_QUERYTYPE_IMMEDIATE, 0);
    switch (fe_info.type) {
        case DVBFE_TYPE_DVBS:
            fe_type = "DVBS";
            break;
        case DVBFE_TYPE_DVBC:
            fe_type = "DVBC";
            break;
        case DVBFE_TYPE_DVBT:
            fe_type = "DVBT";
            break;
        case DVBFE_TYPE_ATSC:
            fe_type = "ATSC";
            break;
    }
    
    // Print adatpter info in json format. Otherwise breaks json marshalling on receive side.
     if (json_output) {
      printf("{ \"fe\": \"%s\", \"type\": \"%s\" }\n", fe_info.name, fe_type);
    } else {
      printf("FE: %s (%s)\n", fe_info.name, fe_type);
    } 

    result = check_frontend(fe, human_readable, count, adapter, json_output);

    dvbfe_close(fe);

    return result;
}

int main(int argc, char *argv[]) {
    unsigned int adapter = 0, frontend = 0, count = 0;
    int human_readable = 0;
    int json_output = 0;
    int opt;

    // Check for --version flag
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--version") == 0) {
            print_version();
            exit(1);
        }
    }

    while ((opt = getopt(argc, argv, "HJa:f:c:u:")) != -1) {
        switch (opt) {
            default:
                usage();
                break;
            case 'a':
                adapter = strtoul(optarg, NULL, 0);
                break;
            case 'c':
                count = strtoul(optarg, NULL, 0);
                break;
            case 'f':
                frontend = strtoul(optarg, NULL, 0);
                break;
            case 'H':
                human_readable = 1;
                break;
            case 'J':
                json_output = 1;
                break;
            case 'u': {
                char *colon = strchr(optarg, ':');
                if (colon == NULL) {
                    usage();
                }
                *colon = '\0';
                udp_addr = optarg;
                udp_port = strtoul(colon + 1, NULL, 0);
                if (setup_udp_socket(udp_addr, udp_port) != 0) {
                    fprintf(stderr, "Failed to set up UDP socket\n");
                    exit(1);
                }
                break;
            }
        }
    }

    do_mon(adapter, frontend, human_readable, count, json_output);

    if (udp_socket >= 0) {
        close(udp_socket);
    }

    return 0;
}

