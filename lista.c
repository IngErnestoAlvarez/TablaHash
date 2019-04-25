#include <stdbool.h>
#include <stdlib.h>
#include "lista.h"

typedef struct nodo nodo_t;

struct nodo{
    nodo_t* sig;
    void* dato;
};


struct lista{
    nodo_t* prim;
    nodo_t* ult;
    size_t largo;
};
/**********************************************/
/*  NODO  */
/*********************************************/
nodo_t* nodo_crear(void* dato){
    nodo_t* nodo = malloc(sizeof(nodo_t));
    if(!nodo) return NULL;
    nodo->dato = dato;
    nodo->sig = NULL;
    return nodo;
}

void* nodo_destruir(nodo_t* nodo){
    void* dato = nodo->dato;
    free(nodo);
    return dato;
}
/**********************************************/
/*  LISTA  */
/*********************************************/

//Crea una lista.
// Post: devuelve una nueva lista vacia.
lista_t *lista_crear(void){
    lista_t* lista = malloc(sizeof(lista_t));
    if(!lista) return NULL;
    lista->prim = NULL;
    lista->ult = NULL;
    lista->largo = 0;
    return lista;
}

//Devuelve verdadero si la lista esta vacia, falso si tiene elementos.
//Pre: que la lista este vacia.
bool lista_esta_vacia(const lista_t *lista){
    return (!lista->largo);
}
//Inserta un elemento al principio de la lista. Devuelve falso en caso de error.
//Pre: la lista fue creada.
//Post: Se agrega un nuevo elemento a la lista. dato se encuentra al principio.
bool lista_insertar_primero(lista_t *lista, void *dato){
    nodo_t* nodo = nodo_crear(dato);
    if(!nodo) return false;
    nodo->sig = lista->prim;
    lista->prim = nodo;
    if(lista_esta_vacia(lista)) lista->ult = nodo;
    lista->largo++;
    return true;
}
//Mira al primer elemento de la lista, devuelve su valor.
//Si la lista esta vacia, devuelve NULL.
//Pre: La lista fue creada.
//Post: Se devuelve el primer elemento de la lista. 
void *lista_ver_primero(const lista_t *lista){
    if(lista_esta_vacia(lista)) return NULL;
    return(lista->prim->dato);
}
//Elimina el primero elemento.Si la lista esta vacia, devuelve NULL.
//Pre: la lista fue creada.
//post: se elimino el primer elemento de la lista(si lo hay), se devuelve su valor
//  y la lista tiene un elemento menos.
void *lista_borrar_primero(lista_t *lista){
    if(lista_esta_vacia(lista)) return NULL;
    nodo_t* primero = lista->prim;
    lista->prim = primero->sig;
    lista->largo--;
    if(lista_esta_vacia(lista)) lista->ult = NULL;
    return nodo_destruir(primero);
}
//Inserta un elemento al fin de la lista. Devuelve falso en caso de error.
//Pre: la lista fue creada.
//Post: Se agrega un nuevo elemento a la lista. dato se encuentra al final.
bool lista_insertar_ultimo(lista_t *lista, void *dato){
    nodo_t* nodo = nodo_crear(dato);
    if(!nodo) return false;
    if(lista_esta_vacia(lista)) lista->prim = nodo; 
    else lista->ult->sig = nodo;
    lista->ult = nodo;
    lista->largo++;
    return true;
}
//Mira al ultimo elemento de la lista, devuelve su valor.
//Si la lista esta vacia, devuelve NULL.
//Pre: La lista fue creada.
//Post: Se devuelve el ultimo elemento de la lista. 
void *lista_ver_ultimo(const lista_t* lista){
    if(lista_esta_vacia(lista)) return NULL;
    return(lista->ult->dato);
}
//Devuelve el largo de la lista.
//Pre: la lista fue creada.
size_t lista_largo(const lista_t *lista){
    return(lista->largo);
}
//Destruye la lista, Y aplica destruir_dato a cada dato para destruirlo
// de la lista(O NULL si no se utiliza)
//Pre: la lista fue creada.
//Post: Destruye la lista, y utiliza destruir_dato a cada elemento.
void lista_destruir(lista_t *lista, void destruir_dato(void *)){
    while(!lista_esta_vacia(lista)){
        if(destruir_dato){
            destruir_dato(lista_borrar_primero(lista));
        }
        else{
            lista_borrar_primero(lista);
        }
    }
    free(lista);
}
/**********************************************/
/* ITERADOR EXTERNO */
/*********************************************/
struct lista_iter{
    nodo_t* ant;
    nodo_t* actual;
    lista_t* lista;
};

