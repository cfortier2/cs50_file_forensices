/****************************************************************************
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image.
 *
 * Chris Fortier
 ***************************************************************************/
 
 
 #include <stdio.h>
 #include <stdlib.h>
 #include <unistd.h>
 #include <string.h>
 #include <stdbool.h>
 
 int main (int argc, char *argv[])
 {
    
    //variables
    long lSize;
 	int files_found = 0;
 	bool verbose = true;
 	    
     //file
    FILE *inptr;
    FILE *outptr = NULL;
    char *outFile = malloc(sizeof(char) * 8);
    
    //check input file
    if ((inptr = fopen("card.raw", "r")) == NULL)
    {
        printf("Could not open card.raw\n.");
        return 1;
    }
    
    //get file size - example from http://cplusplus.com/reference/clibrary/cstdio/fread/
    lSize = 512;
    fseek(inptr, 0, SEEK_END); //move to end of file
    long end = ftell(inptr); //get position of eof 
    rewind(inptr); //move back to 0
    
    	
	//loop through file in chunks of lSize bytes
	for (int i = 0, max = (end/lSize) ; i < max; i++)
	{
			
		//create buffer
		unsigned char buffer[lSize];
		
		//read into buffer
		fread(&buffer, lSize, 1, inptr);
	
		//read first 4 bytes in temp header to compare to known jpeg headers
		unsigned char header[4];
		for (int j = 0; j < 4; j++)
		{
			header[j] = *(buffer + j);
		}
		
		//create known headers
		unsigned char known_a[4] = {0xff, 0xd8, 0xff, 0xe0};
		unsigned char known_b[4] = {0xff, 0xd8, 0xff, 0xe1};
	
		//compare arrays
		if ((memcmp(header, known_a, 4) == 0) || (memcmp(header, known_b, 4) == 0))
		{	
			//there is no file open for writing. Open the next file.
			if (outptr == NULL)
			{
				//create and check output file
				sprintf(outFile, "%.3d.jpg", files_found);
				
				//check file
				if ((outptr = fopen(outFile, "w")) == NULL)
				{
					printf("Could not create %s\n.", argv[3]);
					fclose(inptr);
					return 1;
				}
			}
			//there is currently a file being written, but we have encounterd a new header/file
			else
			{
				//close the current file
				fclose(outptr);
				outptr = NULL;
				
				//create and check new output file
				sprintf(outFile, "%.3d.jpg", files_found);
				
				//check file
				if ((outptr = fopen(outFile, "w")) == NULL)
				{
					printf("Could not create %s\n.", argv[3]);
					fclose(inptr);
					return 1;
				}
			}
			
			//increment counter	
			files_found++;
		}
		
		//if file pointer is not null, write to the file
		if (outptr != NULL)
		{
			//write file
			fwrite(&buffer, lSize, 1, outptr);
		}
		
	}
	    
    //close file
    fclose(inptr);
    free(outFile);	
  	
  	//display output
  	if (verbose)
  	{
  		printf("There were %d files found.\n", files_found);
  	}  
    return 0;
 }
