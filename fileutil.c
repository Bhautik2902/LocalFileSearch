#define _XOPEN_SOURCE 500
#define _GNU_SOURCE
#include<stdio.h>
#include<ftw.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>

char *targetFile;
char *opr;  // copy or move option.
char *destinationFolder;  // to copy or move into.
int fileCount=0;
char *exten;  // extention of the file provided by user.
char set[100][50];  // to track second occurance of files. (initial size is taken 100 where each filename can have 50 characters) 
int setsize=-1;  // keep track of set size.

// UTILITY FUNCTIONS 

// to get extention from file name.
char *getExtention(char *filename) {
    char *ext = strrchr(filename, '.');  //from last occurence of . extention begins
    if (ext && ext != filename) {
        return ext + 1;  // returning position after '.'.
    }
    return NULL; // No extension found
}

int moveFile (const char *sourceFile, char *filename) {

    // constructing the destination path.
    strcat(destinationFolder, "/");
    strcat(destinationFolder, filename);

    if (rename(sourceFile, destinationFolder) == 0) {
        printf("File moved successfully. Path: %s\n", destinationFolder);
        return 0;
    } 
    else 
    {
        perror("File Move");
        return -1;
    }
}

int copyFile (const char *sourceFile, char *filename) {
    // constructing the destination path.
    strcat(destinationFolder, "/");
    strcat(destinationFolder, filename);

    // opening file for binary reading
    FILE *source = fopen(sourceFile, "rb");

    // opening file for binary writing
    FILE *destination = fopen(destinationFolder, "wb");

    // checking if files opened successfully
    if (source == NULL || destination == NULL) {
        perror("Opening file");
        return -1;
    }

    int buff;
    // Doing buffered read and write until hits end of file.
    while ((buff = fgetc(source)) != EOF) {
        fputc(buff, destination);
    }

    fclose(source);
    fclose(destination);

    printf("File copied successfully. Path: %s\n", destinationFolder);
    return 0;
}

int isVisited(char *fname) {  // returns 1 if filename found, 0 otherwise.
    
    for (int i=0; i<=setsize; i++) {
        if (strcmp(set[i], fname) == 0) {  
            return 1;  
        }
    }
    return 0;
}

// Command 1      fileutil [root_path] filename
int searchFile(const char *fpath, const struct stat *sb, int tf, struct FTW *ftwbuf) {

    // getting the filename from path by advancing poitner by offset value.
    const char *filename = fpath + ftwbuf->base;
    
    // comparing current filename with target file 
    if (strcmp(filename, targetFile) == 0) {
        printf("%s\n", fpath);
        fileCount++;
    }

    return 0; // Continue traversal
}

// command 2     filename [source_path] [destination_path] [option] filename
int srchCpMv (const char *fpath, const struct stat *sb, int tf, struct FTW *ftwbuf) {

    char *filename = strdup(fpath + ftwbuf->base);
    
    // comparing current filename with target file 
    if (strcmp(filename, targetFile) == 0) {
        printf("Found: %s\n", fpath);

        // check the operation.
        if (strcmp(opr, "-mv") == 0) {  // move operation.
            moveFile(fpath, filename);
        }
        else if (strcmp(opr, "-cp") == 0) {  // copy operation.
            copyFile(fpath, filename);
        }
        else {  
            printf("fileutil: Invalid option");
        }
        return 1; 
    }
    return 0; 
}

// command 3    fileutil [source_path] [destination_path] extention
int createTar (const char *fpath, const struct stat *sb, int tf, struct FTW *ftwbuf) {

    // fetching file name from whole filepath
    char *filename = strdup(fpath + ftwbuf->base);
    char *ext = getExtention(filename);

    // comparing extention
    if (ext != NULL && (strcmp(ext, exten) == 0)) {

        if (isVisited(filename) == 1) {  // if encounterd another file with same name
            return 0;  // to continue from the next file in traversal
        } 
        printf("found: %s\n", fpath);
        setsize++;  // incresing set size by 1.
        strcpy(set[setsize], filename);  //put first time encounterd filename in set.
    
        // coping matched file to A1 folder.
        char cmd[128];
        snprintf(cmd, sizeof(cmd), "cp %s %s", fpath, destinationFolder);

        if (system(cmd) == 0) {
            fileCount++;
        }
        else {
           perror("copy file");
           return -1;
        }      
    }
    return 0; 
}

int main(int argc, char *argv[]) {

    // validating the command.
    if (argc == 3) {    //print the path if file found, unsuccessful otherwise.
       
        // assigning file path to global reference.
        targetFile = argv[2];
        int flags = 0;
        int isSuccess = nftw(argv[1], searchFile, 0, FTW_PHYS);

        if (isSuccess == -1) {
            perror("nftw");    
            exit(EXIT_FAILURE);        
        }
        else if (isSuccess == 0 && fileCount == 0) {
            printf("fileutil: Search Unsuccessful\n");
        }
        return 0;
    }
    else if (argc == 5) {  // find and move/copy file to given directory.
       
        destinationFolder = argv[2];
        opr = argv[3];
        targetFile = argv[4];

        int isSuccess = nftw(argv[1], srchCpMv, 0, FTW_PHYS);
        if (isSuccess == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
        if (isSuccess == 0) {
            printf("fileutil: Search Unsuccessful\n");
        }

        return 0;
    }
    else if (argc == 4 ) {  // create tar file of files of given extention. 
       
        exten = strdup(argv[3]);
        destinationFolder = argv[2];

        // constructing the destination path.
        strcat(destinationFolder, "/A1");
        
        // create directory to store matching file.
        int dirStatus = mkdir(destinationFolder, 0777);
        if (dirStatus == -1) {
            if (errno == ENOENT) {  // error indicating that provided path is not exists
                // creating the path             
                char cmd[128];
                snprintf(cmd, sizeof(cmd), "mkdir -p %s", destinationFolder);  // creates directory if not exising already.
                if (system(cmd) == -1) 
                    perror("Mkdir");              
            } 
            else {
                perror("Error creating directory");
                exit(EXIT_FAILURE);
            }
        }

        int flags = 0;
        int isSuccess = nftw(argv[1], createTar, 0, FTW_PHYS);
        if (isSuccess == -1) {
            perror("nftw");
            exit(EXIT_FAILURE);
        }
        if (isSuccess == 0) {
            char cmd[128];
            snprintf(cmd, sizeof(cmd), "tar -cf %s -C %s .", "A1.tar", argv[2]);

            if (system(cmd) == 0) {
                printf("fileutil: tar archieve of %d files created.\n", fileCount);

                // removing temporary A1 directory.
                snprintf(cmd, sizeof(cmd), "rm -r %s", destinationFolder);
                system(cmd);
            }
            else {
                printf("fileutil: error occured while creating an archive.\n");
            }
        }     
        return 0;
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

