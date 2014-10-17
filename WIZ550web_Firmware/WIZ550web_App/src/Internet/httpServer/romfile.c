#include <stdio.h>
#include <string.h>

#include "dataflash.h"
#include "romfile.h"

unsigned char search_file_rom(unsigned char *FileName, unsigned long *Address, unsigned long *Size)
//uint8_t search_file_rom(uint8_t * FileName, uint32_t * Address, uint32_t * Size)
{
	uint8_t fileidx = 1;

	ROM_DIR file;
	uint32_t addr = 0;

	while(1)
	{
		read_from_flashbuf((unsigned long)addr, (unsigned char*)&file.b, sizeof(file));

        if(file.b[0]==0xff && file.b[1]==0xff)
		{
			break;
		}

        //search file
		//if(!strcmp((char *)FileName, file.f.name))
		if(!stricmp((char *)FileName, file.f.name)) // stricmp(): Compare String without Case Sensitivity
		{
			*Size = file.f.len;
			*Address = file.f.start;

			return fileidx;
		}
		addr += FILE_HEAD_SIZE;
		fileidx++;
	}

	// Second search phase for web upload
	uint8_t buf[512] = {0, };
	uint16_t i;
	uint32_t header_len = 0;

	addr = 0;
	read_from_flashbuf((unsigned long)addr, buf, sizeof(buf));
	for(i = 0; i < sizeof(buf)-3; i++)
	{
		if(buf[i] == '\r')
		{
			if(buf[i+1] == '\n' && buf[i+2] == '\r' && buf[i+3] == '\n')
			{
				header_len = addr = i+3+1;
				break;
			}
		}
	}

	while(addr)
	{
		read_from_flashbuf((unsigned long)addr, (unsigned char*)&file.b, sizeof(file));

		if(file.b[0]==0xff && file.b[1]==0xff)
		{
			break;
		}

        //search file
		//if(!strcmp((char *)FileName, file.f.name))
		if(!stricmp((char *)FileName, file.f.name)) // stricmp(): Compare String without Case Sensitivity
		{
			*Size = file.f.len;
			*Address = file.f.start + header_len;

			return fileidx;
		}
		addr += FILE_HEAD_SIZE;
		fileidx++;
	}

	fileidx = 0;

	return fileidx;
}
/*
uint8_t search_file_rom(uint8_t * FileName, uint32_t * Address, uint32_t * Size)
{
	uint8_t fileidx = 1;

	ROM_DIR file;
	uint32_t addr = 0;

	while(1)
	{
		read_from_flashbuf((unsigned long)addr, (unsigned char*)&file.b, sizeof(file));

        if(file.b[0]==0xff && file.b[1]==0xff)
		{
			break;
		}

		//search file
		if(!strcmp((char *)FileName, file.f.name))
		{
			*Size = file.f.len;
			*Address = file.f.start;

			return fileidx;
		}
		addr += FILE_HEAD_SIZE;
		fileidx++;
	}

	fileidx = 0;

	return fileidx;
}
*/
