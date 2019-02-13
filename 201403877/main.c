#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "[MIA]disco_201403877.h"
#include "[MIA]reporte_201403877.h"
//#include "lista.h"
#include "matriz.h"

/*
    FUNCIONES PRINCIPALES
*/
void func_mkdisk();
void func_rmdisk();
void func_fdisk();
void func_mount();
void func_unmount();
void func_rep();

/*
    FUNCIONES EXTRAS
*/
void parser();
void space();
long obtenerNum();
long auxNum(int cantidad,int valor);
int unit_sp();

int salida = 1; //bandera para ciclo while principal, en main

/*
    Variables para manejar la cadena de entrada
        - se recorre la cadena posicion por posicion
*/
char entrada[1024]; //cadena de entrada
int size = 0;       //tamaño de la cadena
int ap = 0;         //apuntador a la posicion actual en la cadena
char copia[1024];


int main() {
    while(salida) {
        printf("\n~ $ ");
        while(1) {
            //entrada = (char*)realloc(entrada,size+1);
            int c = getchar();
            if(c == EOF || c == '\n') {
                entrada[size] = 0;
                break;
            }
            entrada[size] = (char)c;
            size++;
        }
        /*
            Verificación de comando multilinea
        */
        if(size>0) {
            int temporal = size-1;
            //quitar espacios en blanco al final
            while(temporal>0 && entrada[temporal]==' ')
                temporal--;
            if(temporal<1 || entrada[temporal]!='\\') {
                //copia = (char*)malloc(sizeof(entrada));
                strcpy(copia,entrada);
                for(int a=0; a<size; a++)
                    entrada[a] = tolower(entrada[a]);
                //printf("%s - %s\n",entrada,copia);
                parser();
                //entrada = 0;
                size = 0;
                ap = 0;
                memset(entrada, 0, sizeof(entrada));
                memset(copia, 0, sizeof(copia));
            } else if(temporal>0 && entrada[temporal]=='\\')
                size = temporal;
        }
    }
    printf("\n\tQue tengas un buen dia!\n");
    return 0;
}

void parser() {
    if(strcmp("exit",entrada)==0)
        salida = 0;
    else if(entrada[ap]=='m' && entrada[ap+1]=='o' && entrada[ap+2]=='s' && entrada[ap+3]=='t' && entrada[ap+4]=='r' && entrada[ap+5]=='a' && entrada[ap+6]=='r') {
        mostrar_mount();
    } else if(entrada[ap]=='#') {
        //no hacer nada
    } else if(entrada[ap]=='m' && entrada[ap+1]=='k' && entrada[ap+2]=='d' && entrada[ap+3]=='i' && entrada[ap+4]=='s' && entrada[ap+5]=='k') {
        ap+=6;
        func_mkdisk();
    } else if(entrada[ap]=='r' && entrada[ap+1]=='m' && entrada[ap+2]=='d' && entrada[ap+3]=='i'&& entrada[ap+4]=='s' && entrada[ap+5]=='k') {
        ap+=6;
        func_rmdisk();
    } else if(entrada[ap]=='f' && entrada[ap+1]=='d' && entrada[ap+2]=='i' && entrada[ap+3]=='s'&& entrada[ap+4]=='k') {
        ap+=5;
        func_fdisk();
    } else if(entrada[ap]=='m' && entrada[ap+1]=='o' && entrada[ap+2]=='u' && entrada[ap+3]=='n'&& entrada[ap+4]=='t') {
        ap+=5;
        func_mount();
    } else if(entrada[ap]=='u' && entrada[ap+1]=='n' && entrada[ap+2]=='m' && entrada[ap+3]=='o'&& entrada[ap+4]=='u' && entrada[ap+5]=='n'&& entrada[ap+6]=='t') {
        ap+=7;
        func_unmount();
    } else if(entrada[ap]=='r' && entrada[ap+1]=='e' && entrada[ap+2]=='p') {
        ap+=3;
        func_rep();
    } else if(entrada[ap]=='e' && entrada[ap+1]=='x' && entrada[ap+2]=='e' && entrada[ap+3]=='c') {
        ap+=4;
        exec();
    } else
        printf("\t###Error: Comando no valido\n");
}


/*
    FUNCIONES PROYECTO
*/



/*
    FUNCIONES PRIMERA FASE
*/

