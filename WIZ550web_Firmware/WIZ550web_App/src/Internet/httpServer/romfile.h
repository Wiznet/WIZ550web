#define ROM_FNAMELEN    20  /* Maximum filename size */
#define FILE_HEAD_SIZE			(ROM_FNAMELEN +11)

typedef struct          /* Filename block structure */
{
    unsigned long len;               /* Length of file in bytes */
    unsigned long start;             /* Start address of file data in ROM */
    unsigned short check;             /* TCP checksum of file */
    unsigned char flags;            /* Embedded Gateway Interface (EGI) flags */
    char name[ROM_FNAMELEN];		/* Lower-case filename with extension */
} ROM_FNAME;

#define MAXFILES    	100 		// Limit on ROM file count (to stop runaway)

typedef union               		// ROM file directory entry format
{
    ROM_FNAME f;                	// Union of filename..
    unsigned char b[sizeof(ROM_FNAME)];  	// ..with byte values for i2c transfer
} ROM_DIR;


unsigned char search_file_rom(unsigned char *FileName, unsigned long *Address, unsigned long *Size);
