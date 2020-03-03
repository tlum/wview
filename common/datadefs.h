#ifndef INC_datadefsh
#define INC_datadefsh
/*---------------------------------------------------------------------------

  FILENAME:
        datadefs.h

  PURPOSE:
        Provide the wview data definitions.

  REVISION HISTORY:
        Date            Engineer        Revision        Remarks
        08/24/03        M.S. Teel       0               Original
        03/23/2008      W. Krenn        1               add WXT510 specials
        07/10/2008      T. Lum          2               Null properties for optional values
        07/07/2009      M.S. Teel       3               Remove pragma packed
                                                        from internal structs
                                                        

  NOTES:


  LICENSE:
        Copyright (c) 2004, Mark S. Teel (mark@teel.ws)

        This source code is released for free distribution under the terms
        of the GNU General Public License.

----------------------------------------------------------------------------*/

/*  ... System include files
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <math.h>
#include <sys/stat.h>

/*  ... Library include files
*/
#ifndef WVIEW_DATA_ONLY
#include <sysdefs.h>
#else
#include <radsysdefs.h>
#endif

/*  ... Local include files
*/

/*  ... data definitions
*/

// Define the wind direction consensus data:
#define WAVG_BIN_SIZE                   18
#define WAVG_INTERVAL                   (2*WAVG_BIN_SIZE)
#define WAVG_NUM_BINS                   (720/WAVG_INTERVAL)
#define WAVG_CONSENSUS_BINS             6
#define WAVG_TOTAL_BINS                 (WAVG_NUM_BINS + WAVG_CONSENSUS_BINS)
typedef struct
{
    short       bins[WAVG_TOTAL_BINS];
}
WAVG, *WAVG_ID;



// Define the data value indices:
typedef enum
{
    DATA_INDEX_barometer            = 0,
    DATA_INDEX_pressure,
    DATA_INDEX_altimeter,
    DATA_INDEX_inTemp,
    DATA_INDEX_outTemp,
    DATA_INDEX_inHumidity,
    DATA_INDEX_outHumidity,
    DATA_INDEX_windSpeed,
    DATA_INDEX_windDir,
    DATA_INDEX_windGust,
    DATA_INDEX_windGustDir,
    DATA_INDEX_rainRate,
    DATA_INDEX_rain,
    DATA_INDEX_dewpoint,
    DATA_INDEX_windchill,
    DATA_INDEX_heatindex,
    DATA_INDEX_rxCheckPercent,
    DATA_INDEX_BASIC_MAX,
    DATA_INDEX_ET                   = DATA_INDEX_BASIC_MAX,
    DATA_INDEX_radiation,
    DATA_INDEX_UV,
    DATA_INDEX_extraTemp1,
    DATA_INDEX_extraTemp2,
    DATA_INDEX_extraTemp3,
    DATA_INDEX_soilTemp1,
    DATA_INDEX_soilTemp2,
    DATA_INDEX_soilTemp3,
    DATA_INDEX_soilTemp4,
    DATA_INDEX_leafTemp1,
    DATA_INDEX_leafTemp2,
    DATA_INDEX_extraHumid1,
    DATA_INDEX_extraHumid2,
    DATA_INDEX_soilMoist1,
    DATA_INDEX_soilMoist2,
    DATA_INDEX_soilMoist3,
    DATA_INDEX_soilMoist4,
    DATA_INDEX_leafWet1,
    DATA_INDEX_leafWet2,
    DATA_INDEX_txBatteryStatus,
    DATA_INDEX_consBatteryVoltage,
    DATA_INDEX_hail,
    DATA_INDEX_hailrate,
    DATA_INDEX_heatingTemp,
    DATA_INDEX_heatingVoltage,
    DATA_INDEX_supplyVoltage,
    DATA_INDEX_referenceVoltage,
    DATA_INDEX_windBatteryStatus,
    DATA_INDEX_rainBatteryStatus,
    DATA_INDEX_outTempBatteryStatus,
    DATA_INDEX_inTempBatteryStatus,

    DATA_INDEX_MAX
}
Data_Indices;


#define DATA_INDEX_MAX(x)    ((x) ? DATA_INDEX_MAX : DATA_INDEX_BASIC_MAX)

// Define sensor types:
typedef enum
{
    SENSOR_INTEMP       = 0,
    SENSOR_OUTTEMP,
    SENSOR_INHUMID,
    SENSOR_OUTHUMID,
    SENSOR_BP,
    SENSOR_WSPEED,
    SENSOR_WGUST,
    SENSOR_DEWPOINT,
    SENSOR_RAIN,                    // cumulative - no avg
    SENSOR_RAINRATE,
    SENSOR_WCHILL,
    SENSOR_HINDEX,
    SENSOR_ET,                      // cumulative - no avg
    SENSOR_UV,
    SENSOR_SOLRAD,
    SENSOR_HAIL,
    SENSOR_HAILRATE,
    SENSOR_MAX
}
SENSOR_TYPES;

