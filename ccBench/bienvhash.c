/********************************************************************
 * BenchIT - Performance Measurement for Scientific Applications
 * Contact: developer@benchit.org
 *
 * $Id: bienvhash.template.c 1 2009-09-11 12:26:19Z william $
 * $URL: svn+ssh://molka@rupert.zih.tu-dresden.de/svn-base/benchit-root/BenchITv6/tools/bienvhash.template.c $
 * For license details see COPYING in the package base directory
 *******************************************************************/
/* used as hash tables for environment variables from compile time.
 * This is a template and will be build to bienvhash.c by adding some bi_put()s
 * and a closing bracket.
 *******************************************************************/

/** @file bienvhash.template.c
* @Brief used as hash tables for environment variables from compile time.
* This is a template and will be build to bienvhash.c by adding some bi_put()s
* and a closing bracket.
*/

/**
* used for file works and printing
*/
#include <stdio.h>
/**
* used for typeconversion e.g. atoi()
*/
#include <stdlib.h>
/**
* used for string-works
*/
#include <string.h>

/**
* used for specific types
*/
#include <ctype.h>

/**
* BenchIT: used for more stringwork
*/
#include "tools/stringlib.h"
#include "tools/stringlib.c"

/**
* BenchIT: header for this file
*/
#include "tools/bienvhash.h"

/**
* EMPTY means not found. makes it easier to read
*/
#define EMPTY NULL

/**
* shorter type name
*/
typedef unsigned int us_int;

/*!@brief defines one element in the hashtable
 */
typedef struct element
{
  /*@{*/
  /*!@brief key of this hash-value (remember. e.g. the name). */
  char *key;
  /*@{*/
  /*!@brief length of the name. */
  int keyLength;
  /*@{*/
  /*!@brief value of this hash-entry. */
  char *value;
  /*@{*/
  /*!@brief length of the value. */
  int valueLength;
  /*@{*/
  /*!@brief pointer to the next element. */
  struct element *next;
} ELEMENT;


int HASH_PRIME;
  /*@{*/
  /*!@brief the number of entries in the hash table. */
int ENTRIES;

  /*@{*/
  /*!@brief the hash table itself. */
ELEMENT **table;


/* internal implementations */
void bi_insert( ELEMENT, ELEMENT *[] );
void bi_find( char *, int, ELEMENT *[], ELEMENT**, int * );
us_int bi_hash( char *, int );

/* visible implementations */
void bi_dumpTable(void);
void bi_dumpTableToFile( FILE ** );
char *bi_get ( const char *, int * );
void bi_initTable(void);
int bi_put( const char *, const char * );
int bi_size(void);
int isEnvEntry( const char *line );
int bi_readParameterFile( const char * );
/* special case: generated from outside and code will be
   appended to this file */
void bi_fillTable(void);

/* IMPLEMENTATIONS */


/*!@brief Inserts an element into the table.
 *
 * @param[in] x an element which shall be inserted to the hashtable.
 * @param[in|out] tab the hashtable, where the element shal be inserted to.
 */
void bi_insert( ELEMENT x, ELEMENT *tab[] )
{
  /* chain length at a specific position in the table */
  int chain_length = 0;
  /* hash index for element */
  int ind;
  /* pointer to element x */
  ELEMENT *el;
  /* compute hash index */
  ind = bi_hash( x.key, x.keyLength );
  /* there is no following element, because its the last inserted  */
  x.next = EMPTY;
  if ( tab[ind] == EMPTY )
  {
      /* no entry yet -> create new chain */
    if ( ( tab[ind] = (ELEMENT *)malloc( sizeof( ELEMENT ) ) ) != NULL )
    {
      memcpy( tab[ind], &x, sizeof( x ) );
    }
    else
    {
      exit( 1 );
    }
  }
  else
  {
      /* find end of the chain */
    for ( el = tab[ind]; el->next != EMPTY; el = el->next )
    {
      chain_length++;
    }
      /* append at the end */
    if ( ( el->next = (ELEMENT *)malloc( sizeof( ELEMENT ) ) ) != NULL )
    {
      memcpy( el->next, &x, sizeof( x ) );
    }
    else
    {
      exit( 1 );
    }
  }
}

/*!@brief look for element with key "key" in "tab".
 *         if found: return ( x, ind )
 *        else      return ( x = NULL, ind = ?? )
 *
 * @param[in] key The hash-key for that value.
 * @param[in] len length of the hash-key
 * @param[in|out] tab the hash table to find element with hash-key key
 * @param[out] x the element with hash-key key
 * @param[out] pos the position of x in the hash-table
 */
void bi_find( char *key, int len, ELEMENT *tab[], ELEMENT **x, int *pos )
{
  us_int ind;
  ELEMENT *el;
  /* get hash-index */
  ind = bi_hash( key, len );
  *pos = ind;

   /* look up element in chain */
  for ( el = tab[ind]; el != EMPTY; el = el->next )
  {
    if ( compare( el->key, key ) == 0 )
    {
      /* found */
      *x = el;
      return;
    }
  }
  /* not found */
  *x = NULL;
}

/*!@brief Calculates the index in the table for a given key.
 *
 * @param[in] key The hash-key to compute index for
 * @param[in] len length of the hash-key
 */
us_int bi_hash( char *key, int kl )
{
  us_int help;
  int i;
  /* if its still 0 */
  if ( HASH_PRIME == 0 )
  {
    /* its not initialized yet */
    /* do it! */
    bi_initTable();
  }
   /* calculate a key out of the first 7 characters or less. */
  help = toupper( key[0] ) - 'A' + 1;
   /* for ( i = 0; i < min( 7, kl ); i++ ) */
  for ( i = 0; i < kl; i++ )
  {
    help = help + 27 * (char)toupper( key[i] ) - 'A' + 1;
  }
   /* to avoid storing strings of different length but with
      the same beginning at the same place */
  help = help + kl;
   /* simple hash function */
  return ( help % HASH_PRIME );
}

