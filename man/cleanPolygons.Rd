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