// Define sensor time frames:
typedef enum
{
    STF_INTERVAL        = 0,
    STF_HOUR,
    STF_DAY,
    STF_WEEK,
    STF_MONTH,
    STF_YEAR,
    STF_ALL,
    STF_MAX
}
SENSOR_TIMEFRAMES;


// the sensor data type:
typedef struct
{
    float               low;
    time_t              time_low;
    float               high;
    time_t              time_high;
    float               when_high;
    float               cumulative;
    int                 samples;
    int                 debug;
}
WV_SENSOR;


//  ... for NOAA_DATA structure
typedef enum
{
    NOAA_TYPE_TEMP_HIGH         = 0,
    NOAA_TYPE_TEMP_LOW,
    NOAA_TYPE_TEMP_MEAN,
    NOAA_TYPE_RAIN,
    NOAA_TYPE_WIND_MEAN,
    NOAA_TYPE_WIND_HIGH,
    NOAA_TYPE_WIND_DIR,
    NOAA_TYPE_MAX
}
NOAA_TYPE;

// ... this structure adds a null property to the USHORT value

typedef struct
{
	  USHORT          value;
	  int             isNull;
}
__attribute__ ((packed)) c_USHORT;

// ... this structure adds a null property to the float value

typedef struct
{
	  float           value;
	  int             isNull;
}
__attribute__ ((packed)) c_FLOAT;

// internal HILOW data store
typedef struct
{
    WV_SENSOR           sensor[STF_MAX][SENSOR_MAX];
    WAVG                wind[STF_MAX];

    float               hourchangetemp;             // difference in degrees
    short               hourchangewind;             // difference in mph
    short               hourchangewinddir;          // difference in degrees
    short               hourchangehumid;            // difference in %
    float               hourchangedewpt;            // difference in degrees
    float               hourchangebarom;            // difference in inches

    float               daychangetemp;              // difference in degrees
    short               daychangewind;              // difference in mph
    short               daychangewinddir;           // difference in degrees
    short               daychangehumid;             // difference in %
    float               daychangedewpt;             // difference in degrees
    float               daychangebarom;             // difference in inches

    float               weekchangetemp;             // difference in degrees
    short               weekchangewind;             // difference in mph
    short               weekchangewinddir;          // difference in degrees
    short               weekchangehumid;            // difference in %
    float               weekchangedewpt;            // difference in degrees
    float               weekchangebarom;            // difference in inches

}
SENSOR_STORE;


