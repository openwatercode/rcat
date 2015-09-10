#' as.xts.rcat_series
#'
#' "rcat_series" 클래스 자료를 xts로 변환하는 함수.
#' @param x rcat 모형에서 사용하는 "rcat_series" 클래스의 변수
#' @return 변환된 \code{\link{xts}} 변수
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @import xts
#' @encoding UTF-8
#' @export
as.xts.rcat_series <- function(x, ...)
{
  sdt <- do.call("ISOdatetime", as.list(c(attr(x, "StartTime"), 0)))
  dts <- seq(sdt, by = paste(attr(x, "Interval"), "min"), length.out = nrow(x))
  xts(as.data.frame(x), dts, ...)
}
