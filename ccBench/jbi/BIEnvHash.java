/********************************************************************
 * BenchIT - Performance Measurement for Scientific Applications
 * Contact: developer@benchit.org
 *
 * $Id: BIEnvHash.template.java 1 2009-09-11 12:26:19Z william $
 * $URL: svn+ssh://molka@rupert.zih.tu-dresden.de/svn-base/benchit-root/BenchITv6/tools/BIEnvHash.template.java $
 * For license details see COPYING in the package base directory
 *******************************************************************/
import java.util.HashMap;
import java.util.Iterator;
import java.io.*;

public class BIEnvHash
{
   private static BIEnvHash instance = null;
   private HashMap table = null;
   private BIEnvHash()
   {
      bi_initTable();
      bi_fillTable();
   }
   public static BIEnvHash getInstance()
   {
      if ( instance == null ) instance = new BIEnvHash();
      return instance;
   }
   /** Puts a new key value mapping into the map and returns the
     * previously assigned value to the key, or null, if it's a
     * new mapping. */
   public String bi_setEnv( String key, String value )
   {
      return bi_put( key, value );
   }
   /** Puts a new key value mapping into the map and returns the
     * previously assigned value to the key, or null, if it's a
     * new mapping. */
   public String bi_put( String key, String value )
   {
      if ( table == null ) bi_initTable();
      Object ret = table.put( key, value );
      if ( ret == null ) return null;
      return (String)ret;
   }
   public String bi_getEnv( String key )
   {
      if (System.getenv(key)!=null) return System.getenv(key);
      return bi_get( key );
   }
   public String bi_get( String key )
   {
      if ( table == null ) return null;
      return (String)(table.get( key ));
   }
   public void bi_dumpTable()
   {
      if ( table == null ) return;
      printf( "\nHashtable dump of all known environment variables at compiletime:" );
      printf( "\n Key            | Value" );
      printf( "\n----------------------------------" );
      Iterator it = table.keySet().iterator();
      while ( it.hasNext() )
      {
         String key = (String)(it.next());
         String value = (String)(table.get( key ));
         printf( "\n " + key + " | " + value );
      }
      printf( "\n" + bi_size() + " entries in total.\n" );
      flush();
   }
   public void dumpToOutputBuffer( StringBuffer outputBuffer )
   {
      outputBuffer.append( "beginofenvironmentvariables\n" );
      Iterator it = table.keySet().iterator();
      while ( it.hasNext() )
      {
         String key = (String)(it.next());
         String value = (String)(table.get( key ));
         StringBuffer b = new StringBuffer();
         for ( int i = 0; i < value.length(); i++ )
         {
            if ( value.charAt( i ) == '"' ) {
               /* check equal number of escape chars->insert escape char */
               int cnt = 0;
               for ( int j = i - 1;  j >= 0; j-- ) {
                  if ( value.charAt( j ) == '\\' ) cnt++;
               }
               /* escape quote */
               if ( cnt % 2 == 0 ) {
                  b.append( '\\' );
               }
            }
            b.append( value.charAt( i ) );
         }
         value = b.toString();
         outputBuffer.append( key + "=\"" + value + "\"\n" );
      }
      outputBuffer.append( "endofenvironmentvariables\n" );
   }
   public int bi_size()
   {
      if ( table != null ) return table.size();
      return -1;
   }
   public void bi_initTable()
   {
      table = new HashMap( 1009 );
   }
   private void printf( String str )
   {
      System.out.print( str );
   }
   private void flush()
   {
      System.out.flush();
      System.err.flush();
   }
   /** Takes a PARAMETER file as argument and adds it's variables
     * to the environment HashMap. */
   public boolean bi_readParameterFile( String fileName )
   {
      boolean retval = false;
      File textFile = null;
      try {
         textFile = new File( fileName );
      }
      catch ( NullPointerException npe ) {
         return retval;
      }
      if ( textFile != null ) {
         if ( textFile.exists() ) {
            FileReader in;
            char ch = '\0';
            int size = (int)(textFile.length());
            int read = 0;
            char inData[] = new char[size + 1];
            try {
               in = new FileReader( textFile );
               in.read( inData, 0, size );
               in.close();
            }
            catch( IOException ex ) {
               return retval;
            } // scan inData for environment variables
            int scanned = 0;
            int it = 0;
            ch = inData[scanned];
            StringBuffer buffer = null;
            while ( scanned < size ) {
               buffer = new StringBuffer();
               // read a line
               while ( ch != '\n' ) {
                  if ( ch != '\r' ) {
                     buffer.append( ch );
                  }
                  scanned += 1;
                  ch = inData[scanned];
               }
               // trim leading and trailing white spaces
               String lineValue = buffer.toString().trim();
               // now check the line
               int eqIndex = -1;
               try {
                  eqIndex = lineValue.indexOf( "=" );
               }
               catch ( NullPointerException npe ) {
                  return retval;
               }
               // only check lines containing an equals sign, no $ sign,
               // and the start with a capital letter
               if ( eqIndex >= 0 && lineValue.indexOf( "$" ) < 0 &&
                  lineValue.substring( 0, 1 ).matches( "[A-Z]" ) ) {
                  String varname = null;
                  String varvalue = null;
                  try {
                     varname = lineValue.substring( 0 , eqIndex );
                     varvalue = lineValue.substring( eqIndex + 1 );
                  }
                  catch ( IndexOutOfBoundsException ioobe ) {
                  }
                  if ( ( varname != null ) && ( varvalue != null ) ) {
                     boolean st1=false, en1=false, st2=false, en2=false;
                     boolean st3=false, en3=false;
                     try {
                        st1=varvalue.startsWith( "'" );
                        en1=varvalue.endsWith( "'" );
                        st2=varvalue.startsWith( "(" );
                        en2=varvalue.endsWith( ")" );
                        st3=varvalue.startsWith( "\"" );
                        en3=varvalue.endsWith( "\"" );
                     }
                     catch ( NullPointerException npe ) {
                        return retval;
                     }
                     if ( ( ( st1 == en1 ) && ( ( st1 != st2 ) && ( st1!=st3 ) ) ) ||
                          ( ( st2 == en2 ) && ( ( st2 != st1 ) && ( st2!=st3 ) ) ) ||
                          ( ( st3 == en3 ) && ( ( st3 != st1 ) && ( st3!=st2 ) ) ) ||
                          ( ( st1 == en1 ) && ( st2 == en2 ) && ( st3 == en3 )
                          && ( st1 == st2 ) && ( st2 == st3 ) && ( st3 == false ) ) ) {
                        table.put( varname, varvalue );
                     }
                  }
               }
               scanned += 1;
               ch = inData[scanned];
               it++;
            }
         }
         else {
            System.err.println( "BenchIT: PARAMETER file \"" + fileName
               + "\" doesn't exist." );
            return retval;
         }
      }
      else {
         System.err.println( "BenchIT: PARAMETER file \"" + fileName
               + "\" not found." );
         return retval;
      }
      return true;
   }
   /* special case: generated from outside and code will be
      appended to this file */
   /** Fills the table with predefined content. */
   public void bi_fillTable()
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
}
