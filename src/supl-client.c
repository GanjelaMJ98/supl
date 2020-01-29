/*
** SUPL clinet cli code
**
** Copyright (c) 2007 Tatu Mannisto <tatu a-t tajuma d-o-t com>
** All rights reserved.
** Redistribution and modifications are permitted subject to BSD license.
**
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <getopt.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>

#include <stdint.h>

#include "supl.h"

typedef enum { FORMAT_DEFAULT, FORMAT_HUMAN } format_t;

static struct fake_pos_s {
  int valid;
  double lat, lon;
  int uncertainty;
} fake_pos = {
  0, 0.0, 0.0, 0
};

static time_t utc_time(int week, long tow) {
  time_t t;

  /* Jan 5/6 midnight 1980 - beginning of GPS time as Unix time */
  t = 315964801;

  /* soon week will wrap again, uh oh... */
  /* TS 44.031: GPSTOW, range 0-604799.92, resolution 0.08 sec, 23-bit presentation */
  t += (1024 + week) * 604800 + tow*0.08;

  return t;
} 
char* d2(int x){
  char buff[10];
  char strip[45];
  sprintf(buff,"%x",x);
  
  sprintf(strip,"%.*s", 4, buff + 4);
  return strip;
}





void message_alm(supl_assist_t *ctx){
  FILE * logFile;
  time_t t = time(NULL);
  struct tm* aTm = localtime(&t);
  char fileName[40];
  sprintf(fileName,"messages/log_%02d-%02d__%02d_%02d_%02d.txt", aTm->tm_mon+1, aTm->tm_mday, aTm->tm_hour, aTm->tm_min, aTm->tm_sec);
  //printf("%s\n", fileName);
  
  if((logFile=fopen(fileName,"w")) == NULL) {
      printf("Cannot open directory file.");
      exit(1);
  }



  printf("MESSAGE ALM:\n");
  for (int i = 0; i < ctx->cnt_alm; i++) {
    struct supl_almanac_s *a = &ctx->alm[i];
    struct supl_ephemeris_s *e = &ctx->eph[i];
    char buff[150];
    char hexstring[150];

    //D2
    char buff_omega[10];
    sprintf(buff_omega,"%X",a->OMEGA_dot);
    sprintf(buff_omega,"%.*s", 4, buff_omega + 4);
    //D1
    char buff_idot[10];
    sprintf(buff_idot,"%X",e->i_dot);
    if(strlen(buff_idot) > 4){
      sprintf(buff_idot,"%s", buff_idot + 4);
    }else{
      sprintf(buff_idot,"%04X",e->i_dot);
    }
    //D5
    char buff_w[10];
    sprintf(buff_w,"%X",a->w);
    if(strlen(buff_w) > 6){
      sprintf(buff_w,"%s", buff_w + 2);
    }else{
      sprintf(buff_w,"%06X", a->w);
    }
    //D6
    char buff_M0[10];
    sprintf(buff_M0,"%X",a->M0);
    if(strlen(buff_M0) > 6){
      sprintf(buff_M0,"%s", buff_M0 + 2);
    }else{
      sprintf(buff_M0,"%06X", a->M0);
    }
    //D4
    char buff_O0[10];
    sprintf(buff_O0,"%X",a->OMEGA_0);
    if(strlen(buff_O0) > 6){
      sprintf(buff_O0,"%s", buff_O0 + 2);
    }else{
      sprintf(buff_O0,"%06X", a->OMEGA_0);
    }


    char buff_AF1[10];
    sprintf(buff_AF1,"%X",a->AF1);
    if(strlen(buff_AF1) > 6){
      sprintf(buff_AF1,"%s", buff_AF1 + 2);
    }else{
      sprintf(buff_AF1,"%06X", a->AF1);
    }
    //printf("%s\n", buff_AF1);
    char buff_AF0[10];
    sprintf(buff_AF0,"%X",a->AF0);
    if(strlen(buff_AF0) > 6){
      sprintf(buff_AF0,"%s", buff_AF0 + 2);
    }else{
      sprintf(buff_AF0,"%06X", a->AF0);
    }
   // printf("%s\n", buff_AF0);
    sprintf(buff, "$PMTK711,%02x,%02X,%X%04X,%X%s,%s00,%X,%06s,%s,%s,%c%c%c%c%c%c",
    //sprintf(buff, "$PMTK711,%02x,%04X,%X%04X,%X%s,%s00,%X,%06s,%s,%s,%c%c%c%c%c%c          %x/%x",
                        a->prn,                     //SV      ok
                        ctx->time.gps_week,         //WEEK    ok
                        64 + a->prn, a->e,          //DO      ok
                        a->toa,buff_idot,           //D1      ------
                        buff_omega,                 //D2      ok
                        a->A_sqrt,                  //D3      ok
                        buff_O0,                    //D4      ok
                        buff_w,                     //D5      ok
                        buff_M0,                    //D6      ok


                        buff_AF0[3],buff_AF0[4],buff_AF1[3],buff_AF1[4],buff_AF1[5],buff_AF0[5]);

                        //a->AF0, a->AF1);            //D7      +-*/
    //printf("%s\n", buff);
  /*  sprintf(hexstring, "B5620B3028%02X%04X%X%04X%X%s%s00%X%06s%s%s%c%c%c%c%c%c0000",
                        a->prn,
                        1024 + ctx->time.gps_week,
                        64 + a->prn, a->e,
                                  //DO      ok
                        a->toa,buff_idot,           //D1      ------
                        buff_omega,                 //D2      ok
                        a->A_sqrt,                  //D3      ok
                        buff_O0,                    //D4      ok
                        buff_w,                     //D5      ok
                        buff_M0,                    //D6      ok
                        buff_AF0[3],buff_AF0[4],buff_AF1[3],buff_AF1[4],buff_AF1[5],buff_AF0[5]);            //D7      +-*/
    //printf("%s\n", hexstring);
    //printf("%d\n", strlen(hexstring));
    printf("%s\n", buff);
    
    int i;
    unsigned char bytearray[34];
    uint8_t str_len = strlen(hexstring);


    for (i = 0; i < (str_len / 2); i++) {
        sscanf(hexstring + 2*i, "%02x", &bytearray[i]);
        //printf("0x%02X,", bytearray[i]);
    }
    int8_t CK_A = 0, CK_B = 0;
    for( uint8_t i = 2; i < (sizeof(bytearray) - 2) ; i++ )
    {
      CK_A = CK_A + bytearray[i];
      CK_B = CK_B + CK_A;
    }

  CK_A &= 0xFF;
  CK_B &= 0xFF;

  bytearray[32] = CK_A;
  bytearray[33] = CK_B;

  for(int i = 0; i<sizeof(bytearray);i++){
    fprintf(logFile,"0x%02x,", bytearray[i]);
  }
  fprintf(logFile,"\n");
  }
}

