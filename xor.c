#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>
#define MINARGS 3

int main(int argc, char* argv[]){

    long int count;

    size_t i, s, bytes_read, xor_max;
    size_t f_sizes[2];

    unsigned char p, q;
    unsigned char c = 0, z = 0;

    char *filename, *res;
    char **bufs = malloc(sizeof(char*) * 2);
    
    FILE *f;

    struct stat attr;

    if(argc < MINARGS){
        fprintf(stderr,
            "Usage: %s file1 file2 [-c] [-z]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    while ((s = getopt (argc, argv, "c:hz")) != -1){
        switch(s){
            case 'c':
                errno = 0;
                count = strtol(optarg, NULL, 10);
                if(errno == ERANGE){
                    fprintf(stderr, "-c argument out of range; aborting\n");
                    exit(EXIT_FAILURE);
                }
                c = 1;
                break;
            case 'z':
                z = 1;
                break;
            case '?':
                fprintf(stderr,
                    "Usage: %s file1 file2 [-c] [-z]\n", argv[0]);
                exit(EXIT_FAILURE);
            default:
                abort();
        }
    }
    
    // lets check if the files actually exist
    for (i = 0; i < MINARGS-1; i++){
        filename = argv[i + optind];
        if(stat(filename, &attr) == -1){
            fprintf(stderr, "Could not stat file \"%s\", exiting;\n", filename);
        }
        errno = 0;
        if(access(filename, R_OK) || errno){
            fprintf(stderr, "File %s not found; exiting\n", argv[i]);
            exit(EXIT_FAILURE);
        }
        bufs[i] = malloc(sizeof(char) * attr.st_size);
        if(!bufs[i]){
            fprintf(stderr,
                    "Could not allocate %li bytes of memory for file %s",
                    attr.st_size, filename);
            exit(EXIT_FAILURE);
        }
        f = fopen(filename, "r");

        // lets read
        if(!(bytes_read = fread(bufs[i], attr.st_size, 1, f))){
            fprintf(stderr, "Could not read %lu bytes from file %s "
                    "only read %lu bytes; exiting\n",
                    attr.st_size, filename, bytes_read);
            exit(EXIT_FAILURE);
        }
        f_sizes[i] = attr.st_size;
    }

    if(c){
        if((count <= f_sizes[0]) && (count <= f_sizes[1])){
            xor_max = count;
        }
        // one of the files are larger than count. If z is valid we 
        // will go for the upper limit
        else if(z){
            xor_max = (f_sizes[0] > f_sizes[1]) ? f_sizes[0]: f_sizes[1];
        }
        else
            xor_max = (f_sizes[0] < f_sizes[1] ) ? f_sizes[0]: f_sizes[1];
    }
    else{
        xor_max = (f_sizes[0] < f_sizes[1] ) ? f_sizes[0]: f_sizes[1];
    }
    res = malloc(sizeof(char) * xor_max);

    for (i = 0; i < xor_max; i++){
        if(z){
            p = (i > f_sizes[0]) ? bufs[0][i] : '\0';
            q = (i > f_sizes[1]) ? bufs[1][i] : '\0';
        }
        else{
            p = bufs[0][i];
            q = bufs[1][i];
        }
        res[i] = p ^ q;
    }
    fwrite(res, xor_max, 1, stdout);
    return EXIT_SUCCESS;
}
