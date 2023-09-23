#include <serialization.h>
#include <stdio.h>
#include <stdlib.h>

/* Serializa todas as chaves presentes no array de strings keys para o
 * buffer keys_buf, que ser� alocado dentro da fun��o. A serializa��o
 * deve ser feita de acordo com o seguinte formato:
 *    | int   | string | string | string |
 *    | nkeys | key1   | key2   | key3   |
 * Retorna o tamanho do buffer alocado ou -1 em caso de erro.
 */
int keyArray_to_buffer(char** keys, char** keys_buf){
    // Verificar se o array de keys é válido. Dúvida: o keys_buf é suposto ser NULL ao início?
    if(keys == NULL || keys_buf != NULL) {
        return -1;
    }

    // Obter o nº de keys (nkeys) no array de keys e calcular o tamanho do keys_buf (keys_buf_size).
    int nkeys = 0;
    int keys_buf_size = sizeof(int); //tamanho inicial serve para guardar o nº de chaves (nkeys) no keys_buf.
    while(keys[nkeys]) {
        keys_buf_size += strlen(keys[nkeys]) + 1; //+1 para o char nulo no final '\0'.
        nkeys++;
    }

    // Alocar memória no keys_buf.
    // '*keys_buf' e não 'keys_buf' pois queremos mudar o valor do ponteiro original passado.
    *keys_buf = (char*)malloc(keys_buf_size);
    if (*keys_buf == NULL) {
        return -1; // Falha ao alocar memória no keys_buf!
    }

    // Copiar o nº de keys (nkeys) para o início do keys_buf.
    memcpy(*keys_buf, &nkeys, sizeof(int)); //memcpy usa endereços! Por isso é que temos &nkeys.
    char* keys_buf_ptr = *keys_buf + sizeof(int); // Pointer que aponta para a posição atual no buffer, neste caso seguindo o exemplo estaria em 'key1'.

    // Copiar as chaves para o keys_buf.
    for (int i = 0; i < nkeys; i++) {
        strcpy(keys_buf_ptr, keys[i]); //copia o keys[i] para a posição atual do ponteiro.
        keys_buf_ptr += strlen(keys[i]) + 1; //incrementar para colocar as keys seguintes.
    }

    return keys_buf_size; //retorna o tamanho alocado no buffer em caso de sucesso.
}

/* De-serializa a mensagem contida em keys_buf, colocando-a num array de
 * strings cujo espaco em mem�ria deve ser reservado. A mensagem contida
 * em keys_buf dever� ter o seguinte formato:
 *    | int   | string | string | string |
 *    | nkeys | key1   | key2   | key3   |
 * Retorna o array de strings ou NULL em caso de erro.
 */
char** buffer_to_keyArray(char* keys_buf){
    // Verificar se o keys_buf é válido.
    if(keys_buf == NULL) {
        return NULL;
    }

    // Ler o nº de chaves (nkeys) do keys_buf.
    int nkeys;
    memcpy(&nkeys, keys_buf, sizeof(int)); //não se usa &keys_buf, pois keys_buf é um ponteiro!

    // Mesmo processo/"método" do ponteiro para ler as keys no keys_buf.
    char* keys_buf_ptr = keys_buf + sizeof(int);

    // Alocar memória no array de strings.
    char** keys = (char**)malloc((nkeys + 1) * sizeof(char*)); //+1 para o char nulo no final '\0'. e * sizeof(char*) por se tratarem de strings.
    if (keys == NULL) {
        return NULL; // Falha ao alocar memória nas keys!
    }

    // Copiar as chaves de keys_buf para o array keys.
    for(int i = 0; i < nkeys; i++) {
        keys[i] = strdup(keys_buf_ptr); // strdup aloca memória para a nova string/key e duplica.

        if (keys[i] == NULL) { //caso haja erro a alocar memória para alguma key.
            for(int j = 0; j < i; j++) {
                free(keys[j]);
            }
            free(keys);
            return NULL; //pois ocorreu erro a alocar memória a alguma key.
        }
        keys_buf_ptr += strlen(keys_buf_ptr) + 1; // Avança para a próxima key. Pois strlen devolve o tamanho da string + char nulo.
    }

}
