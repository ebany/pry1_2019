#ifndef REPORTE_201403877_H
#define REPORTE_201403877_H

#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "[MIA]disco_201403877.h"

void crearDisk(char* rutaDisco,char* rutaImg) {
    FILE* disco;
    disco = fopen(rutaDisco,"r+b");
    if(disco!=NULL) {
        MBR master;
        fseek(disco, 0, SEEK_SET);
        fread(&master,sizeof(MBR),1,disco);
        fclose(disco);

        FILE* archivo;
        archivo = fopen("/home/nikola/Documentos/img.dot","w+");
        fclose(archivo);

        archivo = fopen("/home/nikola/Documentos/img.dot","a+");
        fputs("digraph structs {\n node[shape=record];\n",archivo);
        fputs("struct1 [shape=record,label=\"{MBR|Tamanio= ",archivo);
        //fputs("\n}",archivo);
        char texto[100];
        sprintf(texto,"%d Bytes",master.mbr_tamanio);
        fputs(texto,archivo);
        fputs("\\lFecha= ",archivo);
        struct tm *hora;
        hora = localtime(&master.mbr_fecha_creacion);
        sprintf(texto,"%d-%d-2018\\ldisk_fit= %c\\l}",hora->tm_mday,hora->tm_mon+1,master.disk_fit);
        fputs(texto,archivo);
        fclose(archivo);


        int vacio = -1;
        for(int a=0; a<4; a++) {
            char text[1000];
            if(master.part[a].part_status==1) {
                vacio = a;
                //partes vacias
                if(a==0) {
                    if(master.part[a].part_start>sizeof(MBR)) {
                        archivo = fopen("/home/nikola/Documentos/img.dot","a+");
                        float size_free = master.part[a].part_start-sizeof(MBR);
                        float size_mbr = master.mbr_tamanio;
                        float result = size_free*100/size_mbr;
                        sprintf(text,"|{LIBRE|Size= %d Bytes\\l%.2f%}",(int)size_free,result);
                        fputs(text,archivo);
                        fclose(archivo);
                    }
                } else if(a>0) {
                    if(master.part[a-1].part_start+master.part[a-1].part_size<master.part[a].part_start) {
                        archivo = fopen("/home/nikola/Documentos/img.dot","a+");
                        float size_free = master.part[a].part_start-(master.part[a-1].part_start+master.part[a-1].part_size);
                        float size_mbr = master.mbr_tamanio;
                        float result = size_free*100/size_mbr;
                        sprintf(text,"|{LIBRE|Size= %d Bytes\\l%.2f%}",(int)size_free,result);
                        fputs(text,archivo);
                        fclose(archivo);
                    }
                }
                //particiones
                if(master.part[a].part_type=='p') {   //PRIMARIA
                    archivo = fopen("/home/nikola/Documentos/img.dot","a+");
                    float size_mbr = master.mbr_tamanio;
                    float size_prt = master.part[a].part_size;
                    float size_part = size_prt*100/size_mbr;
                    sprintf(text,"|{%s/Primaria|Size= %d Bytes\\l%.2f%}",master.part[a].part_name,master.part[a].part_size,size_part);
                    fputs(text,archivo);
                    fclose(archivo);
                } else {                         //EXTENDIDA
                    //POSICIONARSE AL INICIO DE LA PARTICION
                    //OBTENER EBR
                    float size_part = 0;

                    float aux_size = 0;

                    if(master.part[a].part_type=='e') {   //PRIMARIA
                        archivo = fopen("/home/nikola/Documentos/img.dot","a+");
                        float size_mbr = master.mbr_tamanio;
                        float size_prt = master.part[a].part_size;
                        aux_size = size_prt;
                        size_part = size_prt*100/size_mbr;
                        sprintf(text,"|{%s/Extendida|Size= %d Bytes\\lAjsute= %c\\l%.2f%|{",master.part[a].part_name,master.part[a].part_size,master.part[a].part_fit,size_part);
                        fputs(text,archivo);
                        fclose(archivo);
                    }
                    //FILE* disco;
                    EXTD auxiliar;
                    long actual = master.part[a].part_start;
                    int flag = 1;
                    int auxContt = 0;
                    while(flag) {
                        disco = fopen(rutaDisco,"r+b");
                        fseek(disco,actual,SEEK_SET);
                        fread(&auxiliar,sizeof(EXTD),1,disco);
                        fclose(disco);

                        if(auxiliar.part_next!=-1) {
                            if(auxiliar.part_status==1) {
                                archivo = fopen("/home/nikola/Documentos/img.dot","a+");
                                float part_size =auxiliar.part_size*100;
                                float size_mbr = master.mbr_tamanio;
                                float result = part_size/aux_size;
                                sprintf(text,"{E\\lB\\lR\\l}|{%s/Logica|Size= %d Bytes\\l%.2f%}|",auxiliar.part_name,auxiliar.part_size,result);
                                fputs(text,archivo);
                                fclose(archivo);
                                if(auxiliar.part_start + auxiliar.part_size<auxiliar.part_next) {
                                    char text1[1000];
                                    archivo = fopen("/home/nikola/Documentos/img.dot","a+");
                                    float size_free = auxiliar.part_next-(auxiliar.part_start + auxiliar.part_size);
                                    sprintf(text1,"{Libre|Size= %d Bytes\\l%.2f%}|",(int)size_free,size_free*100/aux_size);
                                    fputs(text1,archivo);
                                    fclose(archivo);
                                }
                            } else {
                                archivo = fopen("/home/nikola/Documentos/img.dot","a+");
                                float size_free = auxiliar.part_next-(master.part[a].part_start+sizeof(EXTD));
                                sprintf(text,"{E\\lB\\lR\\l}|{Libre|Size= %d Bytes\\l%.2f%}|",(int)size_free,size_free*100/aux_size);
                                fputs(text,archivo);
                                fclose(archivo);
                            }
                        } else {
                            if(auxiliar.part_status==1) {
                                archivo = fopen("/home/nikola/Documentos/img.dot","a+");
                                float part_size =auxiliar.part_size;
                                sprintf(text,"{E\\lB\\lR\\l}|{%s/Logica|Size= %d Bytes\\l%.2f%}|",auxiliar.part_name,auxiliar.part_size,part_size*100/aux_size);
                                fputs(text,archivo);
                                fclose(archivo);
                                if(auxiliar.part_start + auxiliar.part_size<master.part[a].part_start+master.part[a].part_size) {
                                    char text1[1000];
                                    archivo = fopen("/home/nikola/Documentos/img.dot","a+");
                                    float free_size = master.part[a].part_start+master.part[a].part_size-(auxiliar.part_start + auxiliar.part_size);
                                    sprintf(text1,"{Libre|Size= %d Bytes\\l%.2f%}",(int)free_size,free_size*100/aux_size);
                                    fputs(text1,archivo);
                                    fclose(archivo);
                                }
                            }
                        }

                        if(auxiliar.part_next==-1) {
                            flag=0;
                            if(auxContt!=0) {
                                archivo = fopen("/home/nikola/Documentos/img.dot","a+");
                                //fputs("}",archivo);
                                fclose(archivo);
                            }
                        }

                        actual = auxiliar.part_next;
                        auxContt++;
                    }

                    archivo = fopen("/home/nikola/Documentos/img.dot","a+");
                    sprintf(text,"}}");
                    fputs(text,archivo);
                    //fputs("}}",archivo);
                    fclose(archivo);
                }
            }
        }
        char text[1000];
        if(vacio!=-1) {
            if(master.mbr_tamanio>master.part[vacio].part_start+master.part[vacio].part_size) {
                archivo = fopen("/home/nikola/Documentos/img.dot","a+");
                float size_free = master.mbr_tamanio-(master.part[vacio].part_start+master.part[vacio].part_size);
                float size_mbr = master.mbr_tamanio;
                float result = size_free*100/size_mbr;
                sprintf(text,"|{LIBRE|Size= %d Bytes\\n%.2f%}",(int)size_free,result);
                fputs(text,archivo);
                fclose(archivo);
            }

        } else {
            archivo = fopen("/home/nikola/Documentos/img.dot","a+");
            float size_free = master.mbr_tamanio-sizeof(MBR);
            float size_mbr = master.mbr_tamanio;
            float result = size_free*100/size_mbr;
            sprintf(text,"|{LIBRE|Size= %d Bytes\\n%.2f%}",(int)size_free,result);
            fputs(text,archivo);
            fclose(archivo);
        }
        archivo = fopen("/home/nikola/Documentos/img.dot","a+");
        fputs("\"];\n}",archivo);
        fclose(archivo);

        char comando[1000];
        sprintf(comando,"dot -Tjpg /home/nikola/Documentos/img.dot -o \"%s\" &",rutaImg);
        system(comando);
        sleep(2);
        char comando2[1000];
        sprintf(comando2,"nohup display \"%s\" >/dev/null 2>&1&",rutaImg);
        system(comando2);
    } else
        printf("\t###Error: no se pudo generar el reporte\n");
}

