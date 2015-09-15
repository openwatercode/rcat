#' rcat_serieses class
#'
#' CAT의 출력 파일을 읽어오거나 시계열 입력자료를 읽거나 저장 할 때 사용되는 클래스 입니다.
#' @docType class
#' @name rcat_serieses
#' @encoding utf-8
#' @author \href{http://www.i-fam.net/water/}{박희성} \email{hspark90@@i-fam.net}
#' @keywords class
NULL

#' [.rcat_serieses: Select from items of rcat_serieses
#'
#' "rcat_series" 클래스 자료에서 특정노드를 선택해 반환하는 함수.
#' @param x rcat_serieses 클래스의 변수
#' @param i 인덱스
#' @return \code{\link{rcat_serieses}} 형태의 자료
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
'[.rcat_serieses' <- function(x, i, ...)
{
  y <- x[i, ...]
  attr(y, "StartTime") <- attr(y, "StartTime")
  attr(y, "Interval") <- attr(y, "Interval")
  attr(y, "Description") <- attr(y, "Description")
  attr(y, "Name") <- attr(y, "Name")
  class(y) <- class(x)
  y
}
