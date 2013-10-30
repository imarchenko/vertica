# Vertica User-Defined Scalar Function
# Converts a longitude and latitude value into a United States state.
#
# R Dependencies:
#       sp, maps, and maptools

library(sp)
library(maps)
library(maptools)

GeoState <- function(coords) {
        # Computes the USA state based on a longitude and latitude coordinate
        #
        # Args:
        #   coords: A dataframe that contains two items, longitude and latitude coordinates
        #
        # Returns:
        #   A US state if coordinates are within the USA bounds, otherwise return null
        states <- map('state', fill=TRUE, col="transparent", plot=FALSE)
        IDs <- sapply(strsplit(states$names, ":"), function(x) x[1])
        statesSP <- map2SpatialPolygons(states, IDs=IDs, proj4string=CRS("+proj=longlat +datum=wgs84"))

        pointsSP <- SpatialPoints(coords, proj4string=CRS("+proj=longlat +datum=wgs84"))
        indices <- over(pointsSP, statesSP)

        stateNames <- sapply(statesSP@polygons, function(x) x@ID)
        sapply(stateNames[indices], function(x) ifelse(is.na(x), x, (state.abb[grep(sub(":.*", "", x), state.name, ignore.case = TRUE)])))
}

GeoStateFactory <- function() {
        # A Vertica Scalar Function UDx Factory
        # Takes two numeric values (longitude, latitude) and returns a varchar value
        list(name=GeoState,udxtype=c("scalar"),intype=c("float","float"), outtype=c("varchar"))
}