/*!@brief Dumps table to standard out.
 *
 */
void bi_dumpTable(void)
{
  int i, chained, totalChainLength;
  double load_factor;
  double avg_chain_length;
  ELEMENT *ptr;
  printf( "\nHashtable dump of all known environment variables at compiletime:" );
  printf( "\nIndex |  Key[Length]  | Value[Length]" );
  printf( "\n-------------------------------------" );
  chained = 0;
  totalChainLength = 0;
  for ( i = 0; i < HASH_PRIME; i++ )
  {
    if ( table[i] != EMPTY )
    {
      int count = 0;
      ptr = table[i];
      while ( ptr != NULL )
      {
        int key_length=length(ptr->key)+1;
        int value_length=length(ptr->value)+1;
        printf( "\n  %d  | %s[%d] |  %s[%d]", i, ptr->key,
          key_length, ptr->value , value_length );
        ptr = ptr->next;
        count++;
      }
      if ( count > 1 )
      {
        chained++;
        totalChainLength += count;
      }
    }
  }
  load_factor = 1.0*bi_size()/HASH_PRIME;
  avg_chain_length = 1.0*totalChainLength/chained;
  printf( "\n%d entries in total. Load factor: %f. Indexed chains: %d. "
    "Avg chain length: %f.\n",
    bi_size(), load_factor , chained, avg_chain_length );
  fflush( stdout );
}

/*!@brief Dumps table to output stream. (e.g. bit-file)
 * @param(in) bi_out pointer to the output stream, where the table shall be dumped to
 */
void bi_dumpTableToFile( FILE **bi_out )
{
   int i = 0;
   char buf[100000];
  ELEMENT *ptr = 0;
   memset( buf, 0, 100000 );
   sprintf( buf, "beginofenvironmentvariables\n" );
   bi_fprintf( *bi_out, buf );
   /* all possible entries in hashtable */
   for ( i = 0; i < HASH_PRIME; i++ )
   {
       /* if the position in the hash table is not empty */
      if ( table[i] != EMPTY )
      {
        /* ptr is the element in this table (with key, value, ...) */
         ptr = table[i];
         /* and it exists */
         while ( ptr != NULL )
         {
             /* get the value */
            char *value = bi_strdup( ptr->value ), vbuf[100000];
            /* and its length */
            int vlen = strlen( value ), j = 0, vpos = 0;
            /* delete buffer */
            memset( vbuf, 0 , 100000 );
            
            for ( j = 0; j < vlen; j++ )
            {
               if ( value[j] == '"' )
               {
                  /* check equal number of escape chars->insert escape char */
                  int cnt = 0, k = 0;
                  for ( k = j - 1;  k >= 0; k-- )
                  {
                     if ( value[k] == '\\' ) cnt++;
                  }
                  /* escape quote */
                  /* this is only changed in vbuf, not in value, so it counts the correct */
                  /* number of \\s */
                  if ( cnt % 2 == 0 )
                  {
                     vbuf[vpos++] = '\\';
                  }
               }
               vbuf[vpos++] = value[j];
            }
            /* end buffer */
            vbuf[vpos++] = '\0';
            /* print it to file */
            sprintf( buf, "%s=\"%s\"\n", ptr->key, vbuf );
            bi_fprintf( *bi_out, buf );
            /* next element at this position of hashtable */
            ptr = ptr->next;
         }
      }
   }
   sprintf( buf, "endofenvironmentvariables\n" );
   bi_fprintf( *bi_out, buf );
}

/*!@brief Retrieves a value from the table. If the given key
 *   does not exist a null pointer is returned. valueLength
 *   will represent the length of the returned value.
 * @param(in) key String, containing the key to search for
 * @param(out) valuLength length of the returned value for key
 * @returns the value for the key or NULL if not found
 */
char *bi_get ( const char *key, int *valueLength )
{
  char *retval = 0;
  int i;
  ELEMENT *el;

  *valueLength = 0;
  /* find the value */
  bi_find( (char *)key, length( key ) +1, table, &el, &i );
  /* does it exist? */
  if ( el != NULL )
  {
    /* compute length */
    retval = el->value;
    *valueLength = el->valueLength;
  }
  return retval;
}

/*!@brief Creates the table and initializes the fields.
 */
void bi_initTable(void)
{
  int i;
  /* set the hash prime */
  HASH_PRIME = 1009;
  ENTRIES = 0;
  /* allocate memory */
  table = (ELEMENT **)malloc( HASH_PRIME * sizeof( ELEMENT * ) );
   /* mark all entries as FREE */
  for ( i = 0; i < HASH_PRIME; i++ ) table[i] = EMPTY;
}

/*!@brief Puts a Key-Value pair into the table. If the key
 *   already exists, the value will be overwritten.
 *   Returns 0, if the key is new, 1 if a value was
 *   overwritten, and -1 if an error occured.
 * @param(in) key the key for a value, that shall be inserted into table
 * @param(in) value the value for this key
 * returns 0 if the key is new,
 * 1 if the value for this key is overwritten
 * and -1 if an error occured
 */
