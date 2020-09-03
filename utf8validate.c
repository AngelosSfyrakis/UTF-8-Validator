/* The computation of the codepoint is very complex if you do not know how the encoding works, keep reading for a better understanding. */

/* About Codepoints 

Characters with code within the range [U+0000,U+007F] look something like this: 0xxxxxxx

Characters with code within the range [U+0080,U+07FF] look something like this: 110xxxxx 10xxxxxx

Characters with code within the range [U+0800,U+FFFF] look something like this: 1110xxxx 10xxxxxx 10xxxxxx

Characters with code within the range [U+010000,U+10FFFF] look something like this: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

What cd function basically does is: keeping the X's and removing anything else.

*/


#include <stdio.h>


int limit_check(int cdpoint, int limit ){   /* returns -1 if the code-point is over-sized  by checking if the code-point belongs in a smaller range of numbers */
    if (cdpoint < limit){
            printf("Oversized UTF-8 code point: U+%04X\n", cdpoint);
            return -1; }
    return 0;
}

/* cdpoint refers to the first byte that later becomes the code-point and nbytes refers to the number of bytes */
int cd(int cdpoint, int nbytes){   /* compute the code-point, returns -1 if a tail byte is wrong or the next character is EOF, else returns the code-point  */

    int i, next;  // next refers to the next byte

    if(nbytes==2){
        cdpoint +=16;
    }  
    cdpoint = (cdpoint-(192+((nbytes-1)*16)))<<6 ;

    for(i=1; i<=(nbytes-1); i++){   
            if((next=getchar())>>6 == 2){
                cdpoint = (cdpoint | (next-128))<<6;
            }else{
                if(next == -1){
                    printf("Unexpected EOF\n");
                }else{
                    printf("Invalid UTF-8 tail byte: 0x%04X\n", next);
                }
                return -1;
            }
    }
    cdpoint = cdpoint>>6;
    return cdpoint;
}


int main(void){

        int asc = 0;   /* ASCII characters */
        int multibyte = 0;  /* Multi-byte characters */
        int byte, codepoint;  /* Here byte refers to the first byte that is returned by the getchar function. */

        while((byte = getchar())!=EOF){

                if ((byte>>7) == 0){   /* 1 byte required for the  encoding*/
                    asc++;
					codepoint = byte;

                }else if ((byte>>5) == 6){   /* 2 bytes required for the  encoding*/

                    if ((codepoint = cd(byte, 2)) == -1){
                        return -1;  /* Tail byte is wrong or the next character is EOF */
                    }else{
                        multibyte++;
                        if (limit_check(codepoint, 0x0080) == -1) return -1;  /* Check for Over-sized code-points*/
                    }

                }else if((byte>>4) == 14) {    /* 3 bytes required for the  encoding*/

                    if ((codepoint = cd(byte, 3)) == -1){
                        return -1;   /* Tail byte is wrong or the next character is EOF */
                    }else{
                        multibyte++;
                        if (limit_check(codepoint, 0x0800) == -1) return -1;   /* Check for Over-sized code-points*/
                    }

                }else if ((byte>>3) == 30){    /* 4 bytes required for the  encoding*/

                     if ((codepoint = cd(byte, 4)) == -1){
                        return -1;   /* Tail byte is wrong or the next character is EOF */
                     }else{
                        multibyte++;
                        if (limit_check(codepoint, 0x010000) == -1) return -1;   /* Check for Over-sized code-points*/
                    }

				}else{

					printf("Invalid UTF-8 header byte: %04X\n", byte);   /* Check if the header byte of the "new character" does not belong in any of the 4 encodings */
					return -1;
				}

				if (codepoint>0x10FFFF || ((codepoint>=0xD800) && (codepoint<=0xDFFF))){   /* Check for invalid code-points */
					printf("Invalid UTF-8 code point: U+%04X\n", codepoint);
               		return -1;
               	}
        }
        printf("ASCII characters: %d\nMultibye characters: %d\n",asc , multibyte);   /* If the encoding is right it prints the  total of ASCII characters and the total of Multi-byte characters */

        return 0;
}