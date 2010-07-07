//
//  Values are 32 bit values laid out as follows:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +---+-+-+-----------------------+-------------------------------+
//  |Sev|C|R|     Facility          |               Code            |
//  +---+-+-+-----------------------+-------------------------------+
//
//  where
//
//      Sev - is the severity code
//
//          00 - Success
//          01 - Informational
//          10 - Warning
//          11 - Error
//
//      C - is the Customer code flag
//
//      R - is a reserved bit
//
//      Facility - is the facility code
//
//      Code - is the facility's status code
//
//
// Define the facility codes
//


//
// Define the severity codes
//


//
// MessageId: EVMSG_STARTED
//
// MessageText:
//
// "%1" successfully started.
//
#define EVMSG_STARTED                    0x00000001L

//
// MessageId: EVMSG_INSTALLED
//
// MessageText:
//
// "%1" successfully registered.
//
#define EVMSG_INSTALLED                  0x00000002L

//
// MessageId: EVMSG_REMOVED
//
// MessageText:
//
// "%1" successfully unregistered.
//
#define EVMSG_REMOVED                    0x00000003L

//
// MessageId: EVMSG_STOPPED
//
// MessageText:
//
// "%1" successfully stopped.
//
#define EVMSG_STOPPED                    0x00000004L

//
// MessageId: EVMSG_FAILED_REMOVE
//
// MessageText:
//
// Failed to unregister "%1".
//
#define EVMSG_FAILED_REMOVE              0x00000005L

//
// MessageId: EVMSG_FAILED_INIT
//
// MessageText:
//
// Failed to initialize "%1".
//
#define EVMSG_FAILED_INIT                0x00000006L

//
// MessageId: EVMSG_FAILED_CONNECT_SCM
//
// MessageText:
//
// Failed to connect to Service Control Manager (%1).
//
#define EVMSG_FAILED_CONNECT_SCM         0x00000007L

//
// MessageId: EVMSG_FAILED_OPEN_SERVICE
//
// MessageText:
//
// Failed to open Service into Service Control Manager (%1).
//
#define EVMSG_FAILED_OPEN_SERVICE        0x00000008L

//
// MessageId: EVMSG_FAILED_CTRLHANDLER
//
// MessageText:
//
// Failed to register Event Handler for "%1".
//
#define EVMSG_FAILED_CTRLHANDLER         0x00000009L

//
// MessageId: EVMSG_FAILED_STOP
//
// MessageText:
//
// Failed to stop "%1".
//
#define EVMSG_FAILED_STOP                0x0000000AL

//
// MessageId: EVMSG_BAD_REQUEST
//
// MessageText:
//
// "%1" receive an unknown request.
//
#define EVMSG_BAD_REQUEST                0x0000000BL

//
// MessageId: EVMSG_GENERIC_MESSAGE
//
// MessageText:
//
// %1.
//
#define EVMSG_GENERIC_MESSAGE            0x00000013L

//
// MessageId: EVMSG_GENERIC_ERROR
//
// MessageText:
//
// Service encounter error <%1>.
//
#define EVMSG_GENERIC_ERROR              0x00000014L