/*
    sprintf(buff, "$PMTK710,%02X,000000,000000,000000,000000,000000,%02X%X,00%s,000000,%02X%s,%X,%X, %X ,%X,%X,%X,%X00,%X,%X,%X,%X,%X,%X,%X",
                        e->prn, 
                        //
                        //
                        //
                        //
                        //
                        e->IODC, 
                        e->toc,
                        
                        buff_AF1,
                        //e->AF1,
                        e->IODC, 
                        buff_Crs,
                        //e->Crs,
                        
                        e->delta_n, 
                        e->M0,
                        e->Cuc,
                        e->e,
                        e->Cus,
                        e->A_sqrt,

                        e->toe,
                        e->Cic,
                        e->OMEGA_0,
                        e->Cis,
                        e->i0,
                        e->Crc,
                        e->w,
                        e->OMEGA_dot
                        );
    printf("%s\n", buff);
    }
*/

void message_eph(supl_assist_t *ctx){
   printf("MESSAGE EPH:\n");
    
    
    for(int i = 0; i < ctx->cnt_eph; i++) {
    //for(int i = 0; i < 2; i++) {
    char buff[200] = "";
    
    struct supl_almanac_s *a = &ctx->alm[i];
    struct supl_ephemeris_s *e = &ctx->eph[i];
    

    char buff_IODC[20];
    sprintf(buff_IODC,"%X",e->IODC);
    if(strlen(buff_IODC) > 2){
      sprintf(buff_IODC,"%s", buff_IODC + 1);
    }else{
      sprintf(buff_IODC,"%02X", e->IODC);
    }

    char buff_Crs[20];
    sprintf(buff_Crs,"%X",e->Crs);
    if(strlen(buff_Crs) > 6){
      sprintf(buff_Crs,"%s", buff_Crs + 4);
    }else{
      sprintf(buff_Crs,"%04X", e->Crs);
    }

    char buff_AF1[20];
    sprintf(buff_AF1,"%X",e->AF1);
    if(strlen( buff_AF1) > 6){
      sprintf( buff_AF1,"%s",  buff_AF1 + 4);
    }else{
      sprintf( buff_AF1,"%04X", e->AF1);
    }

    char buff_AF0[20];
    sprintf(buff_AF0,"%X",e->AF0);
    if(strlen( buff_AF0) > 6){
      sprintf( buff_AF0,"%s",  buff_AF0 + 4);
    }else{
      sprintf( buff_AF0,"%04X", e->AF0);
    }

    char buff_Cuc[20];
    sprintf(buff_Cuc,"%X",e->Cuc);
    if(strlen( buff_Cuc) > 6){
      sprintf( buff_Cuc,"%s",  buff_Cuc + 4);
    }else{
      sprintf( buff_Cuc,"%04X", e->Cuc);
    }

    char buff_Cic[20];
    sprintf(buff_Cic,"%X",e->Cic);
    if(strlen( buff_Cic) > 4){
      sprintf( buff_Cic,"%s",  buff_Cic + 4);
    }else{
      sprintf( buff_Cic,"%04X", e->Cic);
    }

    char buff_Cis[20];
    sprintf(buff_Cis,"%X",e->Cis);
    if(strlen( buff_Cis) > 4){
      sprintf( buff_Cis,"%s",  buff_Cis + 4);
    }else{
      sprintf( buff_Cis,"%04X", e->Cis);
    }
    char buff_OD[20];
    sprintf(buff_OD,"%X",e->OMEGA_dot);
    if(strlen( buff_OD) > 4)
      sprintf( buff_OD,"%s",  buff_OD + 2);

    char buff_tgd[20];
    sprintf(buff_tgd,"%X",e->tgd);
    if(strlen( buff_tgd) > 2)
      sprintf( buff_tgd,"0000%s",  buff_tgd + 6);
    else
      sprintf(buff_tgd,"%06X",e->tgd);
   
   printf("%X\n", e->OMEGA_0);
   printf("%d\n", e->OMEGA_0);
   
    /*printf("%X\n", e->fill1);
    printf("%X\n", e->e);
    printf("%X\n", e->i_dot);
    printf("%X\n", e->AF2);
    printf("%X\n", e->bits);
    printf("%X\n", e->ura);
    printf("%X\n", e->tgd);
    printf("%X\n", e->AODA);*/                            //AF0
    //sprintf(buff, "000000,000000,000000,000000,00000,%03X%X,00%s,00000,%03X%s,%X,%08X,%s0,%X,%04X,%X,%X00,%s,%08X,%s,%X,%X,%X,%s",
    sprintf(buff, "000000000000000000000000%06s%s%X00%s00000%03X%s%X%08X%s0%07X%04X%X%X00%s%08X%s%X%X%08X%s%s\0",
                        
                        //
                        //
                        //e->i_dot,
                        //
                        buff_tgd,
                        buff_IODC, 
                        e->toc,
                        
                        buff_AF1,
                        //e->AF1,
                        e->IODC, 
                        buff_Crs,
                        //e->Crs,
                        
                        e->delta_n,
                        //buff_dn,
                        e->M0,
                        buff_Cuc,
                        //e->Cuc,
                        e->e,
                        e->Cus,
                        e->A_sqrt,

                        e->toe,
                        //e->Cic,
                        buff_Cic,
                        e->OMEGA_0,
                        //e->Cis,
                        buff_Cis,
                        e->i0,
                        e->Crc,
                        e->w,
                        //e->OMEGA_dot
                        buff_OD,
                        buff_IODC
                        );

    
    int position = 7;
    int c = 0;
    while(position < strlen(buff)){
      for (c = strlen(buff) - 1; c >= position - 1; c--){
        buff[c+1] = buff[c];
      }  
      buff[position-1] = ',';
      position = position+7; 
    }
    char message[50]="";
    sprintf(message, "$PMTK710,%02x,", e->prn);
    printf("%s", message);
    printf("%s\n", buff);
  }
 } 