// LOOP_PKT - define the LOOP (current readings) data format for IPM msgs and
//            station sensor polling; those required for archive generation
//            and HILOW are marked "!" and MUST be provided by the station
#define WVIEW_NUM_EXTRA_SENSORS     16
typedef struct
{
    // minimum required fields - station must provide these:
    float               barometer;              /* ! inches               */
    float               stationPressure;        /* ! inches               */
    float               altimeter;              /* ! inches               */
    float               inTemp;                 /* ! degrees F            */
    float               outTemp;                /* ! degrees F            */
    USHORT              inHumidity;             /* ! percent              */
    USHORT              outHumidity;            /* ! percent              */
    c_USHORT            windSpeed;              /* ! mph                  */
    c_USHORT            windDir;                /* ! degrees              */
    USHORT              windGust;               /* ! mph                  */
    USHORT              windGustDir;            /* ! degrees              */
    float               rainRate;               /* ! in/hr                */
    float               sampleRain;             /* ! inches               */
    float               sampleET;               /* ! ET                   */
    c_USHORT            radiation;              /* ! watts/m^2            */
    c_FLOAT             UV;                     /* ! UV index * 10        */
    float               dewpoint;               /* ! degrees F            */
    float               windchill;              /* ! degrees F            */
    float               heatindex;              /* ! degrees F            */

    // computed values - station should not alter these
    float               stormRain;              /* inches                 */
    time_t              stormStart;             /* time_t                 */
    float               dayRain;                /* inches                 */
    float               monthRain;              /* inches                 */
    float               yearRain;               /* inches                 */
    float               dayET;                  /* inches                 */
    float               monthET;                /* inches                 */
    float               yearET;                 /* inches                 */
    float               intervalAvgWCHILL;      /* degrees F              */
    USHORT              intervalAvgWSPEED;      /* mph                    */
    USHORT              yearRainMonth;          /* 1-12 Rain Start Month  */

    // --- The following may or may not be supported for a given station ---

    // Vantage Pro
    USHORT              rxCheckPercent;         /* 0 - 100                */
    USHORT              tenMinuteAvgWindSpeed;  /* mph                    */
    USHORT              forecastIcon;           /* VP only                */
    USHORT              forecastRule;           /* VP only                */
    USHORT              txBatteryStatus;        /* VP only                */
    USHORT              consBatteryVoltage;     /* VP only                */
    float               extraTemp1;             /* degrees F              */
    float               extraTemp2;             /* degrees F              */
    float               extraTemp3;             /* degrees F              */
    float               soilTemp1;              /* degrees F              */
    float               soilTemp2;              /* degrees F              */
    float               soilTemp3;              /* degrees F              */
    float               soilTemp4;              /* degrees F              */
    float               leafTemp1;              /* degrees F              */
    float               leafTemp2;              /* degrees F              */
    UCHAR               extraHumid1;            /* percent                */
    UCHAR               extraHumid2;            /* percent                */
    UCHAR               soilMoist1;
    UCHAR               soilMoist2;
    UCHAR               leafWet1;
    UCHAR               leafWet2;

    // Vaisala WXT-510
    float               wxt510Hail;             /* inches                 */
    float               wxt510Hailrate;         /* in/hr                  */
    float               wxt510HeatingTemp;      /* degrees F              */
    float               wxt510HeatingVoltage;   /* volts                  */
    float               wxt510SupplyVoltage;    /* volts                  */
    float               wxt510ReferenceVoltage; /* volts                  */
    float               wxt510RainDuration;     /* sec                    */
    float               wxt510RainPeakRate;     /* volts                  */
    float               wxt510HailDuration;     /* sec                    */
    float               wxt510HailPeakRate;     /* volts                  */
    float               wxt510Rain;             /* inches                 */

    // WMR918/968
    float               wmr918Pool;             /* degrees F              */
    UCHAR               wmr918Humid3;           /* percent                */
    UCHAR               wmr918Tendency;         /* WMR's Forecast         */
    UCHAR               wmr918WindBatteryStatus;
    UCHAR               wmr918RainBatteryStatus;
    UCHAR               wmr918OutTempBatteryStatus;
    UCHAR               wmr918InTempBatteryStatus;
    UCHAR               wmr918poolTempBatteryStatus;
    UCHAR               wmr918extra1BatteryStatus;
    UCHAR               wmr918extra2BatteryStatus;
    UCHAR               wmr918extra3BatteryStatus;

    // Generic extra sensor and status support:
    float               extraTemp[WVIEW_NUM_EXTRA_SENSORS];
    USHORT              extraHumidity[WVIEW_NUM_EXTRA_SENSORS];
    UCHAR               windBatteryStatus;
    UCHAR               rainBatteryStatus;
    UCHAR               outTempBatteryStatus;
    UCHAR               consoleBatteryStatus;
    UCHAR               uvBatteryStatus;
    UCHAR               solarBatteryStatus;
    UCHAR               extraTempBatteryStatus[WVIEW_NUM_EXTRA_SENSORS];
} LOOP_PKT;


// ARCHIVE_PKT - define the ARCHIVE data format for IPM msgs and archive record
//               processing
#define ARCHIVE_VALUE_NULL          -100000
typedef struct
{
    // minimum required fields - station must provide these:
    time_t              dateTime;               /* record date and time        */
    long                usUnits;                /* US units (0 or 1)           */
    long                interval;               /* Archive Interval in minutes */
    float               value[DATA_INDEX_MAX];
}
ARCHIVE_PKT;


//  ... this retrieves historical summations from the dbsqliteArchiveGetAverages method
typedef struct
{
    int                 samples[DATA_INDEX_MAX];
    time_t              startTime;
    float               values[DATA_INDEX_MAX];
}
HISTORY_DATA;


//  ... this structure retrieves NOAA data from the dbsqliteNOAAGetDay utility
//  ... and represents a record in the NOAA database wview-noaa.sdb
typedef struct
{
    short           year;
    short           month;
    short           day;
    float           meanTemp;
    float           highTemp;
    char            highTempTime[8];
    float           lowTemp;
    char            lowTempTime[8];
    float           heatDegDays;
    float           coolDegDays;
    float           rain;
    float           avgWind;
    float           highWind;
    char            highWindTime[8];
    int             domWindDir;
}
NOAA_DAY_REC;


/*  ... API methods
*/


#endif

