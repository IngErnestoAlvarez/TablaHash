#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>
#include "lista.h"
#include "hash.h"

#define FACTOR_BALANCE 3
#define FACTOR_PARA_ACHICAMIENTO 4
#define FACTOR_ACHICAMIENTO 2
#define FACTOR_AGRANDAMIENTO 2
//CANTIDAD DE POSICIONES VECTOR "PRIMOS"
#define PRIMO 233

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
    const hash_t* mi_hash;
    lista_iter_t* iterador_lista;
    size_t pos;
};



/**************************************************************/
/*FUNCIONES AUXILIARES*/
/**************************************************************/

size_t funcion_hash(const char* cp)
{
    size_t hash = 0x811c9dc5;
    while (*cp) {
        hash ^= (unsigned char) *cp++;
        hash *= 0x01000193;
    }
    return hash;
}


nodo_t* hash_nodo_crear(const char* clave,void*dato){
    nodo_t* nodo_nuevo = malloc(sizeof(nodo_t));
    if(!nodo_nuevo) return NULL;
    char* nueva_clave = malloc(sizeof(char)*(strlen(clave)+1));
    if(!nueva_clave){
        fprintf(stderr, "No se pudo crear la nueva clave");
        free(nodo_nuevo);
        return NULL;
    }
    memcpy(nueva_clave, clave, strlen(clave));
    nueva_clave[strlen(clave)] = '\0';
    nodo_nuevo->clave = nueva_clave;
    nodo_nuevo->dato = dato;
    return(nodo_nuevo);
}

void* hash_nodo_destruir(nodo_t* nodo){
    free(nodo->clave);
    void* aux = nodo->dato;
    free(nodo);
    return aux;
}

void hashificar(hash_t* viejo, lista_t** tabla_nueva, size_t tam_nuevo){
    for(int j = 0; j < viejo->capacidad; j++){
        while(!lista_esta_vacia(viejo->tabla[j])){
            nodo_t* nodo = lista_borrar_primero(viejo->tabla[j]);
            lista_insertar_ultimo(tabla_nueva[funcion_hash(nodo->clave)%tam_nuevo], nodo);
        }
        lista_destruir(viejo->tabla[j], NULL);
    }
}

bool hash_redimensionar(hash_t* hash, size_t tam_nuevo){
    lista_t** tabla_nueva = malloc(sizeof(lista_t*)*tam_nuevo);
    if(!tabla_nueva){
        fprintf(stderr, "No pudo generarse la tabla para redimensionar");
        return false;
    }
    for(int j=0; j<tam_nuevo; j++){
        tabla_nueva[j] = lista_crear();
        if(!tabla_nueva[j]) {
            for(int k = 0; k<j; k++){
                lista_destruir(tabla_nueva[k], NULL);
            }
            fprintf(stderr, "No se pudieron crear todas las listas.");
            return false;
        }
    }
    hashificar(hash, tabla_nueva, tam_nuevo);
    hash->capacidad = tam_nuevo;
    free(hash->tabla);
    hash->tabla = tabla_nueva;
    return true;
}


