#include <getopt.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include "../include/types.hpp"

static char* read_file(const char* path){
    char* string = NULL;
    FILE* file;

    if(!(file = fopen(path, "r"))){
        perror("Error opening the file");
        return NULL;
    }

    struct stat statbuf;
    if(fstat(fileno(file), &statbuf)){
        fprintf(stderr, "Error retrieving file stats\n");
        goto cleanup;
    }

    if(!S_ISREG(statbuf.st_mode) || statbuf.st_size <= 0){
        fprintf(stderr, "Error processing the file\n");
        goto cleanup;
    }

    if(!(string = (char*) malloc(statbuf.st_size + 1))){
        fprintf(stderr, "Not enough memory\n");
        goto cleanup;
    }

    if(fread(string, 1, statbuf.st_size, file) != (size_t) statbuf.st_size){
        fprintf(stderr, "Error reading the file\n");
        free(string);
        string = NULL;
        goto cleanup;
    }

    string[statbuf.st_size] = '\0';

    cleanup:
        if(file) fclose(file);
        return string;
}

static char* split(char* s){
    while(isspace((unsigned char) *s)) s++;
    char* end = s + strlen(s);
    while(end > s && isspace((unsigned char) end[-1])) end--;
    *end = '\0';
    return s;
}

static uint32_t parseUint32Field(const char* field, const char* fieldName){
    if(*field == '-'){
        fprintf(stderr, "%s can't be negative!\n", fieldName);
        exit(1);
    }

    errno = 0;
    char* endptr;
    int base = (field[0] == '0' && (field[1] == 'x' || field[1] == 'X')) ? 16 : 10;
    unsigned long value = strtoul(field, &endptr, base);

    if(endptr == field || *endptr != '\0'){
        fprintf(stderr, "Invalid value for %s: \"%s\"\n", fieldName, field);
        exit(1);
    }

    if(errno == ERANGE || value > UINT32_MAX){
        fprintf(stderr, "%s exceeds 32 bits: \"%s\"\n", fieldName, field);
        exit(1);
    }
    return (uint32_t) value;
}

uint32_t* parseRom(const char* path, uint32_t rom_size){
    uint32_t capacity = rom_size / 4;
    uint32_t* rom = (uint32_t*)calloc(capacity, sizeof(uint32_t));

    if(capacity != 0 && rom == NULL){
        fprintf(stderr, "out of memory");
        exit(1);
    }

    if(path == NULL){
        return rom;
    }

    char* content = read_file(path);
    if(content == NULL){
        free(rom);
        exit(1);
    }

    const char*p = content;
    uint32_t count = 0;
    uint32_t index = 0;

    while (true)
    {
        while(isspace((unsigned char) *p)){
            if(*p == '\n') index++;
            p++;
        }
        if(*p == '\0') break;

        if(*p == '-'){
            fprintf(stderr, "Negative value\n");
            free(content);
            free(rom);
            exit(1);
        }

        errno = 0;
        char* endptr;
        int base = (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) ? 16 : 10;
        unsigned long value = strtoul(p, &endptr, base);

        if(endptr == p){
            fprintf(stderr, "Invalid value\n");
            free(content);
            free(rom);
            exit(1);
        }

        if(errno == ERANGE || value > UINT32_MAX){
            fprintf(stderr, "Value exceeds 32 bits\n");
            free(content);
            free(rom);
            exit(1);
        }

        if(!isspace((unsigned char)*endptr) && *endptr != '\0'){
            fprintf(stderr, "Invalid value\n");
            free(content);
            free(rom);
            exit(1);
        }

        if(count >= capacity){
            fprintf(stderr, "ROM file has more values then the ROM holds\n");
            free(content);
            free(rom);
            exit(1);            
        }

        rom[count] = (uint32_t) value;
        count++;
        p = endptr;
    }
    free(content);
    return rom;
}

struct Request* parseRequest(const char* file, uint32_t* numRequests){
    
    char* content = read_file(file);

    if(content == NULL){
        exit(1);
    }

    uint32_t ctr = 0;
    uint32_t size = 16;
    struct Request* requestPtr = (struct Request*) malloc(size * sizeof(struct Request));

    if(requestPtr == NULL){
        fprintf(stderr, "out of memory\n");
        free(content);
        exit(1);
    }

    char* line = content;

