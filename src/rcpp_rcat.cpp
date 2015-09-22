/* CAT 모형에 대한 R 인터페이스
 * 작성자: 박희성(hspark90@kict.re.kr)
 * 설명: 기존 CAT 모형을 R에서 구동할 수 있도록 해주는 인터페이스
 * 2015. 8. 10
 */
//   Rcpp Reference
//   http://www.rcpp.org/
//   http://adv-r.had.co.nz/Rcpp.html
//   http://gallery.rcpp.org/
//
#include "StdAfx.h"
#include <Rcpp.h>
#include <string>
#include "include/ModelManager.h"
#include "include/ValidateCheck.h"
#include "include/LargeString.h"

using namespace Rcpp;
using namespace std;


/** @brief 주어진 TSeiresItem을 복제
 *
 * @param src TSeriesItem* 원본 TSeriesItem에 대한 포인터
 * @return TSeriesItem 복제된 TSeriesItem
 */
TSeriesItem Clone(TSeriesItem *src)
{
    TSeriesItem ret;
    int len = src->GetCount();
    for(int i = 0; i < len; i++)
        ret.SetValue(i, src->GetValue(i));
    for(int i = 0; i < 12; i++)
        ret.m_nMonth[i] = src->m_nMonth[i];
    ret.m_dtStart = src->m_dtStart;
    ret.m_Header = src->m_Header;
    ret.m_nCount = src->m_nCount;
    ret.m_nYear = src->m_nYear;
    ret.m_nSize = src->m_nSize;
    return ret;
}

/** @brief 주어진 TSeries를 복제
 *
 * @param src TSeries* 원본 TSeries에 대한 포인터
 * @return TSeries 복제된 TSeries
 */
TSeries Clone(TSeries *src)
{
    TSeries ret;
    int len = src->GetCount();
    for(int i = 0; i < len; i++)
    {
        TSeriesItem *s = src->GetAt(i);
        TSeriesItem d = Clone(s);
        //TSeriesItem d;
        //s->Copy(&d);
        ret.Add(&d);
    }
    ret.m_dtStart = src->m_dtStart;
    ret.m_Header = src->m_Header;
    ret.m_nCount = src->m_nCount;
    ret.m_nSize = src->m_nSize;
    return ret;
}

/** @brief 주어진 TSerieses를 복제
 *
 * @param src TSerieses* 원본 TSerieses에 대한 포인터
 * @return TSerieses 복제된 TSerieses
 */
TSerieses Clone(TSerieses *src)
{
    TSerieses ret;
    int len = src->GetCount();
    for(int i = 0; i < len; i++)
    {
        TSeries *s = src->GetAt(i);
        TSeries d = Clone(s);
        //TSeries d;
        //s->Copy(&d);
        ret.Add(&d);
    }
    ret.m_Header = src->m_Header;
    ret.m_nCount = src->m_nCount;
    ret.m_nSize = src->m_nSize;
    return ret;
}

void TSeriesClear(TSerieses *sr)
{
    int len = sr->GetCount();
    for(int i = 0; i < len; i++)
    {
        TSeries *s = sr->GetAt(i);
        int slen = s->GetCount();
        for(int j = 0; j < slen; j++)
        {
            TSeriesItem *si = s->GetAt(j);
            si->Clear();
        }
        s->Clear();
    }
    sr->Clear();
}

/** @brief 주어진 TSeries의 헤더에 대한 문자열을 반환
 *
 * @param sr TSeries* TSeries에 대한 포인터
 * @return StringVector 주어진 TSeries에 대한 헤더를 문자열로 변환한 1차원 StringVector
 */
StringVector SeriesHeader2String(TSeries *sr)
{
    SERIESHEADER h = sr->m_Header;
    char buff[2048];
    sprintf_s(buff, 2048, "%u %u %u %u %lu %s",
              h.nColumn, h.nCount,
              h.nData, h.nInterval,
              h.nTime, h.szDescript);
    return StringVector::create(string(buff));
}

/** @brief 주어진 TSerieses의 헤더에 대한 문자열을 반환
 *
 * @param sr TSerieses* TSerieses에 대한 포인터
 * @return StringVector 주어진 TSerieses에 대한 헤더를 문자열로 변환한 1차원 StringVector
 */
StringVector SeriesesHeader2String(TSerieses *sr)
{
    SERIESESHEADER h = sr->m_Header;
    char buff[2048];
    sprintf_s(buff, 2048, "%u %u %u %lu %u %s %s",
              h.nPartNo, h.nPartTotal,
              h.nSeries, h.nTime, h.nInterval,
              h.szDescript, h.szName);
    return StringVector::create(string(buff));
}


/** @brief TSeries 객체를 DataFrame으로 변환하여 반환하는 함수
 *
 * @param sr TSeries* 값을 받아낼  TSeries 객체에 대한 포인터
 * @param nCount int nFieldNos에 저장된 유효한 필드의 갯수
 * @param nFieldNos[] int 주어진 TSereis내에 복사할 필드번호 배열의 포인터
 * @return DataFrame TSeries의 시간과 값들이 복사된 DataFrame
 *
 */
DataFrame TSeries2DataFrame(TSeries *sr, int nCount, int nFieldNos[])
{
    DataFrame rdf = DataFrame::create();
	int nFields[30];
	TDate date;

	if(nFieldNos[0] == -1)
	{
        nCount = sr->GetCount();
		for(int i = 0; i < nCount; i++)
			nFields[i] = i;
	}
	else
    {
        // 범위를 벗어난 컬럼넘버를 제외하고 복사
        int  j = 0;
        for(int i = 0, c = sr->GetCount(); i < nCount; i++)
            if(nFieldNos[i] < c || nFieldNos[i] > -1)
                nFields[j++] = nFieldNos[i];
        nCount = j;
		//memcpy(nFields, nFieldNos, sizeof(int) * nCount);
    }

	int rowCount = sr->GetSeries(0)->GetCount();
	//char time_str[20];
	//memset(time_str, 0, 20);
    //date = sr->m_Header.nTime;
    //for(nIndex = 0; nIndex < rowCount; nIndex++)
    //{
    //    sprintf(&(time_str[0]), "%04d/%02d/%02d %02d:%02d",
    //            date.GetYear(), date.GetMonth(), date.GetDay(),
    //            date.GetHour(), date.GetMinute());
    //    row_names(nIndex) = string(time_str);
    //    date += sr->m_Header.nInterval;
    //}
    TDate dt;
	for(int nCol = 0; nCol < nCount; nCol++)
	{
	    TSeriesItem *item = sr->GetSeries(nFields[nCol]);
	    NumericVector val(rowCount);
   	    for(int i = 0; i < rowCount; i++)
	        val(i) = item->GetValue(i);
	    dt.SetDate(item->m_Header.date);
	    val.attr("StartTime") = IntegerVector::create(dt.GetYear(), dt.GetMonth(), dt.GetDay(),
                                              dt.GetHour(), dt.GetMinute());
	    val.attr("Interval") = IntegerVector::create(item->m_Header.nInterval);
	    val.attr("DataType") = IntegerVector::create(item->m_Header.nData);
	    val.attr("class") = StringVector::create("rcat_seriesItem", "numeric");
	    //val.attr("nFields[nCol]") = IntegerVector::create(nFields[nCol]);
	    //item->m_Header.szHeader: 컬럼 이름으로 대체
	    //item->m_Header.nCount: 컬럼자료의 길이로 대체
      rdf[string(item->m_Header.szHeader)] = val;
	}
	rdf.attr("row.names") = seq(1, rowCount);
    dt.SetDate(sr->m_Header.nTime);
    rdf.attr("StartTime") = IntegerVector::create(dt.GetYear(), dt.GetMonth(), dt.GetDay(),
                                          dt.GetHour(), dt.GetMinute());
    rdf.attr("Interval") = IntegerVector::create(sr->m_Header.nInterval);
    rdf.attr("DataType") = IntegerVector::create(sr->m_Header.nData);
	rdf.attr("class") = StringVector::create("rcat_series", "data.frame");
    return rdf;
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
	TFieldList flst;
    TDate dt;
	for(int fli = 0, flc = flst.Parsing(szFields); fli < flc; fli++)
	{
		FIELDITEM *pItem = flst.GetItem(fli);
        for(int sii = 0, sic = sr->GetCount(); sii < sic; sii++)
        {
            TSeries *pSeries = sr->GetAt(sii);
            if(strcmp(pItem->szNode, "*") == 0 || strcmp(pItem->szNode, pSeries->m_Header.szDescript) == 0)
            {
                DataFrame rdf = TSeries2DataFrame(pSeries, pItem->nCount, pItem->nFieldNo);
                ret[string(pSeries->m_Header.szDescript)] = rdf;
            }
        }
	}
    //sr.m_Header.nMagic = FILEMAGICNO;
    //sr.m_Header.nPartNo = 1;
    //sr.m_Header.nPartTotal = 1;
    dt.SetDate(sr->m_Header.nTime);
    ret.attr("StartTime") = IntegerVector::create(dt.GetYear(), dt.GetMonth(), dt.GetDay(),
                                                  dt.GetHour(), dt.GetMinute());
    ret.attr("Interval") = IntegerVector::create(sr->m_Header.nInterval);
    ret.attr("Description") = StringVector::create(string(sr->m_Header.szDescript));
    ret.attr("Name") = StringVector::create(string(sr->m_Header.szName));
	ret.attr("class") = StringVector::create("rcat_serieses");
	return ret;
}

