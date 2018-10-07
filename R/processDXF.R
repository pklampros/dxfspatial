library(Rcpp)
library(sp);
setClass("dxfData", slots = c(
    lines="SpatialLinesDataFrame",
    polys="SpatialPolygonsDataFrame",
    texts="SpatialPointsDataFrame",
    points="SpatialPointsDataFrame",
    blocks="data.frame",
    layers="data.frame"
));
processDXF = function(filepath, scale = 1, prefix) {
  if(!file.exists(filepath)) stop("File not found");
  data = getDXFData(filepath, prefix, scale);
  dxfData = new("dxfData")
  dxfData@lines = data$lines;
  dxfData@polys = data$polys;
  dxfData@texts = data$texts;
  dxfData@texts@data$text = as.character(dxfData@texts@data$text);
  dxfData@points = data$points;
  data$blocks$name = as.character(data$blocks$name);
  dxfData@blocks = data$blocks;
  data$layers$name = as.character(data$layers$name);
  dxfData@layers = data$layers;
  return(dxfData);
}
