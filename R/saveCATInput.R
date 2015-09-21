#' saveCATInput: save rcat_input object
#'
#' \code{\link{rcat_input}} 클래스 자료를 저장하는 함수
#' @param x \code{\link{rcat_input}} 클래스의 변수
#' @param file 저장할 파일의 이름(경로), \code{\link{character}}
#' @return invisible NULL
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
saveCATInput <- function(x, file)
{
  f <- file(file, "w")
  hs_vcat("Version", "%s\n", attr(x, "Version"), file = f)
  hs_vcat("Title", "%s", attr(x, "Title"), file = f)
  hs_vcat("StartTime", "%04d, %02d, %02d, %02d, %02d", attr(x, "StartTime"), file = f)
  hs_vcat("EndTIme", "%04d, %02d, %02d, %02d, %02d", attr(x, "EndTime"), file = f)
  hs_vcat("Parameter", "%d, %d\n", attr(x, "Parameter"), file = f)
  for(i in 1:length(x))
  {
    save_node(x[[i]], file = f)
    cat("\n", file = f)
  }
  close(f)
}

save_node <- function(x, file)
{
  hs_vcat("Node", "%s", gsub("cat_", "", class(x)), file = file)
  hs_vcat("NodeID", "%d", x$NodeID, file = file)
  hs_vcat("Name", "%s", x$Name, file = file)
  hs_vcat("Drain", "%d, %d", x$Drain, file = file)
  hs_vcat("Rainfall", "%s", x$Rainfall, file = file)
  hs_vcat("Evaporation", "%s", x$Evaporation, file = file)
  hs_vcat("Calculation", "%f, %f, %f, %f", x$Calculation, file = file)



  hs_vcat("Position", "%d, %d", x$Position, file = file)
  cat("EndNode\n", file = file)
}

hs_vcat <- function(key, format, vec, ...)
{
  if(!is.null(vec))
    cat(do.call("sprintf", append(paste0(key, " = ", format, "\n"),
                                  as.list(vec))), ...)
}

