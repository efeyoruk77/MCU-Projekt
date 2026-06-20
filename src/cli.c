#include <getopt.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
int main(int argc, char** argv){
    int c;
    char* helpMessage = "What even is C about"; 
    uint32_t latency = 1;
    uint32_t cycles = 1;
    uint32_t rom_size = 0x100000;
    uint32_t block_size = 0x1000;

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

        //TODO: Create a tracefile if tf is set!!!
        
        //TODO: parse the requirements file!!!
    }
    return 0;
}