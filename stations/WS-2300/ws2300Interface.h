#ifndef INC_ws2300Interfaceh
#define INC_ws2300Interfaceh
/*---------------------------------------------------------------------------

  FILENAME:
        ws2300Interface.h

  PURPOSE:
        Provide the La Crosse WS-2300 family station interface API and utilities.

  REVISION HISTORY:
        Date            Engineer        Revision        Remarks
        02/27/2008      M.S. Teel       0               Original

  NOTES:


  LICENSE:
        Copyright (c) 2008, Mark S. Teel (mark@teel.ws)

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
#include <sys/ioctl.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>
#include <errno.h>
#include <math.h>

/*  ... Library include files
*/
#include <sysdefs.h>
#include <radtimeUtils.h>
#include <radmsgLog.h>
#include <radsysutils.h>

/*  ... Local include files
*/
#include <datadefs.h>
#include <dbsqlite.h>
#include <config.h>
#include <computedData.h>
#include <daemon.h>
#include <station.h>
#include <serial.h>
#include <ethernet.h>

#include <ws2300protocol.h>

#endif

