# Copyright 2018-2019 Petros Koutsolampros
# This file is part of dxfspatial.
#
# dxfspatial is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# dxfspatial is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with dxfspatial.  If not, see <https://www.gnu.org/licenses/>.

library(Rcpp)
library(sp);
library(raster);
library(maptools);

processDXF = function(filepath, scale = 1, prefix) {
  if (!file.exists(filepath)) stop("File not found");
  data = getDXFData(filepath, prefix, scale);
  dxfData = list();
  dxfData$lines = data$lines;
  dxfData$polys = data$polys;
  dxfData$texts = data$texts;
  dxfData$texts@data$text = as.character(dxfData$texts@data$text);
  dxfData$points = data$points;
  data$blocks$name = as.character(data$blocks$name);
  dxfData$blocks = data$blocks;
  data$layers$name = as.character(data$layers$name);
  dxfData$layers = data$layers;
  return(dxfData);
}


