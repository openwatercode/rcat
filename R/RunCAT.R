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
RunCAT <- function(infile, outfile, format = "[*:*]")
{
  if(missing(infile) || missing(outfile))
    stop("File Missing!")
  if(!file.exists(infile))
    stop(sprintf("%s does not exists!", infile))
  if(infile == outfile)
    stop("Output file name should not be the same to Input file name!")
  invisible(rcpp_run_cat(path.expand(infile), path.expand(outfile), format))
}
