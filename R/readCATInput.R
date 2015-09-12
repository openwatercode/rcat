#' readCATInput: read from CAT input file to r varialble which is rcat_input class
#'
#' 텍스트로 구성된 CAT 모형을 읽어 "rcat_input" 클래스의 변수로 반환
#' @param filename CAT 모형의 텍스트형 입력자료
#' @return 입력파일의 내용이 들어 있는 \code{\link{rcat_input}} 클래스의 변수
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @useDynLib rcat
#' @import Rcpp
#' @encoding UTF-8
#' @export
#' @examples
#' download.file("http://r.prj.kr/data/rcat/rcat_ex.zip", "rcat_ex.zip")
#' unzip("rcat_ex.zip")
#'
#' model <- readCATInput("dist_101208.txt")
readCATInput <- function(filename)
{
  if(!file.exists(filename))
    stop(sprintf("Error: '%s' file does not exists!!"))
  readInput(filename)
}
