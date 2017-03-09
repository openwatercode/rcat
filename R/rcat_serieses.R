#' rcat_serieses class
#'
#' CAT의 출력 파일을 읽어오거나 시계열 입력자료를 읽거나 저장 할 때 사용되는 클래스
#' 또는 클래스 생성자
#' @docType class
#' @name rcat_serieses
#' @param x Contents of \code{\link{rcat_serieses}}
#' @param ... etc. parameters
#' @encoding utf-8
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @keywords class
#' @export
rcat_serieses <- function(x, ...) UseMethod("rcat_serieses")

#' rcat_serieses.rcat_series: make rcat_serieses from rcat_series
#'
#' \code{\link{rcat_series}} 로 부터 \code{\link{rcat_serieses}}를 생성하는 함수
#' @param x \code{\link{rcat_series}} 클래스의 변수
#' @param description Description of the \code{\link{rcat_serieses}} created.
#' @param name Name of the \code{\link{rcat_serieses}} created.
#' @param ... \code{\link{rcat_serieses}} 함수 참조
#' @return \code{\link{rcat_serieses}} 형태의 자료
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
rcat_serieses.rcat_series <- function(x, description, name, ...)
{
  s <- list()
  attr(s, "StartTime") <- attr(x, "StartTime")
  attr(s, "Interval") <- attr(x, "Interval")
  attr(s, "Description") <- if(missing(description)) "" else description
  attr(s, "Name") <- if(missing(name)) "" else name
  attr(s, "class") <- "rcat_serieses"
  s[[1]] <- x
  return(s)
}


#' is.rcat_serieses
#'
#' \code{\link{rcat_serieses}} 클래스인지 아닌지 알려주는 함수
#' @param x \code{\link{rcat_serieses}} 클래스인지 확인 하고자하는 임의의 변수
#' @return \code{\link{rcat_serieses}} 클래스 자료여부를 알려주는 \code{\link{logical}} 변수
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
is.rcat_serieses <- function(x) inherits(x, "rcat_serieses")


#' print.rcat_serieses: print rcat_serieses contents
#'
#' \code{\link{rcat_series}} 클래스 자료의 간단한 내용을 보여주는 함수
#' @param x \code{\link{rcat_serieses}} 클래스의 변수
#' @param ... \code{\link{print}} 함수 참조
#' @return \code{\link{rcat_serieses}} 형태의 자료
#' @author Heeseong Park \email{hspark90@@i-fam.net}
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
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
`[.rcat_serieses` <- function(x, i)
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
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @importFrom stringr str_trim
#' @export
`+.rcat_serieses` <- function(e1, e2)
{
  if(!is.rcat_serieses(e2))
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
  structure(r,
            StartTime = attr(e1, "StartTime"),
            Interval = attr(e1, "Interval"),
            Name = paste(n1, n2, sep = "."),
            Description = paste(attr(e1, "Description"),
                                attr(e2, "Description"), sep = ","),
            names = c(paste(n1, names(e1), sep = "."),
                      paste(n2, names(e2), sep = ".")),
            class = class(e1))
}


#' c.rcat_serieses: combine rcat_serieses
#'
#' \code{\link{rcat_serieses}} 클래스 자료를 더해 하나로 만드는 함수
#' @param ... 더하고자 하는 \code{\link{rcat_serieses}} 클래스 자료를 나열
#' @return \code{\link{rcat_serieses}} 형태의 자료
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
`c.rcat_serieses` <- function(...)
{
  structure(do.call("c", lapply(list(...), function(x) structure(x, class = "list"))),
            StartTime = attr(..1, "StartTime"),
            Interval = attr(..1, "Interval"),
            Name = attr(..1, "Name"),
            Description = attr(..1, "Description"),
            names = unlist(lapply(list(...), names)),
            class = class(..1))
}

