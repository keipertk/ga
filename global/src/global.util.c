/*$Id: global.util.c,v 1.20 1997-12-13 01:21:50 d3h325 Exp $*/
/*
 * module: global.util.c
 * author: Jarek Nieplocha
 * last modification: Tue Dec 20 09:41:55 PDT 1994
 *
 * DISCLAIMER
 * 
 * This material was prepared as an account of work sponsored by an
 * agency of the United States Government.  Neither the United States
 * Government nor the United States Department of Energy, nor Battelle,
 * nor any of their employees, MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR
 * ASSUMES ANY LEGAL LIABILITY OR RESPONSIBILITY FOR THE ACCURACY,
 * COMPLETENESS, OR USEFULNESS OF ANY INFORMATION, APPARATUS, PRODUCT,
 * SOFTWARE, OR PROCESS DISCLOSED, OR REPRESENTS THAT ITS USE WOULD NOT
 * INFRINGE PRIVATELY OWNED RIGHTS.
 * 
 * 
 * ACKNOWLEDGMENT
 * 
 * This software and its documentation were produced with United States
 * Government support under Contract Number DE-AC06-76RLO-1830 awarded by
 * the United States Department of Energy.  The United States Government
 * retains a paid-up non-exclusive, irrevocable worldwide license to
 * reproduce, prepare derivative works, perform publicly and display
 * publicly by or for the US Government, including the right to
 * distribute to other US Government contractors.
 */

#include "global.h"
#include "globalp.h"
#include "macommon.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>


#ifdef CRAY_T3D
#include <fortran.h>
#endif

#if defined(SUN)
  void fflush();
#endif



/*\ COPY ONE GLOBAL ARRAY INTO ANOTHER
\*/
void ga_copy_(g_a, g_b)
     Integer *g_a, *g_b;
{
Integer atype, btype, adim1, adim2, bdim1, bdim2;
Integer ilo, ihi, jlo, jhi;
Integer me= ga_nodeid_(), index, ld;

   ga_sync_();

#ifdef GA_TRACE
       trace_stime_();
#endif

   ga_check_handle(g_a, "ga_copy");
   ga_check_handle(g_b, "ga_copy");

   if(*g_a == *g_b) ga_error("ga_copy: arrays have to be different ", 0L);

   ga_inquire_(g_a, &atype, &adim1, &adim2);
   ga_inquire_(g_b, &btype, &bdim1, &bdim2);

   if(atype != btype || (atype != MT_F_DBL && atype != MT_F_INT &&
                         atype != MT_F_DCPL))
               ga_error("ga_copy: wrong types ", 0L);

   if(adim1 != bdim1 || adim2!=bdim2 )
               ga_error("ga_copy: arrays not conformant", 0L);

   ga_distribution_(g_a, &me, &ilo, &ihi, &jlo, &jhi);

   if (  ihi>0 && jhi>0 ){
      ga_access_(g_a, &ilo, &ihi, &jlo, &jhi,  &index, &ld);
      switch (atype){
        case MT_F_DBL:
           ga_put_(g_b, &ilo, &ihi, &jlo, &jhi, DBL_MB+index-1, &ld); break;
        case MT_F_DCPL:
           ga_put_(g_b, &ilo, &ihi, &jlo, &jhi, DCPL_MB+index-1, &ld); break;
        case MT_F_INT:
           ga_put_(g_b, &ilo, &ihi, &jlo, &jhi, INT_MB+index-1, &ld);
      }
      ga_release_(g_a, &ilo, &ihi, &jlo, &jhi);
   }

#ifdef GA_TRACE
   trace_etime_();
   op_code = GA_OP_COP;
   trace_genrec_(g_a, ilo, ihi, jlo, jhi, &op_code);
#endif

   ga_sync_();
}



/*\ PRINT g_a[ilo:ihi, jlo:jhi]
\*/
void ga_print_patch_(g_a, ilo, ihi, jlo, jhi, pretty)
        Integer *g_a, *ilo, *ihi, *jlo, *jhi, *pretty;
