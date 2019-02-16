#ifndef MATRIZ_H_INCLUDED
#define MATRIZ_H_INCLUDED

typedef struct datos_matrix{
    char name[1000];

    int numero;

    struct datos_matrix* arriba;
    struct datos_matrix* abajo;
}NODOM;

typedef struct datos_retorno{
    char letra;
    int numero;
    char path_disk[1000];
    char name[1000];
}RET;

typedef struct n_horizontal{
    char letra;
    char path_disk[1000];

    struct n_horizontal* siguiente;
    struct n_horizontal* anterior;
    NODOM* inicio;
}HH;

HH* cabeza_h = 0;


RET* get_datos(char letra, int numero){
    HH* temporal = cabeza_h;
    if(temporal!=NULL){
        RET* nuevo = (RET*) malloc(sizeof(RET));

        NODOM* temporal2 = 0;
        while(temporal!=NULL){
            if(temporal->letra==letra){
                temporal2 = temporal->inicio;
                nuevo->letra = letra;
                memset(nuevo->path_disk,0,1000);
                strcat(nuevo->path_disk,temporal->path_disk);
                break;
            }
            temporal = temporal->siguiente;
        }
        if(temporal2!=NULL){
            while(temporal2!=NULL){
                if(temporal2->numero==numero){
                    memset(nuevo->name,0,1000);
                    strcat(nuevo->name,temporal2->name);
                    return nuevo;
                }
                temporal2 = temporal2->abajo;
            }
        }
    }
    return NULL;
}

char get_letra(char* path){
    if(cabeza_h==NULL)
        return 'a';
    else{
        //buscar si ya esta el path
        HH* temporal = cabeza_h;
        while(temporal!=NULL){
            if(strcmp(temporal->path_disk,path)==0)
                return temporal->letra;
            temporal = temporal->siguiente;
        }
        //buscar primera opcion
        char retorno = 'a';
        temporal = cabeza_h;
        while(temporal!=NULL){
            if(temporal->letra>retorno)
                return retorno;
            temporal = temporal->siguiente;
            retorno++;
        }
        return retorno;
    }
}

int get_numero(char letra){
    HH* temporal = cabeza_h;
    if(temporal==NULL)
        return 1;
    else{
        NODOM* temporal2 = 0;
        while(temporal!=NULL){
            if(temporal->letra==letra){
                temporal2 = temporal->inicio;
                break;
            }
            temporal = temporal->siguiente;
        }
        if(temporal2==NULL)
            return 1;
        else{
            //buscar primera opcion
            int retorno = 1;
            while(temporal2!=NULL){
                if(temporal2->numero > retorno)
                    return retorno;
                temporal2 = temporal2->abajo;
                retorno++;
            }
            return retorno;
        }
    }
}

int verificar_nombre(char* nombre, char letra){
    HH* temporal = cabeza_h;
    if(temporal==NULL)
        return 0;
    else{
        NODOM* temporal2 = 0;
        while(temporal!=NULL){
            if(temporal->letra==letra){
                temporal2 = temporal->inicio;
                break;
            }
            temporal = temporal->siguiente;
        }
        if(temporal2==NULL)
            return 0;
        else{
            while(temporal2!=NULL){
                if(strcmp(temporal2->name,nombre)==0)
                    return 1;
                temporal2 = temporal2->abajo;
            }
        }
    }
    return 0;
}

int add_nodo(char* path, char* name){
    char letra = get_letra(path);
    int numero = get_numero(letra);
    if(verificar_nombre(name,letra)==0){
        //set valores iniciales horizontal
        HH* nuevo = (HH*) malloc(sizeof(HH));
        nuevo->anterior = NULL;
        nuevo->siguiente = NULL;
        nuevo->inicio = NULL;
        nuevo->letra = letra;
        memset(nuevo->path_disk,0,1000);
        strcat(nuevo->path_disk,path);

        //set valores iniciales vertical
        NODOM* nuevo2 = (NODOM*)malloc(sizeof(NODOM));
        nuevo2->abajo = NULL;
        nuevo2->arriba = NULL;
        nuevo2->numero = numero;
        memset(nuevo2->name,0,1000);
        strcat(nuevo2->name,name);

        //insertar horizontal
        HH* temporal = cabeza_h;
        if(temporal==NULL){
            //enlaces
            cabeza_h = nuevo;
            cabeza_h->inicio = nuevo2;
            return 1;
        }else{
            NODOM* temporal2 = 0;
            while(temporal!=NULL){
                if(temporal->letra==letra){
                    temporal2 = temporal->inicio;
                    free(nuevo);
                    break;
                }else if(temporal->letra > letra){
                    //enlaces horizontal
                    nuevo->siguiente = temporal;
                    nuevo->anterior = temporal->anterior;
                    if(temporal->anterior==NULL){
                        cabeza_h = nuevo;
                    }else
                        temporal->anterior->siguiente = nuevo;
                    temporal->anterior = nuevo;
                    //enlace vertical
                    nuevo->inicio = nuevo2;
                    return 1;
                }else if(temporal->siguiente==NULL){
                    //enlaces horizontal
                    nuevo->anterior = temporal;
                    temporal->siguiente = nuevo;
                    //enlaces vertical
                    nuevo->inicio = nuevo2;
                    return 1;
                }
                temporal = temporal->siguiente;
            }
            while(temporal2!=NULL){
                if(temporal2->numero > numero){
                    nuevo2->abajo = temporal2;
                    nuevo2->arriba = temporal2->arriba;
                    if(temporal2->arriba==NULL){
                        temporal->inicio = nuevo2;
                    }else
                        temporal2->arriba->abajo = nuevo2;
                    temporal2->arriba = nuevo2;
                    return 1;
                }else if(temporal2->abajo==NULL){
                    temporal2->abajo = nuevo2;
                    nuevo2->arriba = temporal2;
                    return 1;
                }
                temporal2 = temporal2->abajo;
            }
        }
    }else{
        return 0;
    }
}


