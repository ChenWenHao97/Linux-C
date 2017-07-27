#include<unistd.h>
#include<sys/wait.h>
int main() {
    pid_t child = fork();
    char *tsts[256] = { "ls", "-l", (char*)NULL};
    if (child == 0) {
        execvp(tsts[0], tsts);
    } else {
        waitpid(child, NULL, 0);
    }
}
