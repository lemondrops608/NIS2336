/****************************************************/
/* File: parse.c                                    */
/* The parser implementation for the TINY compiler  */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

static TokenType token; /* holds current token */

/* function prototypes for recursive calls */
static TreeNode * stmt_sequence(void);
static TreeNode * statement(void);
static TreeNode * if_stmt(void);
static TreeNode * repeat_stmt(void);
static TreeNode * assign_stmt(void);
static TreeNode * read_stmt(void);
static TreeNode * write_stmt(void);
static TreeNode * exp(void);
static TreeNode * simple_exp(void);
static TreeNode * term(void);
static TreeNode * factor(void);

static void syntaxError(char * message)
{ fprintf(listing,"\n>>> ");
  fprintf(listing,"Syntax error at line %d: %s",lineno,message);
  Error = TRUE;
}

/* This fuction can be called by the following fuctions. */
static void match(TokenType expected)
{ if (token == expected) token = getToken();
  else {
    syntaxError("unexpected token -> ");
    printToken(token,tokenString);
    fprintf(listing,"      ");
  }
}


/* Each of the following functions represents the process of put one certain non-terminal character on the syntex tree as a node: */
/* You can write other functions referring to the format of the function  stmt_sequence(void). */

TreeNode * stmt_sequence(void)
{ TreeNode * t = statement();
  TreeNode * p = t;
  while ((token!=ENDFILE) && (token!=END) &&
         (token!=ELSE) && (token!=UNTIL))
  { TreeNode * q;
    match(SEMI);
    q = statement();
    if (q!=NULL) {
      if (t==NULL) t = p = q;
      else /* now p cannot be NULL either */
      { p->sibling = q;
        p = q;
      }
    }
  }
  return t;
}

TreeNode * statement(void)
{
  TreeNode * t = NULL;
  switch(token) {
    case IF: t = if_stmt(); break;
    case REPEAT: t = repeat_stmt(); break;
    case ID: t = assign_stmt(); break;
    case READ: t = read_stmt(); break;
    case WRITE: t = write_stmt(); break;
    default:
      syntaxError("unexpected token -> ");
      printToken(token,tokenString);
      token = getToken();
      break;
  }
  return t;
}

TreeNode * repeat_stmt(void)
{
  TreeNode * t = newStmtNode(RepeatK);
  if (t != NULL) {
    match(REPEAT);
    t->child[0] = stmt_sequence();
    match(UNTIL);
    t->child[1] = exp();
  }
  return t;
}

TreeNode * if_stmt(void)
{
  TreeNode * t = newStmtNode(IfK);
  if (t != NULL) {
    match(IF);
    t->child[0] = exp();
    match(THEN);
    t->child[1] = stmt_sequence();
    if (token == ELSE) {
      match(ELSE);
      t->child[2] = stmt_sequence();
    }
    match(END);
  }
  return t;
}


TreeNode * assign_stmt(void)
{
  TreeNode * t = newStmtNode(AssignK);
  if (t != NULL && token == ID) {
    t->attr.name = copyString(tokenString);
    match(ID);
    match(ASSIGN);
    t->child[0] = exp();
  }
  else
    t = NULL;
  return t;
}

TreeNode * read_stmt(void)
{
  TreeNode * t = newStmtNode(ReadK);
  if (t != NULL) {
    match(READ);
    if (token == ID)
      t->attr.name = copyString(tokenString);
    match(ID);
  }
  return t;
}

TreeNode * write_stmt(void)
{
  TreeNode * t = newStmtNode(WriteK);
  if (t != NULL) {
    match(WRITE);
    t->child[0] = exp();
  }
  return t;
}

static int isExpEndToken(TokenType token) {
  return token == RPAREN || token == THEN || token == END || 
         token == UNTIL || token == ELSE || token == ENDFILE ||
         token == SEMI;
}
TreeNode * exp(void)
{
  TreeNode * t = simple_exp();
  if (token == LT || token == EQ) {
    TreeNode * p = newExpNode(OpK);
    if (p != NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
    }
    match(token);
    t->child[1] = simple_exp();
  }
  else if (!isExpEndToken(token))
  {
    syntaxError("unexpected token -> ");
    printToken(token, tokenString);
    match(token);
  }
  return t;
}

TreeNode * simple_exp(void)
{
  TreeNode * t = term();
  while (token == PLUS || token == MINUS) {
    TreeNode * p = newExpNode(OpK);
    if (p != NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      t->child[1] = term();
    }
  }
  return t;
}

TreeNode * term(void)
{
  TreeNode * t = factor();
  while (token == TIMES || token == OVER) {
    TreeNode * p = newExpNode(OpK);
    if (p != NULL) {
      p->child[0] = t;
      p->attr.op = token;
      t = p;
      match(token);
      p->child[1] = factor();
    }
  }
  return t;
}

TreeNode * factor(void)
{
  TreeNode * t = NULL;
  switch(token) {
    case NUM:
      t = newExpNode(ConstK);
      if (t != NULL) {
        t->attr.val = atoi(tokenString);
      }
      match(NUM);
      break;
    case ID:
      t = newExpNode(IdK);
      if (t != NULL) {
        t->attr.name = copyString(tokenString);
      }
      match(ID);
      break;
    case LPAREN:
      match(LPAREN);
      t = exp();
      match(RPAREN);
      break;
    default:
      syntaxError("unexpected token -> ");
      printToken(token,tokenString);
      token = getToken();
      break;
  }
  return t;
}


/****************************************/
/* the primary function of the parser   */
/****************************************/
/* Function parse returns the newly
 * constructed syntax tree
 */
TreeNode * parse(void)
{ TreeNode * t;
  token = getToken(); /*get one token each time and parse it */
  t = stmt_sequence();
  if (token!=ENDFILE)
    syntaxError("Code ends before file\n");
  return t;
}