void exec() {
    int exit = 1;
    int obligatorio = 0;
    char path_arch[1024];
    memset(path_arch, 0, sizeof(path_arch));
    while(exit) {
        space();
        if(ap>=size)
            exit = 0;
        else if(entrada[ap]=='-' && entrada[ap+1]=='p' && entrada[ap+2]=='a' && entrada[ap+3]=='t' && entrada[ap+4]=='h' && entrada[ap+5]=='~' && entrada[ap+6]==':' && entrada[ap+7]=='~') {
            ap+=8;
            int size_arch = 0;
            int flag = 0;
            int flag_comilla = 0;
            if(entrada[ap]=='"')
                flag_comilla = 1;
            int flag_count = ap;

            while(1) {
                //path_arch = (char*)realloc(path_arch,size_arch+1);
                if(entrada[ap]==EOF || entrada[ap]==13 || entrada[ap]=='\n' || entrada[ap]==0 || (flag_comilla==0 && entrada[ap]==32) || (flag_comilla==1 && entrada[ap]=='"' && ap>flag_count)) {
                    path_arch[size_arch] = 0;
                    break;
                }
                if(entrada[ap]!='"') {
                    path_arch[size_arch] = copia[ap];
                    size_arch++;
                }
                ap++;
            }
            obligatorio = 1;
        } else
            ap++;
    }

    if(obligatorio) {
        FILE* lectura;
        lectura = fopen(path_arch,"r");
        //printf("%s -\n",path_arch);
        if(lectura!=NULL) {
            size = 0;
            ap = 0;
            memset(entrada, 0, sizeof(entrada));
            memset(copia, 0, sizeof(copia));

            while(fgets(copia,1024,(FILE*)lectura)) {
                for(int a=0; a<1024; a++) {
                    if(copia[a]==0 || copia[a]=='\0') {
                        size = a;
                        break;
                    }
                }
                if(size>2) {
                    if(copia[0]!='#') {
                        printf("\n\tLectura: %s",copia);
                        strcpy(entrada,copia);
                        for(int a=0; a<size; a++)
                            entrada[a] = tolower(entrada[a]);
                        parser();
                        size = 0;
                        ap = 0;
                        memset(entrada, 0, sizeof(entrada));
                        memset(copia, 0, sizeof(copia));
                    }
                }
            }
        } else
            printf("\t###Error: No se pudo abrir el archivo.\n\n");
        fclose(lectura);
    } else
        printf("\t###Error: Exec no tiene parametro obligatorio.\n");
}

