#pragma once

#include <stdint.h>

#pragma pack(push, 1)
typedef struct {
  // Kind of the control transfer
  uint8_t kind;
  // ?? always zeroed
  uint8_t reserved;
  // mode or config slot
  uint8_t mode;
  // Speed or length of usb packets to follow
  uint8_t speedLength;
  // Brightness. 0 to 50
  uint8_t brightness;
  // Predefined color
  uint8_t color;
  uint8_t reserved2;
  uint8_t checksum;
} header_t;
#pragma pack(pop)


#define KIND_PRESET 0x08
#define KIND_READ_CONFIG 0x92
#define KIND_CUSTOM_CONFIG 0x12

#define RESERVED1 0x00

// This is the list of predefined modes.
// For custom config, this byte describes the config slot
#define MODE_STATIC 0x01
#define MODE_BREATHING 0x02
#define MODE_WAVE 0x03
#define MODE_FADE_ON_KEYPRESS 0x04
#define MODE_MARQUEE 0x05
#define MODE_RIPPLE 0x06
#define MODE_FLASH_ON_KEYPRESS 0x07
#define MODE_NEON 0x08
#define MODE_RAINBOW_MARQUEE 0x09
#define MODE_RAINDROP 0x0a
#define MODE_CIRCLE_MARQUEE 0x0b
#define MODE_HEDGE 0x0c
#define MODE_ROTATE 0x0d

#define COLOR_RED 0x01
#define COLOR_GREEN 0x02
#define COLOR_YELLOW 0x03
#define COLOR_BLUE 0x04
#define COLOR_PURPLE 0x05
#define COLOR_AQUA 0x06
#define COLOR_WHITE 0x07

void fin_header(header_t *hdr) {
  hdr->reserved = 0;
  uint16_t chksumtmp = 0;
  uint8_t *data = (uint8_t*)hdr;
  for (uint8_t i = 0; i < 7; i++) {
    chksumtmp+=data[i];
  }
  hdr->checksum = (uint8_t)(0xff - (chksumtmp & 0xff));
}

int set_mode(libusb_device_handle *handle, uint8_t mode, uint8_t color, uint8_t brightness, uint8_t speed) {
  header_t header;
  header.kind = KIND_PRESET;
  header.reserved = RESERVED1;
  header.mode = 0x33;
  header.speedLength = 0x05;
  header.brightness = 0x32;
  header.color = 0x02;
  header.reserved2 = 0x01;
  fin_header(&header);

  int r = libusb_control_transfer(handle, 0x21, 0x09, 0x0300, 0x0003, (uint8_t*)&header, 0x0008, 0);
  if (r < 0) {
    printf("Control transfer 1 failed! %d\n", r);
    return -1;
  } else {
    printf("Control transfer 1 OK!\n");
  }

  header.mode = mode;
  header.speedLength = speed;
  header.brightness = brightness;
  header.color = color;
  header.reserved2 = 0;
  fin_header(&header);
  r = libusb_control_transfer(handle, 0x21, 0x09, 0x0300, 0x0003, (uint8_t*)&header, 0x0008, 0);
  if (r < 0) {
    printf("Control transfer 2 failed! %d\n", r);
    return -1;
  } else {
    printf("Control transfer 2 OK!\n");
  }
  return 0;
}

int set_custom_mode(libusb_device_handle *handle, uint8_t *data) {
  header_t header;
  header.kind = KIND_CUSTOM_CONFIG;
  header.reserved = RESERVED1;
  header.mode = 1;
  header.speedLength = 8;
  header.brightness = 0;
  header.color = 0;
  header.reserved2 = RESERVED1;
  fin_header(&header);

  int r = libusb_control_transfer(handle, 0x21, 0x09, 0x0300, 0x0003, (uint8_t*)&header, 0x0008, 0);
  if (r < 0) {
    printf("Control transfer 1 failed! %d\n", r);
    return -1;
  } else {
    printf("Control transfer 1 OK!\n");
  }

  for (uint8_t i = 0; i < 8; i++) {
    int transferred = 0;
    r = libusb_interrupt_transfer(handle, (6 | LIBUSB_ENDPOINT_OUT), data + (i * 64), 64, &transferred, 0);
    if (r < 0 || transferred != 64) {
      printf("Interrupt transfer %d failed", i);
    }
  }
  printf("Interrupt transfers OK!\n");

  header.kind = KIND_PRESET;
  header.reserved = RESERVED1;
  header.mode = 0x34;
  header.speedLength = 0x05;
  header.brightness = 0x32;
  header.color = 0x02;
  header.reserved2 = 0x01;
  fin_header(&header);

  r = libusb_control_transfer(handle, 0x21, 0x09, 0x0300, 0x0003, (uint8_t*)&header, 0x0008, 0);
  if (r < 0) {
    printf("Control transfer 2 failed! %d\n", r);
    return -1;
  } else {
    printf("Control transfer 2 OK!\n");
  }
  return 0;
}