//Crea un iterador.
//Si no se pudo crear el iterador, devuelve NULL.
//Pre: la lista fue creada.
// Post: devuelve un nuevo iterador al comienzo de la lista.
lista_iter_t *lista_iter_crear(lista_t *lista){
    lista_iter_t* iter = malloc(sizeof(lista_iter_t));
    if(!iter) return NULL;
    iter->lista = lista;
    iter->actual = lista->prim;
    iter->ant = NULL;
    return iter;
}
//El iterador avanza una posicion en la lista. Si no puede avanzar mas
//devuelve false(cuando llegó al final de la lista, despues del ultimo elemento).
//Pre: el iterador fue creado.
//Post: el iterador esta un posicion mas adelante.
bool lista_iter_avanzar(lista_iter_t *iter){
    if(lista_iter_al_final(iter)) return false;
    iter->ant = iter->actual;
    iter->actual = iter->ant->sig;
    return true;
}
//Mira al elemento en el que esta el iterador, devuelve NULL si la lista esta vacia.
//devuelve el valor en la posicion del iterador.
//Si esta al final de la lista, devuelve NULL.
//Pre: el iterador fue creado.
//Post: Se devuelve el dato(void*) en la posicion del iterador.
void *lista_iter_ver_actual(const lista_iter_t *iter){
    if(!iter->actual) return NULL;
    return(iter->actual->dato);
}
//Inserta un elemento en la posicion actual del iterador, devuelve falso si no se pudo
//insertar el elemento. 
//Pre: el iterador fue creado.
//Post: Inserta un elemento en la posicion actual.
bool lista_iter_insertar(lista_iter_t *iter, void *dato){
    nodo_t* nuevo = nodo_crear(dato);
    if(!nuevo) return false;
    if(lista_esta_vacia(iter->lista)){
        iter->lista->prim = nuevo;
        iter->lista->ult = nuevo;
        iter->lista->largo++;
        iter->actual = nuevo;
        return true;
    }
    if(lista_iter_al_final(iter)) iter->lista->ult = nuevo;
    nuevo->sig = iter->actual;
    iter->actual = nuevo;
    if(iter->ant) iter->ant->sig = nuevo;
    else iter->lista->prim = nuevo;
    iter->lista->largo++;
    return true;
}
//Elimina el elemento actual en el que esta el iterador, devuelve su dato.
//devuelve NULL si no hay elemento que borrar.
//El elemento siguiente al actual, pasa a ser el actual.
//Pre: el iterador fue creado.
//Post: elimina el elemento actual.
void *lista_iter_borrar(lista_iter_t *iter){
    if(lista_iter_al_final(iter)) return NULL;
    nodo_t* aux = iter->actual;
    iter->actual = aux->sig;
    if(iter->ant)iter->ant->sig = iter->actual;
    else iter->lista->prim = iter->actual;
    iter->lista->largo--;
    if(lista_iter_al_final(iter)) iter->lista->ult = iter->ant;
    //if(lista_esta_vacia(iter->lista)) iter->lista->ult = NULL;
    return nodo_destruir(aux);
}
//Devuelve verdadero si el elemento actual del iterador es el ultimo de la lista,
//falso si no lo es.
//Pre: el iterador fue creado. 
bool lista_iter_al_final(const lista_iter_t *iter){
    return(!iter->actual);
}
//Destruye el iterador.
//Pre: El iterador fue creado.
//Post: Elimina el iterador.
void lista_iter_destruir(lista_iter_t *iter){
    free(iter);
}
/**********************************************/
/* ITERADOR INTERNO */
/*********************************************/

//Recorre la lista y le aplica visitar a cada uno de los elementos de la lista.
//Extra te permite sacar algun valor de la funcion que necesites para despues
//Ejemplo: (un contador, un TOTAL, etc).
//Pre: La lista fue creada.
//Post: Cada elemento de la lista sufre "visitar".
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra){
    if(!visitar) return;
    nodo_t* actual = lista->prim;
    while(actual){
        if(!visitar(actual->dato, extra)) return;
        actual = actual->sig;
    }
    
}