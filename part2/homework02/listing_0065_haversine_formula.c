/* ========================================================================

   (C) Copyright 2023 by Molly Rocket, Inc., All Rights Reserved.
   
   This software is provided 'as-is', without any express or implied
   warranty. In no event will the authors be held liable for any damages
   arising from the use of this software.
   
   Please see https://computerenhance.com for more information
   
   ======================================================================== */

/* ========================================================================
   LISTING 65
   ======================================================================== */

static r64 Square(r64 A)
{
    r64 Result = (A*A);
    return Result;
}

static r64 RadiansFromDegrees(r64 Degrees)
{
    r64 Result = 0.01745329251994329577 * Degrees;
    return Result;
}

// NOTE(casey): EarthRadius is generally expected to be 6372.8
static r64 ReferenceHaversine(r64 X0, r64 Y0, r64 X1, r64 Y1, r64 EarthRadius)
{
    /* NOTE(casey): This is not meant to be a "good" way to calculate the Haversine distance.
       Instead, it attempts to follow, as closely as possible, the formula used in the real-world
       question on which these homework exercises are loosely based.
    */
    
    r64 lat1 = Y0;
    r64 lat2 = Y1;
    r64 lon1 = X0;
    r64 lon2 = X1;
    
    r64 dLat = RadiansFromDegrees(lat2 - lat1);
    r64 dLon = RadiansFromDegrees(lon2 - lon1);
    lat1 = RadiansFromDegrees(lat1);
    lat2 = RadiansFromDegrees(lat2);
    
    r64 a = Square(sin(dLat/2.0)) + cos(lat1)*cos(lat2)*Square(sin(dLon/2));
    r64 c = 2.0*asin(sqrt(a));
    
    r64 Result = EarthRadius * c;
    
    return Result;
}
