#ifndef __QLEX_H__
#define __QLEX_H__

#include <qcc.h>

token_t* token_direct_insert(token_t *tp);

void init_lex();

void parse_string(char seperate);
void parse_num(void);
token_t *parse_identifier(void);

void get_token(void);

/* function name: getch
 * description: get a character from standard library 
 */
void getch();

/* function name: lexical_coloring
 * description: do the code syntax shader
 */
void lexical_coloring();


#endif