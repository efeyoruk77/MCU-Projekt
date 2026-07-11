#include <getopt.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#include "../include/types.hpp"

uint32_t* parseRom(const char* file, uint32_t rom_size){
    FILE* f = fopen(file, "r");
    if(f == NULL){
        fprintf(stderr, "Can't open the ROM content file\n");
        exit(1);
    }
    uint32_t capacity = rom_size / 4;
    uint32_t* content = (uint32_t*) calloc(capacity > 0 ? capacity : 1, sizeof(uint32_t));
    if(content == NULL){
        fprintf(stderr, "Out of memory during allocation\n");
        exit(1);
    }

    char line[256];
    uint32_t count = 0;

    while(fgets(line, sizeof(line), f) != NULL){
        char *ptr = line;
        while(isspace((unsigned char) *ptr)){
            ptr++;
        }

        if(*ptr == '\0'){
            continue;
        }

        if(*ptr == '-'){
            fprintf(stderr, "Value can't be negative\n");
            exit(1);
        }

        char *endptr;
        uint32_t value = strtoul(ptr, &endptr, 0);
        while(isspace((unsigned char) *endptr)){
            endptr++;
        }
        if(ptr == endptr || *endptr != '\0'){
            fprintf(stderr, "Invalid value\n");
            exit(1);
        }
        if(count >= capacity){
            fprintf(stderr, "More values than size");
            exit(1);
        }
        content[count] = value;
        count++;
    }
    fclose(f);
    return content;
}

static uint32_t parseValue(char* str, uint32_t line_number, const char* field){
    
}

static char parseLetter(char* str, uint32_t line_number, const char* field){

}

struct Request* parseRequests(const char* file, uint32_t numRequests){
    
}

struct Parameters parse_cli(int argc, char** argv){
    int c;
    char* helpMessage = "What even is C about"; 
    uint32_t latency = 1;
    uint32_t cycles = 1;
    uint32_t rom_size = 0x100000;
    uint32_t block_size = 0x1000;

    Parameters parameters;

    char* tracePath = NULL; 
    char* romContent_path = NULL;

    static struct option long_options[] = {
        [0] = {"cycles", required_argument, 0, 'c'},
        [1] = {"tf", required_argument, 0, 't'},
        [2] = {"help", no_argument, 0, 'h'},
        [3] = {"latency-rom", required_argument, 0, 'l'},
        [4] = {"rom-size", required_argument, 0, 'r'},
        [5] = {"block-size", required_argument, 0, 'b'},
        [6] = {"rom-content", required_argument, 0, 'C'},
        [7] = {0, 0, 0, 0}
    };

    while((c = getopt_long(argc, argv, "hc:t:l:r:b:C:", long_options, NULL)) != -1){
        switch(c){
            case 'c': {

                //check that the value is not negative:
                char *ptr = optarg;
                while(isspace((unsigned char)*ptr)){
                    ptr++;
                }
                if(*ptr == '-'){
                    fprintf(stderr, "Cycles can't be negative!\n");
                    exit(1);
                }

                char *endptr; 
                cycles = strtoul(optarg, &endptr, 0);
                printf("Parsed cycles: %u\n", cycles);
                if(optarg == endptr || *endptr != '\0'){
                    fprintf(stderr, "Error: Invalid number of cycles.\n");
                    exit(1);
                }
                break;
            }
                
            case 'h':
                printf("%s", helpMessage);
                exit(0);
            case 't':
                tracePath = optarg;
                break;
            case 'l':{
                //negative check
                char *ptr = optarg;
                while(isspace((unsigned char)*ptr)){
                    ptr++;
                }
                if(*ptr == '-'){
                    fprintf(stderr, "Latency can't be negative!\n");
                    exit(1);
                }

                char *endptr;
                latency = strtoul(optarg, &endptr, 0);
                printf("Parsed latency: %u\n", latency);
                if(optarg == endptr || *endptr != '\0'){
                    fprintf(stderr, "Invalid latency!\n");
                    exit(1);
                }
                break;
            }
                
            case 'r': {
                char *ptr = optarg;
                while(isspace((unsigned char)*ptr)){
                    ptr++;
                }
                if(*ptr == '-'){
                    fprintf(stderr, "Rom size can't be negative!\n");
                    exit(1);
                }

                char *endptr;
                rom_size = strtoul(optarg, &endptr, 0);
                printf("Parsed rom size: %u\n", rom_size);
                if(optarg == endptr || *endptr != '\0'){
                    fprintf(stderr, "Invalid rom size!\n");
                    exit(1);
                }
                //if rom_size is not a power of 2 give an error and terminate the program
                if((rom_size & (rom_size - 1)) != 0){
                    fprintf(stderr, "Rom size should be a power of 2!\n");
                    exit(1);
                }
                break;
            }

            case 'b': {
                char *ptr = optarg;
                while(isspace((unsigned char)*ptr)){
                    ptr++;
                }
                if(*ptr == '-'){
                    fprintf(stderr, "Block size can't be negative!\n");
                    exit(1);
                }
                char *endptr;
                block_size = strtoul(optarg, &endptr, 0);
                printf("Parsed block_size: %u\n", block_size);
                if(optarg == endptr || *endptr != '\0'){
                    fprintf(stderr, "Invalid block size!\n");
                    exit(1);
                }
                break;
            }
            case 'C': {
                romContent_path = optarg;
                break;
            }

            case '?':
            default:
                fprintf(stderr, "Invalid arguments! Use -h or --help for more information.\n");
                exit(1);
        }
    }
    //TODO: Create a tracefile if tf is set!!!   
    //TODO: parse the requirements file!!!
    if(romContent_path != NULL){
        parameters.romContent = parseRom(romContent_path, rom_size);
    }else{
        parameters.romContent = (uint32_t*) calloc(rom_size / 4 > 0 ? rom_size / 4 : 1, sizeof(uint32_t));
    }

    char *request_file;
    if(optind < argc){
        request_file = argv[optind];
        printf("Request file: %s\n", request_file);
        if(optind + 1 < argc){
            fprintf(stderr, "Error: Too many positional arguments. Expected only the request file!");
            exit(1);
        }
    }else{
        fprintf(stderr, "Request file not given!");
        exit(1);
    }
    parameters.requests = parseRequest(request_file, &parameters.numRequests);
    parameters.cycles = cycles;
    parameters.tracefile = tracePath;
    parameters.latencyRom = latency;
    parameters.romSize = rom_size;
    parameters.blockSize = block_size;
    return parameters   ;
}