void func_mkdisk() {
    int obligatorios[2] = {0,0}; //1.size, 2.path
    int error = 0;
    char errores[1000];
    memset(errores, 0, sizeof(errores));

    //variables datos y auxiliares
    long size_disk = -1;
    char unit = 'm';
    char fit = 'f';
    char path_carpeta[1024];
    char path_arch[1024];
    memset(path_carpeta, 0, sizeof(path_carpeta));
    memset(path_arch, 0, sizeof(path_arch));
    //obtencion de datos
    int exit = 1;
    while(exit) {
        space();
        if(ap>=size)
            exit = 0;
        else if(entrada[ap]=='-' && entrada[ap+1]=='s' && entrada[ap+2]=='i' && entrada[ap+3]=='z' && entrada[ap+4]=='e' && entrada[ap+5]=='~' && entrada[ap+6]==':' && entrada[ap+7]=='~') {
            ap+=8;
            size_disk = obtenerNum();
            if(size_disk>0)
                obligatorios[0] = 1;
            else {
                error = 1;
                strcat(errores,"\t###Error: valor de parametro size invalido\n");
            }
        } else if(entrada[ap]=='-' && entrada[ap+1]=='u' && entrada[ap+2]=='n' && entrada[ap+3]=='i' && entrada[ap+4]=='t' && entrada[ap+5]=='~' && entrada[ap+6]==':' && entrada[ap+7]=='~') {
            ap+=8;
            if(entrada[ap]=='k') {
                unit = 'k';
                ap++;
            } else if(entrada[ap]=='m') {
                ap++;
            } else {
                n_space();
                error = 1;
                strcat(errores,"\t###Error: valor de parametro unit invalido\n");
            }
        } else if(entrada[ap]=='-' && entrada[ap+1]=='f' && entrada[ap+2]=='i' && entrada[ap+3]=='t' && entrada[ap+4]=='~' && entrada[ap+5]==':' && entrada[ap+6]=='~') {
            ap+=7;
            if(entrada[ap]=='b' && entrada[ap+1]=='f') {
                ap+=2;
                fit = 'b';
            } else if(entrada[ap]=='f' && entrada[ap+1]=='f')
                ap+=2;
            else if(entrada[ap]=='w' && entrada[ap+1]=='f') {
                ap+=2;
                fit = 'w';
            } else {
                n_space();
                error = 1;
                strcat(errores,"\t###Error: valor de parametro fit invalido\n");
            }
        } else if(entrada[ap]=='-' && entrada[ap+1]=='p' && entrada[ap+2]=='a' && entrada[ap+3]=='t' && entrada[ap+4]=='h' && entrada[ap+5]=='~' && entrada[ap+6]==':' && entrada[ap+7]=='~') {
            ap+=8;
            char temp[1024];
            memset(temp, 0, sizeof(temp));
            int size_temp = 0;
            int size_carpeta = 0;
            int size_arch = 0;
            int flag = 0;
            int contadorSl = 0;

            int flag_comilla = 0;
            if(entrada[ap]=='"')
                flag_comilla = 1;
            int flag_count = ap;
            while(1) {
                if(entrada[ap]==EOF || entrada[ap]==13 || entrada[ap]=='\n' || entrada[ap]==0 || (flag_comilla==0 && entrada[ap]==32) || (flag_comilla==1 && entrada[ap]=='"' && ap>flag_count)) {
                    path_arch[size_arch] = 0;
                    if(contadorSl==0) {
                        path_carpeta[size_carpeta] = '/';
                        size_carpeta++;
                    }
                    path_carpeta[size_carpeta] = 0;
                    break;
                }
                if(entrada[ap]=='/') {
                    if(flag==1) {
                        path_carpeta[size_carpeta] = '/';
                        size_carpeta++;
                        for(int n=0; n<size_temp; n++) {
                            path_carpeta[size_carpeta] = temp[n];
                            size_carpeta++;
                        }
                        memset(temp, 0, sizeof(temp));
                        size_temp = 0;
                        contadorSl++;
                    } else
                        flag = 1;
                } else if(flag==1) {
                    temp[size_temp] = copia[ap];
                    size_temp++;
                }
                if(entrada[ap]!='"') {
                    path_arch[size_arch] = copia[ap];
                    size_arch++;
                }
                ap++;
            }
            obligatorios[1] = 1;
        } else {
            if(entrada[ap]=='#' || entrada[ap]==13)
                exit = 0;
            else if(entrada[ap]!=32 && entrada[ap]!='\n' && entrada[ap]!=0 && entrada[ap]!=EOF && entrada[ap]!='"') {
                error = 1;
                strcat(errores,"\t###Error: parametro invalido\n");
            }
            ap++;
        }
    }
    //validacion de datos
    if(error) {
        printf("%s",errores);
    } else if(obligatorios[0] && obligatorios[1]) {
        //printf("%s - %s",entrada,path_carpeta);
        if(unit=='m')
            size_disk = size_disk*1024;

        struct stat datosFichero;
        char comando[1000];
        char append[20];
        memset(comando, 0, sizeof(comando));
        memset(append, 0, sizeof(append));

        if(stat(path_carpeta,&datosFichero)==-1) {
            char comando0[1000];
            memset(comando0, 0, sizeof(comando0));
            strcat(comando0,"mkdir -p \"");
            strcat(comando0,path_carpeta);
            strcat(comando0,"\" 2> /dev/null");
            system(comando0);
            //CREAR CARPETA
            if(stat(path_carpeta,&datosFichero)!=-1) {
                strcat(comando,"dd if=/dev/zero of=\"");
                strcat(comando,path_arch);
                strcat(comando,"\" bs=1024 count=");
                sprintf(append,"%d",size_disk);
                strcat(comando,append);
                strcat(comando," 2> /dev/null");
                system(comando);
                if(addMBR(path_arch,size_disk,fit))
                    printf("\t### Disco creado\n");
            } else {
                printf("\t###Error: No se pudo crear la carpeta\n");
            }
        } else {
            if(S_ISDIR(datosFichero.st_mode)) {
                if(datosFichero.st_mode & S_IRUSR) {
                    if(datosFichero.st_mode & S_IWUSR) {
                        //CREAR ARCHIVO
                        FILE* archivo;
                        archivo = fopen(path_arch,"rb");
                        if(archivo==NULL) {                             //no existe el disco
                            strcat(comando,"dd if=/dev/zero of=\"");
                            strcat(comando,path_arch);
                            strcat(comando,"\" bs=1024 count=");
                            sprintf(append,"%d",size_disk);
                            strcat(comando,append);
                            strcat(comando," 2> /dev/null");
                            system(comando);
                            if(addMBR(path_arch,size_disk,fit))
                                printf("\t### Disco creado\n");
                        } else {                                        //ya existe
                            fclose(archivo);
                            printf("\t--Ya existe el disco. Sobrescibir? (S/N): ");
                            char respuesta;
                            int c = 0;
                            while(1) {
                                scanf("%c",&respuesta);
                                c = getchar();
                                if(respuesta!=' ' && respuesta!='\n' && respuesta!=0)
                                    break;
                            }
                            if(respuesta=='s') {                      //Crear archivo
                                strcat(comando,"dd if=/dev/zero of=\"");
                                strcat(comando,path_arch);
                                strcat(comando,"\" bs=1024 count=");
                                sprintf(append,"%d",size_disk);
                                strcat(comando,append);
                                strcat(comando," 2> /dev/null");
                                system(comando);
                                if(addMBR(path_arch,size_disk,fit))
                                    printf("\t### Disco creado\n");
                            }
                        }
                    } else
                        printf("\t###Error: La carpeta no tiene permisos de escritura\n");
                } else
                    printf("\t###Error: No se puede acceder a la carpeta\n");
            }
        }
    } else
        printf("\t###Error: Faltan parametros obligatorios\n");
}

