/* https://cirosantilli.com/linux-kernel-module-cheat#fork */

#define _XOPEN_SOURCE 700
#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h> /* fork */

void print_pid(char *msg) {
    printf("%s ", msg);
    printf(
        "%s pid=%jd ppid=%jd\n",
        msg,
        (intmax_t)getpid(),
        (intmax_t)getppid()
    );
}

int main(void) {
    int status;
    /* This variable will be duplicated on the parent and on the child. */
    int i;
    pid_t pid;
    /* Parent PID */
    pid_t ppid;

    i = 0;
    ppid = getpid();
    if (ppid == -1) {
        perror("getpid");
        exit(EXIT_FAILURE);
    }

    /* Happens on parent only: child does not exist yet! */
    print_pid("before fork");
    /* Flush before fork so that existing output won't be duplicated. */
    fflush(stdout);
    fflush(stderr);

    /* In case of success, PID is set differently on parent and child
     * so you can distinguish between them. For the child, `pid = 0`. */
    pid = fork();
    if (pid == -1) {
        perror("fork");
        assert(false);
    }

    /* Happens both on parent and child. */
    print_pid("after fork");

    if (pid == 0) {
        /* Happens on child only.
         *
         * This print is asynchronous with the process stdout.
         * So it might not be in the line program order.
         * But they both go to the same terminal. */
        print_pid("inside (pid == 0)");

        /* Child has a different PID than its parent */
        pid = getpid();
        if (pid == -1) {
            perror("getpid");
            exit(EXIT_FAILURE);
        }
        assert(pid != ppid);

        /* This only change the child's `i` because memory was cloned (unlike threads). */
        i++;

        /* The child exits here. */
        exit(EXIT_SUCCESS);
    }

    /* Only the parent reaches this point because of the exit call
     * done on the child.
     *
     * Could happen before or after the child executes. */
    print_pid("after (pid == 0)");

    /* Wait for any child to terminate, then wake up.
     * Since we only have on child here, wait for that one child to terminate. */
    wait(&status);
    if (WIFEXITED(status)) {
        assert(status == WEXITSTATUS(EXIT_SUCCESS));
    } else {
        perror("execl abnormal exit");
        assert(false);
    }

    /* fork returns the child pid to the parent.
     *
     * This could be asserted with the getpid in the child,
     * but would require the child to communicate that back to the parent,
     * which would need a `mmap` + `semaphore`,
     * and we don't want to complicate the example too much. */
    print_pid("after wait");
    printf("fork() return = %jd\n", (intmax_t)pid);

    /* Memory was cloned, parent `i` was only modified in child memory. */
    assert(i == 0);

    return EXIT_SUCCESS;
}
