#include <p24FJ128GB206.h>

#include "common.h"
#include "lcd.h"
#include "strm.h"


char numbers_word[11][7]={"Zero","One","Two","Three","Four","Five","Six","Seven","Eight","Nine","Ten"};


void strm_CamelCase(char* str, uint8_t a, uint8_t b){
    while(*str){
        if(*str >= 'a' && *str <= 'z'){
        *str = ('A' + *str - 'a');
        str++;
        }
        str++;
    }
}

void strm_Caesar(char* str, uint8_t freq, uint8_t shift){
    while(*str){
        str++;
        if (*(str-1)!=' '){
            if(*str >= 'A' && *str <= 'z'){
            *str = *str+shift;
            str=str+freq; // Only shift every freq letters
            }
        }
    }
}

void strm_Brnrd(char* str, uint8_t a, uint8_t b){
    char newstr[17] = "                ";
    char* newstrptr= newstr;
    char* temp1 = newstrptr;
    char* temp2 = str;
    while(*str){
        if(*str == 'a' || *str == 'e' || *str == 'i' || *str == 'o' || *str == 'u' || *str == 'A' || *str == 'E' || *str == 'I' || *str == 'O' || *str == 'U'){ 

            str++;
        }
        else{
            *newstrptr=*str;
            str++;
            newstrptr++;
        }
    }
    strcpy(temp2,temp1);    
}
void strm_Leet(char* str,uint8_t a, uint8_t b){
    while(*str){
        if(*str == 'A' || *str == 'a'){
        *str = '4';
        }
        else if (*str == 'E' || *str == 'e'){
        *str = '3';
        }
        else if (*str == 'G' || *str == 'g'){
        *str = '6';
        }
        else if (*str == 'L' || *str == 'l'){
        *str = '1';
        }
        else if (*str == 'O' || *str == 'o'){
        *str = '0';
        }
        else if (*str == 'S' || *str == 's'){
        *str = '5';
        }
        else if (*str == 'T' || *str == 't'){
        *str = '7';
        }
        str++;
    }
}

void strm_Pig(char* str,uint8_t a, uint8_t b){
    uint8_t temp[20];
    uint8_t length, spaces;
    length=strm_Length(str);
    spaces=strm_Spaces(str,temp); 
    if ((length+((spaces+1)*2))<17){ // will it still fit?
        char newstr[17] ="                ";
        char* newstrptr=newstr;
        char* newstrbgn=newstr;
        uint8_t word =1;
        char first;
        char* wordbgn=str;
        char* strbgn=str;
        uint8_t len=0;
        uint8_t count=0;
        while(*str){
            
            if (word==1){
                first=*wordbgn;
                word=0;
                len=0;
            }
            if (*str==' '){
                uint8_t i=0;
                while (i<len-1){
                    wordbgn++;
                    *newstrptr=*wordbgn;
                    if (count<17){newstrptr++;count++;} 
                    i++;
                }
                *newstrptr=first;
                if (count<17){newstrptr++;count++;}
                *newstrptr='a';
                if (count<17){newstrptr++;count++;}
                *newstrptr='y';
                if (count<17){newstrptr++;count++;}
                *newstrptr=' ';
                if (count<17){newstrptr++;count++;}
                word=1;
                wordbgn+=2;
            }
            str++;
            len++;

        }
        int i=0;
        while (i<len-1){
            wordbgn++;
            *newstrptr=*wordbgn;
            if (count<17){newstrptr++;count++;}
            i++;
            }
        *newstrptr=first;
        if (count<17){newstrptr++;count++;}
        *newstrptr='a';
        if (count<17){newstrptr++;count++;}
        *newstrptr='y';
        if (count<17){newstrptr++;count++;}
        *newstrptr=' ';
        if (count<17){newstrptr++;count++;}

        strcpy(strbgn,newstrbgn);
    }
}

void strm_Missing(char* str, uint8_t freq, uint8_t a){
    char newstr[17] = "                ";
    char* newstrptr= newstr;
    char* temp1 = newstrptr;
    char* temp2 = str;
    uint8_t count=1;
    while(*str){
        if(count%freq==0){ 
            if (*str!=' ' && *(str-1)!=' '){
            str++;}
        }
        else{
            *newstrptr=*str;
            str++;
            newstrptr++;
        }
    count++;
    }
    strcpy(temp2,temp1);    
}