/*
  Pretty = 0 ... spew output out with no formatting
  Pretty = 1 ... format output so that it is readable
*/  
{
#define DEV stdout
#define BUFSIZE 6
#define FLEN 80 
Integer i, j,jj, dim1, dim2, type, ibuf[BUFSIZE], jmax, ld=1, bufsize ;
DoublePrecision  dbuf[BUFSIZE];
char name[FLEN];

  ga_sync_();
  ga_check_handle(g_a, "ga_print");
  if(ga_nodeid_() == 0){

     ga_inquire_(g_a,  &type, &dim1, &dim2);
     name[FLEN-1]='\0';
     ga_inquire_name(g_a,  name);
     if (*ilo <= 0 || *ihi > dim1 || *jlo <= 0 || *jhi > dim2){
                      fprintf(stderr,"%d %d %d %d dims: [%d,%d]\n", 
                             *ilo,*ihi, *jlo,*jhi, dim1, dim2);
                      ga_error(" ga_print: indices out of range ", *g_a);
     }

     fprintf(DEV,"\n global array: %s[%d:%d,%d:%d],  handle: %d \n",
             name, *ilo, *ihi, *jlo, *jhi, (int)*g_a);

     bufsize = (type==MT_F_DCPL)? BUFSIZE/2 : BUFSIZE;


     if (!*pretty) {
       for (i=*ilo; i <*ihi+1; i++){
         for (j=*jlo; j <*jhi+1; j+=bufsize){
           jmax = MIN(j+bufsize-1,*jhi);
           switch(type){
              case MT_F_INT:
                   ga_get_(g_a, &i, &i, &j, &jmax, ibuf, &ld);
                   for(jj=0; jj<(jmax-j+1); jj++)
                     fprintf(DEV," %8d",ibuf[jj]);
                   break;

              case MT_F_DBL:
                   ga_get_(g_a, &i, &i, &j, &jmax, dbuf, &ld);
                   for(jj=0; jj<(jmax-j+1); jj++)
                     fprintf(DEV," %11.5f",dbuf[jj]);
                   break;

              case MT_F_DCPL:
                   ga_get_(g_a, &i, &i, &j, &jmax, dbuf, &ld);
                   for(jj=0; jj<(jmax-j+1); jj+=2)
                     fprintf(DEV," %11.5f,%11.5f",dbuf[jj], dbuf[jj+1]);
                   break;
              default: ga_error("ga_print: wrong type",0);
           }
         }
         fprintf(DEV,"\n");
       }
       fflush(DEV);

     } else {

        for (j=*jlo; j<*jhi+1; j+=bufsize){
        jmax = MIN(j+bufsize-1,*jhi);

           fprintf(DEV, "\n"); fprintf(DEV, "\n");

           /* Print out column headers */

           fprintf(DEV, "      ");
           switch(type){
              case MT_F_INT:
                   for (jj=j; jj<=jmax; jj++) fprintf(DEV, "%6d  ", jj);
                   fprintf(DEV,"\n      ");
                   for (jj=j; jj<=jmax; jj++) fprintf(DEV," -------");
                   break;
              case MT_F_DCPL:
                   for (jj=j; jj<=jmax; jj++) fprintf(DEV,"%20d    ", jj);
                   fprintf(DEV,"\n      ");
                   for (jj=j; jj<=2*jmax; jj++) fprintf(DEV," -----------");
                   break;
              case MT_F_DBL:
                   for (jj=j; jj<=jmax; jj++) fprintf(DEV,"%8d    ", jj);
                   fprintf(DEV,"\n      ");
                   for (jj=j; jj<=jmax; jj++) fprintf(DEV," -----------");
           }
           fprintf(DEV,"\n");

           for(i=*ilo; i <*ihi+1; i++){
              fprintf(DEV,"%4i  ",i);

              switch(type){
                 case MT_F_INT:
                      ga_get_(g_a, &i, &i, &j, &jmax, ibuf, &ld);
                      for(jj=0; jj<(jmax-j+1); jj++)
                        fprintf(DEV," %8d",ibuf[jj]);
                      break;

                 case MT_F_DBL:
                      ga_get_(g_a, &i, &i, &j, &jmax, dbuf, &ld);
                      for(jj=0; jj<(jmax-j+1); jj++)
                        fprintf(DEV," %11.5f",dbuf[jj]);
                      break;

                 case MT_F_DCPL:
	              ga_get_(g_a, &i, &i, &j, &jmax, dbuf, &ld);
	              for(jj=0; jj<(jmax-j+1); jj+=2)
	                fprintf(DEV," %11.5f,%11.5f",dbuf[jj], dbuf[jj+1]);
                      break;
                 default: ga_error("ga_print: wrong type",0);
	     }
	     fprintf(DEV,"\n");
         }
         fflush(DEV);
      }
    }
  }
       
  ga_sync_();
}


void ga_print_(g_a)
     Integer *g_a;
{
  Integer type, dim1, dim2;
  Integer ilo=1, jlo=1;
  Integer pretty = 1;

  ga_inquire_(g_a, &type, &dim1, &dim2);

  ga_print_patch_(g_a, &ilo, &dim1, &jlo, &dim2, &pretty);
}
  

