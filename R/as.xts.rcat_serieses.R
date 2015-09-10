#' as.xts.rcat_serieses
#'
#' "rcat_serieses" 클래스 자료에 포함된 모든 "rcat_serires" 변수를 xts로 변환하는 함수.
#' @param x rcat 모형에서 사용하는 "rcat_serieses" 클래스의 변수
#' @return 변환된 \code{\link{xts}}를 포함하는 \code{\link{list}} 변수
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @import xts
#' @encoding UTF-8
#' @export
as.xts.rcat_serieses <- function(x, ...)
{
  lapply(x, function(y) as.xts(y, ...))
}
