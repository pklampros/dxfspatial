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

\name{cleanPolygons}
\alias{cleanPolygons}
\title{
Polygon cleanup
}
\description{
Cleans up polygons. More specifically removes duplicates and aggregates polygons on one or more specific columns to properly identify holes.
}
\usage{
cleanPolygons(polys, aggregateOn)
}
\arguments{
  \item{polys}{
    A SpatialPolygonsDataFrame which to clean up.
  }
  \item{aggregateOn}{
    One ore more columns to aggregate on. This is used to group polygons and then find which ones are holes of others.
  }
}
\value{
Returns a new SpatialPolygonsDataFrame with the cleaned-up polygons
}
\author{
Petros Koutsolampros
}
