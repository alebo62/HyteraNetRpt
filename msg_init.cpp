
#include "messages.h"
#include "stdlib.h"
#include "string.h"
#include "enums.h"

#define htonl(A)    (((static_cast<unsigned long>(A) & 0xff000000) >> 24) | \
    ((static_cast<unsigned long>(A) & 0x00ff0000) >> 8)  | \
    ((static_cast<unsigned long>(A) & 0x0000ff00) << 8)  | \
    ((static_cast<unsigned long>(A) & 0x000000ff) << 24))

#define ntohl    htonl

hdpep_t 			 pep;
hdpep_no_pld_t pep_npld;
hrnp_t         restart;
hrnp_no_pld_t  ack_msg;
hrnp_no_pep_t  conn_req;
hrnp_no_pep_t  close_req;
hrnp_no_pld_t  pwr_up_chk_req;
hrnp_t               remote_monitor;
hrnp_t 				 uart_config_req;
hrnp_t 				 txt_notif_req;

hrnp_t               radio_check;
hrnp_t               radio_en_dis;
hrnp_t               zone_ch;
hrnp_t               serialNum_req;
hrnp_t               chan_status;
hrnp_t               radio_id;
hrnp_t               call_req;
hrnp_t               ptt_press_req;
hrnp_t               ptt_release_req;
hrnp_t               br_cast_req;
hrnp_t               rtp_pld_req;
hrnp_t               dig_audio_rx_req;
hrnp_t               dig_audio_tx_req;
hrnp_t               key_notific_req;
hrnp_t               txt_notific_req;
hrnp_t               bc_txt_notific_req;
hrnp_t               br_cast_analog;
hrnp_t               txt_msg_tx;
struct strCRpt 	 sCallReport1;
struct strCRp    sCallReply1;
struct strCSRp   sCallStopReply1;

struct strMsgType_Ping  sMsgType_Ping;
struct strRMRpt         sRegMsgReport;
struct strMRRpt         sMRRpt;
struct strCRpt          sCallReport;
struct strCRp           sCallReply;
//struct strCallControl   sCallControl;
extern quint32          Radio_Reg_State;
struct strCSRp          sCallStopReply;
struct strCCRpl         sChanChangeReply;
struct strCCRpt         sChanChangeReport;
struct strRldRpl        sReloadReplay;
struct strCtrlRpl       sCtrlReply;
struct strTxtMsg_SimpMsg sTxtMsg_SimpMsg;
//struct strRMRpt 				sRegMsgReport1;
struct strLocMsgType_TrgRep sLocMsgType_TrgRep;

strARSmsg        sARSmsg;

extern unsigned short checksum (unsigned char *ip);
extern void checksum_pep_no_pld(hrnp_no_pld_t *ip);
extern void checksum_pep(hrnp_t *ip);
extern quint8 tx[];
extern quint8 mu_law;
extern QString dest_rrs_ip;
quint8 p[1] = {0};

static int cs;
quint32 ts_current;
quint32 period;
rtp_header rtp_hdr;

quint8 ars_answer[16];
quint8 ars_check[11];
quint8 std_loc_imm_req[15];