int bi_put( const char *key, const char *value )
{
  int retval = -1, kLen = -1, vLen = -1, j = -1;
  /* needed, if element is found in table */
  ELEMENT *e = NULL;
  /* is just needed, if the element is new */
  ELEMENT el;
  if ( ( key == 0 ) || ( value == 0 ) ) return retval;
   /* some environment variables end on dead characters (0x11 and 0x19)
    * which will be eliminated now. */
  kLen = length( key ) + 1;
  vLen = length( value ) + 1;
  IDL(5,printf("try to find %s and set to new value %s ...",key,value));
   /* get or create new element */
  bi_find( (char *)key, kLen, table, &e, &j );
  if ( e != NULL )
  {
    retval = 1;
    IDL(5,printf("found. old is %s\n",e->value));
  }
  else
  {
    retval = 0;
    IDL(5,printf("not found\n"));
  }
  /* if the key exists */
  if  (retval==1)
  {
    /* the value should also exist, but check it */
    if ( e->value != NULL )
    {
      /* and set it free */
      free( e->value );
    }
    /* create the new value, which is one larger then the input by allocating */
    if ( ( e->value = (char *)malloc( (vLen+1) * sizeof( char ) ) ) != NULL )
    {
      /* copy the value given by the function call to the value in the element */
      strncpy( e->value, value, vLen );
      /* set last char to '\0' in case src string was longer */
      e->value[vLen] = '\0';
      /* set lengths */
      e->valueLength = vLen;
    }
    else
    {
      /* if malloc wasn't succesful */
      IDL(-1,printf("Creation NOT succesfull"));
    }
    return retval;
  }
   /* set key if key is new */
  memset( &el, 0, sizeof( ELEMENT ) );
  if ( el.key == 0 )
  {
    if ( ( el.key = (char *)malloc( (kLen+1) * sizeof( char ) ) ) != NULL )
    {
      strncpy( el.key, key, kLen+1 );
      /* set last char to '\0' in case src string was longer */
      el.key[kLen] = '\0';
      el.keyLength = kLen;
    }
    else
    {
      return -1;
    }
  }
   /* set or replace value */
  if ( ( el.value = (char *)malloc( (vLen+1) * sizeof( char ) ) ) != NULL )
  {
    strncpy( el.value, value, vLen );
    /* set last char to '\0' in case src string was longer */
    el.value[vLen] = '\0';
    el.valueLength = vLen;
  }
  else
  {
    if ( ( retval == 0 ) && ( el.key != NULL ) )
    {
      free( el.key );
    }
    return -1;
  }
   /* insert the new element; if the key existed, the element was
      already updated by the above code */
  if ( retval == 0 )
  {
    bi_insert( el, table );
    ENTRIES++;
  }
  return retval;
}

/*!@brief Returns the number of entries stored in the table.
 * @returns number of entries in table
*/
int bi_size(void)
{
  return ENTRIES;
}

/*!@brief internal check for Strings (see returnvalue)
 * @returns 1, if the line starts with a letter, and if it contains an equals
 *   sign and no '$', 0 otherwise.
 */
int isEnvEntry( const char *line )
{
  int retval = 0;
  if ( line == 0 ) return retval;
  /* starts with a uppercase letter? */
  if ( ( line[0] >= 'A' ) && ( line[0] <= 'Z' )
  /* and contains an equal sign and a $? */
    && indexOf( line, '=', 0 ) > 0 && indexOf( line, '$', 0 ) < 0 )
  {
    retval = 1;
  }
  return retval;
}

/*!@brief Takes a PARAMETER file as argument and adds it's variables
 * to the environment hash table.
 * @param fileName name of the file, which contains the parameters to read 
 * @returns 1 on success, 0 else.
 */
int bi_readParameterFile( const char *fileName )
{
  FILE *efp = 0;
  char line[STR_LEN], key[STR_LEN], value[STR_LEN];

  memset( line, 0, STR_LEN );
  memset( key, 0, STR_LEN );
  memset( value, 0, STR_LEN );
  if ( ( efp = fopen( fileName, "r" ) ) == NULL )
  {
    fprintf( stderr, "File %s couldn't be opened for reading!\n",
      fileName );
    return 0;
  }

  while ( fgets( line, STR_LEN, efp ) != NULL )
  {
      /* remove leading and trailing white spaces */
    trim( line, line );
    if ( isEnvEntry( line ) )
    {
      int eqPos = indexOf( line, '=', 0 );
         /* remove EOL at end of line */
      if ( line[length( line )] == '\n' )
      {
        substring( line, line, 0, length( line ) );
      }
         /* extract key and value for hashtable */
      substring( line, key, 0, eqPos );
      substring( line, value, eqPos + 1, length( line ) + 1 );
         /* remove leading and trailing " and ' */
      trimChar( value, value, '\'' );
      trimChar( value, value, '"' );
         /* replace all occurances of \ by \\ */
      escapeChar( value, value, '\\' );
         /* replace all occurances of " by \" */
      escapeChar( value, value, '"' );
      bi_put( key, value );
    }
  }
  return 1;
}

/* special case: generated from outside and code will be
   appended to this file */
