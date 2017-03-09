#' checkCATInput
#'
#' \code{\link{rcat_input}} 클래스와 연결된 파일 등의 무결성을 확인하는 함수
#' @param x 체크하려는 \code{\link{rcat_input}} 클래스의 변수나
#' \code{\link{readCATInput}} 함수 또는 \code{\link{readCATInput2}} 함수를
#' 이용해 읽을 수 있는 파일의 경로
#' @return 체크 결과에 대한 \code{\link{character}} 변수, \code{\link{invisible}}
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
checkCATInput <- function(x)
{
  if(is.character(x))
  {
    if(!file.exists(x))
      stop("Error: File not found")
    m <- readCATInput(x)
  } else m <- x
  if(!is.rcat_input(m))
    stop("Error: parameter is not proper!!")
  msg <- checkModel(m)
  wmsg <- unlist(strsplit(msg, "\r\n"))
  if(length(wmsg) == 0)
    cat("OK!")
  else
  {
    for(i in 1:length(wmsg))
      cat(paste0(wmsg[i], "\n"))
    invisible(wmsg)
  }
}
