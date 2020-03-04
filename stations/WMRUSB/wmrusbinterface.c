/*---------------------------------------------------------------------------

  FILENAME:
        wmrusbinterface.c

  PURPOSE:
        Provide the Oregon Scientific WMR station interface API and utilities.

  REVISION HISTORY:
        Date            Engineer        Revision        Remarks
        03/10/2011      M.S. Teel       1               Original

  NOTES:
        The WMR station provides a USB HID interface for I/O

  LICENSE:

        This source code is released for free distribution under the terms
        of the GNU General Public License.

----------------------------------------------------------------------------*/

/*  ... System include files
*/

/*  ... Library include files
*/

/*  ... Local include files
*/
#include <wmrusbinterface.h>

/*  ... global memory declarations
*/

/*  ... local memory
*/

static WMR_IF_DATA      wmrWorkData;
static void ( *ArchiveIndicator )( ARCHIVE_PKT* newRecord );

static void serialPortConfig( int fd );

////////////****////****  S T A T I O N   A P I  ****////****////////////
/////  Must be provided by each supported wview station interface  //////

// station-supplied init function
// -- Can Be Asynchronous - event indication required --
//
// MUST:
//   - set the 'stationGeneratesArchives' flag in WVIEWD_WORK:
//     if the station generates archive records (TRUE) or they should be
//     generated automatically by the daemon from the sensor readings (FALSE)
//   - Initialize the 'stationData' store for station work area
//   - Initialize the interface medium
//   - do initial LOOP acquisition
//   - do any catch-up on archive records if there is a data logger
//   - 'work->runningFlag' can be used for start up synchronization but should
//     not be modified by the station interface code
//   - indicate init is done by sending the STATION_INIT_COMPLETE_EVENT event to
//     this process (radProcessEventsSend (NULL, STATION_INIT_COMPLETE_EVENT, 0))
//
// OPTIONAL:
//   - Initialize a state machine or any other construct required for the
//     station interface - these should be stored in the 'stationData' store
//
// 'archiveIndication' - indication callback used to pass back an archive record
//   generated as a result of 'stationGetArchive' being called; should receive a
//   NULL pointer for 'newRecord' if no record available; only used if
//   'stationGeneratesArchives' flag is set to TRUE by the station interface
//
// Returns: OK or ERROR
//
int stationInit
(
    WVIEWD_WORK*     work,
    void ( *archiveIndication )( ARCHIVE_PKT* newRecord )
)
{
    int             i;
    STIM            stim;

    memset( &wmrWorkData, 0, sizeof( wmrWorkData ) );

    // save the archive indication callback (we should never need it)
    ArchiveIndicator = archiveIndication;

    // set our work data pointer
    work->stationData = &wmrWorkData;

    // set the Archive Generation flag to indicate the WMR918 DOES NOT
    // generate them
    work->stationGeneratesArchives = FALSE;

    // grab the station configuration now
    if( stationGetConfigValueInt( work,
                                  STATION_PARM_ELEVATION,
                                  &wmrWorkData.elevation )
            == ERROR )
    {
        radMsgLog( PRI_HIGH, "stationInit: stationGetConfigValueInt ELEV failed!" );
        return ERROR;
    }
    if( stationGetConfigValueFloat( work,
                                    STATION_PARM_LATITUDE,
                                    &wmrWorkData.latitude )
            == ERROR )
    {
        radMsgLog( PRI_HIGH, "stationInit: stationGetConfigValueInt LAT failed!" );
        return ERROR;
    }
    if( stationGetConfigValueFloat( work,
                                    STATION_PARM_LONGITUDE,
                                    &wmrWorkData.longitude )
            == ERROR )
    {
        radMsgLog( PRI_HIGH, "stationInit: stationGetConfigValueInt LONG failed!" );
        return ERROR;
    }
    if( stationGetConfigValueInt( work,
                                  STATION_PARM_ARC_INTERVAL,
                                  &wmrWorkData.archiveInterval )
            == ERROR )
    {
        radMsgLog( PRI_HIGH, "stationInit: stationGetConfigValueInt ARCINT failed!" );
        return ERROR;
    }
    if( stationGetConfigValueInt( work,
                                  STATION_PARM_OUTSIDE_CHANNEL,
                                  &wmrWorkData.outsideChannel )
            == ERROR )
    {
        radMsgLog( PRI_HIGH, "stationInit: stationGetConfigValueInt outside channel failed!" );
        return ERROR;
    }

    // set the work archive interval now
    work->archiveInterval = wmrWorkData.archiveInterval;

    // sanity check the archive interval against the most recent record
    if( stationVerifyArchiveInterval( work ) == ERROR )
    {
        // bad magic!
        radMsgLog( PRI_HIGH, "stationInit: stationVerifyArchiveInterval failed!" );
        radMsgLog( PRI_HIGH, "You must either move old archive data out of the way -or-" );
        radMsgLog( PRI_HIGH, "fix the interval setting..." );
        return ERROR;
    }
    else
    {
        radMsgLog( PRI_STATUS, "station archive interval: %d minutes",
                   work->archiveInterval );
    }

    radMsgLog( PRI_STATUS, "Starting station interface: WMR" );

    if( wmrInit( work ) == ERROR )
    {
        radMsgLog( PRI_HIGH, "stationInit: wmrInit failed!" );
        return ERROR;
    }

    radMsgLog( PRI_STATUS, "WMR on USB %4.4X:%4.4X opened ...",
               WMR_VENDOR_ID, WMR_PRODUCT_ID );

    return OK;
}