/*!@brief Fills the table with predefined content. */
void bi_fillTable(void)
{

   bi_put( "BASH", "/bin/sh" );
   bi_put( "BASHOPTS", "cmdhist:complete_fullquote:extquote:force_fignore:hostcomplete:interactive_comments:progcomp:promptvars:sourcepath" );
   bi_put( "BASH_ALIASES", "()" );
   bi_put( "BASH_ARGC", "([0]=\"1\" [1]=\"1\")" );
   bi_put( "BASH_ARGV", "([0]=\"/home/fjb/gwr/BenchIT/kernel/AArch64/memory_bandwidth/C/pthread/SIMD/single-reader/COMPILE.SH\" [1]=\"kernel/AArch64/memory_bandwidth/C/pthread/SIMD/single-reader/\")" );
   bi_put( "BASH_CMDS", "()" );
   bi_put( "BASH_ENV", "/usr/share/Modules/init/bash" );
   bi_put( "BASH_LINENO", "([0]=\"132\" [1]=\"0\")" );
   bi_put( "BASH_SOURCE", "([0]=\"/home/fjb/gwr/BenchIT/kernel/AArch64/memory_bandwidth/C/pthread/SIMD/single-reader/COMPILE.SH\" [1]=\"./COMPILE.SH\")" );
   bi_put( "BASH_VERSINFO", "([0]=\"4\" [1]=\"4\" [2]=\"19\" [3]=\"1\" [4]=\"release\" [5]=\"aarch64-redhat-linux-gnu\")" );
   bi_put( "BASH_VERSION", "4.4.19(1)-release" );
   bi_put( "BENCHITROOT", "/home/fjb/gwr/BenchIT" );
   bi_put( "BENCHIT_ARCH_SHORT", "unknown" );
   bi_put( "BENCHIT_ARCH_SPEED", "unknownM" );
   bi_put( "BENCHIT_CC", "gcc" );
   bi_put( "BENCHIT_CC_C_FLAGS", "" );
   bi_put( "BENCHIT_CC_C_FLAGS_HIGH", "-O0" );
   bi_put( "BENCHIT_CC_C_FLAGS_OMP", "" );
   bi_put( "BENCHIT_CC_C_FLAGS_STD", "-O2" );
   bi_put( "BENCHIT_CC_LD", "gcc" );
   bi_put( "BENCHIT_CC_L_FLAGS", "-lm" );
   bi_put( "BENCHIT_COMPILETIME_CC", "gcc" );
   bi_put( "BENCHIT_CPP_ACML", "" );
   bi_put( "BENCHIT_CPP_ATLAS", "" );
   bi_put( "BENCHIT_CPP_BLAS", "" );
   bi_put( "BENCHIT_CPP_ESSL", "" );
   bi_put( "BENCHIT_CPP_FFTW3", "" );
   bi_put( "BENCHIT_CPP_MKL", "" );
   bi_put( "BENCHIT_CPP_MPI", " -DUSE_MPI" );
   bi_put( "BENCHIT_CPP_PAPI", "-DUSE_PAPI" );
   bi_put( "BENCHIT_CPP_PCL", " -DUSE_PCL" );
   bi_put( "BENCHIT_CPP_PTHREADS", "" );
   bi_put( "BENCHIT_CPP_PVM", "" );
   bi_put( "BENCHIT_CPP_SCSL", "" );
   bi_put( "BENCHIT_CROSSCOMPILE", "0" );
   bi_put( "BENCHIT_CXX", "g++" );
   bi_put( "BENCHIT_CXX_C_FLAGS", "" );
   bi_put( "BENCHIT_CXX_C_FLAGS_HIGH", "-O3" );
   bi_put( "BENCHIT_CXX_C_FLAGS_OMP", "" );
   bi_put( "BENCHIT_CXX_C_FLAGS_STD", "-O2" );
   bi_put( "BENCHIT_CXX_LD", "g++" );
   bi_put( "BENCHIT_CXX_L_FLAGS", "-lm" );
   bi_put( "BENCHIT_DEBUGLEVEL", "0" );
   bi_put( "BENCHIT_DEFINES", " -DLINEAR_MEASUREMENT -DDEBUGLEVEL=0" );
   bi_put( "BENCHIT_ENVIRONMENT", "DEFAULT" );
   bi_put( "BENCHIT_F77", "" );
   bi_put( "BENCHIT_F77_C_FLAGS", "" );
   bi_put( "BENCHIT_F77_C_FLAGS_HIGH", "" );
   bi_put( "BENCHIT_F77_C_FLAGS_OMP", "" );
   bi_put( "BENCHIT_F77_C_FLAGS_STD", "" );
   bi_put( "BENCHIT_F77_LD", "" );
   bi_put( "BENCHIT_F77_L_FLAGS", "-lm" );
   bi_put( "BENCHIT_F90", "" );
   bi_put( "BENCHIT_F90_C_FLAGS", "" );
   bi_put( "BENCHIT_F90_C_FLAGS_HIGH", "" );
   bi_put( "BENCHIT_F90_C_FLAGS_OMP", "" );
   bi_put( "BENCHIT_F90_C_FLAGS_STD", "" );
   bi_put( "BENCHIT_F90_LD", "" );
   bi_put( "BENCHIT_F90_L_FLAGS", "-lm" );
   bi_put( "BENCHIT_F90_SOURCE_FORMAT_FLAG", "" );
   bi_put( "BENCHIT_F95", "" );
   bi_put( "BENCHIT_F95_C_FLAGS", "" );
   bi_put( "BENCHIT_F95_C_FLAGS_HIGH", "" );
   bi_put( "BENCHIT_F95_C_FLAGS_OMP", "" );
   bi_put( "BENCHIT_F95_C_FLAGS_STD", "" );
   bi_put( "BENCHIT_F95_LD", "" );
   bi_put( "BENCHIT_F95_L_FLAGS", "-lm" );
   bi_put( "BENCHIT_F95_SOURCE_FORMAT_FLAG", "" );
   bi_put( "BENCHIT_FILENAME_COMMENT", "0" );
   bi_put( "BENCHIT_HOSTNAME", "KP04" );
   bi_put( "BENCHIT_IGNORE_PARAMETER_FILE", "0" );
   bi_put( "BENCHIT_INCLUDES", "-I. -I/home/fjb/gwr/BenchIT -I/home/fjb/gwr/BenchIT/tools" );
   bi_put( "BENCHIT_INTERACTIVE", "0" );
   bi_put( "BENCHIT_JAVA", "java" );
   bi_put( "BENCHIT_JAVAC", "javac" );
   bi_put( "BENCHIT_JAVAC_FLAGS", "" );
   bi_put( "BENCHIT_JAVAC_FLAGS_HIGH", "-O" );
   bi_put( "BENCHIT_JAVA_FLAGS", "" );
   bi_put( "BENCHIT_JAVA_HOME", "" );
   bi_put( "BENCHIT_KERNELBINARY", "/home/fjb/gwr/BenchIT/bin/AArch64.memory_bandwidth.C.pthread.SIMD.single-reader.0" );
   bi_put( "BENCHIT_KERNELBINARY_ARGS", " " );
   bi_put( "BENCHIT_KERNELNAME", "AArch64.memory_bandwidth.C.pthread.SIMD.single-reader" );
   bi_put( "BENCHIT_KERNEL_ALLOC", "L" );
   bi_put( "BENCHIT_KERNEL_ALWAYS_FLUSH_CPU0", "1" );
   bi_put( "BENCHIT_KERNEL_AVX_STARTUP_REG_OPS", "0" );
   bi_put( "BENCHIT_KERNEL_BURST_LENGTH", "8" );
   bi_put( "BENCHIT_KERNEL_COMMENT", "single threaded memory bandwidth (load)" );
   bi_put( "BENCHIT_KERNEL_CPU_LIST", "0,1,4,32" );
   bi_put( "BENCHIT_KERNEL_DISABLE_CLFLUSH", "0" );
   bi_put( "BENCHIT_KERNEL_ENABLE_CODE_PREFETCH", "0" );
   bi_put( "BENCHIT_KERNEL_ENABLE_PAPI", "0" );
   bi_put( "BENCHIT_KERNEL_FLUSH_ACCESSES", "2" );
   bi_put( "BENCHIT_KERNEL_FLUSH_BUFFER", "G" );
   bi_put( "BENCHIT_KERNEL_FLUSH_EXTRA", "20" );
   bi_put( "BENCHIT_KERNEL_FLUSH_L1", "1" );
   bi_put( "BENCHIT_KERNEL_FLUSH_L2", "1" );
   bi_put( "BENCHIT_KERNEL_FLUSH_L3", "1" );
   bi_put( "BENCHIT_KERNEL_FLUSH_L4", "1" );
   bi_put( "BENCHIT_KERNEL_FLUSH_MODE", "E" );
   bi_put( "BENCHIT_KERNEL_FLUSH_SHARED_CPU", "0" );
   bi_put( "BENCHIT_KERNEL_HUGEPAGES", "0" );
   bi_put( "BENCHIT_KERNEL_HUGEPAGE_DIR", "/mnt/huge" );
   bi_put( "BENCHIT_KERNEL_INSTRUCTION", "ldr128" );
   bi_put( "BENCHIT_KERNEL_LINE_PREFETCH", "0" );
   bi_put( "BENCHIT_KERNEL_LOOP_OVERHEAD_COMPENSATION", "enabled" );
   bi_put( "BENCHIT_KERNEL_MAX", "200000000" );
   bi_put( "BENCHIT_KERNEL_MEM_BIND", "0,1,7-15/4,23-127/8" );
   bi_put( "BENCHIT_KERNEL_MIN", "32000" );
   bi_put( "BENCHIT_KERNEL_NOPCOUNT", "0" );
   bi_put( "BENCHIT_KERNEL_OFFSET", "0" );
   bi_put( "BENCHIT_KERNEL_PAPI_COUNTERS", "PAPI_L2_TCM" );
   bi_put( "BENCHIT_KERNEL_RANDOM", "0" );
   bi_put( "BENCHIT_KERNEL_RUNS", "6" );
   bi_put( "BENCHIT_KERNEL_SERIALIZATION", "mfence" );
   bi_put( "BENCHIT_KERNEL_SHARED_CPU_LIST", "16" );
   bi_put( "BENCHIT_KERNEL_STEPS", "70" );
   bi_put( "BENCHIT_KERNEL_TIMEOUT", "3600" );
   bi_put( "BENCHIT_KERNEL_USE_ACCESSES", "4" );
   bi_put( "BENCHIT_KERNEL_USE_DIRECTION", "FIFO" );
   bi_put( "BENCHIT_KERNEL_USE_MODE", "S" );
   bi_put( "BENCHIT_LD_LIBRARY_PATH", "/home/fjb/gwr/BenchIT/jbi/jni" );
   bi_put( "BENCHIT_LIB_ACML", " -lacml" );
   bi_put( "BENCHIT_LIB_ATLAS", " -latlas" );
   bi_put( "BENCHIT_LIB_BLAS", "-lblas" );
   bi_put( "BENCHIT_LIB_ESSL", " -lessl" );
   bi_put( "BENCHIT_LIB_FFTW3", " -lfftw3" );
   bi_put( "BENCHIT_LIB_MKL", " -lmkl" );
   bi_put( "BENCHIT_LIB_MPI", "" );
   bi_put( "BENCHIT_LIB_PAPI", "" );
   bi_put( "BENCHIT_LIB_PCL", "" );
   bi_put( "BENCHIT_LIB_PTHREAD", "-lpthread" );
   bi_put( "BENCHIT_LIB_PVM", "" );
   bi_put( "BENCHIT_LIB_SCSL", " -lscsl" );
   bi_put( "BENCHIT_LOCAL_CC", "gcc" );
   bi_put( "BENCHIT_LOCAL_CC_C_FLAGS", "" );
   bi_put( "BENCHIT_LOCAL_CC_L_FLAGS", "-lm" );
   bi_put( "BENCHIT_MANDATORY_FILES", "benchit.c interface.h tools/envhashbuilder.c tools/bienvhash.template.c tools/bienvhash.h tools/stringlib.c tools/stringlib.h " );
   bi_put( "BENCHIT_MPICC", "gcc" );
   bi_put( "BENCHIT_MPICC_C_FLAGS", "" );
   bi_put( "BENCHIT_MPICC_C_FLAGS_HIGH", "-O3" );
   bi_put( "BENCHIT_MPICC_C_FLAGS_OMP", "" );
   bi_put( "BENCHIT_MPICC_C_FLAGS_STD", "-O2" );
   bi_put( "BENCHIT_MPICC_LD", "gcc" );
   bi_put( "BENCHIT_MPICC_L_FLAGS", "-lm -lmpi" );
   bi_put( "BENCHIT_MPIF77", "" );
   bi_put( "BENCHIT_MPIF77_C_FLAGS", "" );
   bi_put( "BENCHIT_MPIF77_C_FLAGS_HIGH", "" );
   bi_put( "BENCHIT_MPIF77_C_FLAGS_OMP", "" );
   bi_put( "BENCHIT_MPIF77_C_FLAGS_STD", "" );
   bi_put( "BENCHIT_MPIF77_LD", "" );
   bi_put( "BENCHIT_MPIF77_L_FLAGS", "" );
   bi_put( "BENCHIT_MPIRUN", "mpirun" );
   bi_put( "BENCHIT_NODENAME", "KP04" );
   bi_put( "BENCHIT_NUM_CPUS", "1" );
   bi_put( "BENCHIT_NUM_PROCESSES", "" );
   bi_put( "BENCHIT_NUM_THREADS_PER_PROCESS", "" );
   bi_put( "BENCHIT_OPTIONAL_FILES", "LOCALDEFS/PROTOTYPE_input_architecture LOCALDEFS/PROTOTYPE_input_display " );
   bi_put( "BENCHIT_PARAMETER_FILE", "/home/fjb/gwr/BenchIT/kernel/AArch64/memory_bandwidth/C/pthread/SIMD/single-reader/PARAMETERS" );
   bi_put( "BENCHIT_PROGRESS_DIR", "progress" );
   bi_put( "BENCHIT_RUN_ACCURACY", "2" );
   bi_put( "BENCHIT_RUN_COREDUMPLIMIT", "0" );
   bi_put( "BENCHIT_RUN_EMAIL_ADDRESS", "" );
   bi_put( "BENCHIT_RUN_LINEAR", "1" );
   bi_put( "BENCHIT_RUN_MAX_MEMORY", "0" );
   bi_put( "BENCHIT_RUN_OUTPUT_DIR", "/home/fjb/gwr/BenchIT/output" );
   bi_put( "BENCHIT_RUN_QUEUENAME", "" );
   bi_put( "BENCHIT_RUN_REDIRECT_CONSOLE", "" );
   bi_put( "BENCHIT_RUN_TEST", "0" );
   bi_put( "BENCHIT_RUN_TIMELIMIT", "3600" );
   bi_put( "BENCHIT_USE_VAMPIR_TRACE", "0" );
   bi_put( "BR", "0" );
   bi_put( "COMMENT", "" );
   bi_put( "COMPILE_GLOBAL", "1" );
   bi_put( "CONFIGURE_MODE", "COMPILE" );
   bi_put( "CURDIR", "/home/fjb/gwr/BenchIT" );
   bi_put( "DBUS_SESSION_BUS_ADDRESS", "unix:path=/run/user/1006/bus" );
   bi_put( "DIRSTACK", "()" );
   bi_put( "ENV", "/usr/share/Modules/init/profile.sh" );
   bi_put( "EUID", "1006" );
   bi_put( "GROUPS", "()" );
   bi_put( "HISTCONTROL", "ignoredups" );
   bi_put( "HISTSIZE", "1000" );
   bi_put( "HLL", "C" );
   bi_put( "HOME", "/home/fjb" );
   bi_put( "HOSTNAME", "KP04" );
   bi_put( "HOSTTYPE", "aarch64" );
   bi_put( "IFS", "' 	" );
   bi_put( "KERNELBASEDIR", "/home/fjb/gwr/BenchIT/kernel" );
   bi_put( "KERNELDIR", "/home/fjb/gwr/BenchIT/kernel/AArch64/memory_bandwidth/C/pthread/SIMD/single-reader" );
   bi_put( "KERNELNAME_FULL", "" );
   bi_put( "KERNEL_CC", "gcc" );
   bi_put( "LD_LIBRARY_PATH", ":/usr/local/lib" );
   bi_put( "LESSOPEN", "||/usr/bin/lesspipe.sh %s" );
   bi_put( "LOADEDMODULES", "" );
   bi_put( "LOCAL_BENCHITC_COMPILER", "gcc  -O2  -DLINEAR_MEASUREMENT -DDEBUGLEVEL=0" );
   bi_put( "LOCAL_KERNEL_COMPILER", "gcc" );
   bi_put( "LOCAL_KERNEL_COMPILERFLAGS", " -DNOPCOUNT=0 -DLINE_PREFETCH=0 -O0 -I. -I/home/fjb/gwr/BenchIT -I/home/fjb/gwr/BenchIT/tools -I/home/fjb/gwr/BenchIT/tools/hw_detect -DFORCE_MFENCE -DAVX_STARTUP_REG_OPS=0" );
   bi_put( "LOCAL_LINKERFLAGS", "-lm -lpthread -lnuma" );
   bi_put( "LOGNAME", "fjb" );
   bi_put( "LS_COLORS", "rs=0:di=38;5;33:ln=38;5;51:mh=00:pi=40;38;5;11:so=38;5;13:do=38;5;5:bd=48;5;232;38;5;11:cd=48;5;232;38;5;3:or=48;5;232;38;5;9:mi=01;05;37;41:su=48;5;196;38;5;15:sg=48;5;11;38;5;16:ca=48;5;196;38;5;226:tw=48;5;10;38;5;16:ow=48;5;10;38;5;21:st=48;5;21;38;5;15:ex=38;5;40:*.tar=38;5;9:*.tgz=38;5;9:*.arc=38;5;9:*.arj=38;5;9:*.taz=38;5;9:*.lha=38;5;9:*.lz4=38;5;9:*.lzh=38;5;9:*.lzma=38;5;9:*.tlz=38;5;9:*.txz=38;5;9:*.tzo=38;5;9:*.t7z=38;5;9:*.zip=38;5;9:*.z=38;5;9:*.dz=38;5;9:*.gz=38;5;9:*.lrz=38;5;9:*.lz=38;5;9:*.lzo=38;5;9:*.xz=38;5;9:*.zst=38;5;9:*.tzst=38;5;9:*.bz2=38;5;9:*.bz=38;5;9:*.tbz=38;5;9:*.tbz2=38;5;9:*.tz=38;5;9:*.deb=38;5;9:*.rpm=38;5;9:*.jar=38;5;9:*.war=38;5;9:*.ear=38;5;9:*.sar=38;5;9:*.rar=38;5;9:*.alz=38;5;9:*.ace=38;5;9:*.zoo=38;5;9:*.cpio=38;5;9:*.7z=38;5;9:*.rz=38;5;9:*.cab=38;5;9:*.wim=38;5;9:*.swm=38;5;9:*.dwm=38;5;9:*.esd=38;5;9:*.jpg=38;5;13:*.jpeg=38;5;13:*.mjpg=38;5;13:*.mjpeg=38;5;13:*.gif=38;5;13:*.bmp=38;5;13:*.pbm=38;5;13:*.pgm=38;5;13:*.ppm=38;5;13:*.tga=38;5;13:*.xbm=38;5;13:*.xpm=38;5;13:*.tif=38;5;13:*.tiff=38;5;13:*.png=38;5;13:*.svg=38;5;13:*.svgz=38;5;13:*.mng=38;5;13:*.pcx=38;5;13:*.mov=38;5;13:*.mpg=38;5;13:*.mpeg=38;5;13:*.m2v=38;5;13:*.mkv=38;5;13:*.webm=38;5;13:*.ogm=38;5;13:*.mp4=38;5;13:*.m4v=38;5;13:*.mp4v=38;5;13:*.vob=38;5;13:*.qt=38;5;13:*.nuv=38;5;13:*.wmv=38;5;13:*.asf=38;5;13:*.rm=38;5;13:*.rmvb=38;5;13:*.flc=38;5;13:*.avi=38;5;13:*.fli=38;5;13:*.flv=38;5;13:*.gl=38;5;13:*.dl=38;5;13:*.xcf=38;5;13:*.xwd=38;5;13:*.yuv=38;5;13:*.cgm=38;5;13:*.emf=38;5;13:*.ogv=38;5;13:*.ogx=38;5;13:*.aac=38;5;45:*.au=38;5;45:*.flac=38;5;45:*.m4a=38;5;45:*.mid=38;5;45:*.midi=38;5;45:*.mka=38;5;45:*.mp3=38;5;45:*.mpc=38;5;45:*.ogg=38;5;45:*.ra=38;5;45:*.wav=38;5;45:*.oga=38;5;45:*.opus=38;5;45:*.spx=38;5;45:*.xspf=38;5;45:" );
   bi_put( "MACHTYPE", "aarch64-redhat-linux-gnu" );
   bi_put( "MAIL", "/var/spool/mail/fjb" );
   bi_put( "MANPATH", ":/usr/local/mpich-3.3.2/bin/" );
   bi_put( "MODULEPATH", "/usr/share/Modules/modulefiles:/etc/modulefiles:/usr/share/modulefiles" );
   bi_put( "MODULEPATH_modshare", "/usr/share/modulefiles:1:/etc/modulefiles:1:/usr/share/Modules/modulefiles:1" );
   bi_put( "MODULESHOME", "/usr/share/Modules" );
   bi_put( "MODULES_CMD", "/usr/share/Modules/libexec/modulecmd.tcl" );
   bi_put( "MODULES_RUN_QUARANTINE", "LD_LIBRARY_PATH" );
   bi_put( "OLDCWD", "/home/fjb/gwr/BenchIT" );
   bi_put( "OLDIR", "/home/fjb/gwr/BenchIT" );
   bi_put( "OLDPWD", "/home/fjb/gwr/BenchIT" );
   bi_put( "OMP_DYNAMIC", "FALSE" );
   bi_put( "OMP_NESTED", "FALSE" );
   bi_put( "OMP_NUM_THREADS", "1" );
   bi_put( "OPTERR", "1" );
   bi_put( "OPTIND", "1" );
   bi_put( "OSTYPE", "linux-gnu" );
   bi_put( "PATH", "/home/fjb/gwr/BenchIT/tools:/home/fjb/.local/bin:/home/fjb/bin:/usr/share/Modules/bin:/usr/local/bin:/usr/bin:/usr/local/sbin:/usr/sbin:/usr/local/mpich-3.3.2/bin/" );
   bi_put( "PIPESTATUS", "([0]=\"0\")" );
   bi_put( "POSIXLY_CORRECT", "y" );
   bi_put( "PPID", "54930" );
   bi_put( "PS4", "+ " );
   bi_put( "PWD", "/home/fjb/gwr/BenchIT/tools" );
   bi_put( "SCRIPTNAME", "COMPILE.SH" );
   bi_put( "SELINUX_LEVEL_REQUESTED", "" );
   bi_put( "SELINUX_ROLE_REQUESTED", "" );
   bi_put( "SELINUX_USE_CURRENT_RANGE", "" );
   bi_put( "SHELL", "/bin/bash" );
   bi_put( "SHELLOPTS", "braceexpand:hashall:interactive-comments:posix" );
   bi_put( "SHELLSCRIPT_DEBUG", "0" );
   bi_put( "SHLVL", "2" );
   bi_put( "SSH_CLIENT", "202.197.12.178 53716 22" );
   bi_put( "SSH_CONNECTION", "202.197.12.178 53716 202.197.20.204 22" );
   bi_put( "SSH_TTY", "/dev/pts/2" );
   bi_put( "S_COLORS", "auto" );
   bi_put( "TERM", "xterm-256color" );
   bi_put( "UID", "1006" );
   bi_put( "USER", "fjb" );
   bi_put( "XDG_RUNTIME_DIR", "/run/user/1006" );
   bi_put( "XDG_SESSION_ID", "32380" );
   bi_put( "_", "/home/fjb/gwr/BenchIT/tools/" );
   bi_put( "_CMDLINE_VARLIST", "BENCHIT_KERNELBINARY BENCHIT_KERNELBINARY_ARGS BENCHIT_CMDLINE_ARG_FILENAME_COMMENT BENCHIT_CMDLINE_ARG_PARAMETER_FILE BENCHIT_CMDLINE_ARG_IGNORE_PARAMETER_FILE BENCHIT_NODENAME BENCHIT_CROSSCOMPILE BENCHIT_CMDLINE_ARG_NUM_CPUS BENCHIT_CMDLINE_ARG_NUM_PROCESSES BENCHIT_CMDLINE_ARG_NUM_THREADS_PER_PROCESS BENCHIT_CMDLINE_ARG_RUN_CLEAN BENCHIT_CMDLINE_ARG_RUN_COREDUMPLIMIT BENCHIT_CMDLINE_ARG_RUN_EMAIL_ADDRESS BENCHIT_CMDLINE_ARG_RUN_MAX_MEMORY BENCHIT_CMDLINE_ARG_RUN_QUEUENAME BENCHIT_CMDLINE_ARG_RUN_QUEUETIMELIMIT BENCHIT_CMDLINE_ARG_RUN_REDIRECT_CONSOLE BENCHIT_CMDLINE_ARG_RUN_TEST BENCHIT_CMDLINE_ARG_RUN_USE_MPI BENCHIT_CMDLINE_ARG_RUN_USE_OPENMP " );
   bi_put( "_VARLIST", "'BENCHITROOT" );
   bi_put( "myfile", "tools/stringlib.h" );
   bi_put( "myval", "" );
   bi_put( "myvar", "BENCHIT_CMDLINE_ARG_RUN_USE_OPENMP" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_ALIGNED_MEMORY_H", "NO REVISION (Tue Dec 22 19:35:27 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_HW_DETECT", "NO REVISION, UNABLE TO READ (Tue Dec 22 20:45:02 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_BIENVHASH_H", "NO REVISION (Tue Dec 22 19:35:27 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_FEATURES", "NO REVISION (Tue Dec 22 19:35:27 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_FIRSTTIME", "NO REVISION (Tue Dec 22 19:35:27 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_BENCHSCRIPT_H", "NO REVISION (Tue Dec 22 19:35:27 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_COMPILERVERSION", "NO REVISION, UNABLE TO READ (Tue Dec 22 19:35:27 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_LOC_CONVERT_SH", "NO REVISION (Tue Dec 22 19:35:27 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_REFERENCE_RUN_SH", "NO REVISION (Tue Dec 22 19:35:27 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_CONFIGURE", "NO REVISION (Tue Dec 22 19:35:27 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_QUICKVIEW_SH", "NO REVISION (Tue Dec 22 19:35:27 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_CHANGE_SH_SH", "NO REVISION (Tue Dec 22 19:35:27 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_STRINGLIB_C", "NO REVISION (Tue Dec 22 19:35:27 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_BIENVHASH_TEMPLATE_C", "NO REVISION (Tue Dec 22 19:35:27 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_HELPER_SH", "1.14 (Tue Dec 22 19:35:27 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_BMERGE_SH", "NO REVISION (Tue Dec 22 19:35:27 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_BIENVHASH_TEMPLATE_JAVA", "NO REVISION (Tue Dec 22 19:35:27 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_BENCHSCRIPT_C", "NO REVISION (Tue Dec 22 19:35:27 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_ENVIRONMENTS", "NO REVISION, UNABLE TO READ (Tue Dec 22 19:35:28 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_LOC_REPL", "NO REVISION (Tue Dec 22 19:35:28 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_ENVHASHBUILDER_C", "NO REVISION (Tue Dec 22 19:35:28 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_CMDLINEPARAMS", "NO REVISION (Tue Dec 22 19:35:28 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_FILEVERSION_C", "*/ (Tue Dec 22 19:35:28 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_ERROR_H", "NO REVISION (Tue Dec 22 19:35:28 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_STRINGLIB_H", "NO REVISION (Tue Dec 22 19:35:28 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_ENVHASHBUILDER", "NO REVISION (Tue Dec 22 21:54:54 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_FILEVERSION", "NO REVISION (Tue Dec 22 21:54:54 2020)" );
   bi_put( "BENCHIT_KERNEL_FILE_VERSION_TMP_ENV", "NO REVISION (Tue Dec 22 21:54:54 2020)" );
}
