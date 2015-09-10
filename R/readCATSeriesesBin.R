#' readBinCATSerieses
#'
#' CAT 모형에서 사용하는 변수계열을 저장한 Binary 파일을 읽어오는 함수.
#' @param filename 출력파일의 경로
#' @return rcat 모형에서 사용하는 "rcat_serieses" 클래스의 변수
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @useDynLib rcat
#' @import Rcpp
#' @encoding UTF-8
#' @export
#' @examples
#' download.file("http://r.prj.kr/data/rcat/rcat_ex.zip", "rcat_ex.zip")
#' unzip("rcat_ex.zip")
#'
#' climate_data <- readBinCATSerieses("P_climate_1209.dat")
readBinCATSerieses <- function(filename)
{
  if(missing(filename))
    stop("filename must be needed!")
  ret <- read_cat_serieses(path.expand(filename[1]))
  for(i in 1:length(ret))
    if(!is.null(colnames(ret[[i]]))) Encoding(colnames(ret[[i]])) <- "UTF-8"
  #cls <- class(ret)
  #lapply(ret, function(x) {
  #  if(!is.null(colnames(x))) Encoding(colnames(x)) <- "UTF-8"
  #  x})
  #class(ret) <- cls
  return(ret)
}
