#include <Rcpp.h>
#include "windef.h"
#include "include/ModelManager.h"
#include "CATModel.h"

using namespace Rcpp;

// This is a simple example of exporting a C++ function to R. You can
// source this function into an R session using the Rcpp::sourceCpp
// function (or via the Source button on the editor toolbar). Learn
// more about Rcpp at:
//
//   http://www.rcpp.org/
//   http://adv-r.had.co.nz/Rcpp.html
//   http://gallery.rcpp.org/
//

int runCAT(char* infile, char* outfile, char* format)
{
  TModelManager model;
  int nRet = 0;
// char szFormat[MAX_PATH];

//  memset(szFormat, 0, sizeof(szFormat));

// #ifdef _UNICODE
//   MultiByteToWideChar(CP_ACP, 0, format, strlen(format), szFormat, MAX_PATH);
// #else
//   strcpy(szFormat, format);
// #endif

  model.LoadText(infile);
  model.Calculate(FALSE);

  TSerieses *pResult = model.GetResult();

  if(pResult->GetCount() > 0)
  {
    char *szFileName = strrchr(outfile, '\\');

    if(szFileName)
      pResult->SetName(szFileName + 1);

    if(format == NULL || strlen(format) == 0)
    {
      pResult->Save(outfile);
    }
    else if(format && strlen(format) > 0)
    {
      pResult->SaveText(outfile, format);
    }
    else
      nRet = 2;
  }
  else
    nRet = 1;

  return nRet;
}

// [[Rcpp::export]]
int rcpp_run_cat(CharacterVector input,
                    CharacterVector report,
                    CharacterVector bin) {
  //    CharacterVector x = CharacterVector::create( "foo", "bar" )  ;
  //    NumericVector y   = NumericVector::create( 0.0, 1.0 ) ;
  //    List z            = List::create( x, y ) ;
  int err = runCAT(input(0), report(0), bin(0));

  return err;
}

// [[Rcpp::export]]
NumericVector timesTwo(NumericVector x) {
  return x * 2;
}


// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically
// run after the compilation.
//

/*** R
timesTwo(42)
*/
