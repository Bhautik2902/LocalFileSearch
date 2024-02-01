#include<stdio.h>
#include<ftw.h>
#include<stdbool.h>
#include<regex.h>

// fileutil [ root_dir] filename 
// fileutil [root_dir] [storage_dir] [options] filename
// fileutil [root_dir] [storage_dir] extension
   
void main(int argc, char *argv[]) {

    // validating the command.
    if (argc == 3) {

    }
    else if (argc == 4) {
        
    }
    else if (argc == 5) {
        /* code */
    }
    else if (argc < 3) {
        printf("fileutil: Not enough arguments\n");
    }
    else {
        printf("fileutil: Too many arguments\n");
    }
    
}

bool validatePath (char *path) {
    regex_t rx;
    int retFlag;

    const char *ptn = "^(\\.?\\.?/|/)?([-_a-zA-Z0-9]+/)*[-_a-zA-Z0-9]+$";

    // Compiling the pattern.
    retFlag = regcomp(&rx, ptn, REG_EXTENDED);
    if (retFlag != 0) {
        printf("Failed to compile regex\n");
        return 0;  
    }

    // matching the pattern with file.
    retFlag = regexec(&rx, path, 0, NULL, 0);
}

