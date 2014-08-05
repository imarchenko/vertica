# Vertica User-Defined Scalar Function
# Provides two functions around business dates
# is_business_date to check if a date is a valid business day
# count_business_dates to count the number of business days between a range of dates
#
# R Dependencies:
#       timeDate

is_business_date <- function(v_dataframe) {
    # Determines if a given date is a United States business date (based on New York Stock Exchange holidays)
    #
    # Args:
    #   v_dataframe: A dataframe that contains a date
    #
    # Returns:
    #   A boolean indicating if the date is a business date
    p_day <- v_dataframe[,1]

    if (length(p_day) == 0) {
        return(FALSE)
    } else {
        library('timeDate')
        day <- timeDate(p_day)
    }

    return(isBizday(day, holidays = holidayNYSE(), wday = 1:5))
}

is_business_date_factory <- function() {
    # A Vertica Scalar Function UDx Factory
    # Takes a date as input and outputs a boolean
    list(
        name=is_business_date,
        udxtype=c('scalar'),
        intype=c('date'),
        outtype=c('boolean')
    )
}

count_business_dates <- function(v_dataframe) {
    # Counts the number of business dates between two dates (based on New York Stock Exchange holidays)
    #
    # Args:
    #   v_dataframe: A dataframe that contains a start and end date
    #
    # Returns:
    #   The number of business dates
    p_start_day <- v_dataframe[,1]
    p_end_day <- v_dataframe[,2]

    if (length(p_start_day) == 0 || length(p_end_day) == 0) {
        return(-1)
    } else {
        library('timeDate')
        start_day <- timeDate(p_start_day)
        end_day <- timeDate(p_end_day)
    }

    tS = timeSequence(from = start_day, to = end_day)
    return(as.integer(length(tS[isBizday(tS, holidayNYSE())])))
}

count_business_dates_factory <- function() {
    # A Vertica Scalar Function UDx Factory
    # Takes two dates as input and outputs a numeric
    list(
        name=count_business_dates,
        udxtype=c('scalar'),
        intype=c('date', 'date'),
        outtype=c('int')
    )
}
