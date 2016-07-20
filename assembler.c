// to compile, gcc assembler.c -o assembler
// No error check is provided.
// Variable names cannot start with 0-9.
// hexadecimals are twos complement.
// first address of the code section is zero, data section follows the code section.
// fout tables are formed: jump table, ldi table, label table and variable table.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


char *strupr(char *str)
{
  unsigned char *p = (unsigned char *)str;

  while (*p) {
     *p = toupper(*p);
      p++;
  }

  return str;
}


main(int argc, char *argv[])
{

	if( argc != 2 ) {
		printf("Give only the assembly program's path as the ony argument. Instead of that %d argument(s) supplied.\n\tExample: dasm examples/counter.asm\n\tNow please try again.\n", argc);
		exit(0);
	}

	FILE *fp;
    char line[100];
    char *token = NULL;
	char *op1, *op2, *op3, *label;
	char ch;
	int  chch;
    char sp = '9';
    int jumpdistance;

	int program[1000];
	int counter=0;  //holds the address of the machine code instruction




// A label is a symbol which mark a location in a program. In the example
// program above, the string "lpp", "loop" and "lp1" are labels.
	struct label
	{
		int location;
		char *label;
	};
	struct label labeltable[50]; //there can be 50 labels at most in our programs
	int nooflabels = 0; //number of labels encountered during assembly.




// Jump instructions cannot be assembled readily because we may not know the value of
// the label when we encountered a jump instruction. This happens if the label used by
// that jump instruction appear below that jump instruction. This is the situation
// with the label "loop" in the example program above. Hence, the location of jump
// instructions must be stored.
	struct jumpinstruction
	{
		int location;
		char *label;
	};
	struct jumpinstruction jumptable[100]; //There can be at most 100 jumps
	int noofjumps=0;  //number of jumps encountered during assembly.




// The list of variables in .data section and their locations.
	struct variable
	{
		int location;
		char *name;
	};
	struct variable variabletable[50]; //There can be 50 varables at most.
	int noofvariables = 0;




//Variables and labels are used by ldi instructions.
//The memory for the variables are traditionally allocated at the end of the code section.
//Hence their addresses are not known when we assemble a ldi instruction. Also, the value of
//a label may not be known when we encounter a ldi instruction which uses that label.
//Hence, the location of the ldi instructions must be kept, and these instructions must be
//modified when we discover the address of the label or variable that it uses.
	struct ldiinstruction
	{
		int location;
		char *name;
	};
	struct ldiinstruction lditable[100];
	int noofldis=0;




	fp = fopen(argv[1],"r");

	if (fp != NULL)
	{
        counter = 0;
        while(fgets(line,sizeof line,fp)!= NULL)  //skip till .code section
		{
			token=strtok(line,"\n\t\r ");
			if (strcmp(token,".code")==0)
			{
				printf("\nsegment .code\n");
				break;
			}
		}
		while(fgets(line,sizeof line,fp)!= NULL)  //skip till .code section
		{
			token=strtok(line,"\n\t\r ");
			if (token[strlen(token) - 1] == ':')
			{
				char *p = token;
                p[strlen(token)-1] = 0;
                printf("label\t%s\t%d\n", token, counter);

                labeltable[nooflabels].location = counter;  //buraya bir counter koy. error check
                op1=(char*)malloc(sizeof(token));
                strcpy(op1,token);
                labeltable[nooflabels].label=op1;
                nooflabels++;

                counter--;
			}

            if (strcmp(token,"ldi")==0)
            {
                counter++;
                counter++;
            }
            else
            {
                counter++;
            }
		}

    }

    fp = fopen(argv[1],"r");

    if (fp != NULL)
    {
        counter = 0;
        while(fgets(line,sizeof line,fp)!= NULL)  //skip till .code section
		{
			token=strtok(line,"\n\t\r ");
			if (strcmp(token,".code")==0)
			{
				printf("\nsegment .code\n");
				break;
			}
		}
		while(fgets(line,sizeof line,fp)!= NULL)
		{
			token=strtok(line,"\n\t\r ");  //get the instruction mnemonic or label

//========================================   FIRST PASS  ======================================================
			while (token)
			{
				if (strcmp(token,"ldi")==0)        //---------------LDI INSTRUCTION--------------------
				{
					op1 = strtok(NULL,"\n\t\r ");                                //get the 1st operand of ldi, which is the register that ldi loads
					op2 = strtok(NULL,"\n\t\r ");                                //get the 2nd operand of ldi, which is the data that is to be loaded
					printf("\n\t%s\t%s   %s\n",strupr(token),op1,op2);			// print instruction
					program[counter]=0x1000+(int)strtol(op1, NULL, 0);                        //generate the first 16-bit of the ldi instruction
					printf("> %d\t%04x\n",counter,program[counter]);
					counter++;                                                   //move to the second 16-bit of the ldi instruction
					if ((op2[0]=='0')&&(op2[1]=='x'))                            //if the 2nd operand is twos complement hexadecimal
						program[counter]=(int)strtol(op2+2, NULL, 0)&0xffff;              //convert it to integer and form the second 16-bit
					else if ((  (op2[0])=='-') || ((op2[0]>='0')&&(op2[0]<='9')))       //if the 2nd operand is decimal
						program[counter]=atoi(op2)&0xffff;                         //convert it to integer and form the second 16-bit
					else                                                           //if the second operand is not decimal or hexadecimal, it is a laber or a variable.
					{                                                               //in this case, the 2nd 16-bits of the ldi instruction cannot be generated.
						lditable[noofldis].location = counter;                 //record the location of this 2nd 16-bit
						op1=(char*)malloc(sizeof(op2));                         //and the name of the label/variable that it must contain
						strcpy(op1,op2);                                        //in the lditable array.
						lditable[noofldis].name = op1;
						noofldis++;
					}
					printf("> %d\t%04x\n",counter,program[counter]);
					counter++;                                                     //skip to the next memory location
				}

				else if (strcmp(token,"ld")==0)      //------------LD INSTRUCTION---------------------
				{
					op1 = strtok(NULL,"\n\t\r ");                //get the 1st operand of ld, which is the destination register
					op2 = strtok(NULL,"\n\t\r ");                //get the 2nd operand of ld, which is the source register
					printf("\n\t%s\t%s   %s\n",strupr(token),op1,op2);
					ch = (op1[0]-48) | ((op2[0]-48) << 3);        //form bits 11-0 of machine code. 48 is ASCII value of '0'
					program[counter]=0x2000+((ch)&0x00ff);       //form the instruction and write it to memory
					printf("> %d\t%04x\n",counter,program[counter]);
					counter++;                                   //skip to the next empty location in memory
				}
				else if (strcmp(token,"st")==0) //-------------ST INSTRUCTION--------------------
				{
                    op1 = strtok(NULL,"\n\t\r ");                //get the 1st operand of ld, which is the destination register
                    op2 = strtok(NULL,"\n\t\r ");                //get the 2nd operand of ld, which is the source register
                    printf("\n\t%s\t%s   %s\n",strupr(token),op1,op2);
                    ch = ((op1[0]-48) << 2) | ((op2[0]-48) << 6);        //form bits 11-0 of machine code. 48 is ASCII value of '0'
                    program[counter]=0x3000+((ch)&0x00ff);       //form the instruction and write it to memory
                    printf("> %d\t%04x\n",counter,program[counter]);
                    counter++;                                   //skip to the next empty location in memory
				}
				else if (strcmp(token,"jz")==0) //------------- CONDITIONAL JUMP ------------------
				{
                    op1 = strtok(NULL,"\n\t\r ");			//read the label
                    printf("\n\t%s\t%s\n",strupr(token),op1);
                    jumptable[noofjumps].location = counter;	//write the jz instruction's location into the jumptable
                    op2=(char*)malloc(sizeof(op1)); 		//allocate space for the label
                    strcpy(op2,op1);				//copy the label into the allocated space
                    jumptable[noofjumps].label=op2;			//point to the label from the jumptable
                    noofjumps++;					//skip to the next empty location in jumptable

                    program[counter]=0x4000;
                    int n;
                    for(n = 0; n < nooflabels; n++)
                    {
                        if (strcmp(labeltable[n].label,op1)==0)
                        {
                            jumpdistance = counter - labeltable[n].location;
                            //printf("%d\t%d\t%d\n", counter,labeltable[n].location,jumpdistance);
                            program[counter]=0x4000+0x0fff-jumpdistance;

                        }
                    }

                    printf("> %d\t%04x\n",counter,program[counter]);
                    counter++;					//skip to the next empty location in memory.
				}
				else if (strcmp(token,"jmp")==0)  //-------------- JUMP -----------------------------
				{
					op1 = strtok(NULL,"\n\t\r ");			//read the label
					printf("\n\t%s\t%s\n",strupr(token),op1);
					jumptable[noofjumps].location = counter;	//write the jmp instruction's location into the jumptable
					op2=(char*)malloc(sizeof(op1)); 		//allocate space for the label
					strcpy(op2,op1);				//copy the label into the allocated space
					jumptable[noofjumps].label=op2;			//point to the label from the jumptable
					noofjumps++;					//skip to the next empty location in jumptable

					program[counter]=0x5000;			//write the incomplete instruction (just opcode) to memory
                    int n;
                    for(n = 0; n < nooflabels; n++)
                    {
                        if (strcmp(labeltable[n].label,op1)==0)
                        {
                            jumpdistance = counter - labeltable[n].location;
                            //printf("%d\t%d\t%d\n", counter,labeltable[n].location,jumpdistance);
                            program[counter]=0x5000+0x0fff-jumpdistance;

                        }
                    }

					printf("> %d\t%04x\n",counter,program[counter]);
					counter++;					//skip to the next empty location in memory.
				}
				else if (strcmp(token,"add")==0) //----------------- ADD -------------------------------
				{
					op1 = strtok(NULL,"\n\t\r ");
					op2 = strtok(NULL,"\n\t\r ");
					op3 = strtok(NULL,"\n\t\r ");
					printf("\n\t%s\t%s   %s   %s\n",strupr(token),op1,op2,op3);
					chch = (op1[0]-48) | ((op2[0]-48)<<3) | ((op3[0]-48)<<6);
					program[counter]=0x7000+((chch)&0x00ff); // 7 opcode of ALU operations, 0 ALU code of ADD operation
					printf("> %d\t%04x\n",counter,program[counter]);
					counter++;
				}
				else if (strcmp(token,"sub")==0)
				{
                    op1 = strtok(NULL,"\n\t\r ");
                    op2 = strtok(NULL,"\n\t\r ");
                    op3 = strtok(NULL,"\n\t\r ");
                    printf("\n\t%s\t%s   %s   %s\n",strupr(token),op1,op2,op3);
                    chch = (op1[0]-48) | ((op2[0]-48)<<3) | ((op3[0]-48)<<6);
                    program[counter]=0x7100+((chch)&0x00ff); // 7 opcode of ALU operations, 1 ALU code of SUB operation
                    printf("> %d\t%04x\n",counter,program[counter]);
                    counter++;
				}
				else if (strcmp(token,"and")==0)
				{
                    op1 = strtok(NULL,"\n\t\r ");
                    op2 = strtok(NULL,"\n\t\r ");
                    op3 = strtok(NULL,"\n\t\r ");
                    printf("\n\t%s\t%s   %s   %s\n",strupr(token),op1,op2,op3);
                    chch = (op1[0]-48) | ((op2[0]-48)<<3) | ((op3[0]-48)<<6);
                    program[counter]=0x7200+((chch)&0x00ff); // 7 opcode of ALU operations, 2 ALU code of AND operation
                    printf("> %d\t%04x\n",counter,program[counter]);
                    counter++;
				}
				else if (strcmp(token,"or")==0)
				{
                    op1 = strtok(NULL,"\n\t\r ");
                    op2 = strtok(NULL,"\n\t\r ");
                    op3 = strtok(NULL,"\n\t\r ");
                    printf("\n\t%s\t%s   %s   %s\n",strupr(token),op1,op2,op3);
                    chch = (op1[0]-48) | ((op2[0]-48)<<3) | ((op3[0]-48)<<6);
                    program[counter]=0x7300+((chch)&0x00ff); // 7 opcode of ALU operations, 3 ALU code of OR operation
                    printf("> %d\t%04x\n",counter,program[counter]);
                    counter++;
				}
				else if (strcmp(token,"xor")==0)
				{
                    op1 = strtok(NULL,"\n\t\r ");
                    op2 = strtok(NULL,"\n\t\r ");
                    op3 = strtok(NULL,"\n\t\r ");
                    printf("\n\t%s\t%s   %s   %s\n",strupr(token),op1,op2,op3);
                    chch = (op1[0]-48) | ((op2[0]-48)<<3) | ((op3[0]-48)<<6);
                    program[counter]=0x7400+((chch)&0x00ff); // 7 opcode of ALU operations, 4 ALU code of OR operation
                    printf("> %d\t%04x\n",counter,program[counter]);
                    counter++;
				}
				else if (strcmp(token,"not")==0)
				{
					op1 = strtok(NULL,"\n\t\r ");
					op2 = strtok(NULL,"\n\t\r ");
					printf("\n\t%s\t%s   %s\n",strupr(token),op1,op2);
					ch = (op1[0]-48) | ((op2[0]-48)<<3);
					program[counter]=0x7500+((ch)&0x00ff); // 7 opcode of ALU operations, 5 ALU code of not operation
					printf("> %d\t%04x\n",counter,program[counter]);
					counter++;
				}
				else if (strcmp(token,"mov")==0)
				{
                    op1 = strtok(NULL,"\n\t\r ");
					op2 = strtok(NULL,"\n\t\r ");
					printf("\n\t%s\t%s   %s\n",strupr(token),op1,op2);
					ch = (op1[0]-48) | ((op2[0]-48)<<3);
					program[counter]=0x7600+((ch)&0x00ff); // 7 opcode of ALU operations, 6 ALU code of MOV operation
					printf("> %d\t%04x\n",counter,program[counter]);
					counter++;
				}
				else if (strcmp(token,"inc")==0)
				{
					op1 = strtok(NULL,"\n\t\r ");
					printf("\n\t%s\t%s\n",strupr(token),op1);
					ch = (op1[0]-48) | ((op1[0]-48)<<3);
					program[counter]=0x7700+((ch)&0x00ff); // 7 opcode of ALU operations, 7 ALU code of INC operation
					printf("> %d\t%04x\n",counter,program[counter]);
					counter++;
				}
				else if (strcmp(token,"dec")==0)
				{
                    op1 = strtok(NULL,"\n\t\r ");
					printf("\n\t%s\t%s\n",strupr(token),op1);
					ch = (op1[0]-48) | ((op1[0]-48)<<3);
					program[counter]=0x7800+((ch)&0x00ff); // 7 opcode of ALU operations, 8 ALU code of DEC operation
					printf("> %d\t%04x\n",counter,program[counter]);
					counter++;
				}
                else if (strcmp(token,"push")==0) // PUSH instruction: combination of 4 DEC and 1 ST instruction on Stack Pointer (SP)
                {
                    op1 = strtok(NULL,"\n\t\r ");
                    op2[0] = sp; // Let's say address of SP is 9
                    printf("\n\t%s\t%s\n",strupr(token),op1);
                    ch = (op2[0]-48) | ((op2[0]-48)<<3); // Prepare bitwise instruction format for DEC instructions
                    program[counter]=0x7800+((ch)&0x00ff); // Decrease Stack Pointer 4 times
                    printf("> %d\t%04x\n",counter,program[counter]);
                    counter++;
                    program[counter]=0x7800+((ch)&0x00ff); // Decrease Stack Pointer 4 times
                    printf("> %d\t%04x\n",counter,program[counter]);
                    counter++;
                    program[counter]=0x7800+((ch)&0x00ff); // Decrease Stack Pointer 4 times
                    printf("> %d\t%04x\n",counter,program[counter]);
                    counter++;
                    program[counter]=0x7800+((ch)&0x00ff); // Decrease Stack Pointer 4 times
                    printf("> %d\t%04x\n",counter,program[counter]);
                    counter++;

                    ch = ((op1[0]-48) << 2) | ((op2[0]-48) << 6); // Prepare bitwise instruction format for ST instruction
                    program[counter]=0x3000+((ch)&0x00ff); // Store the value in Stack
                    printf("> %d\t%04x\n",counter,program[counter]);
                    counter++;
                }
                else if (strcmp(token,"pop")==0) // POP instruction: combination of 1 LD and 4 INC instructions on Stack Pointer (SP)
                {
                    op1 = strtok(NULL,"\n\t\r ");
                    op2[0] = sp; // Let's say address of SP is 9
                    printf("\n\t%s\t%s\n",strupr(token),op1);

                    ch = (op1[0]-48) | ((op2[0]-48) << 3); // Prepare bitwise instruction format for LD instruction
                    program[counter]=0x2000+((ch)&0x00ff); // Store the value in Stack
                    printf("> %d\t%04x\n",counter,program[counter]);
                    counter++;

                    ch = (op2[0]-48) | ((op2[0]-48)<<3); // Prepare bitwise instruction format for INC instructions
                    program[counter]=0x7700+((ch)&0x00ff); // Decrease Stack Pointer 4 times
                    printf("> %d\t%04x\n",counter,program[counter]);
                    counter++;
                    program[counter]=0x7700+((ch)&0x00ff); // Decrease Stack Pointer 4 times
                    printf("> %d\t%04x\n",counter,program[counter]);
                    counter++;
                    program[counter]=0x7700+((ch)&0x00ff); // Decrease Stack Pointer 4 times
                    printf("> %d\t%04x\n",counter,program[counter]);
                    counter++;
                    program[counter]=0x7700+((ch)&0x00ff); // Decrease Stack Pointer 4 times
                    printf("> %d\t%04x\n",counter,program[counter]);
                    counter++;
                }

				token = strtok(NULL,",\n\t\r ");
			}
		}


//================================= SECOND PASS ==============================

                //supply the address fields of the jump and jz instructions from the
		int i,j;
		for (i=0; i<noofjumps;i++)                                                                   //for all jump/jz instructions
		{
			j=0;
			while ( strcmp(jumptable[i].label , labeltable[j].label) != 0 )             //if the label for this jump/jz does not match with the
				j++;                                                                // jth label in the labeltable, check the next label..
			program[jumptable[i].location] +=(labeltable[j].location-jumptable[i].location-1)&0x0fff;       //copy the jump address into memory.
		}




                // search for the start of the .data segment
		rewind(fp);
		while(fgets(line,sizeof line,fp)!= NULL)  //skip till .data, if no .data, also ok.
		{
			token=strtok(line,"\n\t\r ");
			if (strcmp(token,".data")==0)
			{
				printf("\nsegment .data\n");
				break;
			}

		}


                // process the .data segment and generate the variabletable[] array.
		int dataarea=0;
 		while(fgets(line,sizeof line,fp)!= NULL)
		{
			token=strtok(line,"\n\t\r ");
			if (strcmp(token,".code")==0)  //go till the .code segment
			{
				printf("\nsegment .code\n");
				break;
			}
			else if (token[strlen(token)-1]==':')
			{
				token[strlen(token)-1]='\0';  //will not cause memory leak, as we do not do malloc
				variabletable[noofvariables].location=counter+dataarea;
				op1=(char*)malloc(sizeof(token));
				strcpy(op1,token);
				variabletable[noofvariables].name=op1;
				token = strtok(NULL,",\n\t\r ");
				if (token==NULL)
					program[counter+dataarea]=0;
				else if (strcmp(token, ".space")==0)
				{
					token=strtok(NULL,"\n\t\r ");
					dataarea+=atoi(token);
				}
				else if((token[0]=='0')&&(token[1]=='x'))
					program[counter+dataarea]=(int)strtol(token, NULL, 0)&0xffff;
				else if ((  (token[0])=='-') || ('0'<=(token[0])&&(token[0]<='9'))  )
					program[counter+dataarea]=atoi(token)&0xffff;
				noofvariables++;
				dataarea++;
			}
		}






// supply the address fields for the ldi instructions from the variable table
		for( i=0; i<noofldis;i++)
		{
			j=0;
			while ((j<noofvariables)&&( strcmp( lditable[i].name , variabletable[j].name)!=0 ))
				j++;
			if (j<noofvariables)
				program[lditable[i].location] = variabletable[j].location;
		}

// supply the address fields for the ldi instructions from the label table
		for( i=0; i<noofldis;i++)
		{
			j=0;
			while ((j<nooflabels)&&( strcmp( lditable[i].name , labeltable[j].label)!=0 ))
				j++;
			if (j<nooflabels){
				program[lditable[i].location] = (labeltable[j].location)&0x0fff;
				printf("%d %d %d\n", i, j, (labeltable[j].location));
			}
		}

//display the resulting tables
		printf("LABEL TABLE\n");
		for (i=0;i<nooflabels;i++)
			printf("%d %s\n", labeltable[i].location, labeltable[i].label);
		printf("\n");
		printf("JUMP TABLE\n");
		for (i=0;i<noofjumps;i++)
			printf("%d %s\n", jumptable[i].location, jumptable[i].label);
		printf("\n");
		printf("VARIABLE TABLE\n");
		for (i=0;i<noofvariables;i++)
			printf("%d %s\n", variabletable[i].location, variabletable[i].name);
		printf("\n");
		printf("LDI INSTRUCTIONS\n");
		for (i=0;i<noofldis;i++)
			printf("%d %s\n", lditable[i].location, lditable[i].name);
		printf("\n");
		fclose(fp);
		fp = fopen("RAM","w");
		fprintf(fp,"v2.0 raw\n");
		for (i=0;i<counter+dataarea;i++)
			fprintf(fp,"%04x\n",program[i]);
	}
}
