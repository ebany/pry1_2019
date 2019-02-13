#ifndef LISTA_H_INCLUDED
#define LISTA_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct datos_lista{
    char letra;
    int numero;
    char path_disk[1000];
    char name[1000];

    struct datos_lista* siguiente;
    struct datos_lista* anterior;
}NODO;

NODO* raiz = 0;



int buscar(char* path_disk, char* name){
    if(raiz!=NULL){
        NODO* temporal = raiz;
        while(temporal!=NULL){
            if(strcmp(path_disk,temporal->path_disk)==0){
                if(strcmp(name,temporal->name)==0)
                    return 1;
            }
            temporal = temporal->siguiente;
        }
    }
    return 0;
}

char buscar_letra(char* path_disk){
    char letra = 'a';
    int flag = 0;
    if(raiz!=NULL){
        NODO* temporal = raiz;
        while(temporal!=NULL){
            if(strcmp(temporal->path_disk,path_disk)==0){
                return temporal->letra;
            }
            temporal = temporal->siguiente;
        }

        temporal = raiz;
        while(temporal!=NULL){
            if(temporal->siguiente==NULL)
                return temporal->letra + 1;
            else if(temporal->letra>letra+1)
                return letra+1;
            else if(temporal->anterior==NULL && temporal->letra>letra){
                return letra;
            }else if(temporal->anterior!=NULL && temporal->anterior->letra!=letra && temporal->letra>letra){
                return letra;
            }else
                letra = temporal->letra;
            temporal = temporal->siguiente;
        }
        //printf("que pasa\n");
    }
    return 'a';
}

int buscar_numero(char letra){
    int numero = 1;
    if(raiz!=NULL){
        NODO* temporal = raiz;
        while(temporal!=NULL){
            if(temporal->letra==letra){
                if(temporal->siguiente==NULL){
                    return temporal->numero+1;
                }else if(temporal->siguiente->letra!=letra){
                    return temporal->numero+1;
                }else if(temporal->numero>numero+1){
                    return numero+1;
                }else if(temporal->anterior==NULL && temporal->numero>numero){
                    return numero;
                }else if(temporal->anterior!=NULL && temporal->anterior->letra!=letra && temporal->numero>numero){
                    return numero;
                }else
                    numero = temporal->numero;
            }
            temporal = temporal->siguiente;
        }
    }
    return 1;
}

int agregar(char letra,int numero,char* path_disk,char* name){
    NODO* nuevo = (NODO*) malloc(sizeof(NODO));
    nuevo->anterior = NULL;
    nuevo->siguiente = NULL;
    nuevo->numero = numero;
    nuevo->letra = letra;
    memset(nuevo->path_disk,0,1000);
    strcat(nuevo->path_disk,path_disk);
    memset(nuevo->name,0,1000);
    strcat(nuevo->name,name);

    if(raiz!=NULL){
        NODO* temporal = raiz;
        while(temporal!=NULL){
            if(temporal->letra==letra){
                if(temporal->numero>numero){ //meter atras
                    if(temporal->anterior == NULL){ //es raiz
                        nuevo->siguiente = temporal;
                        temporal->anterior = nuevo;
                        raiz = nuevo;
                        return 1;
                    }else{  //meter en medio, atras
                        nuevo->siguiente = temporal;
                        nuevo->anterior = temporal->anterior;
                        temporal->anterior->siguiente = nuevo;
                        temporal->anterior = nuevo;
                        return 1;
                    }
                }else{ //meter adelante
                    if(temporal->siguiente == NULL){//no hay mas nodos
                        temporal->siguiente = nuevo;
                        nuevo->anterior = temporal;
                        return 1;
                    }else if(temporal->siguiente!=NULL && temporal->letra!=letra){//fin de letra
                        nuevo->siguiente = temporal->siguiente;
                        nuevo->anterior = temporal;
                        temporal->siguiente->anterior = nuevo;
                        temporal->siguiente = nuevo;
                        return 1;
                    }
                }
            }else if(temporal->letra>letra){
                if(temporal->anterior!=NULL){
                    temporal->anterior->siguiente = nuevo;
                    nuevo->anterior = temporal->anterior;
                    nuevo->siguiente = temporal;
                    temporal->anterior = nuevo;
                }else{
                    nuevo->siguiente = temporal;
                    temporal->anterior = nuevo;
                    raiz = nuevo;
                }
                return 1;
            }else if(temporal->siguiente==NULL){
                temporal->siguiente = nuevo;
                nuevo->anterior = temporal;
                return 1;
            }
            temporal = temporal->siguiente;
        }
    }else{
        raiz = nuevo;
        return 1;
    }
    return 0;
}

/*void mostrar_mount(){
    if(raiz!=NULL){
        NODO* temporal = raiz;
        printf("Particiones montadas:\n");
        while(temporal!=NULL){
            printf("-- vd%c%d\t%s\t%s\n",temporal->letra,temporal->numero,temporal->name,temporal->path_disk);
            temporal = temporal->siguiente;
        }
        printf("\n");
    }else{
        printf("\t### No hay particiones montadas\n\n");
    }
}*/

int eliminar(char letra, int numero){
    if(raiz!=NULL){
        NODO* temp = raiz;
        while(temp!=NULL){
            if(temp->letra==letra && temp->numero==numero){
                if(temp->anterior==NULL)
                    raiz = temp->siguiente;
                else if(temp->siguiente==NULL){
                    temp->anterior->siguiente = temp->siguiente;
                }else{
                    temp->anterior->siguiente = temp->siguiente;
                    temp->siguiente->anterior = temp->anterior;
                }
                free(temp);
                return 1;
            }
            temp = temp->siguiente;
        }
    }
    return 0;
}

char* search_rep(char letra, int numero){

    if(raiz!=NULL){
        NODO* temp = raiz;
        while(temp!=NULL){
            if(temp->letra==letra && temp->numero==numero){
                return temp->path_disk;
            }
            temp = temp->siguiente;
        }
    }
    return 0;
}

char* get_name(char letra, int numero){
    if(raiz!=NULL){
        NODO* temp = raiz;
        while(temp!=NULL){
            if(temp->letra==letra && temp->numero==numero){
                return temp->name;
            }
            temp = temp->siguiente;
        }
    }
    return 0;
}

#endif // LISTA_H_INCLUDED