static int supl_consume_1(supl_assist_t *ctx) {
  if (ctx->set & SUPL_RRLP_ASSIST_REFLOC) {
    fprintf(stdout, "Reference Location:\n");
    fprintf(stdout, "  Lat: %f\n", ctx->pos.lat);
    fprintf(stdout, "  Lon: %f\n", ctx->pos.lon);
    fprintf(stdout, "  Uncertainty: %d (%.1f m)\n", 
	    ctx->pos.uncertainty, 10.0*(pow(1.1, ctx->pos.uncertainty)-1));
  }

  if (ctx->set & SUPL_RRLP_ASSIST_REFTIME) {
    time_t t;

    t = utc_time(ctx->time.gps_week, ctx->time.gps_tow);

    fprintf(stdout, "Reference Time:\n");
    fprintf(stdout, "  GPS Week: %ld\n", ctx->time.gps_week);
    fprintf(stdout, "  GPS TOW:  %ld %lf\n", ctx->time.gps_tow, ctx->time.gps_tow*0.08);
    fprintf(stdout, "  ~ UTC:    %s", ctime(&t));
  }

  if (ctx->set & SUPL_RRLP_ASSIST_IONO) {
    fprintf(stdout, "Ionospheric Model:\n");
    fprintf(stdout, "  # a0 a1 a2 b0 b1 b2 b3\n");
    fprintf(stdout, "  %g, %g, %g",
	    ctx->iono.a0 * pow(2.0, -30), 
	    ctx->iono.a1 * pow(2.0, -27),
	    ctx->iono.a2 * pow(2.0, -24));
    fprintf(stdout, " %g, %g, %g, %g\n",
	    ctx->iono.b0 * pow(2.0, 11), 
	    ctx->iono.b1 * pow(2.0, 14),
	    ctx->iono.b2 * pow(2.0, 16), 
	    ctx->iono.b3 * pow(2.0, 16));
  }

  if (ctx->set & SUPL_RRLP_ASSIST_UTC) {
    fprintf(stdout, "UTC Model:\n");
    fprintf(stdout, "  # a0, a1 delta_tls tot dn\n");
    fprintf(stdout, "  %g %g %d %d %d %d %d %d\n",
	    ctx->utc.a0 * pow(2.0, -30),
	    ctx->utc.a1 * pow(2.0, -50),
	    ctx->utc.delta_tls,
	    ctx->utc.tot, ctx->utc.wnt, ctx->utc.wnlsf,
	    ctx->utc.dn, ctx->utc.delta_tlsf);
  }

  if (ctx->cnt_eph) {
    int i;

    fprintf(stdout, "Ephemeris:");
    fprintf(stdout, " %d satellites\n", ctx->cnt_eph);
    fprintf(stdout, "  # prn delta_n M0 A_sqrt OMEGA_0 i0 w OMEGA_dot i_dot Cuc Cus Crc Crs Cic Cis");
    fprintf(stdout, " toe IODC toc AF0 AF1 AF2 bits ura health tgd OADA\n");

    for (i = 0; i < ctx->cnt_eph; i++) {
      struct supl_ephemeris_s *e = &ctx->eph[i];

      fprintf(stdout, "  prn %d\n delta_n %g\n M0 %g\n A_sqrt %g\n OMEGA_0 %g\n i0 %g\n w %g\n OMEGA_dot %g\n i_dot %g\n",
	      e->prn, 
	      e->delta_n , 
	      e->M0 , 
	      e->A_sqrt, 
	      e->OMEGA_0 , 
	      e->i0  , 
	      e->w , 
	      e->OMEGA_dot , 
	      e->i_dot );
      fprintf(stdout, " Cuc %g Cus %g Crc %g Crs %g Cic %g Cis %g\n",
	      e->Cuc * pow(2.0, -29), 
	      e->Cus * pow(2.0, -29), 
	      e->Crc * pow(2.0, -5), 
	      e->Crs * pow(2.0, -5), 
	      e->Cic * pow(2.0, -29), 
	      e->Cis * pow(2.0, -29));
      fprintf(stdout, " toe %g\n IODC %u\n toc %g\n AF0 %g\n AF1 %g\n AF2 %g\n",
	      e->toe * pow(2.0, 4), 
	      e->IODC, 
	      e->toc , 
	      e->AF0 , 
	      e->AF1 , 
	      e->AF2 );
      fprintf(stdout, "bits %d\n ura %d\n health %d\n tgd %d\n AODA%d\n\n\n\n",
	      e->bits,
	      e->ura,
	      e->health,
	      e->tgd,
	      e->AODA * 900);
    }
  message_eph(ctx);  
  }

  if (ctx->cnt_alm) {
    int i;

    fprintf(stdout, "Almanac:");
    fprintf(stdout, " %d satellites\n", ctx->cnt_alm);
    fprintf(stdout, "  # prn e toa Ksii OMEGA_dot A_sqrt OMEGA_0 w M0 AF0 AF1\n");

    for (i = 0; i < ctx->cnt_alm; i++) {
      struct supl_almanac_s *a = &ctx->alm[i];

      fprintf(stdout, "  %d %g %g %g %g ",
	      a->prn, 
	      a->e * pow(2.0, -21), 
	      a->toa * pow(2.0, 12),
	      a->Ksii * pow(2.0, -19),
	      a->OMEGA_dot * pow(2.0, -38));
      fprintf(stdout, "%g %g %g %g %g %g\n",
	      a->A_sqrt * pow(2.0, -11), 
	      a->OMEGA_0 * pow(2.0, -23),
	      a->w * pow(2.0, -23),
	      a->M0 * pow(2.0, -23),
	      a->AF0 * pow(2.0, -20),
	      a->AF1 * pow(2.0, -38));
    }
  message_alm(ctx);
  
  }

  return 1;
}

