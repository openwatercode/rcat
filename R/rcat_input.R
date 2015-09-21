#' rcat_input class
#'
#' CAT 입력 파일을 읽어오거나 수정할 때 사용되는 클래스 입니다.
#' @docType class
#' @name rcat_input
#' @encoding utf-8
#' @author \href{http://www.i-fam.net/water/}{박희성} \email{hspark90@@i-fam.net}
#' @keywords class
NULL

#' print.rcat_input: print rcat_input contents
#'
#' \code{\link{rcat_input}} 클래스 자료의 간단한 내용을 보여주는 함수
#' @param x \code{\link{rcat_input}} 클래스의 변수
#' @return \code{\link{rcat_input}} 형태의 자료
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
print.rcat_input <- function(x, ...)
{
  cat(sprintf("Title = '%s'\n", attr(x, "Title")))
  cat(sprintf("Version = '%s'\n", attr(x, "Version")))
  cat(sprintf("Start Time = %s\n", do.call("ISOdatetime", as.list(c(attr(x, "StartTime"), 0)))))
  cat(sprintf("End Time = %s\n", do.call("ISOdatetime", as.list(c(attr(x, "EndTime"), 0)))))
  param <- attr(x, "Parameter")
  if(length(param) == 2)
    cat(sprintf("Time Interval = %d\nLoop = %d\n", param[1], param[2]))
  lcount <- sum(sapply(x, class) == "cat_Link")
  ccount <- sum(sapply(x, class) == "cat_Climate")
  ncount <- length(x) - lcount - ccount
  cat(sprintf("Count of Nodes = %d\nCount of Links = %d\nCount of Climates = %d\n",
              ncount, lcount, ccount))
}

#' [.rcat_input: Select from items of rcat_input
#'
#' \code{\link{rcat_input}} 클래스 자료에서 특정노드를 선택해 반환하는 연산자.
#' @param x \code{\link{rcat_input}} 클래스의 변수
#' @param i 인덱스
#' @return \code{\link{rcat_input}} 형태의 자료
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
`[.rcat_input` <- function(x, i, ...)
{
  y <- NextMethod(x)
  attr(y, "Version") <- attr(x, "Version")
  attr(y, "Title") <- attr(x, "Title")
  attr(y, "StartTime") <- attr(x, "StartTime")
  attr(y, "EndTime") <- attr(x, "EndTime")
  attr(y, "Parameter") <- attr(x, "Parameter")
  attr(y, "Range") <- attr(x, "Range")
  attr(y, "NodeCount") <- attr(x, "NodeCount")
  class(y) <- class(x)
  y
}