void ga_print_stats_()
{
int i;
     GAstat_arr = (long*)&GAstat;
     printf("\n                         GA Statistics for process %4d\n",ga_nodeid_());
     printf("                         ------------------------------\n\n");
     printf("       create   destroy   get      put      acc     scatter   gather  read&inc\n");

     printf("calls: ");
     for(i=0;i<8;i++) 
        if(GAstat_arr[i] < 9999) printf("%4ld     ",GAstat_arr[i]);
        else                   printf("%.2e ",(double)GAstat_arr[i]);
     printf("\n");

     printf("bytes total:             %.2e %.2e %.2e %.2e %.2e %.2e\n",
                   GAbytes.gettot, GAbytes.puttot, GAbytes.acctot,
                   GAbytes.scatot, GAbytes.gattot, GAbytes.rditot);

     printf("bytes remote:            %.2e %.2e %.2e %.2e %.2e %.2e\n",
                   GAbytes.gettot - GAbytes.getloc, 
                   GAbytes.puttot - GAbytes.putloc,
                   GAbytes.acctot - GAbytes.accloc,
                   GAbytes.scatot - GAbytes.scaloc,
                   GAbytes.gattot - GAbytes.gatloc,
                   GAbytes.rditot - GAbytes.rdiloc);
     printf("Max memory consumed for GA by this process: %ld bytes\n",GAstat.maxmem);
     if(GAstat.numser)
        printf("Number of requests serviced: %ld\n",GAstat.numser);
     fflush(stdout);
}

   
 

/*\  ERROR TERMINATION
 *   C-version
\*/
void ga_error(string, icode)
     char     *string;
     Integer  icode;
{
extern void Error();
#ifdef SYSV
   extern int SR_caught_sigint;
#endif
#ifdef CRAY_T3D 
#  define FOUT stdout
#else
#  define FOUT stderr
#endif
#define ERR_LEN 400
    int level;
    char error_buffer[ERR_LEN];

    ga_clean_resources(); 

    /* print GA names stack */
    sprintf(error_buffer,"%d:", ga_nodeid_());
    for(level = 0; level < GA_stack_size; level++){
       strcat(error_buffer,GA_name_stack[level]);
       strcat(error_buffer,":");
    }
    strcat(error_buffer,string);
    strcat(error_buffer,":");
       
    if (ga_nnodes_() > 1) Error(error_buffer, icode);
    else{
      fprintf(FOUT,"%s %ld\n",error_buffer,icode);
      perror("system message:");
      fflush(FOUT);
      exit(1);
    }
}




/*\  ERROR TERMINATION
 *   Fortran version
\*/
#ifdef CRAY_T3D
void ga_error_(string, icode)
     _fcd        string;
#else
void ga_error_(string, icode, slen)
     char        *string;
     int         slen;
#endif
     Integer     *icode;
{
#define FMSG 256
char buf[FMSG];
#ifdef CRAY_T3D
      f2cstring(_fcdtocp(string), _fcdlen(string), buf, FMSG);
#else
      f2cstring(string,slen, buf, FMSG);
#endif
      ga_error(buf,*icode);
}





/************** Fortran - C conversion routines for strings ************/

/*\ converts C strings to  Fortran strings
\*/
void c2fstring( cstring, fstring, flen)
     char *cstring, *fstring;
     Integer flen;
{
    int clen = strlen(cstring);

    /* remove terminal \n character if any */

    if(cstring[clen] == '\n') clen--;

    /* Truncate C string into Fortran string */

    if (clen > flen) clen = flen;

    /* Copy characters over */

    flen -= clen;
    while (clen--)
	*fstring++ = *cstring++;

    /* Now terminate with blanks */

    while (flen--)
	*fstring++ = ' ';
}


/*\
 * Strip trailing blanks from fstring and copy it to cstring,
 * truncating if necessary to fit in cstring, and ensuring that
 * cstring is NUL-terminated.
\*/
void f2cstring(fstring, flength, cstring, clength)
    char        *fstring;       /* FORTRAN string */
    Integer      flength;        /* length of fstring */
    char        *cstring;       /* C buffer */
    Integer      clength;        /* max length (including NUL) of cstring */
{
    /* remove trailing blanks from fstring */
    while (flength-- && fstring[flength] == ' ') ;

    /* the postdecrement above went one too far */
    flength++;

    /* truncate fstring to cstring size */
    if (flength >= clength)
        flength = clength - 1;

    /* ensure that cstring is NUL-terminated */
    cstring[flength] = '\0';

    /* copy fstring to cstring */
    while (flength--)
        cstring[flength] = fstring[flength];
}


void ga_debug_suspend()
{
#ifdef SYSV
#  include <sys/types.h>
#  include <unistd.h>

   fprintf(stdout,"ga_debug: process %ld ready for debugging\n",
           (long)getpid());
   fflush(stdout);
   pause();

#endif
}
