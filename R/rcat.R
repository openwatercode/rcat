#' rcat-package: use the CAT(Catchment hylological cycle Assessmet Tool) Model on R Environment
#'
#' R 환경에서 CAT모형을 수행하기 위한 함수.
#' @details
#' \tabular{ll}{
#' Package: \tab rcat\cr
#' Type: \tab Package\cr
#' Version: \tab 0.1.6\cr
#' Date: \tab 2017-03-08\cr
#' License: \tab AGPL-3\cr
#' }
#'
#' CAT을 동적라이브러리 형태로 컴파일하여
#' R 환경에서 스크립트 형태로 간편하게 사용할 수 있도록
#' 작성된 패키지입니다.
#' @docType package
#' @name rcat-package
#' @aliases rcat
#' @encoding utf-8
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @references 수자원의지속적확보기술개발사업단(2008), 도시유역 물순환 해석기술 지침서(TR-2008-05)
#' @references 수자원의지속적확보기술개발사업단(2008), 도시유역 물순환계 정량화 방법(TR-2008-15)
#' @references 한국건설기술연구원(2005), 건강한 물순환체계 구축을 위한 유역진단기법 개발 연구보고서
#' @references 한국건설기술연구원(2005), 우수저류 및 활용기술개발 연구보고서
#' @references 한국건설기술연구원(2009), 도시유역 물순환 해석모형 개발 및 적용 연구보고서
#' @keywords package
#' @examples
#' download.file("http://r.prj.kr/data/rcat/rcat_ex.zip", "rcat_ex.zip")
#' unzip("rcat_ex.zip")
#' # save the output to file
#' RunCAT("dist_101208.txt", "dist_101208.out")
#' @import rskrpatch
NULL

getVariableName <- function(v)
{
  s <- substitute(v)
  browser()
  if (length(s) == 1)
    deparse(s)
  else
    sub("\\(.", "", s[2])
}
