#' getYmDHMS: get integer array about time
#'
#' \code{\link{POSIXt}}로 변환 될 수 있는 시간변수나 문자열에서
#' 연, 월, 일, 시, 분, 초와 같은 값을 얻어 정수 배열로 반환합니다.
#' @param x \code{\link{POSIXt}}로 변환 될 수 있는 시간변수나 문자열
#' @return 연, 월, 일, 시, 분, 초와 같은 \code{\link{integer}} 배열
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
#' @examples getYmDHMS(Sys.time())
getYmDHMS <- function(x)
{
  if(!(is.timeBased(x) || is.character(x)))
    stop("Error: x is not contimebased variables nor character!!")
  t <- as.POSIXlt(x)
  r <- as.integer(c(1900 + t$year, 1 + t$mon, t$mday, t$hour, t$min, t$sec))
  lengx <- length(x)
  if(lengx > 1)
    matrix(r, nrow = lengx)
  else  r
}

#' getYmDHM: get integer array about time
#'
#' @describeIn getYmDHMS
#' @encoding UTF-8
#' @export
getYmDHM <- function(x)
{
  if(!(is.timeBased(x) || is.character(x)))
    stop("Error: x is not contimebased variables nor character!!")
  t <- as.POSIXlt(x)
  r <- as.integer(c(1900 + t$year, 1 + t$mon, t$mday, t$hour, t$min))
  lengx <- length(x)
  if(lengx > 1)
    matrix(r, nrow = lengx)
  else  r
}
