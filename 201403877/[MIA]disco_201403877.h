#ifndef DISCO_201403877_H
#define DISCO_201403877_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "matriz.h"

typedef struct extended {
    int part_status;        //particion activa o no
    char part_fit;       //tipo de la particion B F W
    int part_start;        //byte de inicio de la particion
    int part_size;          //tamaño de la particion en bytes
    int part_next;     //byte en que esta proximo EBR. -1 si no hay
    char part_name[16];    //nombre de particion
} EXTD;

typedef struct PARTICION {
    char part_status;
    char part_type;
    char part_fit;
    int part_start;
    int part_size;
    char part_name[16];
} PRT;

typedef struct MI_MBR {
    int mbr_tamanio;
    time_t mbr_fecha_creacion;
    int mbr_disk_signature;
    char disk_fit;
    PRT part[4];        // 4 particiones disponibles
} MBR;

static PRT reseteo;

int primarias(MBR master) {
    int coincidencias = 0;
    for(int a=0; a<4; a++) {
        if(master.part[a].part_status==1 && master.part[a].part_type=='p') {
            coincidencias++;
        }
    }
    return coincidencias;
}

int extendida(MBR master) {
    int coincidencias = 0;
    for(int a=0; a<4; a++) {
        if(master.part[a].part_status==1 && master.part[a].part_type=='e') {
            coincidencias++;
        }
    }
    return coincidencias;
}

char tipoAjuste(int tipo) {
    //FIT: -1 BF / 0 FF / 1 WF / 2 NULL
    if(tipo==-1) {
        return 'b';
    } else if(tipo==0) {
        return 'f';
    } else if(tipo==1) {
        return 'w';
    }
    return 'n';
}

int partVacia(MBR master) {
    for(int a=0; a<4; a++) {
        if(master.part[a].part_status==0)
            return a;
    }
    return -1;
}

int obtExt(MBR master) {
    for(int a=0; a<4; a++) {
        if(master.part[a].part_status==1 && master.part[a].part_type=='e')
            return a;
    }
    return -1;
}

int validarNombreMBR(char* nombre,MBR master,char* ruta) {
    char aux[16];
    strcpy(aux,nombre);
    int retorno = 0;
    for(int a=0; a<4; a++) {
        if(master.part[a].part_status==1) {
            if(master.part[a].part_type=='p') {   //PRIMARIA
                if(strcmp(aux,master.part[a].part_name)==0)    //YA EXISTE EL NOMBRE
                    return 1;
            } else {                         //EXTENDIDA
                //POSICIONARSE AL INICIO DE LA PARTICION
                //OBTENER EBR
                if(master.part[a].part_type=='e') {   //PRIMARIA
                    if(strcmp(aux,master.part[a].part_name)==0)    //YA EXISTE EL NOMBRE
                        return 1;
                }
                FILE* disco;
                disco = fopen(ruta,"r+b");
                EXTD auxiliar;
                long actual = master.part[a].part_start;
                int flag = 1;
                while(flag) {
                    fseek(disco,actual,SEEK_SET);
                    fread(&auxiliar,sizeof(EXTD),1,disco);
                    if(auxiliar.part_status==0 && auxiliar.part_next==-1) {
                        retorno = 0;
                        break;
                    }
                    if(auxiliar.part_status==1 && strcmp(auxiliar.part_name,nombre)==0)
                        return 1;
                    if(auxiliar.part_next==-1) {
                        retorno = 0;
                        break;
                    }
                    actual = auxiliar.part_next;
                }
                fclose(disco);
            }
        }
    }
    return retorno;
}

int bChar(char entrada, char comparacion) {
    int a = entrada;            //puede ser mayuscula o minuscula
    int b = comparacion;        //siempre es minuscula
    if(a == b) {                //iguales
        return 1;
    } else if(a < b) {           //puede que sea mayuscula
        a+= 32;
        if(a == b)
            return 1;
    }
    return 0;
}

//int addMBR path=ruta disco, size=en kB
int addMBR(char* path, int size,char fit) {
    FILE* disco;
    disco = fopen(path,"r+b");
    if(disco!=NULL) {
        fseek(disco, 0, SEEK_SET);
        MBR infoDisk;
        infoDisk.mbr_tamanio = size*1024;   //en bytes
        infoDisk.mbr_disk_signature = rand();
        infoDisk.disk_fit = fit;
        if((infoDisk.mbr_fecha_creacion=time(0))==(time_t)-1) {
            printf("\t###Error: No se pudo escribir el MBR en el disco (Fecha) mkdisk %s\n",path);
            fclose(disco);
        }
        for(int a=0; a<4; a++)
            infoDisk.part[a].part_status = 0;
        fwrite(&infoDisk,sizeof(MBR),1,disco);
        fclose(disco);
        return 1;
    } else
        printf("\t###Error: No se pudo escribir el MBR en el disco. mkdisk %s\n",path);
    return 0;
}