void func_rmdisk() {
    int obligatorio = 0;
    int exit = 1;
    int error = 0;
    char path_arch1[1024];
    memset(path_arch1, 0, sizeof(path_arch1));
    while(exit) {
        space();
        if(ap>=size) {
            exit = 0;
        } else if(entrada[ap]=='-' && entrada[ap+1]=='p' && entrada[ap+2]=='a' && entrada[ap+3]=='t' && entrada[ap+4]=='h' && entrada[ap+5]=='~' && entrada[ap+6]==':' && entrada[ap+7]=='~') {
            ap+=8;
            int size_arch = 0;

            int flag_comilla = 0;
            if(entrada[ap]=='"')
                flag_comilla = 1;
            int flag_count = ap;

            while(1) {
                //path_arch = (char*)realloc(path_arch,size_arch+1);
                if(entrada[ap]==EOF || entrada[ap]=='\n' || entrada[ap]==0 || (flag_comilla==0 && entrada[ap]==32) || (flag_comilla==1 && entrada[ap]=='"' && ap>flag_count) || entrada[ap]==13) {
                    path_arch1[size_arch] = 0;
                    break;
                }
                if(entrada[ap]!='"') {
                    path_arch1[size_arch] = copia[ap];
                    size_arch++;
                }
                ap++;
            }
            obligatorio = 1;
        } else {
            if(entrada[ap]=='#' || entrada[ap]==13)
                exit = 0;
            else if(entrada[ap]!=32 && entrada[ap]!='\n' && entrada[ap]!=0 && entrada[ap]!=EOF && entrada[ap]!='"')
                error = 1;
            ap++;
        }
    }
    if(error)
        printf("\t###Error: Existen 1 o mas parametros no validos.\n");
    else if(obligatorio) {
        //printf("%s",path_arch1);
        eliminarDisco(path_arch1);
    } else
        printf("\t###Error: Faltan parametros obligatorios\n");
}

