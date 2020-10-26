#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/*****************************************************************/
#define SIZE_TOKEN 50

char token_buffer[SIZE_TOKEN];
int ligne = 0;
const char keyword_token[][6] = {"begin", "end", "read", "write"};
/* list of token */
typedef enum token
{
    BEGIN,
    END,
    READ,
    WRITE,
    ID,
    INITLITERAL,
    PLUSOP,
    LPARENT,
    RPARENT,
    SEMICOLON,
    COMMA,
    ASSINGOP,
    MINUSOP,
    SCANEOF,
    FLOATLITERAL
} token;

char token_string[][13] =
    {
        "BEGIN",
        "END",
        "READ",
        "WRITE",
        "ID",
        "INITLITERAL",
        "PLUSOP",
        "LPARENT",
        "RPARENT",
        "SEMICOLON",
        "COMMA",
        "ASSINGOP",
        "MINUSOP",
        "SCANEOF",
        "FLOATLITERAL"};

/* 
*functions 
*/
// analyseur lexical
token scanner(FILE *);
// vider le buffer
void clear_buffer(void);
// inserer un caractere dans la buffer
void buffer_char(char);
// connaitre si une chaine est un ID ou keyword_token
token check_reserved(void);
// afficher un message d'erreur : la caractere et la ligne
void lexical_error(char);

/*******************************************************************/

int main(int argc, char const *argv[])
{
    // lire le fichier source
    FILE *srcFile = fopen("code.txt", "r");
    FILE *rsltFile = fopen("tokens.txt", "w");
    token unite_lex;
    if (!srcFile || !rsltFile)
    {
        puts("ouverture échouée ");
        exit(EXIT_FAILURE);
    }
    while ((unite_lex = scanner(srcFile)) != SCANEOF)
    {
        fprintf(rsltFile, "%s   ", token_string[(int)unite_lex]);
    }

    fclose(srcFile);
    fclose(rsltFile);
    return 0;
}

/*
 * name : scanner
 * args : void
 * outpur: token 
 */
token scanner(FILE *srcFile)
{
    int in_char, c;
    clear_buffer();
    if (feof(srcFile))
        return SCANEOF;
    while ((in_char = fgetc(srcFile)) != EOF)
    {
        if (isspace(in_char))
            continue; /* do nothing */
        if (in_char == '\n')
            ligne++;
        else
        {
            if (isalpha(in_char))
            {
                buffer_char(in_char); //stocker
                /* continuer l'exploration de alnum*/
                for (c = fgetc(srcFile); isalnum(c) || c == '_'; c = fgetc(srcFile))
                    buffer_char(c);
                /* La fonction FOR sort Lorsqu'elle lit un non alpha-numérique caractère
                // il faut retourner le curseur en arrière par la fonction ungetc()
                / pour que ce caractère ne soit perdu */
                ungetc(c, srcFile);
                return check_reserved();
            }
            else
            {
                if (isdigit(in_char))
                {
                    /*  INITLITERAL := DIGIT | INITLITERAL DIGIT */
                    buffer_char(in_char);
                    for (c = fgetc(srcFile); isdigit(c); c = fgetc(srcFile))
                        buffer_char(c);
                    // FLOAT
                    if (c == '.')
                    {
                        buffer_char(c);
                        for (c = fgetc(srcFile); isdigit(c); c = fgetc(srcFile))
                            buffer_char(c);
                        if (c == 'e' || c == 'E')
                        {
                            buffer_char(c);
                            c = fgetc(srcFile);
                            if (c == '+' || c == '-')
                            {
                                buffer_char(c);
                                c = fgetc(srcFile);
                                if (isdigit(c))
                                {
                                    buffer_char(c);
                                    for (c = fgetc(srcFile); isdigit(c); c = fgetc(srcFile))
                                        buffer_char(c);
                                }
                                else
                                {
                                    lexical_error(c);
                                }
                            }
                            else
                            {
                                lexical_error(c);
                            }
                        }
                        return FLOATLITERAL;
                    }
                    /* La fonction FOR sort Lorsqu'elle lit un non alpha-numérique caractère
                    // il faut retourner le curseur en arrière par la fonction ungetc()
                    / pour que ce caractère ne soit perdu */
                    ungetc(c, srcFile);
                    return INITLITERAL;
                }
                else
                {
                    if (in_char == '.')
                    {
                        buffer_char(in_char);
                        for (c = fgetc(srcFile); isdigit(c); c = fgetc(srcFile))
                            buffer_char(c);
                        if (c == 'e' || c == 'E')
                        {
                            buffer_char(c);
                            c = fgetc(srcFile);
                            if (c == '+' || c == '-')
                            {
                                buffer_char(c);
                                c = fgetc(srcFile);
                                if (isdigit(c))
                                {
                                    buffer_char(c);
                                    for (c = fgetc(srcFile); isdigit(c); c = fgetc(srcFile))
                                        buffer_char(c);
                                }
                                else
                                {
                                    lexical_error(c);
                                }
                            }
                            else
                            {
                                lexical_error(c);
                            }
                        }
                        ungetc(c, srcFile);
                        return FLOATLITERAL;
                    }
                    else
                    {

                        if (in_char == '(')
                            return LPARENT;
                        else
                        {
                            if (in_char == ')')
                                return RPARENT;
                            else
                            {
                                if (in_char == ';')
                                    return SEMICOLON;
                                else
                                {
                                    if (in_char == ',')
                                        return COMMA;
                                    else
                                    {
                                        if (in_char == '+')
                                            return PLUSOP;
                                        else
                                        {
                                            if (in_char == ':')
                                            {
                                                // look for ':='
                                                if ((c = fgetc(srcFile)) == '=')
                                                    return ASSINGOP;
                                                else
                                                {
                                                    ungetc(c, srcFile);
                                                    lexical_error(in_char);
                                                }
                                            }
                                            else
                                            {
                                                if (in_char == '-')
                                                {
                                                    // is it --, comment star
                                                    if ((c = fgetc(srcFile)) == '-')
                                                    {
                                                        do
                                                        {
                                                            in_char = fgetc(srcFile);
                                                        } while (in_char != '\n');
                                                    }
                                                    else
                                                    {
                                                        ungetc(c, srcFile);
                                                        return MINUSOP;
                                                    }
                                                }
                                                else
                                                {
                                                    lexical_error(in_char);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    } /* end of while */
}
/* end of scanner()*/

void clear_buffer()
{
    for (int i; i < SIZE_TOKEN; token_buffer[i] = '\0', i++)
        ;
}

void buffer_char(char c)
{
    token_buffer[strlen(token_buffer)] = c;
}

token check_reserved()
{
    int i;
    for (i = 0; i < 4; i++)
    {
        if (!strcmp(token_buffer, keyword_token[i]))
            return i;
    }
    return ID;
}

void lexical_error(char carac)
{
    printf("\n une erreur dans la ligne %i 'symbole unconnu %c' !!!!\n", ligne, carac);
}