    while(line != NULL && *line != '\0'){

        char* newLine = strchr(line, '\n');
        if(newLine != NULL){
            *newLine = '\0';
        }

        size_t lineLength = strlen(line);

        if(lineLength > 0 &&
           line[lineLength - 1] == '\r'){
            line[lineLength - 1] = '\0';
        }
        char* trimmedLine = line;
        while(isspace((unsigned char)*trimmedLine)){
            trimmedLine++;
        }

        if(*trimmedLine != '\0'){
            char* fields[5];
            char* ptr = trimmedLine;

            for(int i = 0; i < 4; i++){
                char* comma = strchr(ptr, ',');
                if(comma == NULL){
                    fprintf(stderr, "Invalid request line %u: expected 5 fields\n", ctr);
                    free(content);
                    free(requestPtr);
                    exit(1);
                }
                *comma = '\0';
                fields[i] = split(ptr);
                ptr = comma + 1;
            }

            if(strchr(ptr, ',') != NULL){
                fprintf(stderr, "Invalid request line %u: too many fields\n", ctr);
                free(content);
                free(requestPtr);
                exit(1);
            }

            fields[4] = split(ptr);
            struct Request request;

            if(strlen(fields[0]) != 1 ||
               (fields[0][0] != 'R' &&
                fields[0][0] != 'W')){
                fprintf(stderr, "Invalid request type on line %u: \"%s\" (expected R or W)\n", ctr, fields[0]);
                free(content);
                free(requestPtr);
                exit(1);
            }

            request.w = (fields[0][0] == 'W') ? 1 : 0;
            request.addr = parseUint32Field(fields[1], "address");

            if(strlen(fields[4]) != 1 ||
               (fields[4][0] != 'T' &&
                fields[4][0] != 'F')){
                fprintf(stderr, "Invalid wide flag on line %u: \"%s\" (expected T or F)\n", ctr, fields[4]);
                free(content);
                free(requestPtr);
                exit(1);
            }

            request.wide = (fields[4][0] == 'T') ? 1 : 0;

            if(request.w){
                if(fields[2][0] == '\0'){
                    fprintf(stderr, "Missing data value for write request on line %u\n", ctr);
                    free(content);
                    free(requestPtr);
                    exit(1);
                }

                request.data = parseUint32Field(fields[2], "data");

                if(!request.wide &&
                   request.data > 0xFF){
                    fprintf(stderr, "Data value exceeds 1 byte for narrow write on line %u: \"%s\"\n", ctr, fields[2]);
                    free(content);
                    free(requestPtr);
                    exit(1);
                }
            } else {
                if(fields[2][0] != '\0'){
                    fprintf(stderr, "Data value must be empty for read request on line %u\n", ctr);
                    free(content);
                    free(requestPtr);
                    exit(1);
                }
                request.data = 0;
            }

            uint32_t user = parseUint32Field(fields[3], "user");

            if(user > 255){
                fprintf(stderr, "User value out of range on line %u: \"%s\"\n", ctr, fields[3]);
                free(content);
                free(requestPtr);
                exit(1);
            }
            request.user = (uint8_t) user;

            if(ctr >= size){
                size *= 2;
                struct Request* request =
                    (struct Request*)
                    realloc(requestPtr, size * sizeof(struct Request));
                if(request == NULL){
                    fprintf(stderr, "out of memory\n");
                    free(content);
                    free(requestPtr);
                    exit(1);
                }
                requestPtr = request;
            }
            requestPtr[ctr++] = request;
        }
        line = newLine != NULL ? newLine + 1 : NULL;
    }

    free(content);
    *numRequests = ctr;
    return requestPtr;
}



struct Parameters parse_cli(int argc, char** argv){
    int c;
    char* helpMessage = "Usage: ./project [flags] [request_file.csv]\nFlags:\n \tcycles: Maximum cycles for the simulation.\n \ttf: If tracefile should be created with argument name.\n \tlatency-rom, rom-content, rom-size: Parameters for the ROM.\n \tblock-size: Block size to split the main memory for ownership.\nOptional argument: request_file.csv: Path for the request file containing the memory accesses.\n"; 
    uint32_t latency = 1;
    uint32_t cycles = 5000;
    uint32_t rom_size = 0x10000;
    uint32_t block_size = 0x1000;

    struct Parameters parameters;

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
                if(block_size == 0){
                    fprintf(stderr, "Block size can't be zero\n");
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
    //romContent
    parameters.romContent = parseRom(romContent_path, rom_size);
    //requests
    return parameters   ;
}