#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct lista lista_t;

/**********************************************/
/*  LISTA  */
/*********************************************/

//Crea una lista.
// Post: devuelve una nueva lista vacia.
lista_t *lista_crear(void);

//Devuelve verdadero si la lista esta vacia, falso si tiene elementos.
//Pre: que la lista este vacia.
bool lista_esta_vacia(const lista_t *lista);

//Inserta un elemento al principio de la lista. Devuelve falso en caso de error.
//Pre: la lista fue creada.
//Post: Se agrega un nuevo elemento a la lista. dato se encuentra al principio.
bool lista_insertar_primero(lista_t *lista, void *dato);

//Mira al primer elemento de la lista, devuelve su valor.
//Si la lista esta vacia, devuelve NULL.
//Pre: La lista fue creada.
//Post: Se devuelve el primer elemento de la lista. 
void *lista_ver_primero(const lista_t *lista);

//Elimina el primero elemento.Si la lista esta vacia, devuelve NULL.
//Pre: la lista fue creada.
//post: se elimino el primer elemento de la lista(si lo hay), se devuelve su valor
//  y la lista tiene un elemento menos, si la lista no estaba vacia.
void *lista_borrar_primero(lista_t *lista);

//Inserta un elemento al fin de la lista. Devuelve falso en caso de error.
//Pre: la lista fue creada.
//Post: Se agrega un nuevo elemento a la lista. dato se encuentra al final.
bool lista_insertar_ultimo(lista_t *lista, void *dato);

//Mira al ultimo elemento de la lista, devuelve su valor.
//Si la lista esta vacia, devuelve NULL.
//Pre: La lista fue creada.
//Post: Se devuelve el ultimo elemento de la lista. 
void *lista_ver_ultimo(const lista_t* lista);

//Devuelve el largo de la lista.
//Pre: la lista fue creada.
size_t lista_largo(const lista_t *lista);

//Destruye la lista, Y aplica destruir_dato a cada dato para destruirlo
// de la lista(O NULL si no se utiliza)
//Pre: la lista fue creada.
//Post: Destruye la lista, y utiliza destruir_dato a cada elemento.
void lista_destruir(lista_t *lista, void destruir_dato(void *));

/**********************************************/
/* ITERADOR EXTERNO */
/*********************************************/
typedef struct lista_iter lista_iter_t;

//Crea un iterador.
//Si no se pudo crear el iterador, devuelve NULL.
//Pre: la lista fue creada.
// Post: devuelve un nuevo iterador al comienzo de la lista.
lista_iter_t *lista_iter_crear(lista_t *lista);

//El iterador avanza una posicion en la lista. Si no puede avanzar mas
//devuelve false(cuando llegó al final de la lista, despues del ultimo elemento).
//Pre: el iterador fue creado.
//Post: el iterador esta un posicion mas adelante.
bool lista_iter_avanzar(lista_iter_t *iter);

//Mira al elemento en el que esta el iterador, devuelve NULL si la lista esta vacia.
//devuelve el valor en la posicion del iterador.
//Si esta al final de la lista, devuelve NULL.
//Pre: el iterador fue creado.
//Post: Se devuelve el dato(void*) en la posicion del iterador.
void *lista_iter_ver_actual(const lista_iter_t *iter);

//Inserta un elemento en la posicion actual del iterador, devuelve falso si no se pudo
//insertar el elemento. 
//Pre: el iterador fue creado.
//Post: Inserta un elemento en la posicion actual.
bool lista_iter_insertar(lista_iter_t *iter, void *dato);

//Elimina el elemento actual en el que esta el iterador, devuelve su dato.
//devuelve NULL si no hay elemento que borrar.
//El elemento siguiente al actual, pasa a ser el actual.
//Pre: el iterador fue creado.
//Post: elimina el elemento actual.
void *lista_iter_borrar(lista_iter_t *iter);

//Devuelve verdadero si el elemento actual del iterador es el ultimo de la lista,
//falso si no lo es.
//Pre: el iterador fue creado. 
bool lista_iter_al_final(const lista_iter_t *iter);
//Destruye el iterador.
//Pre: El iterador fue creado.
//Post: Elimina el iterador.
void lista_iter_destruir(lista_iter_t *iter);

/**********************************************/
/* ITERADOR INTERNO */
/*********************************************/

//Recorre la lista y le aplica visitar a cada uno de los elementos de la lista.
//Extra te permite sacar algun valor de la funcion que necesites para despues
//Ejemplo: (un contador, un TOTAL, etc).
//Pre: La lista fue creada.
//Post: Cada elemento de la lista sufre "visitar".
void lista_iterar(lista_t *lista, bool visitar(void *dato, void *extra), void *extra);
#endif // LISTA_H