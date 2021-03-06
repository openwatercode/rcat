#' writeBinCATSerieses: write the CAT rcat_serieses class data to a binary file
#'
#' CAT 모형에서 사용하는 변수계열을 Binary 파일로 저장하는 함수 .
#' @param x rcat 모형에서 사용하는 "rcat_serieses" 클래스의 변수
#' @param filename 출력파일의 경로
#' @return 에러 메시지나 저장된 파일이름(보이지 않음)
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @useDynLib rcat
#' @import Rcpp
#' @encoding UTF-8
#' @export
#' @examples
#' download.file("http://r.prj.kr/data/rcat/rcat_ex.zip", "rcat_ex.zip")
#' unzip("rcat_ex.zip")
#'
#' climate_data <- readBinCATSerieses("P_climate_1209.dat")
#' writeBinCATSerieses(climate_data, "cldata.dat")
writeBinCATSerieses <- function(x, filename)
{
  if(missing(filename))
    stop("filename must be needed!")
  if(!(any(class(x) == "rcat_serieses")))
    stop("Error: x is not 'rcat_serieses' object!!")
  if(length(attr(x, "Name")) == 0) attr(x, "Name") <- ""
  if(length(attr(x, "Description")) == 0) attr(x, "Description") <- ""
  if(length(attr(x, "names")) == 0) attr(x, "names") <- rep("", length(x))
  invisible(write_cat_serieses(path.expand(filename[1]), x))
}