void func_fdisk() {
    int obligatorios[2] = {0,0}; //0. path | 1.name
    char errores[1000];
    memset(errores, 0, sizeof(errores));
    int error = 0;

    int primero = 0; //1.size | 2.add | 3.delete

    long size_p = 0;
    char unit_p = 'k';
    char path_arch [1024];
    memset(path_arch, 0, sizeof(path_arch));
    char type_p = 'p';
    char fit_p = 'w';
    int delete_p = 0; //-1.fast | 1. full
    char name_p[1024];
    memset(name_p, 0, sizeof(name_p));
    long add_p = 0;

    int exit = 1;
    while(exit) {
        space();
        if(ap>=size)
            exit = 0;
        else if(entrada[ap]=='-' && entrada[ap+1]=='s' && entrada[ap+2]=='i' && entrada[ap+3]=='z' && entrada[ap+4]=='e' && entrada[ap+5]=='~' && entrada[ap+6]==':' && entrada[ap+7]=='~') {
            ap+=8;
            long size_ = obtenerNum();
            if(primero==0)
                primero = 1;
            if(size_>0) {
                size_p = size_;
            } else {
                error = 1;
                strcat(errores,"\t###Error: El valor de \"size\" debe ser un numero entero, mayor a 0\n");
            }
        } else if(entrada[ap]=='-' && entrada[ap+1]=='u' && entrada[ap+2]=='n' && entrada[ap+3]=='i' && entrada[ap+4]=='t' && entrada[ap+5]=='~' && entrada[ap+6]==':' && entrada[ap+7]=='~') {
            ap+=8;
            if(entrada[ap]=='k' || entrada[ap]=='m' || entrada[ap]=='b')
                unit_p = entrada[ap];
            else {
                error = 1;
                strcat(errores,"\t###Error: Valor incorrecto del parametro \"unit\"\n");
            }
            ap++;
        } else if(entrada[ap]=='-' && entrada[ap+1]=='p' && entrada[ap+2]=='a' && entrada[ap+3]=='t' && entrada[ap+4]=='h' && entrada[ap+5]=='~' && entrada[ap+6]==':' && entrada[ap+7]=='~') {
            ap+=8;
            int size_arch = 0;
            int flag_comilla = 0;
            if(entrada[ap]=='"')
                flag_comilla = 1;
            int flag_count = ap;

            while(1) {
                //path_arch = (char*)realloc(path_arch,size_arch+1);
                if(entrada[ap]==EOF || entrada[ap]==13 || entrada[ap]=='\n' || entrada[ap]==0 || (flag_comilla==0 && entrada[ap]==32) || (flag_comilla==1 && entrada[ap]=='"' && ap>flag_count)) {
                    path_arch[size_arch] = 0;
                    break;
                }
                if(entrada[ap]!='"') {
                    path_arch[size_arch] = copia[ap];
                    size_arch++;
                }
                ap++;
            }
            obligatorios[0] = 1;
        } else if(entrada[ap]=='-' && entrada[ap+1]=='t' && entrada[ap+2]=='y' && entrada[ap+3]=='p' && entrada[ap+4]=='e' && entrada[ap+5]=='~' && entrada[ap+6]==':' && entrada[ap+7]=='~') {
            ap+=8;
            if(entrada[ap]=='p' || entrada[ap]=='e' || entrada[ap]=='l')
                type_p = entrada[ap];
            else {
                error = 1;
                strcat(errores,"\t###Error: Valor incorrecto del parametro \"type\"\n");
            }
            ap++;
        } else if(entrada[ap]=='-' && entrada[ap+1]=='f' && entrada[ap+2]=='i' && entrada[ap+3]=='t' && entrada[ap+4]=='~' && entrada[ap+5]==':' && entrada[ap+6]=='~') {
            ap+=7;
            if((entrada[ap]=='b' && entrada[ap+1]=='f') || (entrada[ap]=='f' && entrada[ap+1]=='f') || (entrada[ap]=='w' && entrada[ap+1]=='f'))
                fit_p = entrada[ap];
            else {
                error = 1;
                strcat(errores,"\t###Error: Valor incorrecto del parametro \"fit\"\n");
            }
            ap+=2;
        } else if(entrada[ap]=='-' && entrada[ap+1]=='d' && entrada[ap+2]=='e' && entrada[ap+3]=='l' && entrada[ap+4]=='e' && entrada[ap+5]=='t' && entrada[ap+6]=='e' && entrada[ap+7]=='~' && entrada[ap+8]==':' && entrada[ap+9]=='~') {
            ap+=10;
            if(primero==0)
                primero = 3;
            if(entrada[ap]=='f' && entrada[ap+1]=='a' && entrada[ap+2]=='s' && entrada[ap+3]=='t') {
                ap+=4;
                delete_p = -1;
            } else if(entrada[ap]=='f' && entrada[ap+1]=='u' && entrada[ap+2]=='l' && entrada[ap+3]=='l') {
                ap+=4;
                delete_p = 1;
            } else {
                error = 1;
                strcat(errores,"\t###Error: Valor incorrecto del parametro \"delete\"\n");
            }
        } else if(entrada[ap]=='-' && entrada[ap+1]=='n' && entrada[ap+2]=='a' && entrada[ap+3]=='m' && entrada[ap+4]=='e' && entrada[ap+5]=='~' && entrada[ap+6]==':' && entrada[ap+7]=='~') {
            ap+=8;
            int size_name = 0;
            int flag_comilla = 0;
            if(entrada[ap]=='"')
                flag_comilla = 1;
            int flag_count = ap;

            while(1) {
                //name_p = (char*)realloc(name_p,size_name+1);
                if(entrada[ap]==EOF || entrada[ap]==13 || entrada[ap]=='\n' || entrada[ap]==0 || (flag_comilla==0 && entrada[ap]==32) || (flag_comilla==1 && entrada[ap]=='"' && ap>flag_count)) {
                    name_p[size_name] = 0;
                    break;
                }
                if(entrada[ap]!='"') {
                    name_p[size_name] = copia[ap];
                    size_name++;
                }
                ap++;
            }
            obligatorios[1] = 1;
        } else if(entrada[ap]=='-' && entrada[ap+1]=='a' && entrada[ap+2]=='d' && entrada[ap+3]=='d' && entrada[ap+4]=='~' && entrada[ap+5]==':' && entrada[ap+6]=='~') {
            ap+=7;
            long valor_add = 0;
            if(primero==0)
                primero = 2;
            if(entrada[ap]=='-') {
                ap++;
                valor_add = obtenerNum()*-1;
                if(valor_add<0)
                    add_p = valor_add;
                else {
                    error = 1;
                    strcat(errores,"\t###Error: Valor incorrecto del parametro \"add\"\n");
                }
            } else {
                valor_add = obtenerNum();
                if(valor_add>0)
                    add_p = valor_add;
                else {
                    error = 1;
                    strcat(errores,"\t###Error: Valor incorrecto del parametro \"add\"\n");
                }
            }
        } else {
            if(entrada[ap]=='#' || entrada[ap]==13)
                exit = 0;
            else if(entrada[ap]!=32 && entrada[ap]!='\n' && entrada[ap]!=0 && entrada[ap]!=EOF && entrada[ap]!='"') {
                error = 1;
                strcat(errores,"\t###Error: parametro invalido\n");
            }
            ap++;
        }
    }

    if(error) {
        printf("%s",errores);
    } else if(obligatorios[0] && obligatorios[1] && primero!=0) {
        if(unit_p=='k')
            size_p = size_p*1024;
        else if(unit_p=='m')
            size_p = size_p*1024*1024;
        add_particion(size_p,unit_p,path_arch,type_p,fit_p,delete_p,name_p,add_p,primero);
    } else
        printf("\t###Error: Faltan parametros obligatorios\n");
}

