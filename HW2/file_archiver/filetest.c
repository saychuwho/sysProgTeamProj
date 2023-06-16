#include <stdio.h>
#include <dirent.h>
#include <string.h>

int main() {
    DIR *dir;
    struct dirent *entry;

    // Directory path
    char *path = "./fileSrc";

    // Open the directory
    dir = opendir(path);
    if (dir == NULL) {
        printf("Unable to open directory\n");
        return 1;
    }

    // Read directory entries
    while ((entry = readdir(dir)) != NULL) {
        // Exclude "." and ".." entries
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            printf("%s\n", entry->d_name);
        }
    }

    // Close the directory
    closedir(dir);

    return 0;
}