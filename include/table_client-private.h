// Grupo 21
// Joao Santos 56380
// Rafael Ferreira 57544
// Ricardo Mateus 56366
#ifndef _TABLE_CLIENT_PRIVATE_
#define _TABLE_CLIENT_PRIVATE_

/*
 * Função que imprime mensagens de erro quando os argumentos passados
 * ao programa são os errados.
 */
void initArgsError();

/*
 * Função que imprime o menu com o input de comandos possíveis
 * a serem realizados pelo utilizador.
 */
void showMenu();

/*
 * Função que lê o input do utilizador e remove as new lines (\n) 
 * para o input ser mais tratado mais fácilmente pelo programa.
 */
void readOption(char* input, int size);

/*
 * Função que verifica se a opção passada é do tipo "put".
 * Devolve 0 se a opção for desse tipo, caso contrário retorna um 
 * valor diferente de 0.
 */
int commandIsPut(char* option);

/*
 * Função que verifica se a opção passada é do tipo "get".
 * Devolve 0 se a opção for desse tipo, caso contrário retorna um 
 * valor diferente de 0.
 */
int commandIsGet(char* option);

/*
 * Função que verifica se a opção passada é do tipo "del".
 * Devolve 0 se a opção for desse tipo, caso contrário retorna um 
 * valor diferente de 0.
 */
int commandIsDel(char* option);

/*
 * Função que verifica se a opção passada é do tipo "size".
 * Devolve 0 se a opção for desse tipo, caso contrário retorna um 
 * valor diferente de 0.
 */
int commandIsSize(char* option);

/*
 * Função que verifica se a opção passada é do tipo "stats".
 * Devolve 0 se a opção for desse tipo, caso contrário retorna um 
 * valor diferente de 0.
 */
int commandIsStats(char* option);

/*
 * Função que verifica se a opção passada é do tipo "quit".
 * Devolve 0 se a opção for desse tipo, caso contrário retorna um 
 * valor diferente de 0.
 */
int commandIsQuit(char* option);

/*
 * Função que verifica se a opção passada é do tipo "gettable".
 * Devolve 0 se a opção for desse tipo, caso contrário retorna um 
 * valor diferente de 0.
 */
int commandIsGetTable(char* option);

/*
 * Função que verifica se a opção passada é do tipo "getkeys".
 * Devolve 0 se a opção for desse tipo, caso contrário retorna um 
 * valor diferente de 0.
 */
int commandIsGetKeys(char* option);

/*
 * Função que executa a operação put, com a dada option, através do r_table.
 * Esta função faz verifica se os argumentos da opção estão corretos.
 */
void executePut(struct rtable_t* r_table, char* option);

/*
 * Função que imprime mensagens de erro quando os argumentos passados
 * ao programa são os errados.
 */
void executeGet(struct rtable_t* r_table, char* option);

/*
 * Função que imprime mensagens de erro quando os argumentos passados
 * ao programa são os errados.
 */
void executeDel(struct rtable_t* r_table, char* option);

/*
 * Função que imprime mensagens de erro quando os argumentos passados
 * ao programa são os errados.
 */
void executeSize(struct rtable_t* r_table);

/*
 * Função que imprime mensagens de erro quando os argumentos passados
 * ao programa são os errados.
 */
void executeGetTable(struct rtable_t* r_table);

/*
 * Função que imprime mensagens de erro quando os argumentos passados
 * ao programa são os errados.
 */
void executeGetKeys(struct rtable_t* rtable);

#endif