void func_mount() {
    char path_arch[1024];
    memset(path_arch, 0, sizeof(path_arch));
    char name_f[1024];
    memset(name_f, 0, sizeof(name_f));
    int flag_prm_invalido = 0;
    int flag_path = 0;
    int flag_name = 0;

    int salida_mount = 1;
    while(salida_mount) {
        space();
        if(ap>=size) {
            salida_mount = 0;
        } else if(entrada[ap]=='-' && entrada[ap+1]=='p' && entrada[ap+2]=='a' && entrada[ap+3]=='t' && entrada[ap+4]=='h' && entrada[ap+5]=='~' && entrada[ap+6]==':' && entrada[ap+7]=='~') {
            ap+=8;
            int size_arch = 0;

            int flag_comilla = 0;
            if(entrada[ap]=='"')
                flag_comilla = 1;
            int flag_count = ap;

            while(1) {
                //path_arch = (char*)realloc(path_arch,size_arch+1);
                if(entrada[ap]==EOF || entrada[ap]==13 || entrada[ap]=='\n' || entrada[ap]==0 || (flag_comilla==0 && entrada[ap]==32) || (flag_comilla==1 && entrada[ap]=='"' && ap>flag_count)) {
                    path_arch[size_arch] = 0;
                    break;
                }
                if(entrada[ap]!='"') {
                    path_arch[size_arch] = copia[ap];
                    size_arch++;
                }
                ap++;
            }
            flag_path = 1;
        } else if(entrada[ap]=='-' && entrada[ap+1]=='n' && entrada[ap+2]=='a' && entrada[ap+3]=='m' && entrada[ap+4]=='e' && entrada[ap+5]=='~' && entrada[ap+6]==':' && entrada[ap+7]=='~') {
            ap+=8;
            int size_name = 0;

            int flag_comilla = 0;
            if(entrada[ap]=='"')
                flag_comilla = 1;
            int flag_count = ap;

            while(1) {
                //name_f = (char*)realloc(name_f,size_name+1);
                if(entrada[ap]==EOF || entrada[ap]==13 || entrada[ap]=='\n' || entrada[ap]==0 || (flag_comilla==0 && entrada[ap]==32) || (flag_comilla==1 && entrada[ap]=='"' && ap>flag_count)) {
                    name_f[size_name] = 0;
                    break;
                }
                if(entrada[ap]!='"') {
                    name_f[size_name] = copia[ap];
                    size_name++;
                }
                ap++;
            }
            flag_name = 1;
        } else {
            if(entrada[ap]=='#' || entrada[ap]==13)
                salida_mount = 0;
            else if(entrada[ap]!=32 && entrada[ap]!='\n' && entrada[ap]!=0 && entrada[ap]!=EOF && entrada[ap]!='"')
                flag_prm_invalido = 1;
            ap++;
        }
    }

    if(flag_prm_invalido)
        printf("\t###Error: Parametro no reconocido\n");

    if(flag_name && flag_path) {
        //montar particion
        FILE* disco;
        disco = fopen(path_arch,"rb");
        if(disco!=NULL) {
            MBR temporal;
            fseek(disco, 0, SEEK_SET);
            fread(&temporal,sizeof(MBR),1,disco);
            fclose(disco);
            int validacion = validarNombreMBR(name_f,temporal,path_arch);
            if(validacion!=0) {
                if(add_nodo(path_arch,name_f)){
                    printf("\t###Particion montada!!\n\n");
                }else
                    printf("\t###Error: La particion ya ha sido montada\n");
                /*if(buscar(path_arch,name_f)==0) {
                    char letra = buscar_letra(path_arch);
                    int numero = buscar_numero(letra);
                    //printf("%c--%d\n",letra,numero);
                    agregar(letra,numero,path_arch,name_f);
                    printf("\t###Particion montada!!\n\n");
                } else {
                    printf("\t###Error: La particion ya ha sido montada\n\n");
                }*/
            } else
                printf("\t###Error: La particion no existe en el disco especificado\n");
        } else
            printf("\t###Error: No se pudo acceder al disco\n");
    } else
        printf("\t###Error: Faltan parametros obligatorios\n");
}

