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
           case NODE_URBAN:
                node_class = string("Urban");
                {
                    TUrban *tn = (TUrban *)node;
                }
                break;
           case NODE_FOREST:
                node_class = string("Forest");
                {
                    TUrban *tn = (TUrban *)node;
                }
                break;
            case NODE_PADDY:
                node_class = string("Paddy");
                {
                    TPaddy *tn = (TPaddy *)node;
                }
                break;
            case NODE_LINK:
                node_class = string("Link");
                {
                    TLink *tn = (TLink *)node;
                }
                break;
            case NODE_JUNC:
                node_class = string("Junction");
                {
                    TJunc *tn = (TJunc *)node;
                }
                break;
            case NODE_OUTLET:
                node_class = string("Outlet");
                {
                    TJunc *tn = (TJunc *)node;
                }
                break;
           case NODE_CLIMATE:
                node_class = string("Climate");
                {
                    TClimate *tn = (TClimate *)node;
                    nl["Rainfall"] = StringVector::create(tn->m_szClimate);
                    nl["Evaporation"] = StringVector::create(tn->m_szEva);
                    nl["Calculation"] = NumericVector::create(tn->m_bUseCalc,
                                                              tn->m_nLat,
                                                              tn->m_nHeight,
                                                              tn->m_nWindHeight);
                }
                break;
            case NODE_IMPORT:
                node_class = string("Import");
                {
                    TImport *tn = (TImport *)node;
                }
                break;
            case NODE_INFILTRO:
                node_class = string("Infiltro");
                {
                    TInfiltro *tn = (TInfiltro *)node;
                }
                break;
            case NODE_BIORETENTION:
                node_class = string("Bioretention");
                {
                    TBioRetention *tn = (TBioRetention *)node;
                }
                break;
            case NODE_WETLAND:
                node_class = string("WetLand");
                {
                    TWetLand *tn = (TWetLand *)node;
                }
                break;
            case NODE_POND:
                node_class = string("Pond");
                {
                    TPond *tn = (TPond *)node;
                    nl["Base"] = NumericVector::create(tn->vol_init,
                                                       tn->vol_eff,
                                                       tn->aqf_ks);
                    nl["Pipe"] = NumericVector::create(tn->pipe_ht,
                                                       tn->pipe_area,
                                                       tn->pipe_coef);
                    nl["Spill"] = NumericVector::create(tn->spill_ht,
                                                        tn->spill_length,
                                                        tn->spill_coef);
                    nl["Offline"] = NumericVector::create(tn->offline_max,
                                                          tn->offline_ratio,
                                                          tn->offline_out);
                    nl["Intake"] = NumericVector::create(tn->intake_type,
                                                         tn->intake_vol,
                                                         tn->m_nTable,
                                                         tn->m_nData);
                    nl["Series"] = StringVector::create(tn->m_szIntake);
                    nl["RateCount"] = NumericVector::create(tn->rate_count);
                    NumericVector wl(tn->rate_count);
                    for(int j = 0; j < tn->rate_count; j++)
                        wl(j) = tn->wl_rate[j][0];
                    nl["WL"] = wl;
                    for(int j = 0; j < tn->rate_count; j++)
                        wl(j) = tn->wl_rate[j][1];
                    nl["VOL"] = wl;
                    for(int j = 0; j < tn->rate_count; j++)
                        wl(j) = tn->wl_rate[j][2];
                    nl["AREA"] = wl;
                    nl["Rainfall"] = NumericVector::create(tn->m_Climates[0].nID,
                                                           tn->m_Climates[0].nEva);
                    nl["EVA"] = NumericVector::create(tn->m_Climates[1].nID,
                                                      tn->m_Climates[1].nEva);
                    nl["Supply"] = NumericVector::create(tn->supply_id);
                    nl["Recharge"] = NumericVector::create(tn->recharge_id);
                }
                break;
            case NODE_RECYCLE:
                node_class = string("Recycle");
                {
                    TRecycle *tn = (TRecycle *)node;
                    nl["Intake"] = IntegerVector::create(tn->rec_cond, tn->rec_intake);
                    IntegerVector nid(tn->m_nRecursive);
                    NumericVector nrain(tn->m_nRecursive);
                    for(int j = 0; j < tn->m_nRecursive; j++)
                    {
                        nid(j) = tn->m_Recursive[j].nID;
                        nrain(j) = tn->m_Recursive[j].nRain;
                    }
                    nl["Nodes"] = DataFrame::create(_["nID"] = nid, _["nRain"] = nrain);
                }
                break;
            case NODE_RAINTANK:
                node_class = string("RainTank");
                {
                    TRainTank *tn = (TRainTank *)node;
                    nl["Volume"] = NumericVector::create(tn->vol_init, tn->vol_min, tn->cap);
                    nl["Use"] = NumericVector::create(tn->use_type, tn->use, tn->m_nTable, tn->m_nData);
                    nl["Series"] = StringVector::create(tn->m_szUse);
                    nl["Supply"] = IntegerVector::create(tn->supply_id);
                }
                break;
            default:
                break;
        }
        List ll = wrap(nl);
        ll.attr("class") = "cat_" + node_class;
        Nodes[i + 1] = ll;
    }
    ret["Nodes"] = wrap(Nodes);
    List wret = wrap(ret);
    wret.attr("class") = "rcat";
    return wret;
}

