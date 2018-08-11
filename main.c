#include <stdio.h>

#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>

#include "enums.h"
#include "data.c"

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: %s mode modeparams...\n", argv[0]);
    return -1;
  }

  libusb_context *ctx = NULL;

  int r = libusb_init(&ctx);
  int exitcode = 0;
  if (r < 0) {
    printf("libusb_init error %d\n", r);
    return 1;
  }
  libusb_device_handle *handle = NULL;
  handle = libusb_open_device_with_vid_pid(ctx, 0x1044, 0x7a39);
  if (handle == NULL) {
    printf("Failed to open device!\n");
    libusb_exit(ctx);
    return 1;
  }
  if (libusb_set_auto_detach_kernel_driver(handle, 0) < 0) {
    printf("Kernel ctrl driver auto detach failed.\n");
    goto exit;
  }
  if (libusb_set_auto_detach_kernel_driver(handle, 3) < 0) {
    printf("Kernel driver auto detach failed.\n");
    goto exit;
  }

  r = libusb_claim_interface(handle, 0);
  if (r < 0) {
    printf("Failed to claim ctrl interface! %d\n", r);
    exitcode = 4;
    goto exit;
  }
  r = libusb_claim_interface(handle, 3);
  if (r < 0) {
    printf("Failed to claim interface! %d\n", r);
    exitcode = 2;
    goto exit;
  }

  char* mode = argv[1];
  if (strcmp(mode, "custom") == 0) {
    if (argc < 3) {
      printf("Usage: %s custom file\n", argv[0]);
      exitcode = -1;
      goto exit;
    }
    // Custom mode
    FILE *fd = fopen(argv[2], "rb");
    if (!fd) {
      printf("fopen(%s) failed: %s\n", argv[2], strerror(errno));
      exitcode = -1;
      goto exit;
    }
    fread(m_white_data, 512, 1, fd);
    fclose(fd);

    r = set_custom_mode(handle, m_white_data);
    if (r < 0) {
      printf("Failed to set custom mode!\n");
      exitcode = -1;
      goto exit;
    }
    exitcode = -1;
    goto exit;
  }

exit:
  libusb_release_interface(handle, 0);
  libusb_release_interface(handle, 3);
  libusb_close(handle);
  libusb_exit(ctx);
  return exitcode;
}
