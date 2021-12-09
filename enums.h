#ifndef ENUMS_H
#define ENUMS_H

#define CALL_FILTER_TIME 100

enum MODES
{
    ANALOG_MODE = 0,
    DIGITAL_MODE
};

enum REG_UNREG{
    REGISTRATE,// = 0
    UNREGISTRATE
};

//enum STATE {
//    OFF,
//    ON
//};

//enum ReceiverType// for txt messages
//{
//    eDevice = 0x01,
//    eTalkGroup = 0x02,
//    eDispatcher = 0x04,
//    eEmail = 0x06
//};

//enum TrgLocationResult
//{
//    eNone = 0x00,
//    eUnsupportedVersion = 0x08,
//    eSyntaxError = 0x0A,
//    eProtocolElementNotSupported = 0x0B,
//    eProtocolElementValueOutOfRange = 0x0D,
//    eQueryInfoNotAttainable = 0x0F,
//    eQueryInfoNotCurrentlyAttainable = 0x10,
//    eReportingWillStop = 0x11,
//    eNoSuchRequest = 0x15,
//    eDuplicateRequestId = 0x16,
//    eInsufficientGpsSatellites = 0x200,
//    eBadGPSGeometry = 0x0201,
//    eIRRSResInsuffData = 0x1005,
//    eIRRSResNoProviders = 0x1006
//};

//enum LocMsgTypes
//{
//    ImmedLocRequest = 1, //  1
//    ImmedLocReport ,  //  2
//    TrigLocaRequest,// (0x03)
//    TrigLocAnswer, //(0x04)
//    TrigLocReport, //(0x05)
//    TrigLocStopRequest, //(0x06)
//    TrigLocStopAnswer //(0x07)
//};

//enum LocMsgTypesHytera
//{
//    StdLocImmReport  = 0xA0, //  1
//    EmgLocReportSrv  = 0xB0,  //  2
//    TrigLocReportSrv = 0xC0,// (0x03)
//    CondTrigReportSrv = 0xD0, //(0x04)
//    RssiReportConfigSrv = 0x0E
//};

//enum RrsMsgTypesHytera
//{
//    RegistrationRrs = 0x03,
//    RegistrationAckRrs = 0x80,
//    DeRegistrationRrs = 0x01,
//    OnLineCheckRrs = 0x02,
//    OnLineCheckAckRrs = 0x82
//};

//enum eARStype
//{
//    RegMessage,// = 0x00 to Kupol
//    RegAck,    // = 0x01 ????
//    SuccessReg,// = 0x02
//    FailureReg,// = 0x03
//    QueryMessage,// =0x04
//    DeregMessage,// =0x05 to Kupol
//    Unknown
//};

//enum TmsMsgTypes
//{
//    TmsServAvail,// (0x00)
//    TmsServAvailAck,// (0x01)
//    TmsAck,// (0x02)
//    SimpleTextMessage// (0x03)
//};

enum Reg
{
    //IDLE ,
//    MTR_SERIAL = 1, // init
//    MTR_ID = 2,
//    MTR_SIGMODE = 3,
//    ZONE_CHAN = 4,
//    AUDIO_ROUTE = 5,

//    READY = 6,  // normal state

//    WAIT_CALL_REPLY = 7,  // call start
//    WAIT_CALL_REPORT = 8,
//    WAIT_SECOND_B41E = 9,

//    WAIT_STOP_CALL_REPLY = 10, // call stop
//    WAIT_STOP_CALL_HANGIN = 11,
//    WAIT_STOP_CALL_ENDED = 12,


   // WAIT_CH_CHANGE_REPLY = 13,  //chan change
//    WAIT_CH_CHANGE_BRDCST = 14,
//    WAIT_CH_CHANGE_SIGID = 15,

//    WAIT_RELOAD_REPLY = 16,

//    WAIT_IN_CALL_IN_PROGRESS = 17, // receive call
//    WAIT_IN_CALL_HANGIN = 18,
//    WAIT_IN_CALL_ENDED = 19,



//    WAIT_CALL_INTERRUPT_REPLY = 20, // call interrupt
//    WAIT_CALL_INTERRUPT_PRGS = 21,

//    WAIT_HARD_PTT = 22,
//    WAIT_HARD_PTT_START = 23,
//    WAIT_HARD_PTT_STOP = 24,
//    WAIT_HARD_PTT_REPLY = 25,
//    WAIT_HARD_PTT_HANGIN = 26,

//    WAIT_CALL_CTRL_REPLY = 27,
//    WAIT_CH_CHNG_SIG_ID = 28
};

//enum XCMP_RadioStatusCondition
//{
//    RSSI = 0x02,
//    ModelNumber = 0x07,
//    SerialNumber = 0x08,
//    ProductESN = 0x09,
//    CurrentSignallingMode = 0x0D,
//    MotoTrboID = 0x0E,
//    Select5RadioId = 0x16,
//    IMEStatus = 0x1C,
//    ARTSStatus = 0x1F
//};

enum typesOfmessages{
    eRegReportMsg = 1,
    eChanChangeReportMsg = 2,
    eChanChangeRequestMsg = 3,
    eChanChangeReplyMsg = 4,
    eCallReportMsg = 5,
    eCallRequestMsg = 6,
    eCallReplayMsg = 7,
    eStopCallRequestMsg = 8,
    eStopCallReply = 9,
    eErrReply = 10,
    eRldRequestMsg = 11,
    eRldReplayMsg = 12,
    eUdpPortNotification = 13,
    eSuControlRequest = 0x0E, // Запрос команды управления абонентом
    eSuControlReply = 0x0F,
    eSuEmergencyReport = 0x10,
    eModemTypeReport = 0x11
};

enum eReqResut{
    eSucces = 0,
    eFailure = 1,
    eIncorrectMode = 2,
    eOptNotSupport = 3,
    eInvalidPar = 4,
    eReplyToBig = 5,
    eSecurLocked = 6,
    eFuncNotAvalible = 7,
    eFutureDisable = 0x10,
    eRadioBusy = 0x11,
    eInvalidTgtAdress = 0x12,
    eSysBusy = 0x13,
    eUnmapPin = 0x21
};

enum CallState{
    eCallInterupt = 2,
    eCallInProgress = 2,
    eCallEnded = 3,
    eCallInit = 4,
    eCallHandTime = 7,
    eCallNoAck = 6,
    eCallDecoded = 8
};

//enum CallTypes{
//    eGroupCall = 1,
//    eEmrgGroupCall = 2,
//    ePrivCall = 3,
//    eEmergPrivCall = 4,
//    eAllCall = 5,
//    eEmergAllCall = 6,
//};

//enum XCMPCallControlFunction //: byte
//{
//    eTerminate = 0x00,
//    eInitiate = 0x01,
//    eOverDialDuringPhoneCall = 0x04,
//    eCancelQueuedCall = 0x05,
//    eQuery = 0x80
//};

//enum SuControlRequestType
//{    // – Тип команды управления удаленной абонентской радиостанцией. Длина – 1 байт. Допустимые значения:
//    Check = 0x00,
//    Inhibit = 0x01,
//    Uninhibit = 0x02,
//    Monitor = 0x03,
//    Dekey = 0x04
//};

//enum EmergencyStatus
//{
//    None = 0x00,
//    Exited = 0x01,
//    EmergencyAlarm = 0x02,
//    EmergencyCall = 0x03
//};

#endif // ENUMS_H
