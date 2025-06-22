/****************************************************/
/* File: lexer.c                                     */
/* The lexer implementation for the TINY compiler */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/


//尚存问题：whitespace
//变量名就是letter
//tny文件最后没有换行
//只有一个前括号

#include "globals.h"
#include "util.h"
#include "scan.h"

/* states in lexer DFA */
typedef enum
{
  START,
  INASSIGN,
  INCOMMENT,
  INNUM,
  INID,
  DONE
} StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0;      /* current position in LineBuf */
static int bufsize = 0;      /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
int getNextChar(void)
{
  if (!(linepos < bufsize))
  {
    lineno++;
    if (fgets(lineBuf, BUFLEN - 1, source))
    {
      if (EchoSource)
        {
          if(lineBuf[strlen(lineBuf)-1] != '\n')
          fprintf(listing, "%4d: %s\n", lineno, lineBuf);
          else
          fprintf(listing, "%4d: %s", lineno, lineBuf);
        }
      bufsize = strlen(lineBuf);
      linepos = 0;
      return lineBuf[linepos++];
    }
    else
    {
      EOF_flag = TRUE;
      return EOF;
    }
  }
  else
    return lineBuf[linepos++];
}

/* ungetNextChar backtracks one character
   in lineBuf */
static void ungetNextChar(void)
{
  if (!EOF_flag)
    linepos--;
}

/* lookup table of reserved words */
static struct
{
  char *str;
  TokenType tok;
} reservedWords[MAXRESERVED] = {{"if", IF}, {"then", THEN}, {"else", ELSE}, {"end", END}, {"repeat", REPEAT}, {"until", UNTIL}, {"read", READ}, {"write", WRITE}};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenType reservedLookup(char *s)
{
  int i;
  for (i = 0; i < MAXRESERVED; i++)
    if (!strcmp(s, reservedWords[i].str))
      return reservedWords[i].tok;
  return ID;
}

/****************************************/
/* the primary function of the lexer  */
/****************************************/
/* function getToken returns the
 * next token in source file
 */
TokenType getToken(void)
{ /* Initialization: */
  /* index for storing into tokenString */
  int tokenStringIndex = 0;
  /* holds current token to be returned */
  TokenType currentToken;
  /* current state - always begins at START */
  StateType state = START;
  /* flag to indicate save to tokenString */
  int save;

  /* The while loop simulates the process of DFA:
     get one lexical unit each time,
     update the currentToken/state/save above
     according to current state and the received lexical unit.
  */
  while (state != DONE)
  {
    int c;
    if(!EOF_flag)
    c = getNextChar();
    save = TRUE;
    switch (state)
    {
      /* Write the process of other cases referring to case INID. */
    case INID:
      if (!isalpha(c))
      { /* backup in the input */
        if(c==EOF)lineno--;
        ungetNextChar();
        save = FALSE;
        state = DONE;
        currentToken = ID;
      }
      break;
    case INASSIGN:
      if (c == '=')
      {
        state = DONE;
        currentToken = ASSIGN;
      }
      else
      {
        ungetNextChar();
        save = FALSE;
        state = DONE;
        currentToken = ERROR;
      }
      break;
    case INCOMMENT:
      if (c == '}')
      {
        state = START;
        save = FALSE;
      }
      else if (c == EOF)
      {
        state = DONE;
        save = FALSE;
        currentToken = ENDFILE;
      }
      else
      { 
        state = INCOMMENT;
        save  = FALSE;
      }
      break;
    case INNUM:
      if (!isdigit(c))
      {
        ungetNextChar();
        save = FALSE;
        state = DONE;
        currentToken = NUM;
      }
      break;
    case START:
      if (c == '{')
      {
        state = INCOMMENT;
        save = FALSE;
      }
      else if(isspace(c))
      {
        save = FALSE;
      }
      else if (isdigit(c))
      {
        state = INNUM;
        save = TRUE;
      }
      else if (isalpha(c))
      {
        state = INID;
        save = TRUE;
      }
      else if (c == ':')
      {
        state = INASSIGN;
        save = TRUE;
      }
      else if(c == ';')
      {
        state = DONE;
        save = FALSE;
        currentToken = SEMI;
      }
      else if (c == '+')
      {
        state = DONE;
        save = FALSE;
        currentToken = PLUS;
      }
      else if (c == '-')
      {
        state = DONE;
        save = FALSE;
        currentToken = MINUS;
      }
      else if (c == '*')
      {
        state = DONE;
        save = FALSE;
        currentToken = TIMES;
      }
      else if (c == '/')
      {
        state = DONE;
        save = FALSE;
        currentToken = OVER;
      }
      else if (c == '(')
      {
        state = DONE;
        save = FALSE;
        currentToken = LPAREN;
      }
      else if (c == ')')
      {
        state = DONE;
        save = FALSE;
        currentToken = RPAREN;
      }
      else if (c == '<')
      {
        state = DONE;
        save = FALSE;
        currentToken = LT;
      }
      else if (c == '=')
      {
        state = DONE;
        save = FALSE;
        currentToken = EQ;
      }
      else if(c==EOF)
      {
        state = DONE;
        save = FALSE;
        currentToken = ENDFILE;
      }
      else
      {
        state = DONE;
        currentToken = ERROR;
      }
      break; 
    }
    if ((save) && (tokenStringIndex <= MAXTOKENLEN))
      tokenString[tokenStringIndex++] = (char)c;
    if (state == DONE)
    {
      tokenString[tokenStringIndex] = '\0';
      if (currentToken == ID)
        currentToken = reservedLookup(tokenString);
    }
  }
  if (TraceLex)
  {
    fprintf(listing, "\t%d: ", lineno);
    printToken(currentToken, tokenString);
  }
  return currentToken;

} /* end getToken */
