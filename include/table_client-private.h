#ifndef _TABLE_CLIENT_PRIVATE_
#define _TABLE_CLIENT_PRIVATE_
void showMenu();

void readOption(char* input, int size);

int commandIsPut(char* option);

int commandIsGet(char* option);

int commandIsDel(char* option);

int commandIsSize(char* option);

int commandIsHeight(char* option);

int commandIsGetTable(char* option);

int commandIsGetKeys(char* option);

void executePut(struct rtable_t* r_table, char* option);

void executeGet(struct rtable_t* r_table, char* option);

void executeDel(struct rtable_t* r_table, char* option);

void executeSize(struct rtable_t* r_table);

void executeGetTable(struct rtable_t* r_table);

void executeGetKeys(struct rtable_t* rtable);

#endif