void func_unmount() {
    int flag_prm_invalido = 0;
    char letra = 'a';
    int numero = 0;
    int flag_id = 0;

    int salida_unmount = 1;
    while(salida_unmount) {
        space();
        if(ap>=size) {
            salida_unmount = 0;
        } else if(entrada[ap]=='-' && entrada[ap+1]=='i' && entrada[ap+2]=='d' && entrada[ap+3]=='~' && entrada[ap+4]==':' && entrada[ap+5]=='~') {
            ap+=6;
            if(entrada[ap]=='v' && entrada[ap+1]=='d') {
                ap+=2;
                if(isalpha(entrada[ap])!=0) {
                    letra = entrada[ap];
                    ap++;
                    numero = obtenerNum();
                    if(numero>0) {
                        flag_id = 1;
                    } else {
                        flag_prm_invalido = 1;
                    }
                } else
                    flag_prm_invalido = 1;
            } else
                flag_prm_invalido = 1;
        } else {
            if(entrada[ap]=='#' || entrada[ap]==13)
                salida_unmount = 0;
            else if(entrada[ap]!=32 && entrada[ap]!='\n' && entrada[ap]!=0 && entrada[ap]!=EOF && entrada[ap]!='"')
                flag_prm_invalido = 1;
            ap++;
        }
    }
    if(flag_id) {
        char path_arch[1000];
        if(delete_nodo(letra,numero)) {
            printf("\t###Particion desmontada\n");
        } else
            printf("\t###Error: No existe la particion\n");
    } else
        printf("\t###Error: Faltan parametros obligatorios\n");
}