void msg_init()
{
//    std_loc_imm_req[0] = 0x08;
//    std_loc_imm_req[1] = 0xA0;
//    std_loc_imm_req[2] = 0x01;
//    std_loc_imm_req[3] = 0x00;
//    std_loc_imm_req[4] = 0x08;
//    std_loc_imm_req[9] = 0x0A;// radio ip
//    std_loc_imm_req[14] = 0x03;


    sMsgType_Ping.header.msgType = 4;
    sMsgType_Ping.header.payloadLength[1] = 1;

//    sLocMsgType_TrgRep.header.msgType = 3;
//    sLocMsgType_TrgRep.header.payloadLength[0] = 0;
//    sLocMsgType_TrgRep.header.payloadLength[1] = 37;
//    memset(sLocMsgType_TrgRep.RadioId, 0, 4 );
//    memset(sLocMsgType_TrgRep.RequestId, 0, 4 );
//    memset(sLocMsgType_TrgRep.Result, 0, 4 );
//    memset(sLocMsgType_TrgRep.Altitude, 0, 4 );
//    memset(sLocMsgType_TrgRep.Heading, 0, 4 );
//    memset(sLocMsgType_TrgRep.Radius, 0, 4 );

    sMRRpt.header.msgType = 1;
    sMRRpt.header.payloadLength[0] = 0;
    sMRRpt.header.payloadLength[1] = 5;
    sMRRpt.none = 0;
    sMRRpt.Msgtype = eModemTypeReport;// =0
    sMRRpt.ver[0] = 1;
    sMRRpt.ver[1] = 2;
    sMRRpt.ModemType = 2; // Hytera!!!!!!!!!

//
//    sARSmsg.header.msgType = 0;
//    sARSmsg.header.payloadLength[0] = 0;
//    sARSmsg.header.payloadLength[1] = 10;
//    sARSmsg.arsMsgType = RegMessage;// =0
//    sARSmsg.csbk = 0;

//    txt_msg_tx.header = 0x7E;
//    txt_msg_tx.version = 0x04;
//    txt_msg_tx.block = 0;
//    txt_msg_tx.opcode = DATA;
//    txt_msg_tx.source = 0x20;
//    txt_msg_tx.dest = 0x10;
//    txt_msg_tx.packet_num[0] = 0;
//    txt_msg_tx.packet_num[1] = 3;
//    txt_msg_tx.length[0] = 0;
//    txt_msg_tx.length[1] = 19;// + message len

//    txt_msg_tx.pep.MsgHdr = 0x09;
//    txt_msg_tx.pep.opcode[0] = 0x00;
//    txt_msg_tx.pep.opcode[1] = 0xA1;
//    txt_msg_tx.pep.num_of_bytes[0] = 0;
//    txt_msg_tx.pep.num_of_bytes[1] = 16;

//    txt_msg_tx.pep.pld = (quint8*) malloc(12 + 280); // 12 + max simvols 140(x2)

//    txt_msg_tx.pep.pld[0] = 0;  // req id
//    txt_msg_tx.pep.pld[1] = 0;
//    txt_msg_tx.pep.pld[2] = 0;
//    txt_msg_tx.pep.pld[3] = 1;

//    txt_msg_tx.pep.pld[4] = 0;//dest_rrs_ip.toInt(); // dest
//    txt_msg_tx.pep.pld[5] = 0;
//    txt_msg_tx.pep.pld[6] = 0;
//    txt_msg_tx.pep.pld[7] = 52;

//    txt_msg_tx.pep.pld[8] = 0;//dest_rrs_ip.toInt();  // source
//    txt_msg_tx.pep.pld[9] = 0;
//    txt_msg_tx.pep.pld[10] = 0;
//    txt_msg_tx.pep.pld[11] = 1;

//    txt_msg_tx.pep.Checksum = 0x00;
//    txt_msg_tx.pep.MsgEnd = 0x03;

//    // используем для подтверждений(len = 16) и запросов(len = 11)
//    ars_answer[0] = 0x11; // rrs

//    ars_answer[1] = 0x00; // opcode
//    ars_answer[2] = 0x80; // opcode

//    ars_answer[3] = 0x00; // size
//    ars_answer[4] = 0x09; // size

//    ars_answer[5] = dest_rrs_ip.toInt(); // ip
//    ars_answer[6] = 0x00; // ip
//    ars_answer[7] = 0x00; // ip
//    ars_answer[8] = 0x00; // ip

//    ars_answer[9] = 0x00; // result

//    ars_answer[10] = 0x00; // time
//    ars_answer[11] = 0x00; // time
//    ars_answer[12] = 0x00; // time
//    ars_answer[13] = 60; // time

//    ars_answer[14] = 0x00; // crc
//    ars_answer[15] = 0x03;




    ars_check[0] = 0x11; // rrs

//    ars_check[1] = 0x00; // opcode
//    ars_check[2] = 0x02;

//    ars_check[3] = 0x00; // size
//    ars_check[4] = 0x04;

//    ars_check[5] = dest_rrs_ip.toInt(); // ip
//    ars_check[6] = 0x00;
//    ars_check[7] = 0x00;
//    ars_check[8] = 0x00;

//    ars_check[9] = 0x00; // crc
//    ars_check[10] = 0x03;

//    radio_check.header = 0x7E;
//    radio_check.version = 0x04;
//    radio_check.block = 0;
//    radio_check.opcode = DATA;
//    radio_check.source = 0x20;
//    radio_check.dest = 0x10;
//    radio_check.packet_num[0] = 0;
//    radio_check.packet_num[1] = 3;
//    radio_check.length[0] = 0;
//    radio_check.length[1] = 12 + 11;
//    radio_check.check_sum[0] = 0;
//    radio_check.check_sum[1] = 0;

//    radio_check.pep.MsgHdr = 0x02;
//    radio_check.pep.opcode[0] = 0x33;
//    radio_check.pep.opcode[1] = 0x08;
//    radio_check.pep.num_of_bytes[0] = 4;
//    radio_check.pep.num_of_bytes[1] = 0;
//    radio_check.pep.pld = (quint8*) malloc(radio_check.pep.num_of_bytes[0]);
//    radio_check.pep.pld[0] = 0;//dest_rrs_ip.toInt();
//    radio_check.pep.pld[1] = 0x00;
//    radio_check.pep.pld[2] = 0x00;
//    radio_check.pep.pld[3] = 0;
//    radio_check.pep.Checksum = 0x00;
//    radio_check.pep.MsgEnd = 0x03;

//    radio_en_dis.header = 0x7E;
//    radio_en_dis.version = 0x04;
//    radio_en_dis.block = 0;
//    radio_en_dis.opcode = DATA;
//    radio_en_dis.source = 0x20;
//    radio_en_dis.dest = 0x10;
//    radio_en_dis.packet_num[0] = 0;
//    radio_en_dis.packet_num[1] = 3;
//    radio_en_dis.length[0] = 0;
//    radio_en_dis.length[1] = 12 + 11;
//    radio_en_dis.check_sum[0] = 0;
//    radio_en_dis.check_sum[1] = 0;

//    radio_en_dis.pep.MsgHdr = 0x02;
//    radio_en_dis.pep.opcode[0] = 0x49;
//    radio_en_dis.pep.opcode[1] = 0x08;
//    radio_en_dis.pep.num_of_bytes[0] = 4;
//    radio_en_dis.pep.num_of_bytes[1] = 0;
//    radio_en_dis.pep.pld = (quint8*) malloc(radio_en_dis.pep.num_of_bytes[0]);
//    radio_en_dis.pep.pld[0] = 0;
//    radio_en_dis.pep.pld[1] = 0x00;
//    radio_en_dis.pep.pld[2] = 0x00;
//    radio_en_dis.pep.pld[3] = 0;
//    radio_en_dis.pep.Checksum = 0x00;
//    radio_en_dis.pep.MsgEnd = 0x03;





//    pwr_up_chk_req.header = 0x7E;
//    pwr_up_chk_req.version = 0x04;
//    pwr_up_chk_req.block = 0;
//    pwr_up_chk_req.opcode = DATA;
//    pwr_up_chk_req.source = 0x20;
//    pwr_up_chk_req.dest = 0x10;
//    pwr_up_chk_req.packet_num[0] = 0;
//    pwr_up_chk_req.packet_num[1] = 1;
//    pwr_up_chk_req.length[0] = 0;
//    pwr_up_chk_req.length[1] = 19;
//    pwr_up_chk_req.pep_npld.MsgHdr = 0x02;
//    pwr_up_chk_req.pep_npld.opcode[0] = 0xC6;
//    pwr_up_chk_req.pep_npld.opcode[1] = 0x00;
//    pwr_up_chk_req.pep_npld.num_of_bytes[0] = 0;
//    pwr_up_chk_req.pep_npld.num_of_bytes[1] = 0;
//    pwr_up_chk_req.pep_npld.Checksum = 0x00;
//    pwr_up_chk_req.pep_npld.MsgEnd = 0x03;

//    zone_ch.header = 0x7E;
//    zone_ch.version = 0x04;
//    zone_ch.block = 0;
//    zone_ch.opcode = DATA;
//    zone_ch.source = 0x20;
//    zone_ch.dest = 0x10;
//    zone_ch.packet_num[0] = 0;
//    zone_ch.packet_num[1] = 3;
//    zone_ch.length[0] = 0;
//    zone_ch.length[1] = 12 + 12;
//    zone_ch.pep.MsgHdr = 0x02;
//    zone_ch.pep.opcode[0] = 0xC4;
//    zone_ch.pep.opcode[1] = 0x00;
//    zone_ch.pep.num_of_bytes[0] = 5;
//    zone_ch.pep.num_of_bytes[1] = 0;
//    zone_ch.pep.pld = (quint8*) malloc(zone_ch.pep.num_of_bytes[0]);
//    zone_ch.pep.pld[0] = 0x01; //read message
//    zone_ch.pep.pld[1] = 0x01;
//    zone_ch.pep.pld[2] = 0x00;
//    zone_ch.pep.pld[3] = 0x01;
//    zone_ch.pep.pld[4] = 0x00;
//    zone_ch.pep.Checksum = 0x00;
//    zone_ch.pep.MsgEnd = 0x03;

//    serialNum_req.header = 0x7E;
//    serialNum_req.version = 0x04;
//    serialNum_req.block = 0;
//    serialNum_req.opcode = DATA;
//    serialNum_req.source = 0x20;
//    serialNum_req.dest = 0x10;
//    serialNum_req.packet_num[0] = 0;
//    serialNum_req.packet_num[1] = 3;
//    serialNum_req.length[0] = 0;
//    serialNum_req.length[1] = (12 + 5) + 1 + 2;
//    serialNum_req.pep.MsgHdr = 0x02;
//    serialNum_req.pep.opcode[0] = 0x01;
//    serialNum_req.pep.opcode[1] = 0x02;
//    serialNum_req.pep.num_of_bytes[0] = 1;
//    serialNum_req.pep.num_of_bytes[1] = 0;
//    serialNum_req.pep.pld = (quint8*) malloc(serialNum_req.pep.num_of_bytes[0]);
//    serialNum_req.pep.pld[0] = 0x02; // s/n
//    serialNum_req.pep.Checksum = 0x00;
//    serialNum_req.pep.MsgEnd = 0x03;

//    chan_status.header = 0x7E;
//    chan_status.version = 0x04;
//    chan_status.block = 0;
//    chan_status.opcode = DATA;
//    chan_status.source = 0x20;
//    chan_status.dest = 0x10;
//    chan_status.packet_num[0] = 0;
//    chan_status.packet_num[1] = 3;
//    chan_status.length[0] = 0;
//    chan_status.length[1] = (12 + 5) + 2 + 2;
//    chan_status.pep.MsgHdr = 0x02;
//    chan_status.pep.opcode[0] = 0xE7;
//    chan_status.pep.opcode[1] = 0x00;
//    chan_status.pep.num_of_bytes[0] = 2;
//    chan_status.pep.num_of_bytes[1] = 0;
//    chan_status.pep.pld = (quint8*) malloc(chan_status.pep.num_of_bytes[0]);
//    chan_status.pep.pld[0] = 0x07; // channel status
//    chan_status.pep.pld[1] = 0x00;
//    chan_status.pep.Checksum = 0x00;
//    chan_status.pep.MsgEnd = 0x03;

//    radio_id.header = 0x7E;
//    radio_id.version = 0x04;
//    radio_id.block = 0;
//    radio_id.opcode = DATA;
//    radio_id.source = 0x20;
//    radio_id.dest = 0x10;
//    radio_id.packet_num[0] = 0;
//    radio_id.packet_num[1] = 3;
//    radio_id.length[0] = 0;
//    radio_id.length[1] = (12 + 5) + 1 + 2;
//    radio_id.pep.MsgHdr = 0x02;
//    radio_id.pep.opcode[0] = 0x52;
//    radio_id.pep.opcode[1] = 0x04;
//    radio_id.pep.num_of_bytes[0] = 1;
//    radio_id.pep.num_of_bytes[1] = 0;
//    radio_id.pep.pld = (quint8*) malloc(radio_id.pep.num_of_bytes[0]);
//    radio_id.pep.pld[0] = 0x00; // radio id
//    radio_id.pep.Checksum = 0x00;
//    radio_id.pep.MsgEnd = 0x03;

//    key_notific_req.header = 0x7E;
//    key_notific_req.version = 0x04;
//    key_notific_req.block = 0;
//    key_notific_req.opcode = DATA;
//    key_notific_req.source = 0x20;
//    key_notific_req.dest = 0x10;
//    key_notific_req.packet_num[0] = 0;
//    key_notific_req.packet_num[1] = 3;
//    key_notific_req.length[0] = 0;
//    key_notific_req.length[1] = (12 + 5) + 7 + 2;;
//    key_notific_req.pep.MsgHdr = 0x02;
//    key_notific_req.pep.opcode[0] = 0xE4;
//    key_notific_req.pep.opcode[1] = 0x10;
//    key_notific_req.pep.num_of_bytes[0] = 7;
//    key_notific_req.pep.num_of_bytes[1] = 0;
//    key_notific_req.pep.pld = (quint8*) malloc(key_notific_req.pep.num_of_bytes[0]);
//    key_notific_req.pep.pld[0] = 3;  // req id
//    key_notific_req.pep.pld[1] = 3;
//    key_notific_req.pep.pld[2] = 2;
//    key_notific_req.pep.pld[3] = 0x18;
//    key_notific_req.pep.pld[4] = 1;
//    key_notific_req.pep.pld[5] = 0x19;
//    key_notific_req.pep.pld[6] = 1;
//    key_notific_req.pep.Checksum = 0x00;
//    key_notific_req.pep.MsgEnd = 0x03;

//    bc_txt_notific_req.header = 0x7E;//broadcast message configuration
//    bc_txt_notific_req.version = 0x04;
//    bc_txt_notific_req.block = 0;
//    bc_txt_notific_req.opcode = DATA;
//    bc_txt_notific_req.source = 0x20;
//    bc_txt_notific_req.dest = 0x10;
//    bc_txt_notific_req.packet_num[0] = 0;
//    bc_txt_notific_req.packet_num[1] = 3;
//    bc_txt_notific_req.length[0] = 0;
//    bc_txt_notific_req.length[1] = (12 + 5) + 8 + 2;;
//    bc_txt_notific_req.pep.MsgHdr = 0x02;
//    bc_txt_notific_req.pep.opcode[0] = 0x47;
//    bc_txt_notific_req.pep.opcode[1] = 0x18;
//    bc_txt_notific_req.pep.num_of_bytes[0] = 8;
//    bc_txt_notific_req.pep.num_of_bytes[1] = 0;
//    bc_txt_notific_req.pep.pld = (quint8*) malloc(bc_txt_notific_req.pep.num_of_bytes[0]);
//    bc_txt_notific_req.pep.pld[0] = 0;// 0-normal txt   1-short txt
//    bc_txt_notific_req.pep.Checksum = 0x00;
//    bc_txt_notific_req.pep.MsgEnd = 0x03;

//    txt_notific_req.header = 0x7E;
//    txt_notific_req.version = 0x04;
//    txt_notific_req.block = 0;
//    txt_notific_req.opcode = DATA;
//    txt_notific_req.source = 0x20;
//    txt_notific_req.dest = 0x10;
//    txt_notific_req.packet_num[0] = 0;
//    txt_notific_req.packet_num[1] = 3;
//    txt_notific_req.length[0] = 0;
//    txt_notific_req.length[1] = (12 + 5) + 1 + 2;;
//    txt_notific_req.pep.MsgHdr = 0x02;
//    txt_notific_req.pep.opcode[0] = 0xD0;
//    txt_notific_req.pep.opcode[1] = 0x10;
//    txt_notific_req.pep.num_of_bytes[0] = 1;
//    txt_notific_req.pep.num_of_bytes[1] = 0;
//    txt_notific_req.pep.pld = (quint8*) malloc(txt_notific_req.pep.num_of_bytes[0]);
//    txt_notific_req.pep.pld[0] = 1;
//    txt_notific_req.pep.Checksum = 0x00;
//    txt_notific_req.pep.MsgEnd = 0x03;

//    rtp_pld_req.header = 0x7E;
//    rtp_pld_req.version = 0x04;
//    rtp_pld_req.block = 0;
//    rtp_pld_req.opcode = DATA;
//    rtp_pld_req.source = 0x20;
//    rtp_pld_req.dest = 0x10;
//    rtp_pld_req.packet_num[0] = 0;
//    rtp_pld_req.packet_num[1] = 3;
//    rtp_pld_req.length[0] = 0;
//    rtp_pld_req.length[1] = (12 + 5) + 5 + 2;
//    rtp_pld_req.pep.MsgHdr = 0x02;
//    rtp_pld_req.pep.opcode[0] = 0x19;
//    rtp_pld_req.pep.opcode[1] = 0x04;
//    rtp_pld_req.pep.num_of_bytes[0] = 5;
//    rtp_pld_req.pep.num_of_bytes[1] = 0;
//    rtp_pld_req.pep.pld = (quint8*) malloc(rtp_pld_req.pep.num_of_bytes[0]);
//    rtp_pld_req.pep.pld[0] = 0x00; // uLaw
//    rtp_pld_req.pep.pld[0] = 0x00;
//    rtp_pld_req.pep.pld[0] = 0x00;
//    rtp_pld_req.pep.pld[0] = 0x00;
//    rtp_pld_req.pep.pld[0] = 0x00;
//    rtp_pld_req.pep.Checksum = 0x00;
//    rtp_pld_req.pep.MsgEnd = 0x03;


//    dig_audio_rx_req.header = 0x7E;
//    dig_audio_rx_req.version = 0x04;
//    dig_audio_rx_req.block = 0;
//    dig_audio_rx_req.opcode = DATA;
//    dig_audio_rx_req.source = 0x20;
//    dig_audio_rx_req.dest = 0x10;
//    dig_audio_rx_req.packet_num[0] = 0;
//    dig_audio_rx_req.packet_num[1] = 3;
//    dig_audio_rx_req.length[0] = 0;
//    dig_audio_rx_req.length[1] = (12 + 5) + 2 + 2;
//    dig_audio_rx_req.pep.MsgHdr = 0x02;
//    dig_audio_rx_req.pep.opcode[0] = 0xDF;
//    dig_audio_rx_req.pep.opcode[1] = 0x00;
//    dig_audio_rx_req.pep.num_of_bytes[0] = 2;
//    dig_audio_rx_req.pep.num_of_bytes[1] = 0;
//    dig_audio_rx_req.pep.pld = (quint8*) malloc(dig_audio_rx_req.pep.num_of_bytes[0]);
//    dig_audio_rx_req.pep.pld[0] = 0x01; //  rx
//    dig_audio_rx_req.pep.pld[1] = 0x01; //  from air
//    dig_audio_rx_req.pep.Checksum = 0x00;
//    dig_audio_rx_req.pep.MsgEnd = 0x03;

//    dig_audio_tx_req.header = 0x7E;
//    dig_audio_tx_req.version = 0x04;
//    dig_audio_tx_req.block = 0;
//    dig_audio_tx_req.opcode = DATA;
//    dig_audio_tx_req.source = 0x20;
//    dig_audio_tx_req.dest = 0x10;
//    dig_audio_tx_req.packet_num[0] = 0;
//    dig_audio_tx_req.packet_num[1] = 3;
//    dig_audio_tx_req.length[0] = 0;
//    dig_audio_tx_req.length[1] = (12 + 5) + 2 + 2;
//    dig_audio_tx_req.pep.MsgHdr = 0x02;
//    dig_audio_tx_req.pep.opcode[0] = 0xDF;
//    dig_audio_tx_req.pep.opcode[1] = 0x00;
//    dig_audio_tx_req.pep.num_of_bytes[0] = 2;
//    dig_audio_tx_req.pep.num_of_bytes[1] = 0;
//    dig_audio_tx_req.pep.pld = (quint8*) malloc(dig_audio_tx_req.pep.num_of_bytes[0]);
//    dig_audio_tx_req.pep.pld[0] = 0x00; //  tx
//    dig_audio_tx_req.pep.pld[1] = 0x01; //  from air
//    dig_audio_tx_req.pep.Checksum = 0x00;
//    dig_audio_tx_req.pep.MsgEnd = 0x03;

//    br_cast_req.header = 0x7E;
//    br_cast_req.version = 0x04;
//    br_cast_req.block = 0;
//    br_cast_req.opcode = DATA;
//    br_cast_req.source = 0x20;
//    br_cast_req.dest = 0x10;
//    br_cast_req.packet_num[0] = 0;
//    br_cast_req.packet_num[1] = 3;
//    br_cast_req.length[0] = 0;
//    br_cast_req.length[1] = (12 + 5) + 5 + 2;

//    br_cast_req.pep.MsgHdr = 0x02;
//    br_cast_req.pep.opcode[0] = 0xC9;
//    br_cast_req.pep.opcode[1] = 0x10;
//    br_cast_req.pep.num_of_bytes[0] = 5;
//    br_cast_req.pep.num_of_bytes[1] = 0;
//    br_cast_req.pep.pld = (quint8*) malloc(br_cast_req.pep.num_of_bytes[0]);
//    br_cast_req.pep.pld[0] = 0x02; // size
//    br_cast_req.pep.pld[1] = 0x00; // rx
//    br_cast_req.pep.pld[2] = 0x01; // notify
//    br_cast_req.pep.pld[3] = 0x01; // tx
//    br_cast_req.pep.pld[4] = 0x01; // notify
//    br_cast_req.pep.Checksum = 0x00;
//    br_cast_req.pep.MsgEnd = 0x03;

//    sTxtMsg_SimpMsg.header.msgType = 2;
//    sTxtMsg_SimpMsg.header.payloadLength[0] = 0;// not now
    /*
     * struct strMsgHeader header;//0-3
    quint8 TxtMsgType = 3; //4
    quint8 ReceiverId[4]; // 5-8
    quint8 SenderId[4];  // 9-12
    quint8 ReceiverType;// – 13 Тип получателя,
    bool IsAckRequired;// – 14 Требуется ли подтверждение,
    quint8 SequenceNumber;// 15 – Последовательный номер,
    quint8 Text[280];// –  16-... Текстовое сообщение
    */

//    call_req.header = 0x7E;
//    call_req.version = 0x04;
//    call_req.block = 0;
//    call_req.opcode = DATA;
//    call_req.source = 0x20;
//    call_req.dest = 0x10;
//    call_req.packet_num[0] = 0;
//    call_req.packet_num[1] = 3;
//    call_req.length[0] = 0;
//    call_req.length[1] = (12 + 5) + 5 + 2;
//    call_req.pep.MsgHdr = 0x02;
//    call_req.pep.opcode[0] = 0x41;
//    call_req.pep.opcode[1] = 0x08;
//    call_req.pep.num_of_bytes[0] = 5;
//    call_req.pep.num_of_bytes[1] = 0;
//    call_req.pep.pld = (quint8*) malloc(call_req.pep.num_of_bytes[0]);
//    call_req.pep.pld[0] = 0x00; // call type  0-priv 1-gr  2-all
//    call_req.pep.pld[1] = 0x00; // 1-0xffffffff radio id
//    call_req.pep.pld[2] = 0x00;
//    call_req.pep.pld[3] = 0x00;
//    call_req.pep.pld[4] = 0x00;
//    call_req.pep.Checksum = 0x00;
//    call_req.pep.MsgEnd = 0x03;

//    ptt_press_req.header = 0x7E;
//    ptt_press_req.version = 0x04;
//    ptt_press_req.block = 0;
//    ptt_press_req.opcode = DATA;
//    ptt_press_req.source = 0x20;
//    ptt_press_req.dest = 0x10;
//    ptt_press_req.packet_num[0] = 0;
//    ptt_press_req.packet_num[1] = 3;
//    ptt_press_req.length[0] = 0;
//    ptt_press_req.length[1] = (12 + 5) + 2 + 2;
//    ptt_press_req.pep.MsgHdr = 0x02;
//    ptt_press_req.pep.opcode[0] = 0x41;
//    ptt_press_req.pep.opcode[1] = 0x00;
//    ptt_press_req.pep.num_of_bytes[0] = 2;
//    ptt_press_req.pep.num_of_bytes[1] = 0;
//    ptt_press_req.pep.pld = (quint8*) malloc(ptt_press_req.pep.num_of_bytes[0]);
//    ptt_press_req.pep.pld[0] = 0x2B; // ptt key
//    ptt_press_req.pep.pld[1] = 1; // press
//    ptt_press_req.pep.Checksum = 0x00;
//    ptt_press_req.pep.MsgEnd = 0x03;

//    ptt_release_req.header = 0x7E;
//    ptt_release_req.version = 0x04;
//    ptt_release_req.block = 0;
//    ptt_release_req.opcode = DATA;
//    ptt_release_req.source = 0x20;
//    ptt_release_req.dest = 0x10;
//    ptt_release_req.packet_num[0] = 0;
//    ptt_release_req.packet_num[1] = 3;
//    ptt_release_req.length[0] = 0;
//    ptt_release_req.length[1] = (12 + 5) + 2 + 2;
//    ptt_release_req.pep.MsgHdr = 0x02;
//    ptt_release_req.pep.opcode[0] = 0x41;
//    ptt_release_req.pep.opcode[1] = 0x00;
//    ptt_release_req.pep.num_of_bytes[0] = 2;
//    ptt_release_req.pep.num_of_bytes[1] = 0;
//    ptt_release_req.pep.pld = (quint8*) malloc(ptt_release_req.pep.num_of_bytes[0]);
//    ptt_release_req.pep.pld[0] = 0x2B; // ptt key
//    ptt_release_req.pep.pld[1] = 0x00; // release
//    ptt_release_req.pep.Checksum = 0x00;
//    ptt_release_req.pep.MsgEnd = 0x03;

//    zone_ch.header = 0x7E;
//    zone_ch.version = 0x04;
//    zone_ch.block = 0;
//    zone_ch.opcode = DATA;
//    zone_ch.source = 0x20;
//    zone_ch.dest = 0x10;
//    zone_ch.packet_num[0] = 0;
//    zone_ch.packet_num[1] = 3;
//    zone_ch.length[0] = 0;
//    zone_ch.length[1] = 12 + 12;
//    zone_ch.pep.MsgHdr = 0x02;
//    zone_ch.pep.opcode[0] = 0xC4;
//    zone_ch.pep.opcode[1] = 0x00;
//    zone_ch.pep.num_of_bytes[0] = 5;
//    zone_ch.pep.num_of_bytes[1] = 0;
//    zone_ch.pep.pld = (quint8*) malloc(zone_ch.pep.num_of_bytes[0]);
//    zone_ch.pep.pld[0] = 0x01; //read message
//    zone_ch.pep.pld[1] = 0x01;
//    zone_ch.pep.pld[2] = 0x00;
//    zone_ch.pep.pld[3] = 0x01;
//    zone_ch.pep.pld[4] = 0x00;
//    zone_ch.pep.Checksum = 0x00;
//    zone_ch.pep.MsgEnd = 0x03;

//    remote_monitor.header = 0x7E;
//    remote_monitor.version = 0x04;
//    remote_monitor.block = 0;
//    remote_monitor.opcode = DATA;
//    remote_monitor.source = 0x20;
//    remote_monitor.dest = 0x10;
//    remote_monitor.packet_num[0] = 0;
//    remote_monitor.packet_num[1] = 3;
//    remote_monitor.length[0] = 0;
//    remote_monitor.length[1] = (12 + 5) + 4 + 2;
//    remote_monitor.pep.MsgHdr = 0x02;
//    remote_monitor.pep.opcode[0] = 0x34;
//    remote_monitor.pep.opcode[1] = 0x08;
//    remote_monitor.pep.num_of_bytes[0] = 4;
//    remote_monitor.pep.num_of_bytes[1] = 0;
//    remote_monitor.pep.pld = (quint8*) malloc(remote_monitor.pep.num_of_bytes[0]);
//    remote_monitor.pep.pld[0] = 0;// because 1-16776415
//    remote_monitor.pep.pld[1] = 0;
//    remote_monitor.pep.pld[2] = 0;
//    remote_monitor.pep.pld[3] = 0;
//    remote_monitor.pep.Checksum = 0x00;
//    remote_monitor.pep.MsgEnd = 0x03;

//    restart.header = 0x7E;
//    restart.version = 0x04;
//    restart.block = 0;
//    restart.opcode = DATA;
//    restart.source = 0x20;
//    restart.dest = 0x10;
//    restart.packet_num[0] = 0;
//    restart.packet_num[1] = 3;
//    restart.length[0] = 0;
//    restart.length[1] = (12 + 5) + 0 + 2;;
//    restart.pep.MsgHdr = 0x02;
//    restart.pep.opcode[0] = 0xC1;
//    restart.pep.opcode[1] = 0x01;
//    restart.pep.num_of_bytes[0] = 0;
//    restart.pep.num_of_bytes[1] = 0;
//    //restart.pep.pld = (quint8*) malloc(restart.pep.num_of_bytes[0]);
//    restart.pep.Checksum = 0x00;
//    restart.pep.MsgEnd = 0x03;

    sCallReport.header.msgType = 1;
    sCallReport.header.payloadLength[0] = 0;
    sCallReport.header.payloadLength[1] = 12;

    sCallReply.header.msgType = 1;
    sCallReply.header.payloadLength[0] = 0;
    sCallReply.header.payloadLength[1] = 7;
    sCallReply.type = eCallReplayMsg;
    sCallReply.none = 0x00;

//    sCallControl.typeMsg[0] = 0x04;
//    sCallControl.typeMsg[1] = 0x1E;
//    sCallControl.rmtPort[0] = sCallControl.rmtPort[1] = 0x00;
//    sCallControl.rmtAdressSize = 3;
//    sCallControl.groupIdSize = 3;
//    sCallControl.rmtAdressType = 0x01;

    sCallStopReply.header.msgType = 1;
    sCallStopReply.header.payloadLength[0] = 0;
    sCallStopReply.header.payloadLength[1] = 7;
    sCallStopReply.type = eStopCallReply;
    sCallStopReply.none = 0x00;

//    sChanChangeReport.header.msgType = 1;
//    sChanChangeReport.header.payloadLength[0] = 0;
//    sChanChangeReport.header.payloadLength[1] = 7;
//    sChanChangeReport.type = eChanChangeReportMsg;

//    sChanChangeReply.header.msgType = 1;
//    sChanChangeReply.header.payloadLength[0] = 0;
//    sChanChangeReply.header.payloadLength[1] = 7;
//    sChanChangeReply.type = eChanChangeReplyMsg;

//    sReloadReplay.header.msgType = 1;
//    sReloadReplay.header.payloadLength[0] = 0;
//    sReloadReplay.header.payloadLength[1] = 7;
//    sReloadReplay.type = eRldReplayMsg;
//    sReloadReplay.none = 0;

    sCtrlReply.header.msgType = 1;
    sCtrlReply.header.payloadLength[0] = 0;
    sCtrlReply.header.payloadLength[1] = 12; // add
    sCtrlReply.type = eSuControlReply;
    sCtrlReply.none = 0;
    //sCtrlReply.reqid[4];
    sCtrlReply.result = 0;
    //sCtrlReply.req_type = 0;
    //sCtrlReply.radio_id[4];
//    sRegMsgReport1.header.msgType = 1;
//    sRegMsgReport1.header.payloadLength[0] = 0;
//    sRegMsgReport1.header.payloadLength[1] = 26;

//    sRegMsgReport1.none = 0x00;
//    sRegMsgReport1.type = eRegReportMsg;
//    sRegMsgReport1.reg_unreg_state = UNREGISTRATE;


    sCallReport1.header.msgType = 1;
    sCallReport1.header.payloadLength[0] = 0;
    sCallReport1.header.payloadLength[1] = 12;
    sCallReport1.receivedId[0] = 0;
    sCallReport1.calledId[0] = 0;
    sCallReport1.type = eCallReportMsg;
    sCallReport1.none = 0x00;

    sCallReply1.header.msgType = 1;
    sCallReply1.header.payloadLength[0] = 0;
    sCallReply1.header.payloadLength[1] = 7;
    sCallReply1.type = eCallReplayMsg;
    sCallReply1.none = 0x00;

    sCallStopReply1.header.msgType = 1;
    sCallStopReply1.header.payloadLength[0] = 0;
    sCallStopReply1.header.payloadLength[1] = 7;
    sCallStopReply1.type = eStopCallReply;
    sCallStopReply1.none = 0x00;
}
