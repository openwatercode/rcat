#' rcat_serieses class
#'
#' CAT의 출력 파일을 읽어오거나 시계열 입력자료를 읽거나 저장 할 때 사용되는 클래스 입니다.
#' @docType class
#' @name rcat_serieses
#' @encoding utf-8
#' @author \href{http://www.i-fam.net/water/}{박희성} \email{hspark90@@i-fam.net}
#' @keywords class
NULL


#' print.rcat_serieses: print rcat_serieses contents
#'
#' \code{\link{rcat_series}} 클래스 자료의 간단한 내용을 보여주는 함수
#' @param x \code{\link{rcat_serieses}} 클래스의 변수
#' @return \code{\link{rcat_serieses}} 형태의 자료
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
print.rcat_serieses <- function(x, ...)
{
  cat(sprintf("Name = '%s'\n", attr(x, "Name")))
  cat(sprintf("Description = '%s'\n", attr(x, "Description")))
  cat(sprintf("StartTime = %s\n", do.call("ISOdatetime", as.list(c(attr(x, "StartTime"), 0)))))
  cat(sprintf("Interval = %d minutes\n", attr(x, "Interval")))
  cat(sprintf("Count of Series = %d\n", length(x)))
}


#' [.rcat_serieses: Select from items of rcat_serieses
#'
#' \code{\link{rcat_series}} 클래스 자료에서 특정노드를 선택해 반환하는 연산자.
#' @param x \code{\link{rcat_serieses}} 클래스의 변수
#' @param i 인덱스
#' @return \code{\link{rcat_serieses}} 형태의 자료
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
`[.rcat_serieses` <- function(x, i, ...)
{
  y <- NextMethod(x)
  attr(y, "StartTime") <- attr(x, "StartTime")
  attr(y, "Interval") <- attr(x, "Interval")
  attr(y, "Description") <- attr(x, "Description")
  attr(y, "Name") <- attr(x, "Name")
  class(y) <- class(x)
  y
}

#' +.rcat_serieses: connect two rcat_serieses
#'
#' \code{\link{rcat_series}} 클래스 자료를 더해 하나로 만들어 주는 연산자.
#' @param e1 \code{\link{rcat_serieses}} 클래스의 변수
#' @param e2 \code{\link{rcat_serieses}} 클래스의 변수
#' @return \code{\link{rcat_serieses}} 형태의 자료
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
`+.rcat_serieses` <- function(e1, e2)
{
  if(!inherits(e2, "rcat_serieses"))
    stop(sprintf("'%s' is not rcat_serieses class!!", deparse(substitute(e2))))
  if (any(attr(e1, "StartTime") != attr(e2, "StartTime")))
    stop("StartTime must be the same!!")
  if (attr(e1, "Interval") != attr(e2, "Interval"))
    stop("Interval must be the same!!")
  n1 <- str_trim(attr(e1, "Name"))
  if (n1 == "")
    n1 <- deparse(substitute(e1))
  n2 <- str_trim(attr(e2, "Name"))
  if (n2 == "")
    n2 <- deparse(substitute(e2))
  if (n1 == n2)
  {
    n1 <- paste0(n1, "1")
    n2 <- paste0(n2, "2")
  }
  r <- c(e1, e2)
  attr(r, "StartTime") <- attr(e1, "StartTime")
  attr(r, "Interval") <- attr(e1, "Interval")
  attr(r, "Name") <- paste(n1, n2, sep = ".")
  attr(r, "Description") <-
    paste(attr(e1, "Description"), attr(e2, "Description"), sep = ",")
  names(r) <-
    c(paste(n1, names(e1), sep = "."), paste(n2, names(e2), sep = "."))
  class(r) <- class(e1)
  r
}
