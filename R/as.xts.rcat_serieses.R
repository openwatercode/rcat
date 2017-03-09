#' as.xts.rcat_serieses: rcat_sereises class data convert to xts
#'
#' "rcat_serieses" 클래스 자료에 포함된 모든 "rcat_serires" 변수를 xts로 변환하는 함수.
#' @param x rcat 모형에서 사용하는 "rcat_serieses" 클래스의 변수
#' @param ... \code{\link{xts}}에 사용되는 기타 매개변수
#' @return 변환된 \code{\link{xts}}를 포함하는 \code{\link{list}} 변수
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @import xts
#' @encoding UTF-8
#' @export
#' @examples
#' download.file("http://r.prj.kr/data/rcat/rcat_ex.zip", "rcat_ex.zip")
#' unzip("rcat_ex.zip")
#'
#' climate_data <- readBinCATSerieses("P_climate_1209.dat")
#' clxts <- as.xts(climate_data)
#' plot(clxts[[1]][,1])
as.xts.rcat_serieses <- function(x, ...)
{
  lapply(x, function(y) as.xts(y, ...))
}