/** @brief CAT 모형의 입력자료를 CAT 모형에 투영하여 반환
 *
 * @param ml List class 가 rcat 인 모형입력자료 리스트
 * @return TModelManager 입력자료가 투영된 CAT 모형
 *
 */
TModelManager *List2Model(List ml)
{
    //!< class 속성이 rcat 인지 확인
    if(as<string>(ml.attr("class")) == "rcat" )
    {
        TModelManager *model = new TModelManager();
        IntegerVector iv;
        iv = as<IntegerVector>(ml["StartTime"]);
        model->m_dtStart = TDate::ToMinute(iv.length() > 0 ? iv[0] : 0,
                                           iv.length() > 1 ? iv[1] : 0,
                                           iv.length() > 2 ? iv[2] : 0,
                                           iv.length() > 3 ? iv[3] : 0,
                                           iv.length() > 4 ? iv[4] : 0);
        iv = as<IntegerVector>(ml["EndTime"]);
        model->m_dtEnd = TDate::ToMinute(iv.length() > 0 ? iv[0] : 0,
                                         iv.length() > 1 ? iv[1] : 0,
                                         iv.length() > 2 ? iv[2] : 0,
                                         iv.length() > 3 ? iv[3] : 0,
                                         iv.length() > 4 ? iv[4] : 0);
        iv = as<IntegerVector>(ml["Parameter"]);
        model->m_nDT = iv.length() > 0 ? iv[0] : 0;
        model->m_nLoop = iv.length() > 1 ? iv[1] : 0;
        List Nodes = ml["Nodes"];
        int loop_count = Nodes.length();
        //model->m_nCount = loop_count = 10;
        for(int i = 0; i < loop_count; i++)
        {
            List node = Nodes[i];
            string node_class = node.attr("class");
            TBaseNode *pNode = NULL;

            if(node_class == "cat_Urban") //!< NODE_URBAN:
            {
                pNode = new TUrban;
            }
            else if(node_class == "cat_Forest") //!< NODE_FOREST:
            {
                pNode = new TUrban;
                pNode->m_nType = NODE_FOREST;
            }
            else if(node_class == "cat_Paddy") //!< NODE_PADDY:
            {
                pNode = new TPaddy;
            }
            else if(node_class == "cat_Link") //!<  NODE_LINK:
            {
                pNode = new TLink;
            }
            else if(node_class == "cat_Junction") //!< NODE_JUNC:
            {
                pNode = new TJunc;
            }
            else if(node_class == "cat_Outlet") //!< NODE_OUTLET:
            {
                pNode = new TJunc;
                pNode->m_nType = NODE_OUTLET;
            }
            else if(node_class == "cat_Climate") //!< NODE_CLIMATE:
            {
                TClimate *pn = new TClimate;
                StringVector sv = as<StringVector>(node["Rainfall"]);
                pn->SetClimateFile(sv.length() > 0 ? sv[0] : (char *)"");
                sv = as<StringVector>(node["Evaporation"]);
                pn->SetEvaFile(sv.length() > 0 ? sv[0] : (char *)"");
                NumericVector nv = as<NumericVector>(node["Calculation"]);
                pn->m_bUseCalc = nv.length() > 0 ? nv[0] != 0.0: false;
                pn->m_nLat = nv.length() > 1 ? nv[1] : 0;
                pn->m_nHeight = nv.length() > 2 ? nv[2] : 0;
                pn->m_nWindHeight = nv.length() > 3 ? nv[3] : 0;
                //{
                //    TClimate *tn = (TClimate *)node;
                //    nl["Rainfall"] = StringVector::create(tn->m_szClimate);
                //    nl["Evaporation"] = StringVector::create(tn->m_szEva);
                //    nl["Calculation"] = NumericVector::create(tn->m_bUseCalc,
                //                                              tn->m_nLat,
                //                                              tn->m_nHeight,
                //                                              tn->m_nWindHeight);
                //}
                pNode = pn;
            }
            else if(node_class == "cat_Import") //!< NODE_IMPORT:
            {
                pNode = new TImport;
            }
            else if(node_class == "cat_Infiltro") //!< NODE_INFILTRO:
            {
                pNode = new TInfiltro;
            }
            else if(node_class == "cat_Bioretention") //!< NODE_BIORETENTION:
            {
                pNode = new TBioRetention;
            }
            else if(node_class == "cat_WetLand") //!< NODE_WETLAND:
            {
                pNode = new TWetLand;
            }
            else if(node_class == "cat_Pond") //!< NODE_POND:
            {
                pNode = new TPond;
            }
            else if(node_class == "cat_Recycle") //!< NODE_RECYCLE:
            {
                TRecycle *pn = new TRecycle;
                iv = as<IntegerVector>(node["Intake"]);
                pn->rec_cond = iv.length() > 0 ? iv[0] : 0;
                pn->rec_intake = iv.length() > 1 ? iv[2] : 0;
                DataFrame df = as<DataFrame>(node["Nodes"]);
                pn->m_nRecursive = df.nrows();
                // TODO (hspark#1#): 최대 5로 설정되어 있으므로 에러 발생 시켜야함
                if(pn->m_nRecursive > 5) {}
                IntegerVector nid = as<IntegerVector>(df["nID"]);
                NumericVector nrain = as<NumericVector>(df["nRain"]);
                for(int j = 0; j < pn->m_nRecursive; j++)
                {
                    pn->m_Recursive[j].nID = nid[j];
                    pn->m_Recursive[j].nRain = nrain[j];
                }
                pNode = pn;
            }
            else if(node_class == "cat_RainTank") //!< NODE_RAINTANK:
            {
                TRainTank * pn = new TRainTank;
                iv = as<NumericVector>(node["Volume"]);
                pn->vol_init = iv.length() > 0 ? iv[0] : 0;
                pn->vol_min = iv.length() > 1 ? iv[1] : 0;
                pn->cap = iv.length() > 2 ? iv[2] : 0;
                iv = as<NumericVector>(node["Use"]);
                pn->use_type = iv.length() > 0 ? iv[0] : 0;
                pn->use = iv.length() > 1 ? iv[1] : 0;
                StringVector sv = as<StringVector>(node["Series"]);
                pn->SetSeriesFileA(sv.length() > 0 ? sv[0] : (char *)"");
                iv = as<IntegerVector>(node["Supply"]);
                pn->supply_id = iv.length() > 0 ? iv[0] : 0;
                //{
                //    TRainTank *tn = (TRainTank *)node;
                //    nl["Base"] = NumericVector::create(tn->vol_init, tn->vol_min, tn->cap);
                //    nl["Use"] = NumericVector::create(tn->use_type, tn->use, tn->m_nTable, tn->m_nData);
                //    nl["Series"] = StringVector::create(tn->m_szUse);
                //    nl["Supply"] = IntegerVector::create(tn->supply_id);
                //}
                pNode = pn;
            }

            if(pNode)
            {
                pNode->SetName((char *)as<string>(node["Name"]).c_str());
                pNode->SetDesc((char *)as<string>(node["Desc"]).c_str());
                pNode->SetID(as<int>(node["NodeID"]));
                model->Add(pNode);
                switch(pNode->GetType())
                {
                    case NODE_IMPORT:
                        //ChangeFilePathA
                        break;
                    case NODE_CLIMATE:
                        //CheckClimateLoad
                        break;
                }
            }
        }
        //model->CheckLoad();
        return model;
    }
    return NULL;
}

// [[Rcpp::export]]
List getModel(StringVector input)
{
    TModelManager model;
    model.LoadText(input(0));
    return Model2List(&model);
}

// [[Rcpp::export]]
List setnrun_cat(List input)
{
    TModelManager *model = List2Model(input);
    if(model != NULL)
        return Model2List(model);
    else
        return List();
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