/** @brief DataFrame 형태의 데이터를 TSeries 객체로 변환
 *
 * @param df DataFrame 클래스 속성이 "rcat_series"인 데이터 프레임
 * @return TSeries* 변환된 TSeries의 포인터
 */
TSeries *DataFrame2Series(DataFrame df, char * name) {
    List lst = as<List>(df);
    TSeries *sr = NULL;
    // lst 의 타입 체크
    StringVector att = as<StringVector>(df.attr("class"));
    bool chk = false;
    for(int i = 0; i < att.length(); i++)
        if(chk |= (att[i] == "rcat_series")) break;
    if(!chk) return sr;
    // lst 헤더 sr로 복사
    sr = new TSeries();
    int ncol = sr->m_Header.nColumn = df.length();
    int nrow = sr->m_Header.nCount = df.nrows();
    sr->m_Header.nData = as<IntegerVector>(df.attr("DataType"))[0];
    sr->m_Header.nInterval = as<IntegerVector>(df.attr("Interval"))[0];
    IntegerVector iv = as<IntegerVector>(df.attr("StartTime"));
    sr->m_Header.nTime = TDate::ToMinute(iv[0], iv[1], iv[2], iv[3], iv[4]);
    memset(sr->m_Header.szDescript, 0, sizeof(sr->m_Header.szDescript));
    strcpy_s(sr->m_Header.szDescript, sizeof(sr->m_Header.szDescript), name);
    // lst Data를 sr로 복사
    StringVector item_names = as<StringVector>(df.attr("names"));
    for(int c = 0; c < ncol; c++)
    {
        NumericVector nv = as<NumericVector>(df[c]);
        TSeriesItem *si = sr->AddItem();
        iv = as<IntegerVector>(nv.attr("StartTime"));
        si->m_Header.date = TDate::ToMinute(iv[0], iv[1], iv[2], iv[3], iv[4]);
        si->m_Header.nInterval = as<IntegerVector>(nv.attr("Interval"))[0];
        //si->m_Header.nCount = nrow;
        si->m_Header.nData = as<IntegerVector>(nv.attr("DataType"))[0];
        memset(si->m_Header.szHeader, 0, sizeof(si->m_Header.szHeader));
        strcpy_s(si->m_Header.szHeader, sizeof(si->m_Header.szHeader), (char *)item_names[c]);
        si->SetSize(nrow);

        for(int r = 0, len = min(nrow, (int)nv.length()); r < len; r++)
            si->SetValue(r, nv[r]);
    }

    return sr;
}

/** @brief List 형태의 데이터를 TSerieses 객체로 변환
 *
 * @param lst List 클래스 속성이 "rcat_serieses" 인 List
 * @return TSerieses* 변환된 TSerieses의 포인터
 */
TSerieses *List2Serieses(List lst) {
    TSerieses *sr = NULL;
    // lst 의 타입 체크
    StringVector att = as<StringVector>(lst.attr("class"));
    bool chk = false;
    for(int i = 0; i < att.length(); i++)
        if(chk |= (att[i] == "rcat_serieses")) break;
    if(!chk) return sr;
    sr = new TSerieses();
    // lst 헤더 sr로 복사
    sr->m_Header.dwVersion = FILECURVERSION;
    sr->m_Header.nInterval = as<IntegerVector>(lst.attr("Interval"))[0];
    sr->m_Header.nMagic = FILEMAGICNO;
    sr->m_Header.nPartNo = 1;
    sr->m_Header.nPartTotal = 1;
    int len = sr->m_Header.nSeries = lst.length();
    IntegerVector iv = as<IntegerVector>(lst.attr("StartTime"));
    sr->m_Header.nTime = TDate::ToMinute(iv[0], iv[1], iv[2], iv[3], iv[4]);
    memset(sr->m_Header.szDescript, 0, sizeof(sr->m_Header.szDescript));
    strcpy_s(sr->m_Header.szDescript, 100, (char *)as<StringVector>(lst.attr("Description"))[0]);
    memset(sr->m_Header.szName, 0, sizeof(sr->m_Header.szName));
    strcpy_s(sr->m_Header.szName, 100,  (char *)as<StringVector>(lst.attr("Name"))[0]);
    // lst Series를 sr로 복사
    StringVector names = as<StringVector>(lst.attr("names"));
    for(int i = 0; i < len; i++)
    {
        DataFrame df = as<DataFrame>(lst[i]);
        TSeries *s = DataFrame2Series(df, names[i]);
        if(s != NULL) sr->Add(s);
    }
    return sr;
}

/** @brief CAT 모형의 입력자료를 List 형태로 변환
 *
 * @param model TModelManager* 입력자료가 로딩된 CAT 모형
 * @return List List 형태로 변환된 CAT 모형의 입력자료
 */
