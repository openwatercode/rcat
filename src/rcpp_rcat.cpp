#include "StdAfx.h"
#include <Rcpp.h>
#include <string>
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

/** @brief
 *
 * @param model TModelManager*
 * @return List
 *
 */
List Model2List(TModelManager *model)
{
    std::map<string, SEXP> ret;
    int loop_count = model->GetCount();
    ret["Version"] = StringVector::create("2.0.0.1");
    ret["Title"] = StringVector::create("RCAT");
    ret["StartTime"] = IntegerVector::create(model->m_dtStart.GetYear(),
                                             model->m_dtStart.GetMonth(),
                                             model->m_dtStart.GetDay(),
                                             model->m_dtStart.GetHour(),
                                             model->m_dtStart.GetMinute());
    ret["EndTime"] = IntegerVector::create(model->m_dtEnd.GetYear(),
                                           model->m_dtEnd.GetMonth(),
                                           model->m_dtEnd.GetDay(),
                                           model->m_dtEnd.GetHour(),
                                           model->m_dtEnd.GetMinute());
    ret["Parameter"] = IntegerVector::create(model->m_nDT, model->m_nLoop);
    //ret["Range"] = StringVector::create("2.0.0.1");
    //ret["NodeCount"] = IntegerVector(loop_count);
    std::map<int, SEXP> Nodes;
    for(int i = 0; i < loop_count; i++)
    {
        TBaseNode *node = model->GetAt(i);
        std::map<string, SEXP> nl;
        string node_class;
        nl["NodeID"] = IntegerVector::create(node->GetID());
        nl["Name"] = StringVector::create(node->GetName());
        nl["Desc"] = StringVector::create(node->GetDesc());

        switch(node->m_nType)
        {
            case NODE_SOURCE:
                node_class = string("Source");
                nl["NodeID"] = IntegerVector(node->GetID());
                break;
            case NODE_TREATMENT:
                node_class = string("Treatment");
                nl["NodeID"] = IntegerVector(node->GetID());
                break;
            case NODE_JUNCTION:
                node_class = string("Junction");
                break;
            case NODE_CLIMATE:
                node_class = string("Climate");
                break;

            case NODE_URBAN:
                node_class = string("Urban");
                break;
            case NODE_FOREST:
                node_class = string("Forest");
                break;
            case NODE_PADDY:
                node_class = string("Paddy");
                break;

            case NODE_IMPORT:
                node_class = string("Import");
                break;
            case NODE_INFILTRO:
                node_class = string("Infiltro");
                break;
            case NODE_WETLAND:
                node_class = string("WetLand");
                break;
            case NODE_POND:
                node_class = string("Pond");
                break;
            case NODE_RAINTANK:
                node_class = string("RainTank");
                break;
            case NODE_RECYCLE:
                node_class = string("Recycle");
                {
                    TRecycle *tn = (TRecycle *)node;
                    nl["Intake"] = IntegerVector::create(tn->rec_cond, tn->rec_intake);
                    IntegerVector nid(tn->m_nRecursive);
                    NumericVector nrain(tn->m_nRecursive);
                    for(int i = 0; i < tn->m_nRecursive; i++)
                    {
                        nid(i) = tn->m_Recursive[i].nID;
                        nrain(i) = tn->m_Recursive[i].nRain;
                    }
                    nl["Nodes"] = DataFrame::create(_["nID"] = nid, _["nRain"] = nrain);
                }
                break;
            case NODE_BIORETENTION:
                node_class = string("Bioretention");
                break;

            case NODE_JUNC:
                node_class = string("Junction");
                break;
            case NODE_OUTLET:
                node_class = string("Outlet");
                break;
            case NODE_LINK:
                node_class = string("Link");
                break;
            default:
                break;
        }
        List ll = wrap(nl);
        ll.attr("class") = "cat_" + node_class;
        Nodes[i + 1] = ll;
    }
    ret["Nodes"] = wrap(Nodes);
    return wrap(ret);
}

/** @brief
 *
 * @return TModelManager
 *
 */
TModelManager List2Model(List ml)
{
    TModelManager model;
    return model;
}

// [[Rcpp::export]]
List getModel(StringVector input)
{
    TModelManager model;
    model.LoadText(input(0));
    return Model2List(&model);
}


/** @brief TSeries 객체를 DataFrame으로 변환하여 반환하는 함수
 *
 * @param sr TSeries* 값을 받아낼  TSeries 객체에 대한 포인터
 * @param nFieldNos[] int 주어진 TSereis내에 복사할 필드번호 배열의 포인터
 * @return DataFrame TSeries의 시간과 값들이 복사된 DataFrame
 *
 */
DataFrame TSeries2DataFrame(TSeries *sr, int nFieldNos[])
//DataFrame TSeries2DataFrame(TSeries *sr, int nCount, int nFieldNos[])
{
    DataFrame df = DataFrame::create();
	int nIndex, nCol, nCount;
	int nFields[30];
	TDate date;

    nCount = sr->GetCount();
	if(nFieldNos[0] == -1)
	{
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

/** @brief TSerieses 객체를 DataFrame 에 대한 리스트 형태로 변환하여 반환
 *
 * @param sr TSerieses* 변환하고자 하는 TSereses 객체에 대한 포인터
 * @param szFields char* 변환하고자하는 노드명과 필드에 대한 문자열 "[*:*]"(모든 노드의 모든 필드
 * @return List 변환된 DataFrame의 List
 */
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
                //ret[string(pSeries->m_Header.szDescript)] = TSeries2DataFrame(pSeries, pItem->nCount, pItem->nFieldNo);
                ret[string(pSeries->m_Header.szDescript)] = TSeries2DataFrame(pSeries, pItem->nFieldNo);
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