int eliminarDisco(char* path) {
    struct stat datosFichero;
    if(stat(path,&datosFichero)!=-1) {
        printf("\t--Confirmar eliminacion (S/N): ");
        char respuesta;
        int c = 0;
        while(1) {
            scanf("%c",&respuesta);
            c = getchar();
            if(respuesta!=' ' && respuesta!='\n' && respuesta!=0)
                break;
        }
        if(bChar(respuesta,'s')) {
            //Eliminar Archivo
            if(datosFichero.st_mode & S_IRUSR) {
                if(datosFichero.st_mode & S_IWUSR) {
                    if(remove(path)==0) {
                        printf("\t### Disco eliminado\n");
                        delete_disk(path);
                        return 1;
                    } else
                        printf("\t###Error: No se pudo eliminar el Disco\n");
                } else
                    printf("\t###Error: El archivo no tiene permisos de escritura\n");
            } else
                printf("\t###Error: No se puede acceder al archivo\n");
        }
    } else
        printf("\t###Error: No existe el archivo\n");
    return 0;
}

int encontrar_posicion(char fit, char* path,long sizeD,char type) {

    int propuesta = 0;
    int libre = 0;
    int tp = 0;
    int tl = 0;

    FILE* disco;
    disco = fopen(path,"r+b");
    if(disco!=NULL) {
        //OBTENER MBR
        fseek(disco,0,SEEK_SET);
        MBR infoDisk;
        fread(&infoDisk,sizeof(MBR),1,disco);
        fclose(disco);

        //NO HAY PARTICIONES, ADD EN LA PRIMERA POSICION
        if(primarias(infoDisk)+extendida(infoDisk)==0 && type!='l')
            return sizeof(MBR);

        if(type=='p' || type=='e') { //primaria o extendida
            for(int a=0; a<4; a++) {
                if(infoDisk.part[a].part_status==1) {
                    if(a==0) {

                        if(infoDisk.part[a].part_start!=sizeof(MBR)) { // hay espacio libre al inicio
                            if(sizeof(MBR)+sizeD<=infoDisk.part[a].part_start) { // si cabe
                                propuesta = sizeof(MBR);
                                libre = infoDisk.part[a].part_start - (sizeof(MBR)+sizeD);
                                if(fit=='f')
                                    return propuesta;
                            }
                        }

                        if(infoDisk.part[a+1].part_status!=1) { //no hay siguiente
                            if(infoDisk.part[a].part_start+infoDisk.part[a].part_size-1<infoDisk.mbr_tamanio) {
                                if(infoDisk.part[a].part_start+infoDisk.part[a].part_size+sizeD<=infoDisk.mbr_tamanio) {
                                    tp = infoDisk.part[a].part_start + infoDisk.part[a].part_size;
                                    tl = infoDisk.mbr_tamanio - (infoDisk.part[a].part_start + infoDisk.part[a].part_size+sizeD);
                                    if(fit=='b') { //mejor ajuste
                                        if(propuesta==0 || libre>tl) //es la primera propuesta
                                            propuesta = tp;
                                        return propuesta;
                                    } else if(fit=='f') { //primer ajuste
                                        return tp;
                                    } else { //peor ajuste
                                        if(propuesta==0 || libre<tl)
                                            propuesta = tp;
                                        return propuesta;
                                    }
                                }
                            }
                        }

                    } else { //1 | 2 | 3

                        if(infoDisk.part[a-1].part_start + infoDisk.part[a-1].part_size!=infoDisk.part[a].part_start) { //hay espacio entre dos
                            if(infoDisk.part[a-1].part_start + infoDisk.part[a-1].part_size+sizeD<=infoDisk.part[a].part_start) { // si cabe
                                tp = infoDisk.part[a-1].part_start + infoDisk.part[a-1].part_size;
                                tl = infoDisk.part[a].part_start - (infoDisk.part[a-1].part_start + infoDisk.part[a-1].part_size + sizeD);
                                if(fit=='b') { //mejor ajuste
                                    if(propuesta==0 || libre>tl) { //es la primera propuesta
                                        propuesta = tp;
                                        libre = tl;
                                    }
                                } else if(fit=='f') { //primer ajuste
                                    return tp;
                                } else { //peor ajuste
                                    if(propuesta==0 || libre<tl) {
                                        propuesta = tp;
                                        libre = tl;
                                    }
                                }
                            }
                        }

                        if(a==3 || infoDisk.part[a+1].part_status!=1) { //revisar la ultima parte
                            if(infoDisk.part[a].part_start+infoDisk.part[a].part_size-1<infoDisk.mbr_tamanio) {
                                if(infoDisk.part[a].part_start+infoDisk.part[a].part_size+sizeD<=infoDisk.mbr_tamanio) {
                                    tp = infoDisk.part[a].part_start + infoDisk.part[a].part_size;
                                    tl = infoDisk.mbr_tamanio - (infoDisk.part[a].part_start + infoDisk.part[a].part_size + sizeD);
                                    if(fit=='b') { //mejor ajuste
                                        if(propuesta==0 || libre>tl) //es la primera propuesta
                                            propuesta = tp;
                                        return propuesta;
                                    } else if(fit=='f') { //primer ajuste
                                        return tp;
                                    } else { //peor ajuste
                                        if(propuesta==0 || libre<tl)
                                            propuesta = tp;
                                        return propuesta;
                                    }
                                }
                            }
                        }

                    }
                }
            }
        } else { //logica
            int pos_ext = 0;
            int size_ext = 0;
            EXTD logica;

            for(int a=0; a<4; a++) {
                if(infoDisk.part[a].part_status==1 && infoDisk.part[a].part_type=='e') {
                    pos_ext = infoDisk.part[a].part_start;
                    size_ext = infoDisk.part[a].part_size;
                    break;
                }
            }
            //printf("%d - %d\n",pos_ext,size_ext);
            if(pos_ext!=0 && sizeD<=size_ext) {
                disco = fopen(path,"r+b");
                fseek(disco,pos_ext,SEEK_SET);
                fread(&logica,sizeof(EXTD),1,disco);
                fclose(disco);

                if(logica.part_status==0 && logica.part_next==-1) { //no hay logicas
                    propuesta = pos_ext;
                } else if(logica.part_status!=0 && logica.part_next==-1 && (((pos_ext + size_ext)-(logica.part_start + logica.part_size))>=sizeD)) { //solo hay una, al inicio
                    propuesta = logica.part_start + logica.part_size;
                } else { //recorrer
                    if(logica.part_status==0 && logica.part_next!=-1) { //hay un espacio al inicio
                        if((logica.part_next - pos_ext)>=sizeD) {
                            propuesta = pos_ext;
                            libre = logica.part_next - (pos_ext + sizeD);
                        }
                        disco = fopen(path,"r+b");
                        fseek(disco,logica.part_next,SEEK_SET);
                        fread(&logica,sizeof(EXTD),1,disco);
                        fclose(disco);
                    }
                    //hay actual y siguiente
                    while(logica.part_next!=-1) {
                        if((logica.part_next-(logica.part_start + logica.part_size))>=sizeD) { //si cabe, hay espacio disponible
                            tp = logica.part_start + logica.part_size;
                            tl = logica.part_next - (logica.part_start + logica.part_size + sizeD);
                            if(fit=='b') { //mejor ajuste
                                if(propuesta==0 || libre>tl) { //es la primera propuesta
                                    propuesta = tp;
                                    libre = tl;
                                }
                            } else if(fit=='f') { //primer ajuste
                                return tp;
                            } else { //peor ajuste
                                if(propuesta==0 || libre<tl) {
                                    propuesta = tp;
                                    libre = tl;
                                }
                            }
                        }
                        disco = fopen(path,"r+b");
                        fseek(disco,logica.part_next,SEEK_SET);
                        fread(&logica,sizeof(EXTD),1,disco);
                        fclose(disco);
                    }
                    //verificar ultima posicion
                    if(((pos_ext + size_ext)-(logica.part_start + logica.part_size))>=sizeD) { //si cabe
                        tp = logica.part_start + logica.part_size;
                        tl = size_ext - (logica.part_start + logica.part_size + sizeD);
                        if(fit=='b') { //mejor ajuste
                            if(propuesta==0 || libre>tl) //es la primera propuesta
                                propuesta = tp;
                            return propuesta;
                        } else if(fit=='f') { //primer ajuste
                            return tp;
                        } else { //peor ajuste
                            if(propuesta==0 || libre<tl)
                                propuesta = tp;
                            return propuesta;
                        }
                    }

                }
            } else
                propuesta = -2;
        }

    }
    return propuesta;
}

