/*************************************************************************
** Written by Thomas Richter (THOR Software - thor@math.tu-berlin.de)   **
** Sponsored by Accusoft Corporation, Tampa, FL and                     **
** the Computing Center of the University of Stuttgart                  **
**************************************************************************

The copyright in this software is being made available under the
license included below. This software may be subject to other third
party and contributor rights, including patent rights, and no such
rights are granted under this license.
 
Copyright (c) 2013-2017, ISO
All rights reserved.

This software module was originally contributed by the parties as
listed below in the course of development of the ISO/IEC 18477 (JPEG
XT) standard for validation and reference purposes:

- University of Stuttgart
- Accusoft

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
  * Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  * Neither the name of the University of Stuttgart or Accusoft nor
    the names of its contributors may be used to endorse or promote
    products derived from this software without specific prior written
    permission.
  * Redistributed products derived from this software must conform to
    ISO/IEC 18477 (JPEG XT) except that non-commercial redistribution
    for research and for furtherance of ISO/IEC standards is permitted.
    Otherwise, contact the contributing parties for any other
    redistribution rights for products derived from this software.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*************************************************************************/
/*
** This header provides the main function.
** This main function is only a demo, it is not part of the libjpeg code.
** It is here to serve as an entry point for the command line image
** compressor.
**
** $Id: main.cpp,v 1.211 2017/02/21 15:48:17 thor Exp $
**
*/

/// Includes
#include "cmd/main.hpp"
#include "std/stdio.hpp"
#include "std/stdlib.hpp"
#include "std/string.hpp"
#include "std/math.hpp"
#include "tools/environment.hpp"
#include "tools/traits.hpp"
#include "interface/hooks.hpp"
#include "interface/tagitem.hpp"
#include "interface/parameters.hpp"
#include "interface/jpeg.hpp"
#include "tools/numerics.hpp"
#include "cmd/bitmaphook.hpp"
#include "cmd/filehook.hpp"
#include "cmd/iohelpers.hpp"
#include "cmd/tmo.hpp"
#include "cmd/defaulttmoc.hpp"
#include "cmd/encodec.hpp"
#include "cmd/encodeb.hpp"
#include "cmd/encodea.hpp"
#include "cmd/reconstruct.hpp"
///

bool oznew = false;

/// Defines
#define FIX_BITS 13
///

/// Parse the subsampling factors off.
void ParseSubsamplingFactors(UBYTE *sx,UBYTE *sy,const char *sub,int cnt)
{
  char *end;

  do {
    int x = strtol(sub,&end,0);
    *sx++ = x;
    if (*end == 'x' || *end == 'X') {
      sub = end + 1;
      int y = strtol(sub,&end,0);
      *sy++ = y;
      if (*end != ',')
        break;
      sub = end + 1;
    } else break;
  } while(--cnt);
}
///

/// ParseDouble
// Parse off a floating point value, or print an error
double ParseDouble(int &argc,char **&argv)
{
  double v;
  char *endptr;
  
  if (argv[2] == NULL) {
    fprintf(stderr,"%s expected a numeric argument.\n",argv[1]);
    exit(25);
  }

  v = strtod(argv[2],&endptr);

  if (*endptr) {
    fprintf(stderr,"%s expected a numeric argument, not %s.\n",argv[1],argv[2]);
    exit(25);
  }

  argc -= 2;
  argv += 2;

  return v;
}
///

/// ParseInt
// Parse off an integer, return it, or print an error.
int ParseInt(int &argc,char **&argv)
{
  long int v;
  char *endptr;
  
  if (argv[2] == NULL) {
    fprintf(stderr,"%s expected a numeric argument.\n",argv[1]);
    exit(25);
  }

  v = strtol(argv[2],&endptr,0);

  if (*endptr) {
    fprintf(stderr,"%s expected a numeric argument, not %s.\n",argv[1],argv[2]);
    exit(25);
  }

  argc -= 2;
  argv += 2;

  return v;
}
///

/// ParseString
// Parse of a string argument, return it, or print an error
const char *ParseString(int &argc,char **&argv)
{
  const char *v;
  
  if (argv[2] == NULL) {
    fprintf(stderr,"%s expects a string argument.\n",argv[1]);
    exit(25);
  }

  v = argv[2];

  argc -= 2;
  argv += 2;

  return v;
}
///

