% Copyright 2018-2019 Petros Koutsolampros
% This file is part of dxfspatial.
%
% dxfspatial is free software: you can redistribute it and/or modify
% it under the terms of the GNU General Public License as published by
% the Free Software Foundation, either version 3 of the License, or
% (at your option) any later version.
%
% dxfspatial is distributed in the hope that it will be useful,
% but WITHOUT ANY WARRANTY; without even the implied warranty of
% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
% GNU General Public License for more details.
%
% You should have received a copy of the GNU General Public License
% along with dxfspatial.  If not, see <https://www.gnu.org/licenses/>.

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