nodo_t* hash_buscar_clave(const hash_t* hash, const char* clave){
    lista_t* lista = hash->tabla[(funcion_hash(clave))%(hash->capacidad)];
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

hash_t *hash_crear(hash_destruir_dato_t destruir_dato){
    hash_t* nuevo_hash = malloc(sizeof(hash_t));
    if(nuevo_hash == NULL) return NULL;
    nuevo_hash->cantidad = 0;
    nuevo_hash->capacidad = PRIMO;
    nuevo_hash->funcion = destruir_dato;
    nuevo_hash->tabla = malloc(sizeof(lista_t*)*nuevo_hash->capacidad);
    if(nuevo_hash->tabla == NULL){
        free(nuevo_hash);
        return NULL;
    }
    for(int i = 0; i<nuevo_hash->capacidad; i++){
        nuevo_hash->tabla[i] = lista_crear();
        if(!nuevo_hash->tabla[i]){
            for(int j = 0; j < i; j++){
                lista_destruir(nuevo_hash->tabla[j], NULL);
            }
            fprintf(stderr, "No se pudo generar una de las listas de la tabla");
            free(nuevo_hash->tabla);
            free(nuevo_hash);
            return NULL;
        }
    }
    return(nuevo_hash);
}



bool hash_guardar(hash_t *hash, const char *clave, void *dato){
    nodo_t* nodo_a_modificar = hash_buscar_clave(hash,clave);
    if(nodo_a_modificar != NULL){
        if(hash->funcion)hash->funcion(nodo_a_modificar->dato);
        nodo_a_modificar->dato = dato;
        return true;
    }
    if(hash->cantidad == (FACTOR_BALANCE*hash->capacidad)){
        if(!hash_redimensionar(hash, (FACTOR_AGRANDAMIENTO*hash->capacidad)))return false;
    }
    nodo_t* nuevo_nodo = hash_nodo_crear(clave,dato);
    if(nuevo_nodo==NULL){
        fprintf(stderr, "\nNo se pudo crear el dato. (hash_guardar)");
        return false;
    }
    size_t posicion_tabla = (funcion_hash(clave)%(hash->capacidad));
    lista_insertar_ultimo(hash->tabla[posicion_tabla],nuevo_nodo);
    hash->cantidad++;
    return true;
}

void *hash_borrar(hash_t *hash, const char *clave){
    if(!hash_cantidad(hash)) return NULL;
    size_t posicion_tabla = (funcion_hash(clave))%hash->capacidad;
    if(lista_esta_vacia(hash->tabla[posicion_tabla])) return NULL;
    lista_iter_t* iterador = lista_iter_crear(hash->tabla[posicion_tabla]);
    while(!lista_iter_al_final(iterador)){
        if(!strcmp(((nodo_t*)lista_iter_ver_actual(iterador))->clave, clave)){
            nodo_t* nodo = lista_iter_borrar(iterador);
            void* dato_borrado = hash_nodo_destruir(nodo);
            lista_iter_destruir(iterador);
            hash->cantidad--;
            if((hash->cantidad < hash->capacidad/FACTOR_PARA_ACHICAMIENTO) && (hash->capacidad != PRIMO)){
                if(!hash_redimensionar(hash, hash->capacidad/FACTOR_ACHICAMIENTO)) fprintf(stderr, "Se guardó el valor, pero no se pudo redimensionar");
            }
            return dato_borrado;
        } 
        lista_iter_avanzar(iterador);
    }
    lista_iter_destruir(iterador);
    return NULL;
}

void *hash_obtener(const hash_t *hash, const char *clave){
    nodo_t* nodo = hash_buscar_clave(hash, clave);
    if(!nodo) return NULL;
    return(nodo->dato);
}

bool hash_pertenece(const hash_t *hash, const char *clave){
    return(hash_buscar_clave(hash, clave));
}
size_t hash_cantidad(const hash_t *hash){
    return(hash->cantidad);
}

void hash_destruir(hash_t *hash){
    for(int i = 0; i < hash->capacidad; i++){
        while(!lista_esta_vacia(hash->tabla[i])){
            if(hash->funcion) hash->funcion(hash_nodo_destruir(lista_borrar_primero(hash->tabla[i])));
            else hash_nodo_destruir(lista_borrar_primero(hash->tabla[i]));
        }
        lista_destruir(hash->tabla[i], NULL);
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
    iter->mi_hash = hash;
    iter->iterador_lista = lista_iter_crear(hash->tabla[iter->pos]);
    if(!iter->iterador_lista){
        free(iter);
        return NULL;
    }
    if(!hash_cantidad(hash)) return iter;
    while(!hash_iter_ver_actual(iter)){
        hash_iter_avanzar(iter);
    }
    return iter;
}

bool hash_iter_avanzar(hash_iter_t *iter){
    if(hash_iter_al_final(iter)) return false;
    lista_iter_avanzar(iter->iterador_lista);
    while(!hash_iter_ver_actual(iter)){
        lista_iter_destruir(iter->iterador_lista);
        iter->iterador_lista = NULL;
        iter->pos++;
        if(hash_iter_al_final(iter)) return false;
        iter->iterador_lista = lista_iter_crear(iter->mi_hash->tabla[iter->pos]);
        if(!iter->iterador_lista){
            fprintf(stderr, "No se pudo crear un iterador nuevo");
            return false;
        }
    }
    return true;
}

const char *hash_iter_ver_actual(const hash_iter_t *iter){
    if(hash_iter_al_final(iter)) return NULL;
    nodo_t* nodo = lista_iter_ver_actual(iter->iterador_lista);
    if(!nodo) return NULL;
    return(nodo->clave);
}
bool hash_iter_al_final(const hash_iter_t *iter){
    if(!iter->mi_hash->cantidad) return true;
    return((iter->pos == (iter->mi_hash->capacidad)));
}

void hash_iter_destruir(hash_iter_t* iter){
    if(iter->iterador_lista) lista_iter_destruir(iter->iterador_lista);
    free(iter);
}
