#ifndef CONFIG_H
#define CONFIG_H

// ARGMAX never bigger than 256, uint8_t used as index
#define ARGMAX 80   // max input line size
#define FILENAME_MAX 16 // filename max size
#define PATH_MAX 64 // path max size
#define NCARGS 16   // max number of parameters
#define ENV_MAX 16  // max number of environment variables
#define IFS ' '     // inter field separator
// TODO: just one path at this moment
#define PATH "/bin" // fixed path
#define HOME "/"    // fixed home


//#define DEBUG 1


#endif

