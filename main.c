#include <stdio.h>

#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <libusb-1.0/libusb.h>

#include "enums.h"
#include "data.c"

int main(int argc, char **argv) {
  libusb_context *ctx = NULL;

  int r = libusb_init(&ctx);
  int exitcode = 0;
  if (r < 0) {
    printf("libusb_init error %d\n", r);
    return 1;
  }

  libusb_set_debug(ctx, 3);

  libusb_device_handle *handle = NULL;
  handle = libusb_open_device_with_vid_pid(ctx, 0x1044, 0x7a39);
  if (handle == NULL) {
    printf("Failed to open device!\n");
    libusb_exit(ctx);
    return 1;
  }
  if (libusb_set_auto_detach_kernel_driver(handle, 0) < 0) {
    printf("Kernel ctrl driver auto detach failed.");
    goto exit;
  }
  if (libusb_set_auto_detach_kernel_driver(handle, 3) < 0) {
    printf("Kernel driver auto detach failed.");
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

  r = set_mode(handle, MODE_RAINDROP, COLOR_BLUE, 0x80, 0x0a);
  if (r < 0) {
    printf("Failed to set mode!");
  }

  sleep(1);
  for (int i = 0; i < 128; i++) {
    printf("Setting key %d to green!", i);
    m_white_data[(i*4) + 2] = 0xff;
    r = set_custom_mode(handle, m_white_data);
    if (r < 0) {
      printf("Failed to set custom mode!");
    }
    m_white_data[(i*4) + 2] = 0x00;
  }
  r = set_mode(handle, MODE_STATIC, COLOR_WHITE, 0x80, 0x0a);
  if (r < 0) {
    printf("Failed to set white!");
  }

  libusb_release_interface(handle, 0);
  libusb_release_interface(handle, 3);

exit:
  libusb_close(handle);
  libusb_exit(ctx);
  return exitcode;
}
