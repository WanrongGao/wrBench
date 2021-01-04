#!/bin/sh
#####################################################################
# BenchIT - Performance Measurement for Scientific Applications
# Contact: developer@benchit.org
#
# $Id: COMPILE.SH 1 2009-09-11 12:26:19Z william $
# $URL: svn+ssh://molka@rupert.zih.tu-dresden.de/svn-base/benchit-root/BenchITv6/COMPILE.SH $
# For license details see COPYING in the package base directory
#####################################################################
# Shellscript compiling kernels
#
#  The script sets the environment-variable CONFIGURE_MODE to COMPILE
#  and then starts tools/configure which sets up the variables
#  needed by the COMPILE.SH's of the kernels.
#####################################################################

# Go to COMPILE.SH directory
cd "`dirname ${0}`" || exit 1

# Set configure mode
CONFIGURE_MODE="COMPILE"

# Switch for cmdlineparams to enable scan for missing BENCHIT_KERNELBINARY
COMPILE_GLOBAL=1

# Load the definitions
. ./tools/configure

#### for compiler version uncomment the following line
# . ./tools/compilerversion/getCompilerVersion

if [ "${KERNELDIR}" = "${BENCHITROOT}" ]; then
	# Global COMPILE.SH was called without a target
	printf "\nERROR: No kernel for compile specified.\n"
	printf "Use COMPILE.SH -h to view the syntax of this command.\n\n"
	exit 1
fi

# figure out compiler versions
if [ ${BENCHIT_CC} ] && [ "${BENCHIT_CC}" != "" ] && { "${BENCHITROOT}/tools/features" have ${BENCHIT_CC} ;}
then
#### for compiler version uncomment the following lines
#  	getCompilerVersion ${BENCHIT_CC}
#  	if [ "${?}" -eq 0 ]
#  	then
#  		BENCHIT_CC_COMPILER_VERSION="${BENCHIT_COMPILER_VERSION}"
#  	fi
#  export BENCHIT_CC_COMPILER_VERSION
####
  export BENCHIT_COMPILETIME_CC="${BENCHIT_CC}"
fi

if [ "${HLL}" = "CXX" ] && [ ${BENCHIT_CXX} ] && [ "${BENCHIT_CXX}" != "" ] && { "${BENCHITROOT}/tools/features" have ${BENCHIT_CXX} ;}
then
#### for compiler version uncomment the following lines
#  	getCompilerVersion ${BENCHIT_CXX}
#  	if [ "${?}" -eq 0 ]
#  	then
#  		BENCHIT_CXX_COMPILER_VERSION="${BENCHIT_COMPILER_VERSION}"
#  	fi
#  export BENCHIT_CXX_COMPILER_VERSION
####
  export BENCHIT_COMPILETIME_CXX="${BENCHIT_CXX}"
fi

if [ "${HLL}" = "C++" ] && [ ${BENCHIT_CXX} ] && [ "${BENCHIT_CXX}" != "" ] && { "${BENCHITROOT}/tools/features" have ${BENCHIT_CXX} ;}
then
#### for compiler version uncomment the following lines
#  	getCompilerVersion ${BENCHIT_CXX}
#  	if [ "${?}" -eq 0 ]
#  	then
#  		BENCHIT_CXX_COMPILER_VERSION="${BENCHIT_COMPILER_VERSION}"
#  	fi
#  export BENCHIT_CXX_COMPILER_VERSION
####
  export BENCHIT_COMPILETIME_CXX="${BENCHIT_CXX}"
fi

if [ "${HLL}" = "CPP" ] && [ ${BENCHIT_CXX} ] && [ "${BENCHIT_CXX}" != "" ] && { "${BENCHITROOT}/tools/features" have ${BENCHIT_CXX} ;}
then
#### for compiler version uncomment the following lines
#  	getCompilerVersion ${BENCHIT_CXX}
#  	if [ "${?}" -eq 0 ]
#  	then
#  		BENCHIT_CXX_COMPILER_VERSION="${BENCHIT_COMPILER_VERSION}"
#  	fi
#  export BENCHIT_CXX_COMPILER_VERSION
####
  export BENCHIT_COMPILETIME_CXX="${BENCHIT_CXX}"
fi

if [ "${HLL}" = "F77" ] && [ ${BENCHIT_F77} ] && [ "${BENCHIT_F77}" != "" ] && { "${BENCHITROOT}/tools/features" have ${BENCHIT_F77} ;}
then
#### for compiler version uncomment the following lines
#  	getCompilerVersion ${BENCHIT_F77}
#  	if [ "${?}" -eq 0 ]
#  	then
#  		BENCHIT_F77_COMPILER_VERSION="${BENCHIT_COMPILER_VERSION}"
#  	fi
#  export BENCHIT_F77_COMPILER_VERSION
####
  export BENCHIT_COMPILETIME_F77="${BENCHIT_F77}"
fi

if [ "${HLL}" = "F90" ] && [ ${BENCHIT_F90} ] && [ "${BENCHIT_F90}" != "" ] && { "${BENCHITROOT}/tools/features" have ${BENCHIT_F90} ;}
then
#### for compiler version uncomment the following lines
#  	getCompilerVersion ${BENCHIT_F90}
#  	if [ "${?}" -eq 0 ]
#  	then
#  		BENCHIT_F90_COMPILER_VERSION="${BENCHIT_COMPILER_VERSION}"
#  	fi
#  export BENCHIT_F90_COMPILER_VERSION
####
  export BENCHIT_COMPILETIME_F90="${BENCHIT_F90}"
fi

if [ "${HLL}" = "F95" ] && [ ${BENCHIT_F95} ] && [ "${BENCHIT_F95}" != "" ] && { "${BENCHITROOT}/tools/features" have ${BENCHIT_F95} ;}
then
#### for compiler version uncomment the following lines
#  	getCompilerVersion ${BENCHIT_F95}
#  	if [ "${?}" -eq 0 ]
#  	then
#  		BENCHIT_F95_COMPILER_VERSION="${BENCHIT_COMPILER_VERSION}"
#  	fi
#  export BENCHIT_F95_COMPILER_VERSION
####
  export BENCHIT_COMPILETIME_F95="${BENCHIT_F95}"
fi

# Source kernel's COMPILE.SH
. "${KERNELDIR}/COMPILE.SH"

