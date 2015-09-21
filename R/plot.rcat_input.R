#' plot.rcat_input: plot the sechmatic view of CAT model
#'
#' "rcat_input" 클래스 자료를 그래프 형태로 보여주는 함수.
#' @param x rcat 모형에서 사용하는 "rcat_input" 클래스의 변수
#' @return NULL
#' @author 박희성 \email{hspark90@@i-fam.net}
#' @import igraph
#' @encoding UTF-8
#' @export
#' @examples
#' download.file("http://r.prj.kr/data/rcat/rcat_ex.zip", "rcat_ex.zip")
#' unzip("rcat_ex.zip")
#' model <- readCATInput("dist_101208.txt")
#' plot(model)
plot.rcat_input <- function(x)
{
  #Nodes <- x
  links.no <- which(unlist(lapply(x, function(x) class(x)[1])) == "cat_Link")
  clims.no <- which(unlist(lapply(x, function(x) class(x)[1])) == "cat_Climate")
  outlet.no <- which(unlist(lapply(x, function(x) class(x)[1])) == "cat_Outlet")
  nodes <- x[-c(links.no, clims.no)]
  links <- x[links.no]
  nntab <- 1:length(nodes)
  nctab <- c(cat_Junction = "gray",
             cat_Outlet = "red",
             cat_WetLand = "green",
             cat_Pond = "cyan",
             cat_Recycle = "orange",
             cat_RainTank = "cyan",
             cat_Infiltro = "yellow",
             cat_Forest = "green",
             cat_Paddy = "yellow",
             cat_Bioretention = "cyan",
             cat_Import = "green",
             cat_Urban = "gray")
  names(nntab) <- unlist(lapply(nodes, function(x) x$NodeID))
  nodes.name <- unlist(lapply(nodes, function(x) x$Name))
  nodes.class <- unlist(lapply(nodes, function(x) class(x)[1]))
  nodes.size <- ifelse(nodes.class == "cat_Junction", 2, 10)
  nodes.color <- nctab[nodes.class]
  links.edges <- nntab[as.character(unlist(lapply(links, function(x) x$Connect)))]

  nodes.pos <- sapply(nodes, function(x) x$Position)
  library(igraph)
  g <- make_empty_graph() +
    vertices(name = nodes.name,
             size = nodes.size,
             color = nodes.color) +
    edges(links.edges, arrow.size = 0.3)
  if(class(nodes.pos) == "matrix")
    plot(g, layout = t(nodes.pos))
  else
    plot(g, layout = layout.kamada.kawai)
  #plot(g, layout = layout.gem)
  #plot(g, layout = layout.graphopt)
  #plot(g, layout = layout.lgl)
}