void crearEBR(char* rutaDisco,char* rutaImg) {
    FILE* disco;
    disco = fopen(rutaDisco,"r+b");
    if(disco!=NULL) {
        MBR master;
        fseek(disco, 0, SEEK_SET);
        fread(&master,sizeof(MBR),1,disco);
        fclose(disco);

        FILE* archivo;
        archivo = fopen("/home/nikola/Documentos/img.dot","w+");
        fclose(archivo);

        archivo = fopen("/home/nikola/Documentos/img.dot","a+");
        fputs("digraph structs {\n node[shape=record];\n",archivo);
        fputs("struct1n [shape=record,label=\"{MBR|mbr_tamanio= ",archivo);
        char texto[100];
        sprintf(texto,"%d\\l",master.mbr_tamanio);
        fputs(texto,archivo);
        fputs("disk_fit= ",archivo);
        sprintf(texto,"%c\\l",master.disk_fit);
        fputs(texto,archivo);
        fputs("mbr_fecha_creacion= ",archivo);
        struct tm *hora;
        hora = localtime(&master.mbr_fecha_creacion);
        sprintf(texto,"%d/%d/18  %d:%d\\lmbr_disk_signature = %d\\l",hora->tm_mday,hora->tm_mon+1,hora->tm_hour,hora->tm_min,master.mbr_disk_signature);
        fputs(texto,archivo);
        fclose(archivo);

        int posEBR = -1;
        for(int a=0; a<4; a++) {
            char text[1000];
            if(master.part[a].part_status==1) {
                if(master.part[a].part_type=='p') {
                    archivo = fopen("/home/nikola/Documentos/img.dot","a+");
                    sprintf(text,"\\l\\lpart_status_%d= 1\\lpart_type_%d= p\\lpart_fit_%d= %c\\lpart_size_%d= %d\\lpart_start_%d= %d\\lpart_name_%d= %s\\l",a,a,a,master.part[a].part_fit,a,master.part[a].part_size,a,master.part[a].part_start,a,master.part[a].part_name);
                    fputs(text,archivo);
                    fclose(archivo);
                } else {
                    posEBR = a;

                    archivo = fopen("/home/nikola/Documentos/img.dot","a+");
                    sprintf(text,"\\l\\lpart_status_%d= 1\\lpart_type_%d= e\\lpart_fit_%d= %c\\lpart_size_%d= %d\\lpart_start_%d= %d\\lpart_name_%d= %s\\l",a,a,a,master.part[a].part_fit,a,master.part[a].part_size,a,master.part[a].part_start,a,master.part[a].part_name);
                    fputs(text,archivo);
                    fclose(archivo);
                }
            }
        }

        archivo = fopen("/home/nikola/Documentos/img.dot","a+");
        fputs("\\l}\"];\n",archivo);
        fclose(archivo);

        //printf("%d\n",posEBR);
        if(posEBR!=-1) {
            char text[1000];
            EXTD auxiliar;
            long actual = master.part[posEBR].part_start;
            int flag = 1;
            int contador = 0;
            while(flag) {
                disco = fopen(rutaDisco,"r+b");
                fseek(disco,actual,SEEK_SET);
                fread(&auxiliar,sizeof(EXTD),1,disco);
                fclose(disco);
                if(auxiliar.part_status==1) {
                    archivo = fopen("/home/nikola/Documentos/img.dot","a+");
                    sprintf(text,"struct%d [shape=record,label=\"{EBR%d|part_status_%d= 1\\lpart_fit_%d= %c\\lpart_start_%d= %d\\lpart_size_%d= %d\\lpart_next_%d= %d\\lpart_name_%d= %s\\l}\"];\n",contador,contador,contador,contador,auxiliar.part_fit,contador,auxiliar.part_start,contador,auxiliar.part_size,contador,auxiliar.part_next,contador,auxiliar.part_name);
                    fputs(text,archivo);
                    fclose(archivo);
                }
                if(auxiliar.part_next==-1)
                    flag=0;
                actual = auxiliar.part_next;
                contador++;
            }
        }

        archivo = fopen("/home/nikola/Documentos/img.dot","a+");
        fputs("\n}",archivo);
        fclose(archivo);

        char comando[1000];
        sprintf(comando,"dot -Tjpg /home/nikola/Documentos/img.dot -o \"%s\" &",rutaImg);
        system(comando);
        sleep(2);
        char comando2[1000];
        sprintf(comando2,"nohup display \"%s\" >/dev/null 2>&1&",rutaImg);
        system(comando2);
    } else
        printf("\t###Error: no se pudo generar el reporte\n");
}

void reportes(char* rutaImg,char* rutaDisco,char* ruta_carpeta,int report,char* name) {

    struct stat datosFichero;
    if(stat(rutaImg,&datosFichero)==-1) {
        char comando0[1000];
        memset(comando0, 0, sizeof(comando0));
        strcat(comando0,"mkdir -p \"");
        strcat(comando0,ruta_carpeta);
        strcat(comando0,"\"");
        system(comando0);
    }
    switch(report) {
    case 1:
        crearDisk(rutaDisco,rutaImg);
        break;
    case 2:
        crearEBR(rutaDisco,rutaImg);
        break;
    case 3:
        //crearSB(rutaDisco,rutaImg,name);
        break;
    case 4: //bm_inodo
        //crearBM(rutaDisco,rutaImg,name,4);
        break;
    case 5://bm_block
        //crearBM(rutaDisco,rutaImg,name,5);
        break;
    }
}

#endif // REPORTE_201403877_H
