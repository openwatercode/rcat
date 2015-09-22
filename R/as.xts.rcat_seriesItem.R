#' as.xts.rcat_seriesItem: rcat_sereisItem class data convert to xts calss
#'
#' \code{\link{rcat_seriesItem}} 클래스 자료를 xts로 변환하는 함수.
#' @param x rcat 모형에서 사용하는 \code{\link{rcat_seriesItem}} 클래스의 변수
#' @param ... \code{\link{as.xts}} 함수의 매개변수 참조
#' @return 변환된 \code{\link{xts}} 변수
#' @seealso \code{\link{as.xts}}
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @import xts
#' @encoding UTF-8
#' @export
#' @examples
#' download.file("http://r.prj.kr/data/rcat/rcat_ex.zip", "rcat_ex.zip")
#' unzip("rcat_ex.zip")
#'
#' climate_data <- readBinCATSerieses("P_climate_1209.dat")
#' clxts <- as.xts(climate_data[[1]]$h)
#' plot(clxts[,1])
as.xts.rcat_seriesItem <- function(x, ...)
{
  sdt <- do.call("ISOdatetime", as.list(c(attr(x, "StartTime"), 0)))
  dts <- seq(sdt, by = paste(attr(x, "Interval"), "min"), length.out = length(x))
  xts(x, dts, ...)
}
