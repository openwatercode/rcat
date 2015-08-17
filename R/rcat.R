#' rcat-package: Run CAT Model on R Environment
#'
#' R 환경에서 CAT모형을 수행하기 위한 함수.
#' @details
#' \tabular{ll}{
#' Package: \tab rcat\cr
#' Type: \tab Package\cr
#' Version: \tab 0.1.0\cr
#' Date: \tab 2015-08-12\cr
#' License: \tab GPL-2\cr
#' }
#'
#' CAT을 동적라이브러리 형태로 컴파일하여
#' R 환경에서 스크립트 형태로 간편하게 사용할 수 있도록
#' 작성된 패키지입니다.
#' @docType package
#' @name rcat-package
#' @aliases rcat
#' @encoding utf-8
#' @author \href{http://www.i-fam.net/water/}{박희성} \email{hspark90@@i-fam.net}
#' @keywords package
#' @examples
#' download.file("http://r.prj.kr/data/rcat/rcat_ex.zip", "rcat_ex.zip")
#' unzip("rcat_ex.zip")
#' RunCAT("dist_101208.txt", "dist_101208.out")
NULL
