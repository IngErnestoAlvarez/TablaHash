
#include <stdbool.h>
#include <stddef.h>
#include "lista.h"
#include "hash.h"

// Los structs deben llamarse "hash" y "hash_iter".
typedef struct nodo nodo_t;

struct nodo{
    void* dato;
    char* clave;
};

struct hash{
    size_t cantidad;
    size_t capacidad;
    lista_t** tabla;
    hash_destruir_dato_t funcion;
};
struct hash_iter{
    hash_t* mi_hash;
    lista_iter_t* iterador_lista;
    size_t pos;
};

/**************************************************************/
//FUNCIONES AUXILIARES//
/**************************************************************/

/*
* Pre: El hash fue creado.
*Devuelve el nodo donde se encontró la clave, si no lo encuentra
*devuelve NULL.
*/
nodo_t* hash_buscar_clave(const hash_t* hash, const char* clave){

}

/**************************************************************/
//FUNCIONES HASH//
/**************************************************************/

/* Crea el hash */
hash_t *hash_crear(hash_destruir_dato_t destruir_dato);



/* Guarda un elemento en el hash, si la clave ya se encuentra en la
 * estructura, la reemplaza. De no poder guardarlo devuelve false.
 * Pre: La estructura hash fue inicializada
 * Post: Se almacenó el par (clave, dato)
 */
bool hash_guardar(hash_t *hash, const char *clave, void *dato);

/* Borra un elemento del hash y devuelve el dato asociado.  Devuelve
 * NULL si el dato no estaba.
 * Pre: La estructura hash fue inicializada
 * Post: El elemento fue borrado de la estructura y se lo devolvió,
 * en el caso de que estuviera guardado.
 */
void *hash_borrar(hash_t *hash, const char *clave);

void *hash_obtener(const hash_t *hash, const char *clave){
    return(hash_buscar_clave(hash, clave)->dato);
}

bool hash_pertenece(const hash_t *hash, const char *clave){
    return(hash_buscar_clave(hash, clave));
}
size_t hash_cantidad(const hash_t *hash){
    return(hash->cantidad);
}

/* Destruye la estructura liberando la memoria pedida y llamando a la función
 * destruir para cada par (clave, dato).
 * Pre: La estructura hash fue inicializada
 * Post: La estructura hash fue destruida
 */
void hash_destruir(hash_t *hash){
    for(int i = 0; i < hash->capacidad; i++){
        lista_destruir(hash->tabla[i], hash->funcion);
    }
    free(hash->tabla);
    free(hash);
}
/* Iterador del hash */

hash_iter_t *hash_iter_crear(const hash_t *hash){
    hash_iter_t* iter = malloc(sizeof(hash_iter_t));
    if(!iter) return NULL;
    iter->pos = 0;
    iter->iterador_lista = lista_iter_crear(hash->tabla[iter->pos]);
    if(!iter->iterador_lista){
        free(iter);
        return NULL;
    }
    return iter;
}

// Avanza iterador
bool hash_iter_avanzar(hash_iter_t *iter);

const char *hash_iter_ver_actual(const hash_iter_t *iter){
    return(lista_iter_ver_actual(iter->iterador_lista));
}
bool hash_iter_al_final(const hash_iter_t *iter){
    return((iter->pos == (iter->mi_hash->capacidad-1)) && (lista_iter_al_final(iter->iterador_lista)));
}

void hash_iter_destruir(hash_iter_t* iter){
    lista_iter_destruir(iter->iterador_lista);
    free(iter);
}