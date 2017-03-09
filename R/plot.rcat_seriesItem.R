#' plot.rcat_seriesItem: plot the rcat_seriesItem
#'
#' \code{\link{rcat_seriesItem}} 클래스 자료를 그래프 형태로 보여주는 함수.
#' @param x rcat 모형에서 사용하는 \code{\link{rcat_seriesItem}} 클래스의 변수
#' @param type \code{\link{plot}} 참조
#' @param xlab \code{\link{plot}} 참조
#' @param ylab \code{\link{plot}} 참조
#' @param main \code{\link{plot}} 참조
#' @param ... \code{\link{plot}} 함수의 매개변수 참조
#' @return NULL
#' @seealso \code{\link{lines.rcat_seriesItem}}
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @import xts
#' @encoding UTF-8
#' @export
#' @examples
#' download.file("http://r.prj.kr/data/rcat/rcat_ex.zip", "rcat_ex.zip")
#' unzip("rcat_ex.zip")
#' iseri <- readBinCATSerieses("P_climate_1209.dat")
#' plot(iseri[[1]]$t)
plot.rcat_seriesItem <- function(x, type = "l", xlab = "Time",
                                 ylab = deparse(substitute(x)),
                                 main = deparse(substitute(x)), ...)
{
  st <- do.call("ISOdatetime", as.list(c(attr(x, "StartTime"), 0)))
  tseq <- seq(st, by = paste(attr(x, "Interval"), "min"),
              length.out = length(x))
  plot(x = tseq, y = x, type = type, xlab = xlab, ylab = ylab,
       main = main, ...)
}