static int supl_consume_2(supl_assist_t *ctx) {
  if (ctx->set & SUPL_RRLP_ASSIST_REFTIME) {
    fprintf(stdout, "T %ld %ld %ld %ld\n", ctx->time.gps_week, ctx->time.gps_tow, 
	    ctx->time.stamp.tv_sec, ctx->time.stamp.tv_usec);
  }

  if (ctx->set & SUPL_RRLP_ASSIST_UTC) {
    fprintf(stdout, "U %d %d %d %d %d %d %d %d\n",
	    ctx->utc.a0, ctx->utc.a1, ctx->utc.delta_tls,
	    ctx->utc.tot, ctx->utc.wnt, ctx->utc.wnlsf,
	    ctx->utc.dn, ctx->utc.delta_tlsf);
  }

  if (ctx->set & SUPL_RRLP_ASSIST_REFLOC) {
    fprintf(stdout, "L %f %f %d\n", ctx->pos.lat, ctx->pos.lon, ctx->pos.uncertainty);
  } else if (fake_pos.valid) {
    fprintf(stdout, "L %f %f %d\n", fake_pos.lat, fake_pos.lon, fake_pos.uncertainty);
  }    

  if (ctx->set & SUPL_RRLP_ASSIST_IONO) {
    fprintf(stdout, "I %d %d %d %d %d %d %d\n",
	    ctx->iono.a0, ctx->iono.a1, ctx->iono.a2,
	    ctx->iono.b0, ctx->iono.b1, ctx->iono.b2, ctx->iono.b3);
  }

  if (ctx->cnt_eph) {
    int i;

    fprintf(stdout, "E %d\n", ctx->cnt_eph);

    for (i = 0; i < ctx->cnt_eph; i++) {
      struct supl_ephemeris_s *e = &ctx->eph[i];

      fprintf(stdout, "e %d %d %d %d %d %d %d %d %d %d",
	      e->prn, e->delta_n, e->M0, e->A_sqrt, e->OMEGA_0, e->i0, e->w, e->OMEGA_dot, e->i_dot, e->e);
      fprintf(stdout, " %d %d %d %d %d %d",
	      e->Cuc, e->Cus, e->Crc, e->Crs, e->Cic, e->Cis);
      fprintf(stdout, " %d %d %d %d %d %d",
	      e->toe, e->IODC, e->toc, e->AF0, e->AF1, e->AF2);
      fprintf(stdout, " %d %d %d %d %d\n",
	      e->bits, e->ura, e->health, e->tgd, e->AODA);
    }
  }

  if (ctx->cnt_alm) {
    int i;

    fprintf(stdout, "A %d\n", ctx->cnt_alm);
    for (i = 0; i < ctx->cnt_alm; i++) {
      struct supl_almanac_s *a = &ctx->alm[i];

      fprintf(stdout, "a %d %d %d %d %d ",
	      a->prn, a->e, a->toa, a->Ksii, a->OMEGA_dot);
      fprintf(stdout, "%d %d %d %d %d %d\n",
	      a->A_sqrt, a->OMEGA_0, a->w, a->M0, a->AF0, a->AF1);
    }
  }

  if (ctx->cnt_acq) {
    int i;

    fprintf(stdout, "Q %d %d\n", ctx->cnt_acq, ctx->acq_time);
    for (i = 0; i < ctx->cnt_acq; i++) {
      struct supl_acquis_s *q = &ctx->acq[i];

      fprintf(stdout, "q %d %d %d ",
	      q->prn, q->parts, q->doppler0);
      if (q->parts & SUPL_ACQUIS_DOPPLER) {
	fprintf(stdout, "%d %d ", q->doppler1, q->d_win); 
      } else {
	fprintf(stdout, "0 0 ");
      }
      fprintf(stdout, "%d %d %d %d ",
	      q->code_ph, q->code_ph_int, q->bit_num, q->code_ph_win);
      if (q->parts & SUPL_ACQUIS_ANGLE) {
	fprintf(stdout, "%d %d\n", q->az, q->el);
      } else {
	fprintf(stdout, "0 0\n");
      }
    }
  }

  return 1;
}

