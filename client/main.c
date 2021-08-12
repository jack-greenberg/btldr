/*
 * Author: Jack Greenberg <j@jackgreenberg.co>
 * Copyright 2021
 */
#include <errno.h>
#include <getopt.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "microjson/mjson.h"

struct ecu_t {
    char name[64];
    char description[512];
    char designer[128];
    int node_id;
};

static struct ecu_t ecu;

const struct json_attr_t ecu_attrs[] = {
    {"name", t_string, .addr.string = ecu.name, .len = 64},
    {"description", t_string, .addr.string = ecu.description, .len = 512},
    {"designer", t_string, .addr.string = ecu.designer, .len = 128},
    {"node_id", t_integer, .addr.integer = &ecu.node_id},
};

// static int verbose_flag;

struct option long_options[] = {{"help", no_argument, 0, 'h'},
                                {"verbose", no_argument, 0, 'v'},
                                {NULL, 0, NULL, 0}};

#define ERR_PRINT fprintf(stderr, 
void print_usage(char* prg) {
    // clang-format off
    fprintf(stderr, "%s - Flash Over CAN.\n", prg);
    fprintf(stderr, "\nUsage: %s [options] <command> [args]\n", prg);
    fprintf(stderr, "  %s flash <node_id> <hex_file>\n", prg);
    fprintf(stderr, "  %s version <node_id>\n", prg);
    fprintf(stderr, "  %s list\n", prg);
    fprintf(stderr, "\nOptions:\n");
    fprintf(stderr, "    -h, --help      Display this text and exit\n");
    fprintf(stderr, "    -l, --ecu-list  Path to ecus.json file (default is in current directory)\n");
    fprintf(stderr, "    -v, --verbose   Be very loud\n");
    // clang-format on
}

int main(int argc, char** argv) {
    char* cmd = argv[0];
    int verbose_flag = 0;

    int opt, optindex;
    while ((opt = getopt_long(argc, argv, "-:hl:v", long_options, &optindex))
           != -1) {
        switch (opt) {
            case 'h':
                print_usage(cmd);
                break;
            case 'v':
                verbose_flag = 1;
                break;
            case '?':
            default:
                fprintf(stderr, "Unknown option: %c\n", optopt);
                print_usage(cmd);
                return 1;
        }
    }

    if (verbose_flag) {
        printf("Verbose\n");
    }

    return 0;
}