/// PrintLicense
// Print out the license of the codec.
void PrintLicense(void)
{

#if ISO_CODE
  /*printf(""
         "This software module was originally contributed by the parties as\n"
         "listed below in the course of development of the ISO/IEC 18477 (JPEG\n"
         "XT) standard for validation and reference purposes:\n"
         "\n"
         "- University of Stuttgart\n"
         "- Accusoft\n"
         "\n"
         "Redistribution and use in source and binary forms, with or without\n"
         "modification, are permitted provided that the following conditions are\n"
         "met:\n"
         "* Redistributions of source code must retain the above copyright notice,\n"
         "  this list of conditions and the following disclaimer.\n"
         "* Redistributions in binary form must reproduce the above copyright notice,\n"
         "  this list of conditions and the following disclaimer in the documentation\n"
         "  and/or other materials provided with the distribution.\n"
         "* Neither the name of the <ORGANIZATION> nor the names of its\n"
         "  contributors may be used to endorse or promote products derived from this\n"
         "  software without specific prior written permission.\n"
         "* Redistributed products derived from this software must conform to\n"
         "  ISO/IEC 18477 (JPEG XT) except that non-commercial redistribution\n"
         "  for research and for furtherance of ISO/IEC standards is permitted.\n"
         "  Otherwise, contact the contributing parties for any other\n"
         "  redistribution rights for products derived from this software.\n"
         "\n"
         "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS\n"
         "\"AS IS\" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT\n"
         "LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR\n"
         "A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT\n"
         "HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,\n"
         "SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT\n"
         "LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,\n"
         "DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY\n"
         "THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n"
         "(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE\n"
         "OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n"
         "\n\n"
         );
		 */
	printf("JPEG modified v0.1 by PhuNM");
#else
  printf(""
         "jpeg Copyright (C) 2012-2014 Thomas Richter, University of Stuttgart\n"
         "and Accusoft\n\n"
         "This program comes with ABSOLUTELY NO WARRANTY; for details see \n"
         "README.license.gpl\n"
         "This is free software, and you are welcome to redistribute it\n"
         "under certain conditions, see again README.license.gpl for details.\n\n"
         );
#endif
}
///

