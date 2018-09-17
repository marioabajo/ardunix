/*
 * 
 */


/* ARDUNIX CONFIG PARAMETERS
 * ========================= */

//#define CMD_LS false
//#define CMD_PS false
//#define CMD_PWD false
//#define CMD_SH false
//#define CMD_SH_SIMPLE true
//#define CMD_FREE false
//#define CMD_TIMES false
//#define CMD_SET false
//#define CMD_TRUE false
//#define CMD_FALSE false
//#define CMD_CAT false
//#define CMD_ECHO false
//#define CMD_DEBUG false
//#define DEBUG 1

#include "ardunix.h"

// Add files (and content) to internal filesystem
const char PROGMEM _etc_test[] = "test";
const char PROGMEM _etc_test_empty[] = "empty";
const char PROGMEM _etc_script[] = "script";
const char PROGMEM script[] = "#!/bin/sh\n\nif /bin/false\nthen\n  /bin/ls /bin\nelse\n  /bin/ls /etc\nfi\n";

#define PROGFS_ENT_ETC_EXTRAS \
        PROGFS_ENTRY(_etc_script, script, FS_EXEC | FS_READ, sizeof(script)) \
        PROGFS_ENTRY(_etc_test, NULL, FS_DIR | FS_EXEC | FS_READ, 0) \
        PROGFS_ENTRY(_etc_test_empty, NULL, FS_EXEC | FS_READ, 0) \
        PROGFS_ENTRY(0, 0, 0, 0)

#define PROGFS_ENT_BIN_EXTRAS
/* END ARDUNIX CONFIG */

// Init ardunix internal filesystem
DEF_PROGFS

#ifdef __AVR__

FILE uart;

static void serial_setup()
{
	// Init serial port
	uart_init();
	// Init streams
	fdev_setup_stream(&uart, uart_putchar, uart_getchar, _FDEV_SETUP_RW);
	stdin = stdout = stderr = &uart;

}

void setup()
{
	serial_setup();
	printf_P(PSTR("Booting...\n"));
}

void loop()
#else
int main(void)
#endif
{
	int8_t ret;

	// Initial process (similar to "init" in linux)
	init_proc();

	// Show welcome message
	execl_P(PSTR("cat"), PSTR("/etc/issue"), 0);
	/*printf("DEBUG: %s\n", normalize_path("a/etc"));
	printf("DEBUG: %s\n", normalize_path("./abc/3"));
	printf("DEBUG: %s\n", normalize_paths("/home/a","../.bad/1"));
	printf("DEBUG: %s\n", normalize_path("////gr/"));
	printf("DEBUG: %s\n", normalize_path("/1/2/.////3/../../a/2/b"));
	printf("DEBUG: %s\n", normalize_path("//.//really/.././long/string/.to/check/../break/the/limits.././././/a"));*/
	

	// exec call for debugging purpose
	//execl_P(PSTR("sh"), PSTR("/etc/script"), 0);
	//execl("/bin/debug", "-s", 0);
	execl("free", 0);

	ret = execl("sh", 0);

	// Warning message in case the process end's and introduce a delay before respawn
	printf_P(PSTR("Init process exited(%d), waiting 10 seconds to restart\n"), ret);
	delay(10000);
}

