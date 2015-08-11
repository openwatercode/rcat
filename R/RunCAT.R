#' RunCAT
#'
#' RunCAT 모형 구동 함수.
#' @param infile 입력파일의 경로
#' @param outfile 출력파일의 경로
#' @param format 포맷
#' @return NULL
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @useDynLib rcat
#' @import Rcpp
#' @encoding UTF-8
#' @export
RunCAT <- function(infile = "", outfile = "", format = "")
{
  rcpp_run_cat(infile, outfile, format)
}
