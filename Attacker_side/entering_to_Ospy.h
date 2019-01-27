#include "start_menu.h"
#include <string.h>

#define LICENSE_KEY_LENGTH 20

typedef enum {REGISTERED_SUCCESSFULLY, USERNAME_TAKEN, INVALID_USERNAME, LICENES_KEY_INVALID, PASSWORD_VERIFICTION_INVALID} registration_status;
typedef enum {ENTERING_ACTION_SUCCESS, ENTERING_ACTION_FAILURE} ENTERING_STATUS;

typedef struct {
	char licenses_key[LICENSE_KEY_LENGTH];
} licenses_key_item;

ENTERING_STATUS log_in_attacker();
ENTERING_STATUS register_attacker();
void buy_license_key();
void copytoclipboard(const char * str);



