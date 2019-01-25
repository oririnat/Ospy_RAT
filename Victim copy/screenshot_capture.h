#include <stdio.h>
#include <unistd.h>

#define MAX_DISPLAYS_NUM 32
#define SCREENSHOT_IMAGE_NAME "abc.jpg"

typedef enum {CAPTURE_SUCCESSFULLY, CAPTURE_FAILED} SCREENSHOT_STATUS;
SCREENSHOT_STATUS capture_screenshot();