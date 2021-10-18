#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <string.h>

char* pwd() {
    char* pwd = malloc(PATH_MAX);
    if (getcwd(pwd, PATH_MAX) != NULL) {
        return pwd;
    }
    return NULL;
}

int cd(char* path) {
    if (path == NULL) {
        printf("error path %s", path);
        return -1;
    }
    if (chdir(path) < 0) {
        printf("Error change directory to %s\n", path);
        return -1;
    }
    return 0;
}

char* ls() {
    char* result = malloc(1024);
    char* p = result;
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    if (d) {
        while ((dir = readdir(d)) != NULL) {
            //printf("%s\n", dir->d_name);
            char* file_name = dir->d_name;
            int len = strlen(file_name);
            strcpy(p, file_name);
            p = p + len;
            *p = '\n';
            p++;
    }
    *p = '\0';
    closedir(d);
  }

  return result;
}

int main(int argc, char **argv) {
    char* current_dir = pwd();
    if (current_dir != NULL) {
        printf("Current directory %s\n", current_dir);
    }

    char* changeToDir = "..";
    int rc = cd(changeToDir);
    if (rc == 0) {
        printf("Change to %s success\n", changeToDir);
    }

    current_dir = pwd();
    if (current_dir != NULL) {
        printf("Current directory %s\n", current_dir);
    }

   char* files = ls();
   printf(files);

   free(current_dir);
   free(files);
}