static char *usage_str =
"Usage:\n"
"%s options [supl-server]\n"
"Options:\n"
"  --almanac|-a					request also almanac data\n"
"  --cell gsm:mcc,mns:lac,ci|wcdma:mcc,msn,uc	set current gsm/wcdma cell id\n"
"  --cell gsm:mcc,mns:lac,ci:lat,lon,uncert	set known gsm cell id with position\n"
"  --format|-f human				machine parseable output\n"
"  --debug|-d <n>				1 == RRLP, 2 == SUPL, 4 == DEBUG\n"
"  --debug-file file				write debug to file\n"
"  --help|-h					show this help\n"
"Example:\n"
"%1$s --cell=gsm:244,5:0x59e2,0x31b0:60.169995,24.939995,127 --cell=gsm:244,5:0x59e2,0x31b0\n"
;

static void usage(char *progname) {
  printf(usage_str, progname);
}

static struct option long_opts[] = {
  { "cell", 1, 0, 0 },
  { "debug", 1, 0, 'd' },
  { "format", 1, 0, 'f' },
  { "test", 1, 0, 't' },
  { "set-pos", 1, 0, 0 },
  { "pos-helper", 1, 0, 0 },
  { "debug-file", 1, 0, 0 },
  { "help", 0, 0, 'h' },
  { "almanac", 0, 0, 'a' },
  { 0, 0, 0 }
};