void strm_Reverse(char* str, uint8_t a, uint8_t b){
    uint8_t len=0;
    char newstr[17] = "                ";
    char* newstrptr= newstr;
    char* temp1 = newstrptr;
    char* temp2 = str;
    // char *end = string + strlen(string)-1;
    while(*str){
        str++;
        len++;
    }
    str=temp2;
    newstrptr+=len-1;
    while(*str){
        *newstrptr=*str;
        newstrptr--;
        str++;
    }
    strcpy(temp2,temp1);  
}


void strm_Scramble(char* str,uint8_t a, uint8_t b){
    //to do, 
}

void strm_Nada(char* str,uint8_t a, uint8_t b){
    //nothing
}

void strm_genPush(char* command, char* name, uint8_t number){
    char* numb;
    if (number > 0){
        numb=numbers_word[number];
    }
    else{
        numb="";
    }
    char newstr[33] = "                                ";
    char push[6]="Push ";
    char* pushptr =push;
    char* newstrptr= newstr;
    char* temp1 = newstrptr;
    char* temp2 = command;
    while(*pushptr){
        *newstrptr=*pushptr;
        newstrptr++;
        pushptr++;
    }
    while(*name){
        *newstrptr=*name;
        newstrptr++;
        name++;
    }
    if (number >0){
        *newstrptr=' ';
        newstrptr++;   
    }

    while(*numb){
        *newstrptr=*numb;
        newstrptr++;
        numb++;
    }
    *newstrptr='!';
    newstrptr++;
    strcpy(temp2,temp1);
}

void strm_genSet(char* command, char* name, uint8_t value){
    char newstr[33] = "                                ";
    char set[5]="Set ";
    char to[5]=" to ";
    char* val=numbers_word[value];
    char* setptr =set;
    char* toptr =to;
    char* newstrptr= newstr;
    char* temp1 = newstrptr;
    char* temp2 = command;
    while(*setptr){
        *newstrptr=*setptr;
        newstrptr++;
        setptr++;
    }
    while(*name){
        *newstrptr=*name;
        newstrptr++;
        name++;
    }
    while(*toptr){
        *newstrptr=*toptr;
        newstrptr++;
        toptr++;
    }
    while(*val){
        *newstrptr=*val;
        newstrptr++;
        val++;
    }
    *newstrptr='!';
    newstrptr++;
    strcpy(temp2,temp1);
}

void strm_genAct(char* command, char* name, uint8_t number, uint8_t action){
    char* numb;
    if (number > 0){
        numb=numbers_word[number];
    }
    else{
        numb="";
    }
    char newstr[33] = "                                ";
    char act[10]="Activate ";
    char deact[12]="Deactivate ";
    char* actptr =act;
    char* deactptr=deact;
    char* newstrptr= newstr;
    char* temp1 = newstrptr;
    char* temp2 = command;

    if (action==1){
    while(*actptr){
        *newstrptr=*actptr;
        newstrptr++;
        actptr++;
    }
    }
    if (action==0){
    while(*deactptr){
        *newstrptr=*deactptr;
        newstrptr++;
        deactptr++;
    }
    }
    while(*name){
        *newstrptr=*name;
        newstrptr++;
        name++;
    }
    if (numb > 0) {
        *newstrptr=' ';
        newstrptr++;
    }
    while(*numb){
        *newstrptr=*numb;
        newstrptr++;
        numb++;
    }
    *newstrptr='!';
    newstrptr++;
    strcpy(temp2,temp1);
}
 
uint8_t strm_Spaces(char* string, uint8_t* space_loc){
    uint8_t space_num=0;
    uint8_t i;
    for (i = 0; i < 32; i++){
        if (*string){
            if (*string==' '){
                string++;
                if(*string != ' '){
                    if (*string){                 
                         space_loc[space_num]=i;
                         space_num++;
                         string--;
                    }
                }
            }
            string++;
        }
    }
    return space_num;
}  

uint8_t strm_Length(char* string){
    uint8_t len=0;
    uint8_t i;
    for (i = 0; i < 33; i++){
        if (*string){
            if (*string==' '){
                string++;
                if(*string == ' '){
                    return len;
                }
                else if (!*string){                
                    return len;
                }
                string--;
            }
            
            len++;
            string++;
        }
    }
    return len;
}  


