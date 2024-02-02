#include<stdio.h>
#include<ftw.h>
#include<regex.h>
#include<string.h>

// fileutil [ root_dir] filename 
// fileutil [root_dir] [storage_dir] [options] filename
// fileutil [root_dir] [storage_dir] extension

char *targetFile;

struct FTW {
    int base;
    int level;
    int flag;
};   

int validatePath (char *path) {
    regex_t rx;
    int retFlag;

    const char *ptn = "^(/[-_a-zA-Z0-9]+)+$";

    // Compiling the pattern.
    retFlag = regcomp(&rx, ptn, REG_EXTENDED);
    if (retFlag != 0) {
        return 0;  
    }

    // matching the pattern with filepath.
    retFlag = regexec(&rx, path, 0, NULL, 0);
    if (retFlag == 0) {
        return 1;
    }
    else if (retFlag ==  REG_NOMATCH) {
        return 0;
    }

    regfree(&rx);   
}

int processFiles(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    // getting the filename from path by advancing poitner by offset value.
    const char *filename = fpath + ftwbuf->base;
    
    // comparing current filename with target file 
    if (strcmp(filename, targetFile) == 0) {
        printf("%s\n", fpath);
        return 1;  // to stop traversal.
    }
    return 0; // Continue traversal
}

int main(int argc, char *argv[]) {

    // validating the command.
    if (argc == 3) {    //print the path if file found, unsuccessful otherwise.
        if (validatePath(argv[1]) == 0) {
            printf("fileutil: Provided file path is not valid\n");
            return 1;
        }
        targetFile = argv[2];
        int flags = 0;
        int isSuccess = nftw(argv[1], processFiles, 0, flags);

        if (isSuccess == -1) {
            perror("ftw");
            return 1;
        }
        if (isSuccess == 0) {
            printf("fileutil: Search Unsuccessful\n");
        }
        return 0;
    }
    else if (argc == 5) {  // find and move/copy file to given directory.
        if (validatePath(argv[1])==0 || validatePath(argv[2])==0) {
            printf("fileutil: One or all provided file path is not valid\n");
            return 1;
        }

    }
    else if (argc == 4 ) {  // find and move/copy file with given extention to given dir.
        if (validatePath(argv[1])==0 || validatePath(argv[2])==0) {
            printf("fileutil: One or all provided file path is not valid\n");
            return 1;
        }
    }
    else if (argc < 3) {
        printf("fileutil: Not enough arguments\n");
    }
    else {
        printf("fileutil: Too many arguments\n");
    }
    
}

