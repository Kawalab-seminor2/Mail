#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

int
main(int argc, char* argv[]) {
    int n = 0;
    struct dirent *de;
    DIR *d = opendir("UserA");
    while ((de = readdir(d))) {
        struct stat st;
        if (stat(de->d_name, &st) == 0 && st.st_mode & S_IFREG)
            n++;
    }
    closedir(d);
    printf("ファイルが %d 個あります\n", n);
    return 0;
}
