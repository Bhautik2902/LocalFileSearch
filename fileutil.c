#include<stdio.h>
#include<ftw.h>
#include<regex.h>
#include<string.h>

// fileutil [ root_dir] filename 
// fileutil [root_dir] [storage_dir] [options] filename
// fileutil [root_dir] [storage_dir] extension

char *targetFile;
char *opr;  // copy or move option.
char *destinationFolder;  // to copy or move into.

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

int searchFile(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    // getting the filename from path by advancing poitner by offset value.
    const char *filename = fpath + ftwbuf->base;
    
    // comparing current filename with target file 
    if (strcmp(filename, targetFile) == 0) {
        printf("%s\n", fpath);
        return 1;  // to stop traversal.
    }
    return 0; // Continue traversal

}

int moveFile (const char *sourceFile, char *filename) {

    // constructing the destination path.
    strcat(destinationFolder, "/");
    strcat(destinationFolder, filename);

    if (rename(sourceFile, destinationFolder) == 0) {
        printf("File moved to destination successfully.\n");
        return 0;
    } 
    else {
        perror("Error moving file.");
        return -1;
    }
}

int copyFile (const char *sourceFile, char *filename) {
    // constructing the destination path.
    strcat(destinationFolder, "/");
    strcat(destinationFolder, filename);

    FILE *source = fopen(sourceFile, "rb");
    FILE *destination = fopen(destinationFolder, "wb");

    // checking if files opened successfully
    if (source == NULL || destination == NULL) {
        perror("Couldn't open the file");
        return -1;
    }

    int buff;
    while ((buff = fgetc(source)) != EOF) {
        fputc(buff, destination);
    }

    fclose(source);
    fclose(destination);

    printf("File copied successfully.\n");
    return 0;
}

int srchCpMv (const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    const char *filename = fpath + ftwbuf->base;
    
    // comparing current filename with target file 
    if (strcmp(filename, targetFile) == 0) {
        printf("Found: %s\n", fpath);

        // check operation.
        if (strcmp(opr, "-mv") == 0) {  // move operation.
            moveFile(fpath, filename);
        }
        else if (strcmp(opr, "-cp") == 0) {  // copy operation.
            copyFile(fpath, filename);
        }
        return 1; 
    }
    return 0; 
}



int main(int argc, char *argv[]) {

    // validating the command.
    if (argc == 3) {    //print the path if file found, unsuccessful otherwise.
        if (validatePath(argv[1]) == 0) {
            printf("fileutil: Provided file path is not valid\n");
            return 1;
        }

        // assigning file path to global reference.
        targetFile = argv[2];
        int flags = 0;
        int isSuccess = nftw(argv[1], searchFile, 0, flags);

        if (isSuccess == -1) {
            printf("fileutil: file tree traversal failed\n");
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
        
        destinationFolder = argv[2];
        opr = argv[3];
        targetFile = argv[4];

        int flags = 0;
        int isSuccess = nftw(argv[1], srchCpMv, 0, flags);
        if (isSuccess == -1) {
            printf("fileutil: file tree traversal failed\n");
            return 1;
        }
        if (isSuccess == 0) {
            printf("fileutil: Search Unsuccessful\n");
        }
        return 0;


    }
    else if (argc == 4 ) {  // find and move/copy file with given extention to given dir.
        if (validatePath(argv[1])==0 || validatePath(argv[2])==0) {
            printf("fileutil: One or all provided file path is not valid\n");
            return 1;
        }
    }
    else if (argc < 3) {
        printf("fileutil: Not enough arguments\n\n");
        printf("Synopsis\n\n");
        printf("  fileutil [root_dir] filename\n");
        printf("  fileutil [root_dir] [storage_dir] [options] filename\n");
        printf("  fileutil [root_dir] [storage_dir] extension\n");
    }
    else {
        printf("fileutil: Too many arguments\n\n");
        printf("Synopsis:\n");
        printf("  fileutil [root_dir] filename\n");
        printf("  fileutil [root_dir] [storage_dir] [options] filename\n");
        printf("  fileutil [root_dir] [storage_dir] extension\n");
    }
    
}