List Model2List(TModelManager *model)
{
    //map<string, SEXP> ret;
    int loop_count = model->GetCount();
    map<int, SEXP> Nodes;
    for(int i = 0; i < loop_count; i++)
    {
        TBaseNode *node = model->GetAt(i);
        map<string, SEXP> nl;
        string node_class;
        nl["NodeID"] = IntegerVector::create(node->GetID());
        nl["Name"] = StringVector::create(node->GetName());
        nl["Desc"] = StringVector::create(node->GetDesc());

        switch(node->m_nType)
        {
            case NODE_URBAN:
            case NODE_FOREST:
                switch(node->m_nType)
                {
                    case NODE_URBAN:
                        node_class = string("Urban");
                        break;
                    case NODE_FOREST:
                        node_class = string("Forest");
                        break;
                }
                {
                    TUrban *tn = (TUrban *)node;
                    nl["Topology"] = NumericVector::create(tn->Area,
                                                           tn->slope,
                                                           tn->Aratio_imp,
                                                           tn->Aratio_per,
                                                           tn->Aratio_per_plant,
                                                           tn->depC_imp,
                                                           tn->depC_per);
                    nl["Soil"] = NumericVector::create(tn->theta_per,
                                                       tn->soil_th_per,
                                                       tn->theta_s_per,
                                                       tn->theta_r_per,
                                                       tn->theta_FC_per,
                                                       tn->theta_W_per,
                                                       tn->ks_per,
                                                       tn->ksi_per,
                                                       tn->n_mualem);
                    nl["River"] = NumericVector::create(tn->gwE,
                                                        tn->rivE,
                                                        tn->riv_th,
                                                        tn->ku_riv,
                                                        tn->Area_riv,
                                                        tn->aqf_S,
                                                        tn->aqf_Top,
                                                        tn->aqf_Bot);
                    nl["Infiltro"] = NumericVector::create(tn->infilt_method,
                                                           tn->PSI,
                                                           tn->ht_fc,
                                                           tn->ht_fo,
                                                           tn->ht_k);
                    nl["Intake"] = NumericVector::create(tn->gw_intake_rate,
                                                         tn->leakage_rate);
                    nl["GWout"] = NumericVector::create(tn->gw_move_node,
                                                        tn->slope_method,
                                                        tn->slope_aqf,
                                                        tn->node_len,
                                                        tn->conn_len,
                                                        tn->Kgw);
                    nl["Evaporation"] = NumericVector::create(tn->solcov,
                                                              tn->ET_method,
                                                              tn->LAI[0],
                                                              tn->LAI[1],
                                                              tn->LAI[2],
                                                              tn->LAI[3],
                                                              tn->LAI[4],
                                                              tn->LAI[5],
                                                              tn->LAI[6],
                                                              tn->LAI[7],
                                                              tn->LAI[8],
                                                              tn->LAI[9],
                                                              tn->LAI[10],
                                                              tn->LAI[11]);
                    IntegerVector nid(tn->m_nClimates);
                    NumericVector nrain(tn->m_nClimates);
                    NumericVector neva(tn->m_nClimates);
                    for(int j = 0; j < tn->m_nClimates; j++)
                    {
                        nid(j) = tn->m_Climates[j].nID;
                        nrain(j) = tn->m_Climates[j].nRain;
                        neva(j) = tn->m_Climates[j].nEva;
                    }
                    nl["Weather"] = DataFrame::create(_["nID"] = nid,
                                                      _["nRain"] = nrain,
                                                      _["nEva"] = neva);
                }
                break;
            case NODE_PADDY:
                node_class = string("Paddy");
                {
                    TPaddy *tn = (TPaddy *)node;
                    nl["Topology"] = NumericVector::create(tn->Area,
                                                           tn->slope,
                                                           tn->Aratio_imp,
                                                           tn->Aratio_per,
                                                           tn->Aratio_per_plant,
                                                           tn->depC_imp,
                                                           tn->depC_per);
                    nl["Soil"] = NumericVector::create(tn->theta_per,
                                                       tn->soil_th_per,
                                                       tn->theta_s_per,
                                                       tn->theta_r_per,
                                                       tn->theta_W_per,
                                                       tn->theta_FC_per,
                                                       tn->ks_per,
                                                       tn->ksi_per,
                                                       tn->n_mualem);
                    nl["River"] = NumericVector::create(tn->gwE,
                                                        tn->rivE,
                                                        tn->riv_th,
                                                        tn->ku_riv,
                                                        tn->Area_riv,
                                                        tn->aqf_S,
                                                        tn->aqf_Top,
                                                        tn->aqf_Bot);
                    nl["Intake"] = NumericVector::create(tn->gw_intake_rate,
                                                         tn->leakage_rate);
                    nl["GWout"] = NumericVector::create(tn->gw_move_node,
                                                        tn->slope_method,
                                                        tn->slope_aqf,
                                                        tn->node_len,
                                                        tn->conn_len,
                                                        tn->Kgw);
                    nl["Evaporation"] = NumericVector::create(tn->solcov,
                                                              tn->ET_method,
                                                              tn->LAI[0],
                                                              tn->LAI[1],
                                                              tn->LAI[2],
                                                              tn->LAI[3],
                                                              tn->LAI[4],
                                                              tn->LAI[5],
                                                              tn->LAI[6],
                                                              tn->LAI[7],
                                                              tn->LAI[8],
                                                              tn->LAI[9],
                                                              tn->LAI[10],
                                                              tn->LAI[11]);
                    IntegerVector nid(tn->m_nClimates);
                    NumericVector nrain(tn->m_nClimates);
                    NumericVector neva(tn->m_nClimates);
                    for(int j = 0; j < tn->m_nClimates; j++)
                    {
                        nid(j) = tn->m_Climates[j].nID;
                        nrain(j) = tn->m_Climates[j].nRain;
                        neva(j) = tn->m_Climates[j].nEva;
                    }
                    nl["Weather"] = DataFrame::create(_["nID"] = nid,
                                                      _["nRain"] = nrain,
                                                      _["nEva"] = neva);
                    nl["Irrigation"] = IntegerVector::create(tn->irr_start_mon,
                                                             tn->irr_start_day,
                                                             tn->irr_end_mon,
                                                             tn->irr_end_day);
                    nl["Coefficient"] = NumericVector::create(tn->irr_supply,
                                                              tn->surf_dr_cf,
                                                              tn->surf_dr_depth,
                                                              tn->soil_dr_cf,
                                                              tn->udgw_dr_cf);
                    NumericVector surf_dr_ht(12);
                    for(int j = 0; j < 12; j++) surf_dr_ht[j] = tn->surf_dr_ht[j];
                    nl["Drain"] = surf_dr_ht;
                }
                break;
            case NODE_LINK:
                node_class = string("Link");
                {
                    TLink *tn = (TLink *)node;
                    nl["Method"] = IntegerVector::create(tn->Method);
// TODO (hspark#1#): Method 번호에 따라 선택적으로 매개변수를 설정할 필요있음

                    nl["Muskingum"] = NumericVector::create(tn->DT,
                                                            tn->X,
                                                            tn->K);
                    nl["Cunge"] = NumericVector::create(tn->Delta_dis,
                                                        tn->Routing_So,
                                                        tn->Routing_N,
                                                        tn->Routing_B,
                                                        tn->Peak_t);
                    nl["Kinematic"] = NumericVector::create(tn->mann,
                                                            tn->slope_riv,
                                                            tn->length_riv,
                                                            tn->Bottom_riv,
                                                            tn->Top_riv,
                                                            tn->depth_riv,
                                                            tn->init_route);
                    nl["Connect"] = IntegerVector::create(tn->m_nStartID,
                                                          tn->m_nEndID);
                }
                break;
            case NODE_JUNC:
                node_class = string("Junction");
                break;
            case NODE_OUTLET:
                node_class = string("Outlet");
                break;
            case NODE_CLIMATE:
                node_class = string("Climate");
                {
                    TClimate *tn = (TClimate *)node;
                    nl["Rainfall"] = StringVector::create(tn->m_szClimate);
                    nl["Evaporation"] = StringVector::create(tn->m_szEva);
                    nl["Calculation"] = NumericVector::create(tn->m_bUseCalc ? 1 : 0,
                                                              tn->m_nLat,
                                                              tn->m_nHeight,
                                                              tn->m_nWindHeight);
                    // tn->m_sClimate
                    // tn->m_sClimateSrc
                }
                break;
            case NODE_IMPORT:
                node_class = string("Import");
                {
                    TImport *tn = (TImport *)node;
                    nl["Type"] = IntegerVector::create(tn->type);
                    nl["Constant"] = NumericVector::create(tn->m_nConst);
                    nl["Series"] = StringVector::create(tn->m_szSeries); // 확인 필요
                    nl["Table"] = IntegerVector::create(tn->m_nTable,
                                                        tn->m_nData);
                    nl["Leakage"] = NumericVector::create(tn->m_nLeakage);
                }
                break;
            case NODE_INFILTRO:
                node_class = string("Infiltro");
                {
                    TInfiltro *tn = (TInfiltro *)node;
                    nl["Aquifer"] = NumericVector::create(tn->area,
                                                          tn->aqf_top,
                                                          tn->aqf_bot,
                                                          tn->aqf_S_coef,
                                                          tn->potential,
                                                          tn->in_node_id);
                    nl["GWMove"] = NumericVector::create(tn->gw_node_id,
                                                         tn->slope_method,
                                                         tn->slope_aqf,
                                                         tn->node_len,
                                                         tn->conn_len,
                                                         tn->Kgw);
                }
                break;
            case NODE_BIORETENTION:
                node_class = string("BioRetention");
                {
                    TBioRetention *tn = (TBioRetention *)node;
                    nl["Aquifer"] = NumericVector::create(tn->area,
                                                          tn->aqf_top,
                                                          tn->aqf_bot,
                                                          tn->aqf_S_coef,
                                                          tn->potential,
                                                          tn->in_node_id);
                    nl["GWMove"] = NumericVector::create(tn->gw_node_id,
                                                         tn->slope_method,
                                                         tn->slope_aqf,
                                                         tn->node_len,
                                                         tn->conn_len,
                                                         tn->Kgw);
                    nl["Evaporation"] = NumericVector::create(tn->LAI[0],
                                                              tn->LAI[1],
                                                              tn->LAI[2],
                                                              tn->LAI[3],
                                                              tn->LAI[4],
                                                              tn->LAI[5],
                                                              tn->LAI[6],
                                                              tn->LAI[7],
                                                              tn->LAI[8],
                                                              tn->LAI[9],
                                                              tn->LAI[10],
                                                              tn->LAI[11]);
                    nl["Rainfall"] = NumericVector::create(tn->m_Climates[0].nID); //,
                                                           //tn->m_Climates[0].nRain);
                    nl["EVA"] = NumericVector::create(tn->m_Climates[1].nID); //,
                                                      //tn->m_Climates[1].nEva);
                }
                break;
            case NODE_WETLAND:
                node_class = string("WetLand");
                {
                    TWetLand *tn = (TWetLand *)node;
                    nl["Base"] = NumericVector::create(tn->vol_init,
                                                       tn->vol_max,
                                                       tn->bypass,
                                                       tn->aqf_ks);
                    nl["Pipe"] = NumericVector::create(tn->pipe_ht,
                                                       tn->pipe_area,
                                                       tn->pipe_coef);
                    nl["RateCount"] = IntegerVector::create(tn->rate_count);
                    NumericVector wl(tn->rate_count);
                    for(int j = 0; j < tn->rate_count; j++)
                        wl[j] = tn->wl_rate[j][0];
                    nl["WL"] = wl;
                    NumericVector vol(tn->rate_count);
                    for(int j = 0; j < tn->rate_count; j++)
                        vol[j] = tn->wl_rate[j][1];
                    nl["VOL"] = vol;
                    NumericVector area(tn->rate_count);
                    for(int j = 0; j < tn->rate_count; j++)
                        area[j] = tn->wl_rate[j][2];
                    nl["AREA"] = area;
                    nl["Rainfall"] = NumericVector::create(tn->m_Climates[0].nID); //,
                                                           //tn->m_Climates[0].nRain);
                    nl["EVA"] = NumericVector::create(tn->m_Climates[1].nID); //,
                                                      //tn->m_Climates[1].nEva);
                    nl["Recharge"] = IntegerVector::create(tn->recharge_id);
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
                    nl["RateCount"] = IntegerVector::create(tn->rate_count);
                    NumericVector wl(tn->rate_count);
                    for(int j = 0; j < tn->rate_count; j++)
                        wl(j) = tn->wl_rate[j][0];
                    nl["WL"] = wl;
                    NumericVector vol(tn->rate_count);
                    for(int j = 0; j < tn->rate_count; j++)
                        vol(j) = tn->wl_rate[j][1];
                    nl["VOL"] = vol;
                    NumericVector area(tn->rate_count);
                    for(int j = 0; j < tn->rate_count; j++)
                        area(j) = tn->wl_rate[j][2];
                    nl["AREA"] = area;
                    nl["Rainfall"] = IntegerVector::create(tn->m_Climates[0].nID); //,
                                                           //tn->m_Climates[0].nRain);
                    nl["EVA"] = IntegerVector::create(tn->m_Climates[1].nID); //,
                                                      //tn->m_Climates[1].nEva);
                    nl["Supply"] = IntegerVector::create(tn->supply_id);
                    nl["Recharge"] = IntegerVector::create(tn->recharge_id);
                }
                break;
            case NODE_RECYCLE:
                node_class = string("Recycle");
                {
                    TRecycle *tn = (TRecycle *)node;
                    nl["Intake"] = NumericVector::create(tn->rec_cond, tn->rec_intake);
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
        ll.attr("class") = StringVector::create("cat_" + node_class);
        Nodes[i + 1] = ll;
    }
    //ret["Nodes"] = wrap(Nodes);
    //ret["Version"] = StringVector::create("2.0.0.1");
    //ret["Title"] = StringVector::create("RCAT");
    //ret["StartTime"] = IntegerVector::create(model->m_dtStart.GetYear(),
    //                                         model->m_dtStart.GetMonth(),
    //                                         model->m_dtStart.GetDay(),
    //                                         model->m_dtStart.GetHour(),
    //                                         model->m_dtStart.GetMinute());
    //ret["EndTime"] = IntegerVector::create(model->m_dtEnd.GetYear(),
    //                                       model->m_dtEnd.GetMonth(),
    //                                       model->m_dtEnd.GetDay(),
    //                                       model->m_dtEnd.GetHour(),
    //                                       model->m_dtEnd.GetMinute());
    //ret["Parameter"] = IntegerVector::create(model->m_nDT, model->m_nLoop);
    //ret["Range"] = StringVector::create("2.0.0.1");
    //ret["NodeCount"] = IntegerVector(loop_count);
    //List wret = wrap(ret);
    List wret = wrap(Nodes);
    wret.attr("Version") = StringVector::create("2.0.0.1");
    wret.attr("Title") = StringVector::create("RCAT");
    wret.attr("StartTime") = IntegerVector::create(model->m_dtStart.GetYear(),
                                                   model->m_dtStart.GetMonth(),
                                                   model->m_dtStart.GetDay(),
                                                   model->m_dtStart.GetHour(),
                                                   model->m_dtStart.GetMinute());
    wret.attr("EndTime") = IntegerVector::create(model->m_dtEnd.GetYear(),
                                                 model->m_dtEnd.GetMonth(),
                                                 model->m_dtEnd.GetDay(),
                                                 model->m_dtEnd.GetHour(),
                                                 model->m_dtEnd.GetMinute());
    wret.attr("Parameter") = IntegerVector::create(model->m_nDT, model->m_nLoop);
    wret.attr("class") = "rcat_input";
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
    //!< class 속성이 rcat_input 인지 확인
    if(as<string>(ml.attr("class")) == "rcat_input" )
    {
        TModelManager *model = new TModelManager();
        IntegerVector iv = as<IntegerVector>(ml.attr("StartTime"));
        model->m_dtStart = TDate::ToMinute(iv.length() > 0 ? iv[0] : 0,
                                           iv.length() > 1 ? iv[1] : 0,
                                           iv.length() > 2 ? iv[2] : 0,
                                           iv.length() > 3 ? iv[3] : 0,
                                           iv.length() > 4 ? iv[4] : 0);
        iv = as<IntegerVector>(ml.attr("EndTime"));
        model->m_dtEnd = TDate::ToMinute(iv.length() > 0 ? iv[0] : 0,
                                         iv.length() > 1 ? iv[1] : 0,
                                         iv.length() > 2 ? iv[2] : 0,
                                         iv.length() > 3 ? iv[3] : 0,
                                         iv.length() > 4 ? iv[4] : 0);
        iv = as<IntegerVector>(ml.attr("Parameter"));
        model->m_nDT = iv.length() > 0 ? iv[0] : 0;
        model->m_nLoop = iv.length() > 1 ? iv[1] : 0;

        List Nodes = ml;
        int loop_count = Nodes.length();
        //model->m_nCount = loop_count = 10;
        for(int i = 0; i < loop_count; i++)
        {
            List node = Nodes[i];
            string node_class = as<string>(as<StringVector>(node.attr("class"))[0]);
            TBaseNode *pNode = NULL;

            if(node_class == "cat_Urban" || //!< NODE_URBAN:
               node_class == "cat_Forest") //!< NODE_FOREST:
            {
                TUrban *pn = new TUrban;
                if(node.containsElementNamed("Topology"))
                {
                    NumericVector nv = as<NumericVector>(node["Topology"]);
                    pn->Area = nv.length() > 0 ? nv[0] : 0;
                    pn->slope = nv.length() > 1 ? nv[1] : 0;
                    pn->Aratio_imp = nv.length() > 2 ? nv[2] : 0;
                    //pn->Aratio_per = nv.length() > 3 ? nv[3] : 0;
                    pn->Aratio_per_plant = nv.length() > 4 ? nv[4] : 0;
                    pn->depC_imp = nv.length() > 5 ? nv[5] : 0;
                    pn->depC_per = nv.length() > 6 ? nv[6]: 0;
                }
                if(node.containsElementNamed("Soil"))
                {
                    NumericVector nv = as<NumericVector>(node["Soil"]);
                    pn->theta_per = nv.length() > 0 ? nv[0] : 0;
                    pn->soil_th_per = nv.length() > 1 ? nv[1] : 0;
                    pn->theta_s_per = nv.length() > 2 ? nv[2] : 0;
                    pn->theta_r_per = nv.length() > 3 ? nv[3] : 0;
                    pn->theta_W_per = nv.length() > 5 ? nv[5] : 0;
                    pn->theta_FC_per = nv.length() > 4 ? nv[4] : 0;
                    pn->ks_per = nv.length() > 6 ? nv[6] : 0;
                    pn->ksi_per = nv.length() > 7 ? nv[7] : 0;
                    pn->n_mualem = nv.length() > 8 ? nv[8] : 0;
                }
                if(node.containsElementNamed("River"))
                {
                    NumericVector nv = as<NumericVector>(node["River"]);
                    pn->gwE = nv.length() > 0 ? nv[0] : 0;
                    pn->rivE = nv.length() > 1 ? nv[1] : 0;
                    pn->riv_th = nv.length() > 2 ? nv[2] : 0;
                    pn->ku_riv = nv.length() > 3 ? nv[3] : 0;
                    pn->Area_riv = nv.length() > 4 ? nv[4] : 0;
                    pn->aqf_S = nv.length() > 5 ? nv[5] : 0;
                    pn->aqf_Top = nv.length() > 6 ? nv[6] : 0;
                    pn->aqf_Bot = nv.length() > 7 ? nv[7] : 0;
                }
                if(node.containsElementNamed("Infiltro"))
                {
                    NumericVector nv = as<NumericVector>(node["Infiltro"]);
                    pn->infilt_method = nv.length() > 0 ? nv[0] : 0;
                    pn->PSI = nv.length() > 1 ? nv[1] : 0;
                    pn->ht_fc = nv.length() > 2 ? nv[2] : 0;
                    pn->ht_fo = nv.length() > 3 ? nv[3] : 0;
                    pn->ht_k = nv.length() > 4 ? nv[4] : 0;
                }
                if(node.containsElementNamed("Intake"))
                {
                    NumericVector nv = as<NumericVector>(node["Intake"]);
                    pn->gw_intake_rate = nv.length() > 0 ? nv[0] : 0;
                    pn->leakage_rate = nv.length() > 1 ? nv[1] : 0;
                }
                if(node.containsElementNamed("GWout"))
                {
                    NumericVector nv = as<NumericVector>(node["GWout"]);
                    pn->gw_move_node = nv.length() > 0 ? nv[0] : 0;
                    pn->slope_method = nv.length() > 1 ? nv[1] : 0;
                    pn->slope_aqf = nv.length() > 2 ? nv[2] : 0;
                    pn->node_len = nv.length() > 3 ? nv[3] : 0;
                    pn->conn_len = nv.length() > 4 ? nv[4] : 0;
                    pn->Kgw = nv.length() > 5 ? nv[5] : 0;
                }
                if(node.containsElementNamed("Evaporation"))
                {
                    NumericVector nv = as<NumericVector>(node["Evaporation"]);
                    pn->solcov = nv.length() > 0 ? nv[0] : 0;
                    pn->ET_method = nv.length() > 1 ? nv[1] : 0;
                    for(int j = 0; j < 12; j++)
                        pn->LAI[j] = nv.length() > 2 + j ? nv[2 + j] : 0;
                }
                if(node.containsElementNamed("Weather"))
                {
                    DataFrame df = as<DataFrame>(node["Weather"]);
                    pn->m_nClimates = df.nrows();
                    for(int j = 0; j < pn->m_nClimates; j++)
                    {
                        pn->m_Climates[j].nID = as<IntegerVector>(df["nID"])[j];
                        pn->m_Climates[j].nRain = as<IntegerVector>(df["nRain"])[j];
                        pn->m_Climates[j].nEva = as<IntegerVector>(df["nEva"])[j];
                    }
                }
                pNode = pn;
                if(node_class == "cat_Forest")
                    pNode->m_nType = NODE_FOREST;
            }
            else if(node_class == "cat_Paddy") //!< NODE_PADDY:
            {
                TPaddy *pn = new TPaddy;
                if(node.containsElementNamed("Topology"))
                {
                    NumericVector nv = as<NumericVector>(node["Topology"]);
                    pn->Area = nv.length() > 0 ? nv[0] : 0;
                    pn->slope = nv.length() > 1 ? nv[1] : 0;
                    pn->Aratio_imp = nv.length() > 2 ? nv[2] : 0;
                    pn->Aratio_per = nv.length() > 3 ? nv[3] : 0;
                    pn->Aratio_per_plant = nv.length() > 4 ? nv[4] : 0;
                    pn->depC_imp = nv.length() > 5 ? nv[5] : 0;
                    pn->depC_per = nv.length() > 6 ? nv[6]: 0;
                }
                if(node.containsElementNamed("Soil"))
                {
                    NumericVector nv = as<NumericVector>(node["Soil"]);
                    pn->theta_per = nv.length() > 0 ? nv[0] : 0;
                    pn->soil_th_per = nv.length() > 1 ? nv[1] : 0;
                    pn->theta_s_per = nv.length() > 2 ? nv[2] : 0;
                    pn->theta_r_per = nv.length() > 3 ? nv[3] : 0;
                    pn->theta_W_per = nv.length() > 4 ? nv[4] : 0;
                    pn->theta_FC_per = nv.length() > 5 ? nv[5] : 0;
                    pn->ks_per = nv.length() > 6 ? nv[6] : 0;
                    pn->ksi_per = nv.length() > 7 ? nv[7] : 0;
                    pn->n_mualem = nv.length() > 8 ? nv[8] : 0;
                }
                if(node.containsElementNamed("River"))
                {
                    NumericVector nv = as<NumericVector>(node["River"]);
                    pn->gwE = nv.length() > 0 ? nv[0] : 0;
                    pn->rivE = nv.length() > 1 ? nv[1] : 0;
                    pn->riv_th = nv.length() > 2 ? nv[2] : 0;
                    pn->ku_riv = nv.length() > 3 ? nv[3] : 0;
                    pn->Area_riv = nv.length() > 4 ? nv[4] : 0;
                    pn->aqf_S = nv.length() > 5 ? nv[5] : 0;
                    pn->aqf_Top = nv.length() > 6 ? nv[6] : 0;
                    pn->aqf_Bot = nv.length() > 7 ? nv[7] : 0;
                }
                if(node.containsElementNamed("Intake"))
                {
                    NumericVector nv = as<NumericVector>(node["Intake"]);
                    pn->gw_intake_rate = nv.length() > 0 ? nv[0] : 0;
                    pn->leakage_rate = nv.length() > 1 ? nv[1] : 0;
                }
                if(node.containsElementNamed("GWout"))
                {
                    NumericVector nv = as<NumericVector>(node["GWout"]);
                    pn->gw_move_node = nv.length() > 0 ? nv[0] : 0;
                    pn->slope_method = nv.length() > 1 ? nv[1] : 0;
                    pn->slope_aqf = nv.length() > 2 ? nv[2] : 0;
                    pn->node_len = nv.length() > 3 ? nv[3] : 0;
                    pn->conn_len = nv.length() > 4 ? nv[4] : 0;
                    pn->Kgw = nv.length() > 5 ? nv[5] : 0;
                }
                if(node.containsElementNamed("Evaporation"))
                {
                    NumericVector nv = as<NumericVector>(node["Evaporation"]);
                    pn->solcov = nv.length() > 0 ? nv[0] : 0;
                    pn->ET_method = nv.length() > 1 ? nv[1] : 0;
                    for(int j = 0; j < 12; j++)
                        pn->LAI[j] = nv.length() > 2 + j ? nv[2 + j] : 0;
// TODO (hspark#1#): 읽어온 값과 내보낸 값이 틀려지는 이유는 뭘까?
                }
                if(node.containsElementNamed("Weather"))
                {
                    DataFrame df = as<DataFrame>(node["Weather"]);
                    pn->m_nClimates = df.nrows();
                    for(int j = 0; j < pn->m_nClimates; j++)
                    {
                        pn->m_Climates[j].nID = as<IntegerVector>(df["nID"])[j];
                        pn->m_Climates[j].nRain = as<IntegerVector>(df["nRain"])[j];
                        pn->m_Climates[j].nEva = as<IntegerVector>(df["nEva"])[j];
                    }
                }
                if(node.containsElementNamed("Irrigation"))
                {
                    IntegerVector iv = as<IntegerVector>(node["Irrigation"]);
                    pn->irr_start_mon = iv.length() > 0 ? iv[0] : 0;
                    pn->irr_start_day = iv.length() > 1 ? iv[1] : 0;
                    pn->irr_end_mon = iv.length() > 2 ? iv[2] : 0;
                    pn->irr_end_day = iv.length() > 3 ? iv[3] : 0;
                }
                if(node.containsElementNamed("Coefficient"))
                {
                    NumericVector nv = as<NumericVector>(node["Coefficient"]);
                    pn->irr_supply = nv.length() > 0 ? nv[0] : 0;
                    pn->surf_dr_cf = nv.length() > 1 ? nv[1] : 0;
                    pn->surf_dr_depth = nv.length() > 2 ? nv[2] : 0;
                    pn->soil_dr_cf = nv.length() > 3 ? nv[3] : 0;
                    pn->udgw_dr_cf = nv.length() > 4 ? nv[4] : 0;
                }
                if(node.containsElementNamed("Drain"))
                {
                    NumericVector nv = as<NumericVector>(node["Drain"]);
                    for(int j = 0; j < 12; j++)
                        pn->surf_dr_ht[j] = nv.length() > j ? nv[j] : 0;
                }
                pNode = pn;
            }
            else if(node_class == "cat_Link") //!<  NODE_LINK:
            {
                TLink *pn = new TLink;
                if(node.containsElementNamed("Method"))
                {
                    IntegerVector iv = as<IntegerVector>(node["Method"]);
                    pn->Method = iv.length() > 0 ? iv[0] : 0;
                }
                if(node.containsElementNamed("Muskingum"))
                {
                    NumericVector nv = as<NumericVector>(node["Muskingum"]);
                    pn->DT = nv.length() > 0 ? nv[0] : 0;
                    pn->X = nv.length() > 1 ? nv[1] : 0;
                    pn->K = nv.length() > 2 ? nv[2] : 0;
                }
                if(node.containsElementNamed("Cunge"))
                {
                    NumericVector nv = as<NumericVector>(node["Cunge"]);
                    pn->Delta_dis = nv.length() > 0 ? nv[0] : 0;
                    pn->Routing_So = nv.length() > 1 ? nv[1] : 0;
                    pn->Routing_N = nv.length() > 2 ? nv[2] : 0;
                    pn->Routing_B = nv.length() > 3 ? nv[3] : 0;
                    pn->Peak_t = nv.length() > 4 ? nv[4] : 0;
                }
                if(node.containsElementNamed("Kinematic"))
                {
                    NumericVector nv = as<NumericVector>(node["Kinematic"]);
                    pn->mann = nv.length() > 0 ? nv[0] : 0;
                    pn->slope_riv = nv.length() > 1 ? nv[1] : 0;
                    pn->length_riv = nv.length() > 2 ? nv[2] : 0;
                    pn->Bottom_riv = nv.length() > 3 ? nv[3] : 0;
                    pn->Top_riv = nv.length() > 4 ? nv[4] : 0;
                    pn->depth_riv = nv.length() > 5 ? nv[5] : 0;
                    pn->init_route = nv.length() > 6 ? nv[6] : 0;
                }
                if(node.containsElementNamed("Connect"))
                {
                    IntegerVector iv = as<IntegerVector>(node["Connect"]);
                    pn->m_nStartID = iv.length() > 0 ? iv[0] : 0;
                    pn->m_nEndID = iv.length() > 1 ? iv[1] : 0;
                }
                pNode = pn;
            }
            else if(node_class == "cat_Junction") //!< NODE_JUNC:
            {
                TJunc *pn = new TJunc;
                pNode = pn;
            }
            else if(node_class == "cat_Outlet") //!< NODE_OUTLET:
            {
                TJunc *pn = new TJunc;
                pNode = pn;
                pNode->m_nType = NODE_OUTLET;
            }
            else if(node_class == "cat_Climate") //!< NODE_CLIMATE:
            {
                TClimate *pn = new TClimate;
                if(node.containsElementNamed("Rainfall"))
                {
                    StringVector sv = as<StringVector>(node["Rainfall"]);
                    pn->SetClimateFile(sv.length() > 0 ? sv[0] : (char *)"");
                }
                if(node.containsElementNamed("Evaporation"))
                {
                    StringVector sv = as<StringVector>(node["Evaporation"]);
                    pn->SetEvaFile(sv.length() > 0 ? sv[0] : (char *)"");
                }
                if(node.containsElementNamed("Calculation"))
                {
                    NumericVector nv = as<NumericVector>(node["Calculation"]);
                    pn->m_bUseCalc = nv.length() > 0 ? nv[0] != 0.0: false;
                    pn->m_nLat = nv.length() > 1 ? nv[1] : 0;
                    pn->m_nHeight = nv.length() > 2 ? nv[2] : 0;
                    pn->m_nWindHeight = nv.length() > 3 ? nv[3] : 0;
                }
                pNode = pn;
            }
            else if(node_class == "cat_Import") //!< NODE_IMPORT:
            {
                TImport *pn = new TImport;
                if(node.containsElementNamed("Type"))
                {
                    IntegerVector iv = as<IntegerVector>(node["Type"]);
                    pn->type = iv.length() > 0 ? iv[0] : 0;
                }
                if(node.containsElementNamed("Constant"))
                {
                    NumericVector nv = as<NumericVector>(node["Constant"]);
                    pn->m_nConst = nv.length() > 0 ? nv[0] : 0;
                }
                if(node.containsElementNamed("Series"))
                {
                    StringVector sv = as<StringVector>(node["Series"]);
                    pn->SetSeriesFileA(sv.length() > 0 ? sv[0] : (char *)"");
                }
                if(node.containsElementNamed("Table"))
                {
                    IntegerVector iv = as<IntegerVector>(node["Table"]);
                    pn->m_nTable = iv.length() > 0 ? iv[0] : 0;
                    pn->m_nData = iv.length() > 1 ? iv[1] : 0;
                }
                if(node.containsElementNamed("Leakage"))
                {
                    NumericVector nv = as<NumericVector>(node["Leakage"]);
                    pn->m_nLeakage = nv.length() > 0 ? nv[0] : 0;
                }
                pNode = pn;
            }
            else if(node_class == "cat_Infiltro") //!< NODE_INFILTRO:
            {
                TInfiltro *pn = new TInfiltro;
                if(node.containsElementNamed("Aquifer"))
                {
                    NumericVector nv = as<NumericVector>(node["Aquifer"]);
                    pn->area = nv.length() > 0 ? nv[0] : 0;
                    pn->aqf_top = nv.length() > 1 ? nv[1] : 0;
                    pn->aqf_bot = nv.length() > 2 ? nv[2] : 0;
                    pn->aqf_S_coef = nv.length() > 3 ? nv[3] : 0;
                    pn->potential = nv.length() > 4 ? nv[4] : 0;
                    pn->in_node_id = nv.length() > 5 ? nv[5] : 0;
                }
                if(node.containsElementNamed("GWMove"))
                {
                    NumericVector nv = as<NumericVector>(node["GWMove"]);
                    pn->gw_node_id = nv.length() > 0 ? nv[0] : 0;
                    pn->slope_method = nv.length() > 1 ? nv[1] : 0;
                    pn->slope_aqf = nv.length() > 2 ? nv[2] : 0;
                    pn->node_len = nv.length() > 3 ? nv[3] : 0;
                    pn->conn_len = nv.length() > 4 ? nv[4] : 0;
                    pn->Kgw = nv.length() > 5 ? nv[5] : 0;
                }
                pNode = pn;
            }
            else if(node_class == "cat_BioRetention") //!< NODE_BIORETENTION:
            {
                TBioRetention *pn = new TBioRetention;
                if(node.containsElementNamed("Aquifer"))
                {
                    NumericVector nv = as<NumericVector>(node["Aquifer"]);
                    pn->area = nv.length() > 0 ? nv[0] : 0;
                    pn->aqf_top = nv.length() > 1 ? nv[1] : 0;
                    pn->aqf_bot = nv.length() > 2 ? nv[2] : 0;
                    pn->aqf_S_coef = nv.length() > 3 ? nv[3] : 0;
                    pn->potential = nv.length() > 4 ? nv[4] : 0;
                    pn->in_node_id = nv.length() > 5 ? nv[5] : 0;
                }
                if(node.containsElementNamed("GWMove"))
                {
                    NumericVector nv = as<NumericVector>(node["GWMove"]);
                    pn->gw_node_id = nv.length() > 0 ? nv[0] : 0;
                    pn->slope_method = nv.length() > 1 ? nv[1] : 0;
                    pn->slope_aqf = nv.length() > 2 ? nv[2] : 0;
                    pn->node_len = nv.length() > 3 ? nv[3] : 0;
                    pn->conn_len = nv.length() > 4 ? nv[4] : 0;
                    pn->Kgw = nv.length() > 5 ? nv[5] : 0;
                }
                if(node.containsElementNamed("Evaporation"))
                {
                    NumericVector nv = as<NumericVector>(node["Evaporation"]);
                    for(int j = 0; j < 12; j++)
                        pn->LAI[j] = nv.length() > j ? nv[j] : 0;
                }
                if(node.containsElementNamed("Rainfall"))
                {
                    NumericVector nv = as<NumericVector>(node["Rainfall"]);
                    pn->m_Climates[0].nID = nv.length() > 0 ? nv[0] : 0;
                    pn->m_Climates[0].nRain = 100; //nv.length() > 1 ? nv[1] : 0;
                }
                if(node.containsElementNamed("EVA"))
                {
                    NumericVector nv = as<NumericVector>(node["EVA"]);
                    pn->m_Climates[1].nID = nv.length() > 0 ? nv[0] : 0;
                    pn->m_Climates[1].nEva = 100; //nv.length() > 1 ? nv[1] : 0;
                }

                pNode = pn;
            }
            else if(node_class == "cat_WetLand") //!< NODE_WETLAND:
            {
                TWetLand *pn = new TWetLand;
                if(node.containsElementNamed("Base"))
                {
                    NumericVector nv = as<NumericVector>(node["Base"]);
                    pn->vol_init = nv.length() > 0 ? nv[0] : 0;
                    pn->vol_max = nv.length() > 1 ? nv[1] : 0;
                    pn->bypass = nv.length() > 2 ? nv[2] : 0;
                    pn->aqf_ks = nv.length() > 3 ? nv[3] : 0;

                }
                if(node.containsElementNamed("Pipe"))
                {
                    NumericVector nv = as<NumericVector>(node["Pipe"]);
                    pn->pipe_ht = nv.length() > 0 ? nv[0] : 0;
                    pn->pipe_area = nv.length() > 1 ? nv[1] : 0;
                    pn->pipe_coef = nv.length() > 2 ? nv[2] : 0;

                }
                if(node.containsElementNamed("RateCount"))
                {
                    IntegerVector iv = as<IntegerVector>(node["RateCount"]);
                    pn->rate_count = iv.length() > 0 ? iv[0] : 0;
                    if(node.containsElementNamed("WL"))
                    {
                        NumericVector nv = as<NumericVector>(node["WL"]);
                        for(int j = 0; j < pn->rate_count; j++)
                            pn->wl_rate[j][0] = nv.length() > j ? nv[j] : 0;

                    }
                    if(node.containsElementNamed("VOL"))
                    {
                        NumericVector nv = as<NumericVector>(node["VOL"]);
                        for(int j = 0; j < pn->rate_count; j++)
                            pn->wl_rate[j][1] = nv.length() > j ? nv[j] : 0;

                    }
                    if(node.containsElementNamed("AREA"))
                    {
                        NumericVector nv = as<NumericVector>(node["AREA"]);
                        for(int j = 0; j < pn->rate_count; j++)
                            pn->wl_rate[j][2] = nv.length() > j ? nv[j] : 0;

                    }
                }
                if(node.containsElementNamed("Rainfall"))
                {
                    NumericVector nv = as<NumericVector>(node["Rainfall"]);
                    pn->m_Climates[0].nID = nv.length() > 0 ? nv[0] : 0;
                    pn->m_Climates[0].nRain = 100; //nv.length() > 1 ? nv[1] : 0;

                }
                if(node.containsElementNamed("EVA"))
                {
                    NumericVector nv = as<NumericVector>(node["EVA"]);
                    pn->m_Climates[1].nID = nv.length() > 0 ? nv[0] : 0;
                    pn->m_Climates[1].nEva = 100; //nv.length() > 1 ? nv[1] : 0;

                }
                if(node.containsElementNamed("Recharge"))
                {
                    IntegerVector iv = as<IntegerVector>(node["Recharge"]);
                    pn->recharge_id = iv.length() > 0 ? iv[0] : 0;

                }

                pNode = pn;
            }
            else if(node_class == "cat_Pond") //!< NODE_POND:
            {
                TPond *pn = new TPond;
                if(node.containsElementNamed("Base"))
                {
                    NumericVector nv = as<NumericVector>(node["Base"]);
                    pn->vol_init = nv.length() > 0 ? nv[0] : 0;
                    pn->vol_eff = nv.length() > 1 ? nv[1] : 0;
                    pn->aqf_ks = nv.length() > 2 ? nv[2] : 0;
                }
                if(node.containsElementNamed("Pipe"))
                {
                    NumericVector nv = as<NumericVector>(node["Pipe"]);
                    pn->pipe_ht = nv.length() > 0 ? nv[0] : 0;
                    pn->pipe_area = nv.length() > 1 ? nv[1] : 0;
                    pn->pipe_coef = nv.length() > 2 ? nv[2] : 0;
                }
                if(node.containsElementNamed("Spill"))
                {
                    NumericVector nv = as<NumericVector>(node["Spill"]);
                    pn->spill_ht = nv.length() > 0 ? nv[0] : 0;
                    pn->spill_length = nv.length() > 1 ? nv[1] : 0;
                    pn->spill_coef = nv.length() > 2 ? nv[2] : 0;
                }
                if(node.containsElementNamed("Offline"))
                {
                    NumericVector nv = as<NumericVector>(node["Offline"]);
                    pn->offline_max = nv.length() > 0 ? nv[0] : 0;
                    pn->offline_ratio = nv.length() > 1 ? nv[1] : 0;
                    pn->offline_out = nv.length() > 2 ? nv[2] : 0;
                }
                if(node.containsElementNamed("Intake"))
                {
                    NumericVector nv = as<NumericVector>(node["Intake"]);
                    pn->intake_type = nv.length() > 0 ? nv[0] : 0;
                    pn->intake_vol = nv.length() > 1 ? nv[1] : 0;
                    pn->m_nTable = nv.length() > 2 ? nv[2] : 0;
                    pn->m_nData = nv.length() > 3 ? nv[3] : 0;
                }
                if(node.containsElementNamed("Series"))
                {
                    StringVector sv = as<StringVector>(node["Series"]);
                    pn->SetSeriesFileA(sv.length() > 0 ? sv[0] : (char *)"");
                }
                if(node.containsElementNamed("RateCount"))
                {
                    IntegerVector nv = as<IntegerVector>(node["RateCount"]);
                    pn->rate_count = nv.length() > 0 ? nv[0] : 0;
                }
                if(node.containsElementNamed("WL"))
                {
                    NumericVector nv = as<NumericVector>(node["WL"]);
                    for(int j = 0; j < pn->rate_count; j++)
                        pn->wl_rate[j][0] = nv.length() > j ? nv[j] : 0;
                }
                if(node.containsElementNamed("VOL"))
                {
                    NumericVector nv = as<NumericVector>(node["VOL"]);
                    for(int j = 0; j < pn->rate_count; j++)
                        pn->wl_rate[j][1] = nv.length() > j ? nv[j] : 0;
                }
                if(node.containsElementNamed("AREA"))
                {
                    NumericVector nv = as<NumericVector>(node["AREA"]);
                    for(int j = 0; j < pn->rate_count; j++)
                        pn->wl_rate[j][2] = nv.length() > j ? nv[j] : 0;
                }
                if(node.containsElementNamed("Rainfall"))
                {
                    IntegerVector nv = as<IntegerVector>(node["Rainfall"]);
                    pn->m_Climates[0].nID = nv.length() > 0 ? nv[0] : 0;
                    pn->m_Climates[0].nRain = 100; //nv.length() > 1 ? nv[1] : 0;
                }
                if(node.containsElementNamed("EVA"))
                {
                    IntegerVector nv = as<IntegerVector>(node["EVA"]);
                    pn->m_Climates[1].nID = nv.length() > 0 ? nv[0] : 0;
                    pn->m_Climates[1].nEva = 100; //nv.length() > 1 ? nv[1] : 0;
                }
                if(node.containsElementNamed("Supply"))
                {
                    IntegerVector nv = as<IntegerVector>(node["Supply"]);
                    pn->supply_id = nv.length() > 0 ? nv[0] : 0;
                }
                if(node.containsElementNamed("Recharge"))
                {
                    IntegerVector nv = as<IntegerVector>(node["Recharge"]);
                    pn->recharge_id = nv.length() > 0 ? nv[0] : 0;
                }
                pNode = pn;
            }
            else if(node_class == "cat_Recycle") //!< NODE_RECYCLE:
            {
                TRecycle *pn = new TRecycle;
                if(node.containsElementNamed("Intake"))
                {
                    NumericVector iv = as<NumericVector>(node["Intake"]);
                    pn->rec_cond = iv.length() > 0 ? iv[0] : 0;
                    pn->rec_intake = iv.length() > 1 ? iv[1] : 0;
                }
                if(node.containsElementNamed("Nodes"))
                {
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
                }
                pNode = pn;
            }
            else if(node_class == "cat_RainTank") //!< NODE_RAINTANK:
            {
                TRainTank * pn = new TRainTank;
                if(node.containsElementNamed("Volume"))
                {
                    NumericVector nv = as<NumericVector>(node["Volume"]);
                    pn->vol_init = nv.length() > 0 ? nv[0] : 0;
                    pn->vol_min = nv.length() > 1 ? nv[1] : 0;
                    pn->cap = nv.length() > 2 ? nv[2] : 0;
                }
                if(node.containsElementNamed("Use"))
                {
                    NumericVector nv = as<NumericVector>(node["Use"]);
                    pn->use_type = nv.length() > 0 ? nv[0] : 0;
                    pn->use = nv.length() > 1 ? nv[1] : 0;
                    pn->m_nTable = nv.length() > 2 ? nv[2] : 0;
                    pn->m_nData = nv.length() > 3 ? nv[3] : 0;
                }
                if(node.containsElementNamed("Series"))
                {
                    StringVector sv = as<StringVector>(node["Series"]);
                    pn->SetSeriesFileA(sv.length() > 0 ? sv[0] : (char *)"");
                }
                if(node.containsElementNamed("Supply"))
                {
                    IntegerVector iv = as<IntegerVector>(node["Supply"]);
                    pn->supply_id = iv.length() > 0 ? iv[0] : 0;
                }
                pNode = pn;
            }

            if(pNode)
            {
                if(node.containsElementNamed("Name"))
                    pNode->SetName((char *)as<string>(node["Name"]).c_str());
                if(node.containsElementNamed("Desc"))
                    pNode->SetDesc((char *)as<string>(node["Desc"]).c_str());
                if(node.containsElementNamed("NodeID"))
                    pNode->SetID(as<int>(node["NodeID"]));
                model->Add(pNode);
                switch(pNode->GetType())
                {
                    case NODE_IMPORT:
                        model->ChangeFilePathA(&(((TImport*)pNode)->m_szSeries[0]));
                        break;
                    case NODE_CLIMATE:
// TODO (hspark#1#): Climate 노드에 데이터를 읽어들이는 부분 ...
//데이터를 파일에서 읽어오는 것이 아니라 List로 부터 읽어오려면 List에 데이터를 저장하는 것 부터 작업해야함
                        TClimate *pclm = (TClimate *)pNode;
                        model->ChangeFilePathA(pclm->m_szClimate);
                        pclm->LoadSeries();
                        //model->CheckClimateLoad((TClimate*)pNode);
                        break;
                }
            }
        }
        model->CheckLoad();
        return model;
    }
    return NULL;
}

// [[Rcpp::export]]
List readInput(StringVector input)
{
    TModelManager model;
    model.LoadText(input(0));
    return Model2List(&model);
}

// [[Rcpp::export]]
List listmodel2listmodel(List input)
{
    List ret;
    TModelManager *model = List2Model(input);
    ret = Model2List(model);
    return ret;
}

// [[Rcpp::export]]
StringVector chekModel(List input)
{
    List ret;
    TModelManager *model = List2Model(input);
    TValidateCheck chk;
    int chkn = chk.Check(model);
    string msg;
    if(chkn != 0) msg = string(chk.GetMessage()->GetBuffer());
    if(model != NULL)
        return StringVector::create(msg);
    return StringVector::create();
}

// [[Rcpp::export]]
List run_cat2(List input, CharacterVector filter)
{
    List ret;
    TModelManager *model = List2Model(input);
    TValidateCheck chk;
    int chkn = chk.Check(model);
    string msg;
    if(chkn != 0) msg = string(chk.GetMessage()->GetBuffer());

    if(model != NULL)
    {
        model->Calculate(FALSE);
        TSerieses *result = model->GetResult();

        ret = List::create(_["ErrCount"] = chkn,
                            _["ErrMsgs"] = StringVector::create(msg),
                            //_["CAT_INPUT"] = Model2List(model),
                            _["CAT_RESULT"] = TSerieses2List(result, (char *)filter(0)));
        TSeriesClear(result);
    }
    else
    {
        ret = List::create();
    }
    delete model;
    return ret;
}


string runcat(char* infile, char* outfile, char* format)
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

  TSeriesClear(pResult);
  return msg;
}

// [[Rcpp::export]]
SEXP run_cat1(List params)
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

  List ret;
  if(pResult->GetCount() > 0)
    ret = List::create(_["msg"] = StringVector::create(msg),
                       _["ret"] = TSerieses2List(pResult, format));
  else
    ret = List::create(_["msg"] = StringVector::create(msg));
  return ret;
}

