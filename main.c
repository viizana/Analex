//  Aurtor desse código horrivel -> Vinícius Zanotti RA 231022719
//  Sistema Operacional -> Fedora Linux 41 (Workstation Edition) x86_64 -- Linux 6.13.10-200.fc41.x86_64
//  Compilador -> gcc (GCC) 14.2.1 20250110 (Red Hat 14.2.1-7)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#define TRUE 1
#define FALSE 0
#define SIZE 16
#define NUMERO_DE_PASCALKWORD 16
#define NUMERO_DE_SIMBOLOS_SIMPLES 12
#define NUMERO_DE_SIMBOLOS_COMPOSTOS 3
#define SUP (1<<30)
//program, label, var,
//procedure, function, begin, end, if, then, else, while,
//do, or, and, div, not
char pascal_keyword[][SIZE] = {"PROGRAM","LABEL","VAR","PROCEDURE","FUNCTION","BEGIN","END","IF","THEN","ELSE",
    "WHILE","DO","OR","AND","DIV","NOT"};
// . ; , ( ) : = < > + - * := (* *)
char pascal_symbol[][3] = {".",";",",","(",")",":","=","<",">","+","-","*",":=","(*","*)"};
int proximo = -1;
size_t linha = 1,count = 0;
FILE *source = NULL,*out = NULL;
char simbolo[SUP];

// Implemantação de estrutura string
typedef struct string{
    char *arr;
    size_t cap,size;
}string;


string* string_alloc(){
    string *n = malloc(sizeof(string));
    n->arr = malloc(sizeof(char));
    n->cap = 1;
    n->size = 0;
    n->arr[0] = '\0';
    return n;
}
void string_push_back(string *s,int ch){
    if(++s->size == s->cap){
        s->cap <<= 1;
        char *aux = realloc(s->arr,s->cap);
        if(!aux){
            fprintf(stderr,"Erro de realocao de memoria\n");
            exit(1);
        }
        s->arr = aux;
    }
    s->arr[s->size-1] = (char)ch;
    s->arr[s->size] = '\0';
}
void string_earase(string *s){
    free(s->arr);
    //s->cap = 1;
    s->size = 0;
}

void PROXIMO(){
    proximo = fgetc(source);
    proximo = toupper(proximo);
    proximo == '\n' ? ++linha : linha;
}

void ERRO(){
    fprintf(out,"linha -> %lu\n",proximo == '\n' ? linha-1:linha);
    exit(1);
}

char* CODIGO(string *lex){
    return lex->arr;
}

int is_pascal_special_symbol(){
    for(int i = 0; i < NUMERO_DE_SIMBOLOS_SIMPLES;++i)
        if(proximo == (int)pascal_symbol[i][0])
            return TRUE;
    return FALSE;
}

int is_pascal_keyword(string *lex){
    for(int i = 0;i < NUMERO_DE_PASCALKWORD;++i)
        if(!strcmp(lex->arr,pascal_keyword[i]))
            return 1;
    return 0;
}

void montar_simbolo(string *lex){
    string_push_back(lex,proximo);
    PROXIMO();
    if(lex->arr[0] == ':' && proximo == '='){
        string_push_back(lex,proximo);
        PROXIMO();
    }
    if(lex->arr[0] == '('&&proximo == '*'){
        string_push_back(lex,proximo);
        PROXIMO();
    }
    if(lex->arr[0] == '*' && proximo == ')'){
        string_push_back(lex,proximo);
        PROXIMO();
    }
}

void montar_palavra(string *lex){
    do{
       string_push_back(lex,proximo);
       PROXIMO();
    }
    while(isalpha(proximo)||isdigit(proximo));
}

void montar_numero(string *lex){
    do{
        string_push_back(lex,proximo);
        PROXIMO();
    }
    while(isdigit(proximo));
}

void write_tokens(){
    fprintf(out,"linha(%lu): %s\n",proximo == '\n'?linha-1:linha,simbolo);
}

void ANALISADOR_LEXICO(){
    string *lex = string_alloc();
    while(isblank(proximo) || proximo =='\n')
        PROXIMO();
    if(is_pascal_special_symbol()){
        montar_simbolo(lex);
        strcpy(simbolo,CODIGO(lex));
    }
    else{
        if(isalpha(proximo)){
            montar_palavra(lex);
            if(is_pascal_keyword(lex)){
                strcpy(simbolo,CODIGO(lex));
            }
            else {
                sprintf(simbolo,"id[%s]",lex->arr);
            }
        }
        else{
            if(isdigit(proximo)){
                montar_numero(lex);
                if(isalpha(proximo)){
                    fprintf(out,"Identificador mal formatado na ");
                    ERRO();
                }
                strcpy(simbolo,lex->arr);
            }
            else 
                if(proximo != EOF) 
                    ERRO(); 
        }
    }
    write_tokens();
    string_earase(lex);
    free(lex);
}

int main(void)
{
    source = fopen("Trab1_Compiladores.txt","r");
    out = fopen("output.txt","w");
    PROXIMO();
    while(proximo != EOF) 
        ANALISADOR_LEXICO();
    fclose(source);
    fclose(out);
    return 0;
}