int delete_nodo(char letra, int numero){
    //1:eliminado | 0:no existe
    HH* temporal = cabeza_h;
    while(temporal!=NULL && temporal->letra!=letra)
        temporal = temporal->siguiente;
    if(temporal!=NULL){
        NODOM* temporal2 = temporal->inicio;
        while(temporal2!=NULL && temporal2->numero!=numero)
            temporal2 = temporal2->abajo;
        if(temporal2!=NULL){
            if(temporal2->arriba!=NULL){
                temporal2->arriba->abajo = temporal2->abajo;
                if(temporal2->abajo!=NULL)
                    temporal2->abajo->arriba = temporal2->arriba;
                free(temporal2);
            }else{
                if(temporal2->abajo!=NULL){
                    temporal->inicio = temporal2->abajo;
                    temporal2->abajo->arriba = NULL;
                    free(temporal2);
                }else{
                    if(temporal->anterior!=NULL){
                        temporal->anterior->siguiente = temporal->siguiente;
                        if(temporal->siguiente!=NULL)
                            temporal->siguiente->anterior = temporal->anterior;
                    }else{
                        if(temporal->siguiente!=NULL){
                            cabeza_h = temporal->siguiente;
                            temporal->siguiente->anterior = NULL;
                        }else{
                            cabeza_h = NULL;
                        }
                    }
                    free(temporal2);
                    free(temporal);
                }
            }
            return 1;
        }
    }
    return 0;
}

void mostrar_mount(){
    if(cabeza_h!=NULL){
        HH* temporal = cabeza_h;
        while(temporal!=NULL){
            printf("\tDisco: %s\n",temporal->path_disk);
            NODOM* temporal2 = temporal->inicio;
            while(temporal2!=NULL){
                printf("\t\tvd%c%d\t%s\n",temporal->letra,temporal2->numero,temporal2->name);
                temporal2 = temporal2->abajo;
            }
            temporal = temporal->siguiente;
        }
    }else{
        printf("\t### No hay particiones montadas\n\n");
    }
}

void delete_disk(char* path){
    HH* temporal = cabeza_h;
    while(temporal!=NULL){
        if(strcmp(temporal->path_disk,path)==0){
            if(temporal->anterior!=NULL){
                temporal->anterior->siguiente = temporal->siguiente;
                if(temporal->siguiente!=NULL)
                    temporal->siguiente->anterior = temporal->anterior;
            }else{
                if(temporal->siguiente!=NULL){
                    temporal->siguiente->anterior = NULL;
                    cabeza_h = temporal->siguiente;
                }else{
                    cabeza_h = NULL;
                }
            }
            free(temporal);
            break;
        }
        temporal = temporal->siguiente;
    }
}

void delete_particion(char* path, char* name){
    HH* temporal = cabeza_h;
    if(temporal!=NULL){
        NODOM* temporal2 = 0;
        while(temporal!=NULL){
            if(strcmp(temporal->path_disk,path)==0){
                temporal2 = temporal->inicio;
                break;
            }
            temporal = temporal->siguiente;
        }
        if(temporal2!=NULL){
            while(temporal2!=NULL){
                if(strcmp(temporal2->name,name)==0){
                    if(temporal2->arriba!=NULL){
                        temporal2->arriba->abajo = temporal2->abajo;
                        if(temporal2->abajo!=NULL){
                            temporal2->abajo->arriba = temporal2->arriba;
                        }
                        free(temporal2);
                    }else{
                        if(temporal2->abajo!=NULL){
                            temporal->inicio = temporal2->abajo;
                            temporal2->abajo->arriba = NULL;
                            free(temporal2);
                        }else{
                            if(temporal->anterior!=NULL){
                                temporal->anterior->siguiente = temporal->siguiente;
                                if(temporal->siguiente!=NULL)
                                    temporal->siguiente->anterior = temporal->anterior;
                            }else{
                                if(temporal->siguiente!=NULL){
                                    temporal->siguiente->anterior = NULL;
                                    cabeza_h = temporal->siguiente;
                                }else{
                                    cabeza_h = NULL;
                                }
                            }
                        }
                    }
                    break;
                }
                temporal2 = temporal2->abajo;
            }
        }
    }
}

#endif // MATRIZ_H_INCLUDED