/// PrintUsage
// Print the usage of the program and exit
void PrintUsage(const char *progname)
{    
  printf("Usage: %s [options] source target\n"
          "default is to decode the jpeg input and write a ppm output\n"
          "use -q [1..100] or -p to enforce encoding\n\n"
          "-q quality : selects the encoding mode and defines the quality of the base image\n"
          "-Q quality : defines the quality for the extension layer\n"
          "-quality q : use a profile and part specific weighting between base and extension\n"
          "             layer quality\n"
#if ISO_CODE
          "-profile X : encode according to profile X, X = a,b,c\n"
          "             as defined in the JPEG XT standard. Without any parameter,\n"
          "             the encoder tries to figure out the profile itself.\n"
#endif
          "-r         : enable the residual codestream for HDR and lossless\n"
          "             coding, requires -q and -Q to define base and\n"
          "             enhancement layer quality.\n"
          "-r12       : use a 12 bit residual image instead of an 8 bit residual\n"
          "             image.\n"
          "-rl        : enforce a int-to-int lossless DCT in the residual domain\n"
          "             for lossless coding enabled by -Q 100\n"
          "-ro        : disable the DCT in the residual domain, quantize spatially for\n"
          "             near-lossless coding\n"
          "-ldr file  : specifies a separate file containing the base layer\n"
          "             for encoding.\n"
          "-R bits    : specify refinement bits for the base images.\n"
          "             This works like -r but in the DCT domain.\n"
          "-rR bits   : specify refinement bits for the residual image.\n"
          "-N         : enable noise shaping of the prediction residual\n"
          "-l         : enable lossless coding without a residual image by an\n"
          "             int-to-int DCT, also requires -c and -q 100 for true lossless\n"
          "-c         : disable the RGB to YCbCr decorrelation transformation\n"
          "-xyz       : indicates that the HDR image is in the XYZ colorspace\n"
          "             note that the image is not *converted* to this space, but\n"
          "             is assumed to be encoded in this space.\n"
          "-cxyz      : similar to the above, but uses the dedicated C transformation\n"
          "             to implement a XYZ colorspace conversion.\n"
          "-sp        : use separate LUTs for each component.\n"
          "-md        : use the median instead of the center of mass\n"
          "             for constructing the inverse TMO of profile C.\n"
          "-ct        : use the center of mass instead of the median\n"
          "             for constructing the inverse TMO of profile C.\n"
          "-sm iter   : use <iter> iterations to smooth out the histogram for\n"
          "             inverse-TMO based algorithms. Default is not to smooth\n"
          "             the histogram.\n"
          "-ncl       : disable clamping of out-of-gamut colors.\n"
          "             this is automatically enabled for lossless.\n"
          "-h         : optimize the Huffman tables\n"
          "-v         : use progressive instead of sequential encoding\n"
          "             available for all coding schemes (-r,-a,-l and default)\n"
          "-qv        : use a simplified scan pattern for progressive that only\n"
          "             separates AC from DC bands and may improve the performance\n"
#if ISO_CODE
          "-ae factor : define an auto-exposure value for part 7 profile B.\n"
          "             By default, this factor is 0.6\n"
          "-e exposure: define a manual exposure value for part 7 profile B.\n"
          "             Auto-exposure, i.e. not using this parameter, is recommended.\n"
#endif
          "-g gamma   : define the exponent for the gamma for the LDR domain, or rather, for\n"
          "             mapping HDR to LDR. A suggested value is 2.4 for mapping scRGB to sRBG.\n"
          "             This option controls the base-nonlinearity that generates the\n"
          "             HDR pre-cursor image from the LDR image. It is also used in the\n"
          "             absense of -ldr (i.e. no LDR image) to tonemap the HDR input image.\n"
          "             Use -g 0 to use an approximate inverse TMO as base-nonlinearity, and\n"
          "             for tonemapping with the Reinhard operator if the LDR image is missing.\n"
#if ISO_CODE
          "-epsn eps  : define the numerator normalizer for profile B encoding,\n"
          "             defaults to 1e-7\n"
          "-epsd eps  : define the denominator normalizer for profile B endocoding,\n"
          "             defaults to 1e-7\n"
          "-lr        : encode the residual for profile B log-encoded, not gamma-encoded.\n"
#endif
          "-gf file   : define the inverse one-point L-nonlinearity on decoding from a file\n"
          "             this file contains one (ASCII encoded) digit per line, 256*2^h lines\n"
          "             in total, where h is the number of refinement bits. Each line contains\n"
          "             an (integer) output value the corresponding input is mapped to.\n"
          "-z mcus    : define the restart interval size, zero disables it\n"
          "-s WxH,... : define subsampling factors for all components\n"
          "             note that these are NOT MCU sizes\n"
          "             Default is 1x1,1x1,1x1 (444 subsampling)\n"
          "             1x1,2x2,2x2 is the 420 subsampling often used\n"
          "-sr WxH,...: define subsampling in the residual domain\n"
          "-rs        : encode the residual image in sequential (rather than the modified residual)\n"
          "             coding mode\n"
          "-rv        : encode the residual image in progressive coding mode\n"
          "-ol        : open loop encoding, residuals are based on original, not reconstructed\n"
          "-dz        : improved deadzone quantizer, may help to improve the R/D performance\n"
          "-qt n      : define the quantization table. The following tables are currently defined:\n"
          "             n = 0 the default tables from Annex K of the JPEG standard (default)\n"
          "             n = 1 a completely flat table that should be PSNR-optimal\n"
          "             n = 2 a MS-SSIM optimized table\n"
          "             n = 3 the table suggested by ImageMagick\n"
          "             n = 4 a HSV-PSNR optimized table\n"
          "             n = 5 the table from Klein, Silverstein and Carney:\n"
          "                   Relevance of human vision to JPEG-DCT compression (1992)\n"
          "             n = 6 the table from Watson, Taylor, Borthwick:\n"
          "                   DCTune perceptual optimization of compressed dental X-Rays (1997)\n"
          "             n = 7 the table from Ahumada, Watson, Peterson:\n"
          "                   A visual detection model for DCT coefficient quantization (1993)\n"
          "             n = 8 the table from Peterson, Ahumada and Watson:\n"
          "                   An improved detection model for DCT coefficient quantization (1993)\n"
          "-rqt n     : defines the quantization table for the residual stream in the same way\n"
          "-al file   : specifies a one-component pgm/pfm file that contains an alpha component\n"
          "             or the code will write the alpha component to.\n"
          "             This demo code DOES NOT implement compositing of alpha and background\n"
          "-am mode   : specifes the mode of the alpha: 1 (regular) 2 (premultiplied) 3 (matte-removal)\n"
          "-ab r,g,b  : specifies the matte (background) color for mode 3 as RGB triple\n"
          "-ar        : enable residual coding for the alpha channel, required if the\n"
          "             alpha channel is larger than 8bpp\n"
          "-ar12      : use a 12 bit residual for the alpha channel\n"
          "-aR bits   : set refinement bits in the alpha base codestream\n"
          "-arR bits  : set refinement bits in the residual alpha codestream\n"
          "-aol       : enable open loop coding for the alpha channel\n"
          "-adz       : enable the deadzone quantizer for the alpha channel\n"
          "-all       : enable lossless DCT for alpha coding\n"
          "-alo       : disable the DCT in the residual alpha channel, quantize spatially.\n"
          "-aq qu     : specify a quality for the alpha base channel (usually the only one)\n"
          "-aQ qu     : specify a quality for the alpha extension layer\n"
          "-aqt n     : specify the quantization table for the alpha channel\n"
          "-arqt n    : specify the quantization table for residual alpha\n"
          "-aquality q: specify a combined quality for both\n"
          ,progname);
}
///

