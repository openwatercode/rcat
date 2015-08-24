#' RunCAT
#'
#' RunCAT 모형 구동 함수.
#' @param infile 입력파일의 경로
#' @param outfile 출력파일의 경로, 생략하면 리스트로 결과가 반환됨
#' @param format 포맷, "" 일 경우 바이너리 형태로 저장되며,
#' "[노드명:필드번호1,필드번호2...]" 노드명의 지정필드를 텍스트파일로 출력
#' "[*|*]" 일 경우 모든 노드의 결과가 텍스트 형태로 저장됨
#'
#' 출력파일이 생략된 경우 선택적으로 결과를 반환받을 수 있음
#' @return NULL
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @useDynLib rcat
#' @import Rcpp
#' @encoding UTF-8
#' @export
#' @examples
#' download.file("http://r.prj.kr/data/rcat/rcat_ex.zip", "rcat_ex.zip")
#' unzip("rcat_ex.zip")
#' # save the output to file
#' RunCAT("dist_101208.txt", "dist_101208.out")
#' # get the output to list of data.frame
#' result <- RunCAT("dist_101208.txt")
#' names(result)
#' # create xts and plotting
#' outlet1 <- result[["Outlet 1"]]
#' library(xts)
#' outlet1.flowtotal <- xts(outlet1[,1],as.POSIXct(rownames(outlet1)))
#' plot(outlet1.flowtotal, type="l")
RunCAT <- function(infile, outfile, format = "[*:*]")
{
  if (missing(infile))
    stop("File Missing!")
  if (!file.exists(infile))
    stop(sprintf("%s does not exists!", infile))
  if (missing(outfile)) {
    ret <-
      run_cat(list(
        infile = path.expand(infile), format = format
      ))
    lapply(ret, function(x) {
      Encoding(colnames(x)) <- "UTF-8"
      x
    })
  } else {
    if (infile == outfile)
      stop("Output file name should not be the same to Input file name!")
    invisible(rcpp_run_cat(path.expand(infile), path.expand(outfile), format))
  }
}
