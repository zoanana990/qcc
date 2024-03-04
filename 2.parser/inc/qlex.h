#ifndef __QLEX_H__
#define __QLEX_H__

#include <qcc.h>

#define CH_EOF          (-1)

token_t *token_direct_insert(token_t *tp);

token_t *token_insert(char *ptr_data);

void init_lex();

token_t *parse_identifier(void);

void get_token(void);
char *get_token_string(int str_num);

/* function name: getch
 * description: get a character from standard library 
 */
void getch();

/* function name: lexical_coloring
 * description: do the code syntax shader
 */
void lexical_coloring();


#endif