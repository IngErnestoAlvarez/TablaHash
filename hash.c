#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include "lista.h"
#include "hash.h"

#define TAM_PRIMOS 7

const int PRIMOS[] = {233, 467, 967, 1931, 4127, 9103, 20857, 51283};

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
/*FUNCIONES AUXILIARES*/
/**************************************************************/

nodo_t* nodo_crear(char* clave,void*dato){
    nodo_t* nodo_nuevo=malloc(sizeof(nodo_t));
    if(!nodo_nuevo) return NULL;
    char* nueva_clave = malloc(sizeof(char)*(strlen(clave)+1));
    if(!nueva_clave){
        fprintf(stderr, "No se pudo crear la nueva clave");
        free(nodo_nuevo);
        return NULL;
    }
    strcpy(nueva_clave, clave);
    nueva_clave[strlen(clave)] = '\0';
    nodo_nuevo->clave = nueva_clave;
    nodo_nuevo->dato=dato;
    return(nodo_nuevo);
}

void* nodo_destruir(nodo_t* nodo){
    free(nodo->clave);
    void* aux = nodo->dato;
    free(nodo);
    return aux;
}


//!IMPLEMENTAR HASH_REDIMENSIONAR//


size_t funcion_hash(const char* cp)
{
    size_t hash = 0x811c9dc5;
    while (*cp) {
        hash ^= (unsigned char) *cp++;
        hash *= 0x01000193;
    }
    return hash;
}

/* Pre: El hash fue creado.
*Devuelve el nodo donde se encontró la clave, si no lo encuentra
*devuelve NULL.*/
nodo_t* hash_buscar_clave(const hash_t* hash, const char* clave){
    lista_t* lista = hash->tabla[(funcion_hash(clave))%hash->capacidad];
    if(lista_esta_vacia(lista)) return NULL;
    lista_iter_t* iter = lista_iter_crear(lista);
    if(!iter){
        fprintf(stderr, "El iterador de lista no pudo crearse. (hash_buscar_clave)");
        return NULL;
    }
    nodo_t* nodo = NULL;
    while(!lista_iter_al_final(iter)){
        if(!strcmp(((nodo_t*)lista_iter_ver_actual(iter))->clave, clave)){
            nodo = lista_iter_ver_actual(iter);
        }
        lista_iter_avanzar(iter);
    }
    lista_iter_destruir(iter);
    return nodo;
}

/**********************************************/
/* FUNCIONES DE HASH */
/*********************************************/

/* Crea el hash */
hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t* nuevo_hash=malloc(sizeof(hash_t));
    if(nuevo_hash == NULL) return NULL;
    nuevo_hash->cantidad=0;
    nuevo_hash->capacidad=0;
    nuevo_hash->funcion=destruir_dato;
    nuevo_hash->tabla=malloc(sizeof(lista_t)*PRIMOS[nuevo_hash->capacidad]);
    if(nuevo_hash->tabla == NULL){
        free(nuevo_hash);
        return NULL;
    }
    return(nuevo_hash);
}



/* *****************************************************************
 *                          WORK IN PROGRESS
 * *****************************************************************/

bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    nodo_t* nodo_a_modificar=hash_buscar_clave(hash,clave);
    if(nodo_a_modificar!=NULL){
        nodo_a_modificar->dato=dato;
        return true;
    }
    if(!hash_redimensionar(hash){
        fprintf(stderr, "\nNo se pudo redimensionar el hash (hash_guardar).");
        return false;
    }
    nodo_t* nuevo_nodo=nodo_crear(clave,dato);
    if(nuevo_nodo==NULL){
        fprintf(stderr, "\nNo se pudo crear el dato. (hash_guardar)");
        return false;
    } 
    int posicion_tabla = (funcion_hash(clave))%hash->capacidad;
    lista_insertar_ultimo(hash->tabla[posicion_tabla],nuevo_nodo);
    hash->cantidad++;
    return true;
}

//?Hice el cambio para no usar (al pedo) hash_pertenece, asique hash borrar ya deberia estar.
//?Me parece medio feo el como use el sigue, pero es que ya estaba regalado, no se que opinas.

void *hash_borrar(hash_t *hash, const char *clave){
     int posicion_tabla = (funcion_hash(clave))%hash->capacidad;
     lista_iter_t* iterador=lista_iter_crear(hash->tabla[posicion_tabla]);
     bool sigue=true;
     nodo_t* nodo_borrado = NULL;
     while(!lista_iter_al_final(iterador) && sigue){
        if(strcmp(((nodo_t*)lista_iter_ver_actual(iterador))->clave,clave)){
            nodo_borrado=lista_iter_borrar(iterador);
            sigue=false;
        } 
        lista_iter_avanzar(iterador);
     }
     if(sigue) return NULL; // si sigue siendo verdadero, es porque no estaba en la lista.
     void* dato_borrado=nodo_destruir(nodo_borrado);
     lista_iter_destruir(iterador);
     hash->cantidad--;
     return dato_borrado;
}

/* *****************************************************************
 *                          /WORK IN PROGRESS
 * *****************************************************************/

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

/**********************************************/
/* ITERADOR DEL HASH */
/*********************************************/
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

bool hash_iter_avanzar(hash_iter_t *iter){
    if(lista_iter_avanzar(iter->iterador_lista)) return true;
    iter->pos++;
    if(hash_iter_al_final(iter)) return false;
    lista_iter_destruir(iter->iterador_lista);
    iter->iterador_lista = lista_iter_crear(iter->mi_hash->tabla[iter->pos]);
    if(!iter->iterador_lista){
        fprintf(stderr, "No se pudo crear un iterador nuevo");
    }
    return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
    return(lista_iter_ver_actual(iter->iterador_lista));
}
bool hash_iter_al_final(const hash_iter_t *iter){
    return((iter->pos == (iter->mi_hash->capacidad)));
}

void hash_iter_destruir(hash_iter_t* iter){
    lista_iter_destruir(iter->iterador_lista);
    free(iter);
}
