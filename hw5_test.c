/*
 * HW5 test scenario 
 * Edited by Mournighan
 */
#include <stdio.h>
#include <errno.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <signal.h>

void print_test() {
    printf("@@@@uid: %d\n", getuid());
    printf("@@@@pid: %d\n", getpid());
}
// Args in Bytes
int main(int argc, char **argv) {
    printf("==================================\n");
    printf("Welcome to hw5_test\n");
    // Getting access to the user struct we want to edit
  struct passwd *pwd_struct = getpwnam(argv[1]);
  if (pwd_struct == NULL) {
    printf("Failed to fetch passwd struct\n");
    return -1;
  }
  printf("pw->uid=%d, pw->name=%s\n", pwd_struct->pw_uid, pwd_struct->pw_name);
    // Get mem limit in bytes
  int limit = atoi(argv[2]);
    // Number of processes 
  pid_t *pids = (pid_t*)malloc((argc-3)*sizeof(pid_t));
    // Set the limit of the user struct
  syscall(378, pwd_struct->pw_uid, limit);

  int i, mem;
    // Display current process and user information
  print_test();
      
  for (i = 3; i < argc; ++i) {
    // Set amount to allocate  in Bytes
    // 4KB page on the nexus
     mem = atoi(argv[i]);
    // fork()
     pid_t p = pids[i-3] = fork();
    // Child action
     if (0 == p) {
        printf("child process start malloc: pid=%d, uid=%d, mem=%d\n", getpid(), getuid(), mem);
        //  Allocate the memory as an array of chars
        char *m = (char*)malloc(mem);
        // See if malloc passed
        if(m == NULL)
        {
            printf("Malloc failed with pid=%d, uid=%d, mem=%d\n", getpid(), getuid(), mem);
            return -1;
        }
        //memset(m, 0, mem);
        // Set all memory to 0. s
        int j = 0;
        for (j = 0; j < mem; j++) {
            // Set all to null
            m[j] = '\0';
        }
        printf("child process finish malloc: pid=%d, uid=%d, mem=%d\n", getpid(), getuid(), mem);
        // Free
        free(m);
        return 0;
     }
    // Parent Process    
  }
    // Wait for all childrenl
  for (i = 0; i < argc-3; ++i) {
    waitpid(pids[i], NULL, 0);
  }
  // Free 
  free(pids);
  printf("==================================\n");
  return 0;
}

