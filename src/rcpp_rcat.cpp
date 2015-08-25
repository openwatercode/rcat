#include <Rcpp.h>
#include <string>
#include "StdAfx.h"
#include "include/ModelManager.h"
#include "include/ValidateCheck.h"
#include "include/LargeString.h"

using namespace Rcpp;
using namespace std;

// This is a simple example of exporting a C++ function to R. You can
// source this function into an R session using the Rcpp::sourceCpp
// function (or via the Source button on the editor toolbar). Learn
// more about Rcpp at:
//
//   http://www.rcpp.org/
//   http://adv-r.had.co.nz/Rcpp.html
//   http://gallery.rcpp.org/
//

DataFrame TSeries2DataFrame(TSeries *sr, int nCount, int nFieldNos[])
{
    DataFrame df = DataFrame::create();
	int nIndex, nCol;
	int nFields[30];
	TDate date;

	if(nFieldNos[0] == -1)
	{
		nCount = sr->GetCount();
		for(nIndex = 0; nIndex < nCount; nIndex++)
			nFields[nIndex] = nIndex;
	}
	else
		memcpy(nFields, nFieldNos, sizeof(int) * nCount);

	StringVector col_names(nCount - 1);
	int rowCount = sr->GetSeries(0)->GetCount();
	StringVector row_names(rowCount);
	char time_str[20];
	memset(time_str, 0, 20);
    date = sr->m_Header.nTime;
    for(nIndex = 0; nIndex < rowCount; nIndex++)
    {
        sprintf(&(time_str[0]), "%04d/%02d/%02d %02d:%02d",
                date.GetYear(), date.GetMonth(), date.GetDay(),
                date.GetHour(), date.GetMinute());
        row_names(nIndex) = string(time_str);
        date += sr->m_Header.nInterval;
    }
	for(nCol = 0; nCol < nCount - 1; nCol++)
	{
	    col_names(nCol) = string(sr->GetSeries(nFields[nCol])->m_Header.szHeader);
	    NumericVector val(rowCount);
   	    for(nIndex = 0; nIndex < rowCount; nIndex++)
	    {
	        val(nIndex) = sr->GetSeries(nFields[nCol])->GetValue(nIndex);
	    }
        df[string(sr->GetSeries(nFields[nCol])->m_Header.szHeader)] = val;
	}
	df.attr("row.names") = row_names;
	df.attr("class") = "data.frame";
    return df;
}

List TSerieses2List(TSerieses *sr, char *szFields)
{
    List ret = List::create();
	TFieldList list;

	list.Parsing(szFields);

	for(int nIndex = 0; nIndex < list.GetCount(); nIndex++)
	{
		FIELDITEM *pItem = list.GetItem(nIndex);

		for(int nSeries = 0; nSeries < sr->GetCount(); nSeries++)
		{
			TSeries *pSeries = sr->GetAt(nSeries);

			if(strcmp(pItem->szNode, "*") == 0 || strcmp(pSeries->m_Header.szDescript, pItem->szNode) == 0)
			{
			    ret[string(pSeries->m_Header.szDescript)] = TSeries2DataFrame(pSeries, pItem->nCount, pItem->nFieldNo);
			}
		}
	}
	return ret;
}


string runCAT(char* infile, char* outfile, char* format)
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
  TValidateCheck chk;
  nRet = chk.Check(&model);
  string msg;
  if(nRet != 0) msg = string(chk.GetMessage()->GetBuffer());

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

  return msg;
}

// [[Rcpp::export]]
List run_cat(List params)
{
  TModelManager model;
  char *infile = as<CharacterVector>(params["infile"])(0);
  char *format = as<CharacterVector>(params["format"])(0);

  model.LoadText(infile);
  TValidateCheck chk;
  int nRet = chk.Check(&model);
  string msg;
  if(nRet != 0) msg = string(chk.GetMessage()->GetBuffer());
  model.Calculate(FALSE);

  TSerieses *pResult = model.GetResult();

  if(pResult->GetCount() > 0)
    return List::create(_["msg"] = StringVector::create(msg),
                        _["ret"] = TSerieses2List(pResult, format));
  else
    return List::create(_["msg"] = StringVector::create(msg));
}

// [[Rcpp::export]]
StringVector rcpp_run_cat(CharacterVector input,
                    CharacterVector report,
                    CharacterVector bin) {
  //    CharacterVector x = CharacterVector::create( "foo", "bar" )  ;
  //    NumericVector y   = NumericVector::create( 0.0, 1.0 ) ;
  //    List z            = List::create( x, y ) ;
  string msg = runCAT(input(0), report(0), bin(0));
  return StringVector::create(msg);
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