int add_particion(long sizeD,char unit,char* path,char type,char fit,int deleteD,char* name,long add,int primero) {
    FILE* disco;
    disco = fopen(path,"r+b");
    if(disco!=NULL) {
        //OBTENER MBR
        fseek(disco,0,SEEK_SET);
        MBR infoDisk;
        fread(&infoDisk,sizeof(MBR),1,disco);
        fclose(disco);

        if(primero==3) {        //eliminar

            int validacion = 0;
            disco = fopen(path,"r+b");
            for(int a=0; a<4; a++) {
                if(infoDisk.part[a].part_status==1) {
                    if(infoDisk.part[a].part_type=='e') {
                        if(strcmp(infoDisk.part[a].part_name,name)==0) {
                            validacion++;
                            printf("\t--Confirmar eliminacion (S/N): ");
                            char respuesta;
                            int c = 0;
                            while(1) {
                                scanf("%c",&respuesta);
                                c = getchar();
                                if(respuesta!=' ' && respuesta!='\n' && respuesta!=0)
                                    break;
                            }
                            if(bChar(respuesta,'s')) {
                                if(deleteD==1) { //FULL
                                    char extra = '\0';
                                    fseek(disco,infoDisk.part[a].part_start,SEEK_SET);
                                    for(int c=0; c<infoDisk.part[a].part_size; c++)
                                        fwrite(&extra,sizeof(char),1,disco);
                                }
                                //REACOMODACION
                                for(int b=a; b<3; b++)
                                    infoDisk.part[b] = infoDisk.part[b+1];
                                infoDisk.part[3] = reseteo;
                                infoDisk.part[3].part_status = 0;
                                fseek(disco,0,SEEK_SET);
                                fwrite(&infoDisk,sizeof(MBR),1,disco);
                                delete_particion(path,name);
                                printf("\t### Eliminacion de particion extendida exitosa\n");
                            }
                        } else {
                            //OBTENER EBR validacion++
                            EXTD obtenerLo;

                            long anterior = 0;
                            long actualL = infoDisk.part[a].part_start;
                            //fseek(disco,actualL,SEEK_SET);
                            //fread(&obtenerLo,sizeof(EXTD),1,disco);

                            int flagB = 1;
                            //printf("%ld\n",actualL);
                            long actu = 0;
                            long sigu = 0;

                            while(flagB) {
                                fseek(disco,actualL,SEEK_SET);
                                fread(&obtenerLo,sizeof(EXTD),1,disco);

                                actu = actualL;
                                sigu = obtenerLo.part_next;
                                //printf(" %ld %ld",actualL,sigu);

                                if(obtenerLo.part_status==1 && strcmp(obtenerLo.part_name,name)==0) {
                                    //ELIMINAR
                                    printf("\t--Confirmar eliminacion (S/N): ");
                                    char respuesta;
                                    int c = 0;
                                    while(1) {
                                        scanf("%c",&respuesta);
                                        c = getchar();
                                        if(respuesta!=' ' && respuesta!='\n' && respuesta!=0)
                                            break;
                                    }
                                    if(bChar(respuesta,'s')) {
                                        if(anterior==0) {           //es el primero
                                            EXTD auxiliar;
                                            auxiliar.part_status = 0;
                                            auxiliar.part_next = sigu;
                                            fseek(disco,actualL,SEEK_SET);
                                            fwrite(&auxiliar,sizeof(EXTD),1,disco);
                                            if(deleteD==1) { //borrar completo
                                                fseek(disco,actualL+sizeof(EXTD),SEEK_SET);
                                                char borr = '\0';
                                                for(int n=0; n<obtenerLo.part_size-sizeof(EXTD); n++)
                                                    fwrite(&borr,sizeof(char),1,disco);
                                            }
                                            delete_particion(path,name);
                                            printf("\t### Eliminacion exitosa de particions logica\n");
                                        } else {                     //en medio o ultimo
                                            //printf("   %ld ",anterior);
                                            EXTD auxiliar;
                                            fseek(disco,anterior,SEEK_SET);
                                            fread(&auxiliar,sizeof(EXTD),1,disco);
                                            auxiliar.part_next = sigu;
                                            //printf(" %ld %d %s",auxiliar.proximoEBR,auxiliar.activo,auxiliar.nombre);
                                            fseek(disco,anterior,SEEK_SET);
                                            fwrite(&auxiliar,sizeof(EXTD),1,disco);
                                            //printf("%ld",obtenerLo.sizeP+sizeof(EXTD));
                                            if(deleteD==1) { //borrar completo
                                                fseek(disco,actualL,SEEK_SET);
                                                char borr = '\0';
                                                for(int n=0; n<obtenerLo.part_size; n++)
                                                    fwrite(&borr,sizeof(char),1,disco);
                                            }
                                            delete_particion(path,name);
                                            printf("\t### Eliminacion exitosa de particion logica\n");
                                        }
                                    }
                                    validacion++;
                                    flagB = 0;
                                    break;
                                }
                                if(sigu==-1) {
                                    flagB = 0;
                                    //printf("\t###Error: No se encontro la particion logica\n\n");
                                }
                                anterior = actualL;
                                actualL = sigu;
                            }
                        }
                    } else {
                        if(strcmp(infoDisk.part[a].part_name,name)==0) {
                            validacion++;
                            printf("\t--Confirmar eliminacion (S/N): ");
                            char respuesta;
                            int c = 0;
                            while(1) {
                                scanf("%c",&respuesta);
                                c = getchar();
                                if(respuesta!=' ' && respuesta!='\n' && respuesta!=0)
                                    break;
                            }
                            if(bChar(respuesta,'s')) {
                                if(deleteD==1) { //FULL
                                    char extra = '\0';
                                    fseek(disco,infoDisk.part[a].part_start,SEEK_SET);
                                    for(int c=0; c<infoDisk.part[a].part_size; c++)
                                        fwrite(&extra,sizeof(char),1,disco);
                                    //printf("full ");
                                }
                                //REACOMODACION
                                for(int b=a; b<3; b++)
                                    infoDisk.part[b] = infoDisk.part[b+1];
                                infoDisk.part[3] = reseteo;
                                infoDisk.part[3].part_status = 0;
                                fseek(disco,0,SEEK_SET);
                                fwrite(&infoDisk,sizeof(MBR),1,disco);
                                delete_particion(path,name);
                                printf("\t### Eliminacion exitosa de particion primaria\n");
                            }
                        }
                    }
                }
            }
            fclose(disco);
            if(validacion==0)
                printf("\t###Error: No existe ninguna particion con el nombre especificado\n");

        } else if(primero==2) {  //agregar o quitar espacio

            long sizeReal = 0;
            switch(unit) {
            case 'b':        //byte
                sizeReal = add;
                break;
            case 'k':         //kbyte
                sizeReal = add * 1024;
                break;
            case 'm':         //mbyte
                sizeReal = add * 1024 * 1024;
                break;
            }
            //printf("%d %d %s %d",sizeD,add,nombre,sizeReal);
            //fclose(disco);
            int validacion = validarNombreMBR(name,infoDisk,path);
            disco = fopen(path,"r+b");
            if(validacion==1) {         //si existe el nombre
                //buscar particion
                for(int nx=0; nx<4; nx++) {
                    if(infoDisk.part[nx].part_status==1) {
                        if(strcmp(infoDisk.part[nx].part_name,name)==0) {
                            long disponible  = infoDisk.part[nx].part_start + infoDisk.part[nx].part_size + sizeReal;
                            if(nx==3) {  //ultimo
                                //disponible = infoDisk.part[a].inicioP + infoDisk.part[a].sizeP + sizeReal;
                                if(disponible>infoDisk.part[nx].part_start && disponible-1<=infoDisk.mbr_tamanio) {
                                    if(infoDisk.part[nx].part_type!='e') {
                                        infoDisk.part[nx].part_size += sizeReal;
                                        fseek(disco,0,SEEK_SET);
                                        fwrite(&infoDisk,sizeof(MBR),1,disco);
                                        printf("\t### Tamaño de particion modificada\n");
                                    } else {
                                        if(sizeReal>0) {
                                            infoDisk.part[nx].part_size += sizeReal;
                                            fseek(disco,0,SEEK_SET);
                                            fwrite(&infoDisk,sizeof(MBR),1,disco);
                                            printf("\t### Tamaño de particion modificada\n");
                                        } else {
                                            fseek(disco,infoDisk.part[nx].part_start,SEEK_SET);
                                            EXTD ultima;
                                            fread(&ultima,sizeof(EXTD),1,disco);
                                            if(ultima.part_next==-1 && ultima.part_start+ultima.part_size!=infoDisk.part[nx].part_start+infoDisk.part[nx].part_size) {
                                                infoDisk.part[nx].part_size += sizeReal;
                                                fseek(disco,0,SEEK_SET);
                                                fwrite(&infoDisk,sizeof(MBR),1,disco);
                                                printf("\t### Tamaño de particion modificada\n");
                                            } else if(ultima.part_next!=-1) {
                                                while(ultima.part_next!=-1) {
                                                    fseek(disco,ultima.part_next,SEEK_SET);
                                                    fread(&ultima,sizeof(EXTD),1,disco);
                                                }
                                                if(ultima.part_start+ultima.part_size!=infoDisk.part[nx].part_start+infoDisk.part[nx].part_size) {
                                                    infoDisk.part[nx].part_size += sizeReal;
                                                    fseek(disco,0,SEEK_SET);
                                                    fwrite(&infoDisk,sizeof(MBR),1,disco);
                                                    printf("\t### Tamaño de particion modificada\n\n");
                                                } else
                                                    printf("\t###Error: No es posible modificar el tamaño de la particion\n");
                                            } else {
                                                printf("\t###Error: No es posible modificar el tamaño de la particion\n");
                                            }
                                        }
                                    }
                                } else
                                    printf("\t###Error: Se sobrepasa el limite inferior de la particion o el tamaño del disco con el nuevo valor\n");
                            } else {     //no es ultimo
                                if(infoDisk.part[nx+1].part_status==1) {  //existe una particion despues
                                    //disponible = infoDisk.part[a].inicioP + infoDisk.part[a].sizeP + sizeReal;
                                    if(disponible>infoDisk.part[nx].part_start && disponible<=infoDisk.part[nx+1].part_start) {
//----------------------------------------------------
                                        if(infoDisk.part[nx].part_type!='e') {
                                            infoDisk.part[nx].part_size += sizeReal;
                                            fseek(disco,0,SEEK_SET);
                                            fwrite(&infoDisk,sizeof(MBR),1,disco);
                                            printf("\t### Tamaño de particion modificada\n");
                                        } else {
                                            if(sizeReal>0) {
                                                infoDisk.part[nx].part_size += sizeReal;
                                                fseek(disco,0,SEEK_SET);
                                                fwrite(&infoDisk,sizeof(MBR),1,disco);
                                                printf("\t### Tamaño de particion modificada\n");
                                            } else {
                                                fseek(disco,infoDisk.part[nx].part_start,SEEK_SET);
                                                EXTD ultima;
                                                fread(&ultima,sizeof(EXTD),1,disco);
                                                if(ultima.part_next==-1 && ultima.part_start+ultima.part_size!=infoDisk.part[nx].part_start+infoDisk.part[nx].part_size) {
                                                    infoDisk.part[nx].part_size += sizeReal;
                                                    fseek(disco,0,SEEK_SET);
                                                    fwrite(&infoDisk,sizeof(MBR),1,disco);
                                                    printf("\t### Tamaño de particion modificada\n");
                                                } else if(ultima.part_next!=-1) {
                                                    while(ultima.part_next!=-1) {
                                                        fseek(disco,ultima.part_next,SEEK_SET);
                                                        fread(&ultima,sizeof(EXTD),1,disco);
                                                    }
                                                    if(ultima.part_start+ultima.part_size!=infoDisk.part[nx].part_start+infoDisk.part[nx].part_size) {
                                                        infoDisk.part[nx].part_size += sizeReal;
                                                        fseek(disco,0,SEEK_SET);
                                                        fwrite(&infoDisk,sizeof(MBR),1,disco);
                                                        printf("\t### Tamaño de particion modificada\n");
                                                    } else
                                                        printf("\t###Error: No es posible modificar el tamaño de la particion\n");
                                                } else {
                                                    printf("\t###Error: No es posible modificar el tamaño de la particion\n");
                                                }
//----------------------------------------------------
                                            }
                                        }
                                    } else
                                        printf("\t###Error: No es posible modificar el tamaño de la particion\n");
                                } else {                             //no hay particion
                                    //disponible = infoDisk.part[a].inicioP + infoDisk.part[a].sizeP + sizeReal;
                                    if(disponible>infoDisk.part[nx].part_start && disponible-1<=infoDisk.mbr_tamanio) {
//----------------------------------------------------
                                        if(infoDisk.part[nx].part_type!='e') {
                                            infoDisk.part[nx].part_size += sizeReal;
                                            fseek(disco,0,SEEK_SET);
                                            fwrite(&infoDisk,sizeof(MBR),1,disco);
                                            printf("\t### Tamaño de particion modificada\n");
                                        } else {
                                            if(sizeReal>0) {
                                                infoDisk.part[nx].part_size += sizeReal;
                                                fseek(disco,0,SEEK_SET);
                                                fwrite(&infoDisk,sizeof(MBR),1,disco);
                                                printf("\t### Tamaño de particion modificada\n");
                                            } else {
                                                fseek(disco,infoDisk.part[nx].part_start,SEEK_SET);
                                                EXTD ultima ;
                                                fread(&ultima,sizeof(EXTD),1,disco);
                                                if(ultima.part_next==-1 && ultima.part_start+ultima.part_size!=infoDisk.part[nx].part_start+infoDisk.part[nx].part_size) {
                                                    infoDisk.part[nx].part_size += sizeReal;
                                                    fseek(disco,0,SEEK_SET);
                                                    fwrite(&infoDisk,sizeof(MBR),1,disco);
                                                    printf("\t### Tamaño de particion modificada\n");
                                                } else if(ultima.part_next!=-1) {
                                                    while(ultima.part_next!=-1) {
                                                        fseek(disco,ultima.part_next,SEEK_SET);
                                                        fread(&ultima,sizeof(EXTD),1,disco);
                                                    }
                                                    if(ultima.part_start+ultima.part_size!=infoDisk.part[nx].part_start+infoDisk.part[nx].part_size) {
                                                        infoDisk.part[nx].part_size += sizeReal;
                                                        fseek(disco,0,SEEK_SET);
                                                        fwrite(&infoDisk,sizeof(MBR),1,disco);
                                                        printf("\t### Tamaño de particion modificada\n");
                                                    } else
                                                        printf("\t###Error: No es posible modificar el tamaño de la particion\n");
                                                } else {
                                                    printf("\t###Error: No es posible modificar el tamaño de la particion\n");
                                                }
                                            }
                                        }
                                    } else
                                        printf("\t###Error: Se sobrepasa el limite inferior de la particion o el tamaño del disco con el nuevo valor\n");
//----------------------------------------------------
                                }
                            }
                            break;
                        }
                        if(infoDisk.part[nx].part_type=='e') {
                            //OBTENER EBR
                            EXTD obtenerLo;

                            long anterior = 0;
                            long actualL = infoDisk.part[nx].part_start;

                            int flagB = 1;
                            long actu = 0;
                            long sigu = 0;
                            int valval = 0;

                            while(flagB) {
                                fseek(disco,actualL,SEEK_SET);
                                fread(&obtenerLo,sizeof(EXTD),1,disco);

                                actu = actualL;
                                sigu = obtenerLo.part_next;

                                if(obtenerLo.part_status==1 && strcmp(obtenerLo.part_name,name)==0) {
                                    valval++;
                                    //Modificar
                                    long disponible  = obtenerLo.part_start + obtenerLo.part_size + sizeReal;
                                    if(obtenerLo.part_next==-1) {  //es ultimo
                                        if(disponible>obtenerLo.part_start && disponible<=infoDisk.part[nx].part_start+infoDisk.part[nx].part_size) {
                                            obtenerLo.part_size += sizeReal;
                                            fseek(disco,actualL,SEEK_SET);
                                            fwrite(&obtenerLo,sizeof(EXTD),1,disco);
                                            printf("\t### Tamaño de particion Logica modificada\n");
                                        } else
                                            printf("\t###Error: Se sobrepasa el limite inferior de la particion logica o el tamaño de la particion extendida\n");
                                    } else {                         //hay mas
                                        if(disponible>obtenerLo.part_start && disponible<=obtenerLo.part_next) {
                                            obtenerLo.part_size += sizeReal;
                                            fseek(disco,actualL,SEEK_SET);
                                            fwrite(&obtenerLo,sizeof(EXTD),1,disco);
                                            printf("\t### Tamaño de particion Logica modificada\n");
                                        } else
                                            printf("\t###Error: No es posible modificar el tamaño de la particion logica\n");
                                    }
                                    flagB = 0;
                                    break;
                                }
                                if(sigu==-1) {
                                    flagB = 0;
                                }
                                anterior = actualL;
                                actualL = sigu;
                            }
                            if(valval==1)
                                break;
                        }
                    }
                }
            } else
                printf("\t###Error: No existe particion con el nombre indicado\n");
            fclose(disco);

        } else {                 //crear

            int validar = 0;
            int valNom = validarNombreMBR(name,infoDisk,path);
            if((type=='p' && primarias(infoDisk)<3) || (type=='e' && extendida(infoDisk)==0)) { //PRIMARIAS Y EXTENDIDAS
                if(valNom==0) {
                    if(sizeD<infoDisk.mbr_tamanio-sizeof(MBR)) { //la particion es menor al tamaño disponible
                        long auxxx = encontrar_posicion(infoDisk.disk_fit,path,sizeD,type);
                        if(auxxx>0) {
                            //encontrar posicion disponible
                            int posicion = 0;
                            for(int a=0; a<4; a++) {
                                if(infoDisk.part[a].part_status!=1) {
                                    posicion = a;
                                    break;
                                }
                            }
                            //llenar datos
                            infoDisk.part[posicion].part_status = 1;
                            infoDisk.part[posicion].part_type = type;
                            infoDisk.part[posicion].part_fit = fit;
                            infoDisk.part[posicion].part_start = auxxx;
                            infoDisk.part[posicion].part_size = sizeD;
                            strcpy(infoDisk.part[posicion].part_name,name);
                            //agregar ebr
                            if(type=='e') {
                                disco = fopen(path,"r+b");
                                EXTD logica;
                                logica.part_status = 0;
                                logica.part_next = -1;
                                fseek(disco,infoDisk.part[posicion].part_start,SEEK_SET);
                                fwrite(&logica,sizeof(EXTD),1,disco);
                                fclose(disco);
                            }
                            //reordenar
                            PRT aux;
                            for(int c=0; c<3; c++) {
                                for(int d=c+1; d<4; d++) {
                                    if(infoDisk.part[c].part_status==1 && infoDisk.part[d].part_status==1) {
                                        if(infoDisk.part[c].part_start>infoDisk.part[d].part_start) {
                                            aux = infoDisk.part[c];
                                            infoDisk.part[c] = infoDisk.part[d];
                                            infoDisk.part[d] = aux;
                                        }
                                    }
                                }
                            }
                            //sobre escribir
                            disco = fopen(path,"r+b");
                            fseek(disco,0,SEEK_SET);
                            fwrite(&infoDisk,sizeof(MBR),1,disco);
                            fclose(disco);
                            printf("\t### Particion %c creada\n",type);
                        } else
                            printf("\t###Error: No hay espacio disponible\n");
                    } else
                        printf("\t###Error: La particion es mayor al tamanio del disco\n");
                } else
                    printf("\t###Error: Ya existe una particion con el mismo nombre\n");
            } else if(type=='l') {   //logicas

                if(valNom==0) {
                    long auxxx = encontrar_posicion(infoDisk.disk_fit,path,sizeD,type);
                    if(auxxx==-2){
                        printf("\t###Error: No existe particion extendida\n");
                    }else if(auxxx>0) {
                        //llenar datos
                        EXTD nuevoLogic;
                        nuevoLogic.part_fit = fit;
                        strcpy(nuevoLogic.part_name,name);
                        nuevoLogic.part_next = -1;
                        nuevoLogic.part_size = sizeD;
                        nuevoLogic.part_start = auxxx;
                        nuevoLogic.part_status = 1;
                        //obtener primer ebr
                        int pos_ext = 0;
                        EXTD logica;
                        for(int a=0; a<4; a++) {
                            if(infoDisk.part[a].part_status==1 && infoDisk.part[a].part_type=='e') {
                                pos_ext = infoDisk.part[a].part_start;
                                break;
                            }
                        }

                        //ver donde colocar
                        disco = fopen(path,"r+b");
                        fseek(disco,pos_ext,SEEK_SET);
                        fread(&logica,sizeof(EXTD),1,disco);
                        fclose(disco);

                        if(logica.part_status==0 && logica.part_next==-1) { //no hay logicas
                            disco = fopen(path,"r+b");
                            fseek(disco,pos_ext,SEEK_SET);
                            fwrite(&nuevoLogic,sizeof(EXTD),1,disco);
                            fclose(disco);
                            printf("\t###Particion logica creada1\n");
                        } else if(logica.part_status!=0 && logica.part_next==-1) { //solo hay una, al inicio
                            //sobreescribir actual
                            logica.part_next = nuevoLogic.part_start;
                            disco = fopen(path,"r+b");
                            fseek(disco,pos_ext,SEEK_SET);
                            fwrite(&logica,sizeof(EXTD),1,disco);
                            //escribir siguiente ebr
                            fseek(disco,logica.part_next,SEEK_SET);
                            fwrite(&nuevoLogic,sizeof(EXTD),1,disco);
                            fclose(disco);
                            printf("\t###Particion logica creada2\n");
                        } else { //recorrer
                            if(logica.part_status==0 && logica.part_next!=-1) { //hay un espacio al inicio
                                if(logica.part_next>nuevoLogic.part_start) {
                                    nuevoLogic.part_next = logica.part_next;
                                    disco = fopen(path,"r+b");
                                    fseek(disco,pos_ext,SEEK_SET);
                                    fwrite(&nuevoLogic,sizeof(EXTD),1,disco);
                                    fclose(disco);
                                    printf("\t###Particion logica creada3\n");
                                    return 0;
                                }
                                disco = fopen(path,"r+b");
                                fseek(disco,logica.part_next,SEEK_SET);
                                fread(&logica,sizeof(EXTD),1,disco);
                                fclose(disco);
                            }
                            //hay actual y siguiente
                            while(logica.part_next!=-1) {
                                if(logica.part_next>nuevoLogic.part_start) { //si cabe, hay espacio disponible
                                    //sobreescribir actual
                                    nuevoLogic.part_next = logica.part_next;
                                    logica.part_next = nuevoLogic.part_start;
                                    disco = fopen(path,"r+b");
                                    fseek(disco,logica.part_start,SEEK_SET);
                                    fwrite(&logica,sizeof(EXTD),1,disco);
                                    //escribir siguiente ebr
                                    fseek(disco,logica.part_next,SEEK_SET);
                                    fwrite(&nuevoLogic,sizeof(EXTD),1,disco);
                                    fclose(disco);
                                    printf("\t###Particion logica creada4\n");
                                    return 0;
                                }
                                disco = fopen(path,"r+b");
                                fseek(disco,logica.part_next,SEEK_SET);
                                fread(&logica,sizeof(EXTD),1,disco);
                                fclose(disco);
                            }
                            //insertar al final
                            //sobreescribir actual
                            logica.part_next = nuevoLogic.part_start;
                            disco = fopen(path,"r+b");
                            fseek(disco,logica.part_start,SEEK_SET);
                            fwrite(&logica,sizeof(EXTD),1,disco);
                            //escribir siguiente ebr
                            fseek(disco,logica.part_next,SEEK_SET);
                            fwrite(&nuevoLogic,sizeof(EXTD),1,disco);
                            fclose(disco);
                            printf("\t###Particion logica creada5\n");
                        }


                    } else
                        printf("\t###Error: No hay espacio disponible\n");
                } else
                    printf("\t###Error: Ya existe una particion con el mismo nombre\n");

            } else
                printf("\t###Error: No se permite mas particiones %c\n",type);
        }

    } else
        printf("\t###Error: No se pudo acceder al archivo\n");
    return 0;
}

#endif // DISCO_201403877_H