// station-supplied exit function
//
// Returns: N/A
//
void stationExit( WVIEWD_WORK* work )
{
    wmrExit( work );
    return;
}

// station-supplied function to retrieve positional info (lat, long, elev) -
// should populate 'work' fields: latitude, longitude, elevation
// -- Synchronous --
//
// - If station does not store these parameters, they can be retrieved from the
//   wview.conf file (see daemon.c for example conf file use) - user must choose
//   station type "Generic" when running the wviewconfig script
//
// Returns: OK or ERROR
//
int stationGetPosition( WVIEWD_WORK* work )
{
    // just set the values from our internal store - we retrieved them in
    // stationInit
    work->elevation     = ( int16_t )wmrWorkData.elevation;
    if( wmrWorkData.latitude >= 0 )
        work->latitude      = ( int16_t )( ( wmrWorkData.latitude * 10 ) + 0.5 );
    else
        work->latitude      = ( int16_t )( ( wmrWorkData.latitude * 10 ) - 0.5 );
    if( wmrWorkData.longitude >= 0 )
        work->longitude     = ( int16_t )( ( wmrWorkData.longitude * 10 ) + 0.5 );
    else
        work->longitude     = ( int16_t )( ( wmrWorkData.longitude * 10 ) - 0.5 );

    radMsgLog( PRI_STATUS, "station location: elevation: %d feet",
               work->elevation );

    radMsgLog( PRI_STATUS, "station location: latitude: %3.1f %c  longitude: %3.1f %c",
               ( float )abs( work->latitude ) / 10.0,
               ( ( work->latitude < 0 ) ? 'S' : 'N' ),
               ( float )abs( work->longitude ) / 10.0,
               ( ( work->longitude < 0 ) ? 'W' : 'E' ) );

    return OK;
}

// station-supplied function to indicate a time sync should be performed if the
// station maintains time, otherwise may be safely ignored
// -- Can Be Asynchronous --
//
// Returns: OK or ERROR
//
int stationSyncTime( WVIEWD_WORK* work )
{
    // We don't use the WMR time...
    return OK;
}

// station-supplied function to indicate sensor readings should be performed -
// should populate 'work' struct: loopPkt (see datadefs.h for minimum field reqs)
// -- Can Be Asynchronous --
//
// - indicate readings are complete by sending the STATION_LOOP_COMPLETE_EVENT
//   event to this process (radProcessEventsSend (NULL, STATION_LOOP_COMPLETE_EVENT, 0))
//
// Returns: OK or ERROR
//
int stationGetReadings( WVIEWD_WORK* work )
{
    wmrGetReadings( work );

    return OK;
}

// station-supplied function to indicate an archive record should be generated -
// MUST populate an ARCHIVE_RECORD struct and indicate it to 'archiveIndication'
// function passed into 'stationInit'
// -- Asynchronous - callback indication required --
//
// Returns: OK or ERROR
//
// Note: 'archiveIndication' should receive a NULL pointer for the newRecord if
//       no record is available
// Note: This function will only be invoked by the wview daemon if the
//       'stationInit' function set the 'stationGeneratesArchives' to TRUE
//
int stationGetArchive( WVIEWD_WORK* work )
{
    // just indicate a NULL record, WMR918 does not generate them (and this
    // function should never be called!)
    ( *ArchiveIndicator )( NULL );
    return OK;
}

// station-supplied function to indicate data is available on the station
// interface medium (serial or ethernet) -
// It is the responsibility of the station interface to read the data from the
// medium and process appropriately. The data does not have to be read within
// the context of this function, but may be used to stimulate a state machine.
// -- Synchronous --
//
// Returns: N/A
//
void stationDataIndicate( WVIEWD_WORK* work )
{
    return;
}

// station-supplied function to receive IPM messages - any message received by
// the generic station message handler which is not recognized will be passed
// to the station-specific code through this function.
// It is the responsibility of the station interface to process the message
// appropriately (or ignore it).
// -- Synchronous --
//
// Returns: N/A
//
void stationMessageIndicate( WVIEWD_WORK* work, int msgType, void* msg )
{
    if( msgType == WVIEW_MSG_TYPE_STATION_DATA )
    {
        // Receive data from our reader thread:
        wmrReadData( work, ( WMRUSB_MSG_DATA* )msg );
    }

    return;
}

// station-supplied function to indicate the interface timer has expired -
// It is the responsibility of the station interface to start/stop the interface
// timer as needed for the particular station requirements.
// The station interface timer is specified by the 'ifTimer' member of the
// WVIEWD_WORK structure. No other timers in that structure should be manipulated
// in any way by the station interface code.
// -- Synchronous --
//
// Returns: N/A
//
void stationIFTimerExpiry( WVIEWD_WORK* work )
{
    // restart our IF timer:
    radProcessTimerStart( work->ifTimer, WMR_PROCESS_TIME_INTERVAL );

    // Process data:
    while( wmrProcessData( work ) );

    if( work->DebugStationByteCount )
    {
        if( ++work->StatCount >= 60 )
        {
            radMsgLog( PRI_MEDIUM, "STATS: raw:%d stream:%d pkt:%d cksum:%d unk:%d",
                       work->UsbRawBytes,
                       work->StreamBytes,
                       work->PacketBytes,
                       work->ChecksumBytes,
                       work->UnknownPacketType );
            work->StatCount = 0;
        }
    }

    return;
}

////////////****////  S T A T I O N   A P I   E N D  ////****////////////

