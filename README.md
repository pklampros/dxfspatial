An R package to extract spatial information (lines, polylines, points) from structured dxf files

### Installation

Make sure the devtools package is installed. For windows you may also need Rtools. Then in R:

```{r}
library(devtools)
install_github("orange-vertex/dxfspatial")
library(dxfspatial)
```

### Use
Take a look at the structure of the example dxf file under inst/extdata in this repository.  
In a nutshell, put the various elements of the plan (lines, points, polylines, texts) under layers named accordingly and starting with a prefix.  
The prefix in the example dxf is "@_".  
The possible types for the layers are poly, point, text and line.  
The package will understand a layer called `@_poly-gallery` as a layer with polygons and will create a SpatialPolygonsDataFrame including relevant information. Make sure the naming follows the the example exactly i.e. `@_line-gallery` is correct, not `@_Line-gallery` (capital L), not `@_line_gallery` (underscore instead of dash after line) and not `@_lines_gallery` (lines instead of line). The valid prefixes are `@_poly-`, `@_line-`, `@_point-`, `@_text-`.
Points and texts are both created as SpatialPointsDataFrame, the texts with an additional column.  
To separate the different floors, put each floor in a different block, named in a similar fashion and with the same prefix i.e. "@_floor1".  
Even if only one floor is in the floor plan it needs to be placed in a block and it given a name.  
The command to read a properly structured dxf is thus:  

```{r}
dxf = processDXF(<filename>, <scale>, <prefix>)
```
For example `dxf = processDXF("gallery_func.dxf", 0.001, "@_")` will load the dxf file called "gallery_func.dxf", will rescale all coordinates by multiplying them by 0.001 (i.e. if the plan is in millimeters and we need meters in R) and which is using the prefix "@_" for the layers and blocks.

### Note on polygons
The library loads polygons and tries to 'clean' them. This means that it will remove duplicates and if one is inside another then the former will be designated a hole of the latter (if they belong to the same layer and block).
If this is not desired then the command might also be called adding "**cleanPolygons = FALSE**" that will not clean the polygons:

```{r}
dxf = processDXF(<filename>, <scale>, <prefix>, cleanPolygons = FALSE)
```
with the cleaning made optionally later using the command `cleanPolygons`:
```{r}
# To clean up polygons if they belong in the same block AND layer
dxf$polys = cleanPolygons(dxf$polys, c("block", "layer"))

$ To clean up polygons if they belong in the same layer
dxf$polys = cleanPolygons(dxf$polys, "layer")

$ To clean up polygons if they belong in the same block
dxf$polys = cleanPolygons(dxf$polys, "block")

$ To clean up polygons if they have the same colour
dxf$polys = cleanPolygons(dxf$polys, "colour")
```
