#' saveCATInput: save rcat_input object
#'
#' \code{\link{rcat_input}} 클래스 자료를 저장하는 함수
#' @param x \code{\link{rcat_input}} 클래스의 변수
#' @param file 저장할 파일의 이름(경로), \code{\link{character}}
#' @return invisible NULL
#' @author Heeseong Park \email{hspark90@@i-fam.net}
#' @encoding UTF-8
#' @export
saveCATInput <- function(x, file)
{
  f <- file(file, "w")
  hs_vcat("Version", attr(x, "Version"), file = f)
  cat("\n", file = f)
  hs_vcat("Title", attr(x, "Title"), file = f)
  starr <- attr(x, "StartTime")
  hs_vcat("StartTime", c(formatC(starr[1], width = 4, flag = "0"),
                         formatC(starr[-1], width = 2, flag = "0")),
          file = f)
  etarr <- attr(x, "EndTime")
  hs_vcat("EndTime", c(formatC(etarr[1], width = 4, flag = "0"),
                       formatC(etarr[-1], width = 2, flag = "0")),
          file = f)
  hs_vcat("Parameter", attr(x, "Parameter"), file = f)
  rng <- attr(x, "Range")
  len <- length(x)
  if(!is.null(rng) && length(rng) == 4)
    hs_vcat("Range", rng, file = f)
  hs_vcat("NodeCount", len, file = f)
  cat("\n", file = f)
  for(i in 1:len)
  {
    save_node(x[[i]], file = f)
    cat("\n", file = f)
  }
  close(f)
}

save_node <- function(x, file)
{
  c <- class(x)
  hs_vcat("Node", gsub("cat_", "", c), file = file)
  #hs_vcat("NodeID", x$NodeID, file = file)
  #hs_vcat("Name", x$Name, file = file)
  #hs_vcat("Desc", x$Desc, file = file)
  for(k in names(x))
  {
    type <- getInputNodeType(c, k)
    switch(type,
      character = hs_vcat(k, x = x[[k]], file = file),
      integer = hs_vcat(k, x = x[[k]], file = file),
      numeric = hs_vcat(k, x = round(x[[k]], 9), file = file),
      data.frame =
        hs_vcat(k, x = c(as.character(nrow(x[[k]])),
          switch(k,
            Nodes = apply(x[[k]], 1,
                          function(x)
                            paste0(x[1], ":", round(x[2], 9))),
            Weather = apply(x[[k]], 1,
                            function(x)
                              paste0(paste0(x[1], ":", round(x[2], 9)),
                                     ";", round(x[3], 9))),
            NULL)),
          file = file
        ),
      NULL=
    )
  }
  cat("EndNode\n", file = file)
}

hs_vcat <- function(key, x = NULL, ...)
{
  s <- switch(class(x),
    character = do.call("paste", append(as.list(x), list(sep = ", "))),
    integer = do.call("paste", append(as.list(x), list(sep = ", "))),
    numeric = do.call("paste", append(as.list(x), list(sep = ", "))),
    NULL
  )
  cat(key, ...)
  if(!is.null(x)) cat(" =", ...)
  if(!is.null(s) && nchar(s) != 0) { cat(" ", ...); cat(s, ...) }
  cat("\n", ...)
#  if(!is.null(x))
#    cat(do.call("sprintf", append(paste0(key, " = ", format, "\n"),
#                                  as.list(x))), ...)
}