/// main
int main(int argc,char **argv)
{
  int quality       = -1;
  int hdrquality    = -1;
  int maxerror      = 0;
  int levels        = 0;
  int restart       = 0;
  int lsmode        = -1; // Use JPEGLS
  int hiddenbits    = 0;  // hidden DCT bits
  int riddenbits    = 0;  // hidden bits in the residual domain
  int ahiddenbits   = 0;  // hidden DCT bits in the base alpha codestream
  int ariddenbits   = 0;  // hidden DCT bits in the residual alpha codestream.
  int resprec       = 8;  // precision in the residual domain
  int aresprec      = 8;  // precision of the residual alpha
  double gamma      = 0.0;
#if ISO_CODE
  double epsilonn   = 1e-7; // the normalizing epsilon in the numerator
  double epsilond   = 1e-7; // the normalizing epsilon in the denominator for the profile B encoder
  bool linearres    = false;
#endif
  bool pyramidal    = false;
  bool residuals    = false;
  int  colortrafo   = JPGFLAG_MATRIX_COLORTRANSFORMATION_YCBCR;
  bool lossless     = false;
  bool optimize     = false;
  bool accoding     = false;
  bool qscan        = false;
  bool progressive  = false;
  bool writednl     = false;
  bool noiseshaping = false;
  bool rprogressive = false;
  bool rsequential  = false;
  bool raccoding    = false;
  bool serms        = false;
  bool aserms       = false;
  bool abypass      = false;
  bool losslessdct  = false;
  bool dctbypass    = false;
  bool openloop     = false;
  bool deadzone     = false;
  bool lagrangian   = false;
  bool dering       = false;
  bool aopenloop    = false;
  bool adeadzone    = false;
  bool alagrangian  = false;
  bool adering      = false;
  bool xyz          = false;
  bool cxyz         = false;
  bool separate     = false;
  bool noclamp      = false;
  bool setprofile   = false;
#if ISO_CODE
  bool gammaspecd   = false;
#endif
  bool median       = true;
  int splitquality  = -1;
  int profile       = 2;    // profile C.
#if ISO_CODE
  double factor     = 0.6;  // auto-exposure for profile B
  double exposure   = -1.0; // manual exposure value for profile B.
#endif
  const char *sub       = NULL;
  const char *ressub    = NULL;
  const char *ldrsource = NULL;
  const char *lsource   = NULL;
  const char *alpha     = NULL; // source or target of the alpha plane 
  bool alpharesiduals   = false;
  int alphamode         = JPGFLAG_ALPHA_REGULAR; // alpha mode
  int matte_r = 0,matte_g = 0,matte_b = 0; // matte color for alpha.
  int alphaquality      = 70;
  int alphahdrquality   = 0;
  int alphasplitquality = -1;
  int tabletype         = 0; // quantization table types
  int residualtt        = 0;
  int alphatt           = 0;
  int residualalphatt   = 0;
  int smooth            = 0; // histogram smoothing

  PrintLicense();
  fflush(stdout);

  while(argc > 3 && argv[1][0] == '-') {
    if (!strcmp(argv[1],"-q")) {
      quality = ParseInt(argc,argv);
    } else if (!strcmp(argv[1],"-Q")) {
      hdrquality = ParseInt(argc,argv);
    } else if (!strcmp(argv[1],"-quality")) {
      splitquality = ParseInt(argc,argv);
    } else if (!strcmp(argv[1],"-profile")) {
      const char *s = ParseString(argc,argv);
      setprofile    = true;
      if (!strcmp(s,"a") || !strcmp(s,"A")) {
        profile = 0;
      } else if (!strcmp(s,"b") || !strcmp(s,"B")) {
        profile = 1;
      } else if (!strcmp(s,"c") || !strcmp(s,"C")) {
        profile = 2;
      } else if (!strcmp(s,"d") || !strcmp(s,"D")) {
        profile = 4;
      } else {
        fprintf(stderr,"unknown profile definition %s, only profiles a,b,c and d exist",
                s);
        return 20;
      }
    } else if (!strcmp(argv[1],"-m")) {
      maxerror = ParseInt(argc,argv);
    } else if (!strcmp(argv[1],"-md")) {
      median = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-ct")) {
      median = false;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-sm")) {
      smooth = ParseInt(argc,argv);
    } else if (!strcmp(argv[1],"-z")) {
      restart = ParseInt(argc,argv);
    } else if (!strcmp(argv[1],"-r")) {
      residuals = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-R")) {
      hiddenbits = ParseInt(argc,argv);
      if (hiddenbits < 0 || hiddenbits > 4) {
        fprintf(stderr,"JPEG XT allows only between 0 and 4 refinement bits.\n");
        return 20;
      }
    } else if (!strcmp(argv[1],"-rR")) {
      riddenbits = ParseInt(argc,argv);
    } else if (!strcmp(argv[1],"-n")) {
      writednl   = true;
      argv++;
      argc--;
    } 
#if ISO_CODE
    else if (!strcmp(argv[1],"-ae")) {
      factor = ParseDouble(argc,argv);
      profile = 1; // take this as an indicator of profile B.
    } else if (!strcmp(argv[1],"-e")) {
      exposure = ParseDouble(argc,argv);
      profile = 1; // take this as an indicator of profile B
    } 
#endif
    else if (!strcmp(argv[1],"-c")) {
      colortrafo = JPGFLAG_MATRIX_COLORTRANSFORMATION_NONE;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-cls")) {
      colortrafo = JPGFLAG_MATRIX_COLORTRANSFORMATION_LSRCT;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-sp")) {
      separate = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-s")) {
      sub    = ParseString(argc,argv);
    } else if (!strcmp(argv[1],"-sr")) {
      ressub = ParseString(argc,argv);
    } else if (!strcmp(argv[1],"-ncl")) {
      noclamp = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-al")) {
      alpha   = ParseString(argc,argv);
    } else if (!strcmp(argv[1],"-am")) {
      alphamode = ParseInt(argc,argv);
      if (alphamode < 0 || alphamode > 3) {
        fprintf(stderr,"the alpha mode specified with -am must be between 0 and 3\n");
        return 20;
      }
    } else if (!strcmp(argv[1],"-ab")) {
      const char *matte = ParseString(argc,argv);
      if (sscanf(matte,"%d,%d,%d",&matte_r,&matte_g,&matte_b) != 3) {
        fprintf(stderr,"-ab expects three numeric arguments separated comma, i.e. r,g,b\n");
        return 20;
      }
    } else if (!strcmp(argv[1],"-all")) {
      aserms = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-alo")) {
      abypass = true;
      argv++;
      argc--;
    }
    else if (!strcmp(argv[1],"-h")) {
      optimize = true;
      argv++;
      argc--;
    } 
    else if (!strcmp(argv[1],"-qv")) {
      qscan       = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-v")) {
      progressive = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-rv")) {
      rprogressive = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-rs")) {
      rsequential = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-r12")) {
      resprec   = 12;
      residuals = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-rl")) {
      losslessdct = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-ro")) {
      dctbypass = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-xyz")) {
      xyz = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-cxyz")) {
      cxyz = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-N")) {
      noiseshaping = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-ol")) {
      openloop = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-dz")) {
      deadzone = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-qt")) {
      tabletype = ParseInt(argc,argv);
    } else if (!strcmp(argv[1],"-rqt")) {
      residualtt = ParseInt(argc,argv);
    } else if (!strcmp(argv[1],"-aqt")) {
      alphatt = ParseInt(argc,argv);
    } else if (!strcmp(argv[1],"-arqt")) {
      residualalphatt = ParseInt(argc,argv);
    } else if (!strcmp(argv[1],"-aol")) {
      aopenloop = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-adz")) {
      adeadzone = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-ldr")) {
      ldrsource = ParseString(argc,argv);
    } else if (!strcmp(argv[1],"-l")) {
      serms = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-g")) {
      gamma = ParseDouble(argc,argv);
#if ISO_CODE
      gammaspecd = true;
#endif
    } else if (!strcmp(argv[1],"-gf")) {
      lsource = ParseString(argc,argv);
#if ISO_CODE   
    } else if (!strcmp(argv[1],"-epsd")) {
      epsilond = ParseDouble(argc,argv);
    } else if (!strcmp(argv[1],"-epsn")) {
      epsilonn = ParseDouble(argc,argv);
    } else if (!strcmp(argv[1],"-lr")) {
      linearres = true;
      argv++;
      argc--;
#endif      
    } else if (!strcmp(argv[1],"-aq")) {
      alphaquality = ParseInt(argc,argv);
    } else if (!strcmp(argv[1],"-aQ")) {
      alphahdrquality = ParseInt(argc,argv);
    } else if (!strcmp(argv[1],"-aquality")) {
      alphasplitquality = ParseInt(argc,argv);
    } else if (!strcmp(argv[1],"-ar")) {
      alpharesiduals = true;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-ar12")) {
      alpharesiduals = true;
      aresprec = 12;
      argv++;
      argc--;
    } else if (!strcmp(argv[1],"-aR")) {
      ahiddenbits = ParseInt(argc,argv);
    } else if (!strcmp(argv[1],"-arR")) {
      ariddenbits = ParseInt(argc,argv);
    }
    else if (!strcmp(argv[1],"-ls")) {
      lsmode = ParseInt(argc,argv);
    } else {
      fprintf(stderr,"unsupported command line switch %s\n",argv[1]);
      return 20;
    }
  }

  //
  // Use a very simplistic quality split.
  if (splitquality >= 0) {
    switch(profile) {
    case 0:
#if ISO_CODE
      SplitQualityA(splitquality,quality,hdrquality);
#endif
      break;
    case 1:
#if ISO_CODE
      SplitQualityB(splitquality,quality,hdrquality);
#endif
      break;
    case 2:
    case 4:
      SplitQualityC(splitquality,residuals,quality,hdrquality);
      break;
    }
  }

  //
  // The alpha channel is encoded with something that works like part 6.
  if (alphasplitquality > 0) {
    SplitQualityC(alphasplitquality,alpharesiduals,alphaquality,alphahdrquality);
  }

  if (argc != 3) {
    if (argc > 3) {
      fprintf(stderr,"Error in argument parsing, argument %s not understood or parsed correctly.\n"
              "Run without arguments for a list of command line options.\n\n",
              argv[1]);
      exit(20);
    }

    PrintUsage(argv[0]);
    
    return 5;
  }

  if (quality < 0 && lossless == false && lsmode < 0) {
    Reconstruct(argv[1],argv[2],colortrafo,alpha,serms);
  } else {
    switch(profile) {
    case 0:
#if ISO_CODE
      if (xyz || cxyz) {
        fprintf(stderr,"**** XYZ color input currently not yet supported for profile A, sorry. ****\n");
        exit(20);
      }
      if (serms) {
        fprintf(stderr,"**** Profile A does not support lossless coding. ****\n");
        exit(20);
      }
      if (!openloop) {
        fprintf(stderr,
                "**** WARNING: Closed loop coding used for profile A. ****\n"
                "**** this works, but is not recommended. Try -ol to  ****\n"
                "**** switch to open loop coding.                     ****\n"
                );
      }
      EncodeA(argv[1],ldrsource,argv[2],quality,hdrquality,
              tabletype,residualtt,colortrafo,
              progressive,rprogressive,
              hiddenbits,riddenbits,optimize,
              openloop,deadzone,lagrangian,dering,
              noclamp,gammaspecd?gamma:-1.0, // make 2.4 the default gamma, unless specified otherwise
              median,smooth,
              sub,ressub,
              alpha,alphamode,matte_r,matte_g,matte_b,
              alpharesiduals,alphaquality,alphahdrquality,
              alphatt,residualalphatt,
              ahiddenbits,ariddenbits,aresprec,
              aopenloop,adeadzone,alagrangian,adering,
              aserms,abypass);
#else
      fprintf(stderr,"**** Profile A encoding not supported due to patented IPRs.\n");
#endif
      break;
    case 1:
#if ISO_CODE
      if (xyz || cxyz) {
        fprintf(stderr,"**** XYZ color input currently not yet supported for profile A, sorry. ****\n");
        exit(20);
      }
      if (serms) {
        fprintf(stderr,"**** Profile B does not support lossless coding. ****\n");
        exit(20);
      }
      if (!openloop) {
        fprintf(stderr,
                "**** WARNING: Closed loop coding used for profile B. ****\n"
                "**** this works, but is not recommended. Try -ol to  ****\n"
                "**** switch to open loop coding.                     ****\n"
                );
      }
      EncodeB(argv[1],ldrsource,argv[2],exposure,factor,
              gammaspecd?gamma:-1.0,
              epsilonn,epsilond,median,smooth,linearres,
              quality,hdrquality,
              tabletype,residualtt,colortrafo,
              progressive,rprogressive,hiddenbits,riddenbits,
              optimize,openloop,deadzone,lagrangian,dering,noclamp,sub,ressub,
              alpha,alphamode,matte_r,matte_g,matte_b,
              alpharesiduals,alphaquality,alphahdrquality,
              alphatt,residualalphatt,
              ahiddenbits,ariddenbits,aresprec,
              aopenloop,adeadzone,alagrangian,adering,
              aserms,abypass);
#else
      fprintf(stderr,"**** Profile B encoding not supported due to patented IPRs.\n");
#endif
      break;
    case 2:
    case 4:
      if (setprofile && ((residuals == false && hiddenbits == false && profile != 4) || profile == 2))
        residuals = true;
      EncodeC(argv[1],ldrsource,argv[2],lsource,quality,hdrquality,
              tabletype,residualtt,maxerror,
              colortrafo,lossless,progressive,
              residuals,optimize,accoding,
              rsequential,rprogressive,raccoding,
              qscan,levels,pyramidal,writednl,restart,
              gamma,
              lsmode,noiseshaping,serms,losslessdct,
              openloop,deadzone,lagrangian,dering,
              xyz,cxyz,
              hiddenbits,riddenbits,resprec,separate,
              median,noclamp,smooth,dctbypass,
              sub,ressub,
              alpha,alphamode,matte_r,matte_g,matte_b,
              alpharesiduals,alphaquality,alphahdrquality,
              alphatt,residualalphatt,
              ahiddenbits,ariddenbits,aresprec,
              aopenloop,adeadzone,alagrangian,adering,
              aserms,abypass);
      break;
    }
  }
  
  return 0;
}
///

