#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (mkguest("guest", 16*1024*1024) < 0) {
    printf("Error creating a guest OS\n");
    exit(1);
  }

  int status;
  wait(&status);
  if (status != 0) {
    printf("Guest OS crashed\n");
    exit(1);
  }

  return 0;
}
