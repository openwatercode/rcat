#' lines.rcat_seriesItem: plot the rcat_seriesItem
#'
#' \code{\link{rcat_seriesItem}} 클래스 자료를 그래프 형태로 보여주는 함수.
#' @param x rcat 모형에서 사용하는 \code{\link{rcat_seriesItem}} 클래스의 변수
#' @param ... \code{\link{lines}} 함수의 매개변수를 참조
#' @return NULL
#' @seealso \code{\link{plot.rcat_seriesItem}}
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @import xts
#' @importFrom graphics lines
#' @encoding UTF-8
#' @export
#' @examples
#' download.file("http://r.prj.kr/data/rcat/rcat_ex.zip", "rcat_ex.zip")
#' unzip("rcat_ex.zip")
#' iseri <- readBinCATSerieses("P_climate_1209.dat")
#' plot(iseri[[1]]$t)
#' lines(iseri[[1]]$t, col = "blue")
lines.rcat_seriesItem <- function(x, ...)
{
  st <- do.call("ISOdatetime", as.list(c(attr(x, "StartTime"), 0)))
  tseq <- seq(st, by = paste(attr(x, "Interval"), "min"),
              length.out = length(x))
  lines(x = tseq, y = x, ...)
}