// [[Rcpp::export]]
StringVector run_cat0(CharacterVector input,
                    CharacterVector output,
                    CharacterVector filter) {
  //    CharacterVector x = CharacterVector::create( "foo", "bar" )  ;
  //    NumericVector y   = NumericVector::create( 0.0, 1.0 ) ;
  //    List z            = List::create( x, y ) ;
  string msg = runcat(input(0), output(0), filter(0));
  return StringVector::create(msg);
}

// [[Rcpp::export]]
SEXP read_cat_serieses(StringVector filename) {
    TSerieses *sr = new TSerieses();
    char *fn = filename[0];
    DWORD chk = TSerieses::GetVersion(fn);
    if(chk == 0xffff || chk == 0)
        return StringVector::create("Error: File is not binary rcat serieses file!!");
    sr->Load(fn);
    List ret = TSerieses2List(sr, (char *)"[*:*]");
    TSeriesClear(sr);
    return ret;
}

// [[Rcpp::export]]
SEXP write_cat_serieses(StringVector filename, List data) {
    char *fn = filename[0];
    TSerieses *sr = List2Serieses(data);
    if(sr != NULL)
    {
        sr->Save(fn);
        TSeriesClear(sr);
        return StringVector::create(string(fn));
    }
    else
        return StringVector::create(string("Error: Could not be serialized it!!"));
}

// [[Rcpp::export]]
SEXP rcat_load()
{
    return IntegerVector::create(0);
}

// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically
// run after the compilation.
//

/*** R
rcat_load()
*/
