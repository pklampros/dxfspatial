\name{processDXF}
\alias{processDXF}
\title{
Process a DXF file
}
\description{
Reads a DXF file and extracts spatial layers such as polygons, lines, points and texts.
}
\usage{
processDXF(filepath, scale = 1, prefix)
}
%- maybe also 'usage' for other objects documented here.
\arguments{
  \item{filepath}{
    The location of the DXF file
  }
  \item{scale}{
    Scale to resize the spatial layers. If for example the DXF is drawn in millimeters but needs to be imported to meters then the scale here should be set to 0.001.
  }
  \item{prefix}{
    Prefix to identify which layers and which blocks are to be extracted (i.e. the ones that start with this prefix).
  }
}
\value{
Returns a list of four items, lines (a SpatialLinesDataFrame), polys (a SpatialPolygonsDataFrame), points (a SpatialPointsDataFrame) and texts (a SpatialPointsDataFrame with an extra column for the text's content)
}
\author{
Petros Koutsolampros
}