void func_rep() {
    char path_arch[1024];
    memset(path_arch, 0, sizeof(path_arch));
    char path_carpeta[1024];
    memset(path_carpeta, 0, sizeof(path_carpeta));
    char name_f[1024];
    memset(name_f, 0, sizeof(name_f));
    char letra = 'a';
    int numero = 1;

    int flag_id = 0;
    int flag_path = 0;
    int flag_name = 0;
    int flag_prm_invalido = 0;
    int salida_rep = 1;
    while(salida_rep) {
        space();
        if(ap>=size) {
            salida_rep = 0;
        } else if(entrada[ap]=='-' && entrada[ap+1]=='i' && entrada[ap+2]=='d' && entrada[ap+3]=='~' && entrada[ap+4]==':' && entrada[ap+5]=='~') {
            ap+=6;
            if(entrada[ap]=='v' && entrada[ap+1]=='d') {
                ap+=2;
                if(isalpha(entrada[ap])!=0) {
                    letra = entrada[ap];
                    ap++;
                    numero = obtenerNum();
                    if(numero>0) {
                        flag_id = 1;
                    }

                }
            } else
                flag_prm_invalido = 1;
        } else if(entrada[ap]=='-' && entrada[ap+1]=='n' && entrada[ap+2]=='a' && entrada[ap+3]=='m' && entrada[ap+4]=='e' && entrada[ap+5]=='~' && entrada[ap+6]==':' && entrada[ap+7]=='~') {
            ap+=8;
            int size_name = 0;
            int flag_comilla = 0;
            if(entrada[ap]=='"')
                flag_comilla = 1;
            int flag_count = ap;

            while(1) {
                //name_f = (char*)realloc(name_f,size_name+1);
                if(entrada[ap]==EOF || entrada[ap]==13 || entrada[ap]=='\n' || entrada[ap]==0 || (flag_comilla==0 && entrada[ap]==32) || (flag_comilla==1 && entrada[ap]=='"' && ap>flag_count)) {
                    name_f[size_name] = 0;
                    break;
                }
                if(entrada[ap]!='"') {
                    name_f[size_name] = entrada[ap];
                    size_name++;
                }
                ap++;
            }
            flag_name = 1;
        } else if(entrada[ap]=='-' && entrada[ap+1]=='p' && entrada[ap+2]=='a' && entrada[ap+3]=='t' && entrada[ap+4]=='h' && entrada[ap+5]=='~' && entrada[ap+6]==':' && entrada[ap+7]=='~') {
            ap+=8;
            int size_arch = 0;
            int size_carpeta = 0;
            char temp[1024];
            memset(temp, 0, sizeof(temp));
            int sizeTemp = 0;

            int flag_comilla = 0;
            if(entrada[ap]=='"')
                flag_comilla = 1;
            int flag_count = ap;

            int flag = 0;
            int contadorSl = 0;

            while(1) {
                //path_arch = (char*)realloc(path_arch,size_arch+1);
                if(entrada[ap]==EOF || entrada[ap]==13 || entrada[ap]=='\n' || entrada[ap]==0 || (flag_comilla==0 && entrada[ap]==32) || (flag_comilla==1 && entrada[ap]=='"' && ap>flag_count)) {
                    path_arch[size_arch] = 0;
                    if(contadorSl==0) {
                        //path_carpeta = (char*)realloc(path_carpeta,size_carpeta+1);
                        path_carpeta[size_carpeta] = '/';
                        size_carpeta++;
                    }
                    path_carpeta[size_carpeta] = 0;
                    break;
                }
                if(entrada[ap]=='/') {
                    if(flag==1) { //ya viene algo
                        //path_carpeta = (char*)realloc(path_carpeta,size_carpeta+1);
                        path_carpeta[size_carpeta] = '/';
                        size_carpeta++;
                        for(int n=0; n<sizeTemp; n++) {
                            //path_carpeta = (char*)realloc(path_carpeta,size_carpeta+1);
                            path_carpeta[size_carpeta] = temp[n];
                            size_carpeta++;
                        }
                        memset(temp, 0, sizeof(temp));
                        sizeTemp = 0;
                        contadorSl++;
                    } else
                        flag = 1;
                } else if(flag==1) {
                    //temp = (char*)realloc(temp,sizeTemp+1);
                    temp[sizeTemp] = copia[ap];
                    sizeTemp++;
                }
                if(entrada[ap]!='"') {
                    path_arch[size_arch] = copia[ap];
                    size_arch++;
                }
                ap++;
            }
            flag_path = 1;

        } else {
            if(entrada[ap]=='#' || entrada[ap]==13)
                salida_rep = 0;
            else if(entrada[ap]!=32 && entrada[ap]!='\n' && entrada[ap]!=0 && entrada[ap]!=EOF && entrada[ap]!='"')
                flag_prm_invalido = 1;
            ap++;
        }
    }

    if(flag_prm_invalido)
        printf("\t###Error: valor de Parametro incorrecto\n");

    if(flag_id && flag_name && flag_path) {
        RET* datos = get_datos(letra,numero);

        /*char* direccion = search_rep(letra,numero);
        char* name_part = get_name(letra,numero);*/
        if(datos!=NULL) {
            if(strcmp(name_f,"mbr")==0) {
                reportes(path_arch,datos->path_disk,path_carpeta,2,datos->name);
            } else if(strcmp(name_f,"disk")==0) {
                reportes(path_arch,datos->path_disk,path_carpeta,1,datos->name);
            } else if(strcmp(name_f,"sb")==0) {
                reportes(path_arch,datos->path_disk,path_carpeta,3,datos->name);
            } else if(strcmp(name_f,"bm_inode")==0) {
                reportes(path_arch,datos->path_disk,path_carpeta,4,datos->name);
            } else if(strcmp(name_f,"bm_block")==0) {
                reportes(path_arch,datos->path_disk,path_carpeta,5,datos->name);
            } else
                printf("\t###Error: no existe ningun reporte con el nombre especificado\n");
        } else
            printf("\t###Error: No existe ninguna particion montada con el id indicado\n");
    } else {
        if(flag_id==0)
            printf("\t###Error: Parametro id es obligatorio\n");
        if(flag_name==0)
            printf("\t###Error: Parametro name es obligatorio\n");
        if(flag_path==0)
            printf("\t###Error: Parametro path es obligatorio\n");
    }
}

/*
    FUNCIONES EXTRAS
*/

void space() {
    while(entrada[ap]==' ')
        ap++;
}

int n_space() {
    if(entrada[ap+1]==' ') {
        ap++;
        return 1;
    } else {
        while(entrada[ap]!=' ' || entrada[ap]!='\n' || entrada[ap]!=EOF || entrada[ap]!=0)
            ap++;
        return 0;
    }
}

long obtenerNum() { //validad 00000
    if(entrada[ap]!=' ' && entrada[ap]!='\n' && entrada[ap]!=0 && entrada[ap]!='-') {
        int temp = ap;
        int valor = entrada[ap]-48;
        //printf("%d",numero);
        ap++;
        long acumulado = obtenerNum();
        if(valor>=0 && valor<10) {
            if(acumulado>=0) {
                return auxNum(ap-temp,valor)+acumulado;
            } else
                return -1;
        } else                             //no es un numero
            return -1;
    }
    return 0;
}

long auxNum(int cantidad, int valor) {
    switch(cantidad) {
    case 1:
        return valor;
    case 2:
        return valor *10;
    case 3:
        return valor *100;
    case 4:
        return valor *1000;
    case 5:
        return valor *10000;
    case 6:
        return valor *100000;
    case 7:
        return valor *1000000;
    case 8:
        return valor *10000000;
    case 9:
        return valor *100000000;
    case 10:
        return valor *1000000000;
    }
    return 0;
}