static int parse_fake_pos(char *str, struct fake_pos_s *fake_pos) {
  if (sscanf(str, "%lf,%lf,%d",
	     &fake_pos->lat, &fake_pos->lon, &fake_pos->uncertainty) == 3) {
    fake_pos->valid = 1;
    return 0;
  } 
	
  if (sscanf(str, "%lf,%lf",
	     &fake_pos->lat, &fake_pos->lon) == 2) {
    fake_pos->uncertainty = 121; /* 1000 km */
    fake_pos->valid = 1;
    return 0;
  }

  return 1;
}

int main(int argc, char *argv[]) {
  int err;
  format_t format = FORMAT_DEFAULT;
  int debug_flags = 0;
  FILE *debug_f = 0;
  int request = 0;
  supl_assist_t assist;
  char *server;
  supl_ctx_t ctx;

  supl_ctx_new(&ctx);
  server = "supl.google.com";
  //server = "localhost";

  while (1) {
    int opt_index;
    int c;

    c = getopt_long(argc, argv, "ad:f:t:", long_opts, &opt_index);
    if (c == -1) break;
    switch (c) {
    case 0:
      switch (opt_index) {

      case 0: /* gsm/wcdma cell */
	{	
	  int mcc, mns, lac, ci, uc, uncertainty;
	  double lat, lon;

	  if (sscanf(optarg, "gsm:%d,%d:%x,%x:%lf,%lf,%d",
		     &mcc, &mns, &lac, &ci, &lat, &lon, &uncertainty) == 7) {
	    supl_set_gsm_cell_known(&ctx, mcc, mns, lac, ci, lat, lon, uncertainty);
	    break;
	  }

	  if (sscanf(optarg, "gsm:%d,%d:%x,%x",
		     &mcc, &mns, &lac, &ci) == 4) {
	    supl_set_gsm_cell(&ctx, mcc, mns, lac, ci);
	    break;
	  }

	  if (sscanf(optarg, "wcdma:%d,%d,%x",
		     &mcc, &mns, &uc) == 3) {
	    supl_set_wcdma_cell(&ctx, mcc, mns, uc);
	    break;
	  }
        }
	
	fprintf(stderr, "Ugh, cell\n");
	break;

      case 4: /* set-pos */
	if (parse_fake_pos(optarg, &fake_pos)) {
	  fprintf(stderr, "Ugh, set-pos\n");
	}
	break;

      case 5: /* pos-helper */

	break;

      case 6: /* debug-file */
	debug_f = fopen(optarg, "w");
	if (!debug_f) {
	  fprintf(stderr, "Error: open debug file %s (%s)\n", optarg, strerror(errno));
	}
	break;

      }

      break;

    case 'a':
      request |= SUPL_REQUEST_ALMANAC;
      break;

    case 'f':
      if (strcmp(optarg, "human") == 0) {
	format = FORMAT_HUMAN;
      }
      break;

    case 'd': 
      {
	int debug = atoi(optarg);

	if (debug & 0x01)
	  debug_flags |= SUPL_DEBUG_RRLP;
	if (debug & 0x02)
	  debug_flags |= SUPL_DEBUG_SUPL;
	if (debug & 0x04)
	  debug_flags |= SUPL_DEBUG_DEBUG;
      }
      break;

    case 't':
      switch (atoi(optarg)) {
      case 0:
	supl_set_gsm_cell(&ctx, 244, 5, 0x59e2, 0x31b0);
	break;
      case 1:
	supl_set_gsm_cell_known(&ctx, 244, 5, 995763, 0, 60.169995, 24.939995, 121);
	break;
      case 2:
	supl_set_gsm_cell(&ctx, 244, 5, 995763, 0x31b0);
	supl_set_gsm_cell_known(&ctx, 244, 5, 995763, 0x31b0, 60.169995, 24.939995, 121);
	break;
      case 3:
	supl_set_wcdma_cell(&ctx, 244, 5, 995763);
	break;
      }
      break;    

    case 'h':
      usage(argv[0]);
      exit(1);

    default:
      usage(argv[0]);
      exit(1);
    }
  }

  if (optind + 1 == argc) {
    server = argv[optind];
  }

  if (!fake_pos.valid && getenv("SUPL_FAKE_POS")) {
    parse_fake_pos(getenv("SUPL_FAKE_POS"), &fake_pos);
  }

#ifdef SUPL_DEBUG
  if (debug_flags) {
    supl_set_debug(debug_f ? debug_f : stderr, debug_flags);
  }
#endif

  supl_request(&ctx, request);

  err = supl_get_assist(&ctx, server, &assist);
  if (err < 0) {
    fprintf(stderr, "SUPL protocol error %d\n", err);
    exit(1);
  }

  if (debug_f) {
    fclose(debug_f);
  }

  switch (format) {
  case FORMAT_DEFAULT: 
    supl_consume_2(&assist);
    break;
  case FORMAT_HUMAN:
    supl_consume_1(&assist);
    break;
  }

  supl_ctx_free(&ctx);

  return 0;
}
