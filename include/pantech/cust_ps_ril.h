#ifndef _CUST_PS_RIL_ 
#define _CUST_PS_RIL_

/***************************************************************************
                                 TARGET
****************************************************************************/
#define FEATURE_PS_VZW_SPECIFIC /* USA - VZW 3GPP */


/***************************************************************************
                                 COMMON
****************************************************************************/


/***************************************************************************
                                 AS
****************************************************************************/


/***************************************************************************
                                 MM
****************************************************************************/
/*
2012.11.28. kdhyun : Set initial RAT to requesting RAT in manual selection mode
                     - Eons.java, GSMPhone.java, NetworkSetting.java, OperatorInfo.java, qcrili.h, qcril_qmi_nas.c, RIL.java, ril_commands.h, SystemOperatorActivity.java
                     - GsmServiceStateTracker.java
*/
#define FEATURE_PS_MANUAL_SELECTION_RAT

/*
2012.04.26. cmjung : Q 1526 íŒ¨ì¹˜ í›„ ê°„í—ì ìœ¼ë¡œ No service ìƒíƒœë¡œ ë³€ê²½ë˜ëŠ” í˜„ìƒ ë° Manual selection ì‹œ registration ì•ˆë˜ëŠ” í˜„ìƒ ìˆ˜ì •
                    - modified file : ril.h, Qcril.c
*/
//#define FEATURE_SKY_CP_RIL_FW_ANDROID_REQUEST_HNDL_MAX_EVT_ID_FIX -> FEATURE_VZW_CP_COMMON_RIL_EVENT_MAX
#define FEATURE_SKY_CP_FW_DEDICATED_ANDROID_REQ_BUG_FIX

/*
2012.6.13. kdhyun : India¿¡¼­ MCC 404, 405°¡ È¥ÀçµÇ´Â °æ¿ì hard-codingµÈ operator nameÀ» Á¦´ë·Î ¸ø °¡Á®¿À´Â ¹®Á¦ ¼öÁ¤
                   - modified file : qcril_qmi_nas2.c
2012.7.7. kdhyun : Operator name added and changed for VZW requirements
*/
#ifdef FEATURE_PS_VZW_SPECIFIC
#define FEATURE_PS_HARDCODING_OPERATOR_NAME
#endif

/*
2012.6.29. cmjung : modified the network name  mismatch in notification after reject from network.
                    - modified file : GSMPhone.java, NotificationMgr.java
*/
#define FEATURE_PS_MATCH_NETWORK_NAME_IN_NOTIFICATION_AFTER_REJECT

/*
2012.7.4. kdhyun : UI ¸Þ´º¿¡¼­ network mode ÁøÀÔ½Ã prefer mode¸¸ °í·ÁÇÏ°í acq order´Â °í·ÁÇÏÁö ¾Êµµ·Ï ÇÔ (NETWORK_MODE_WCDMA_PREF(0)·Î ¼³Á¤µÇ¾î¾ß ÇÔ)
                   - modified file : qcril_qmi_nas2.c
2012.7.16. kdhyun : UI ¸Þ´º¿¡¼­ network mode¸¦ GlobalÀÌ³ª CDMA only·Î º¯°æ½Ã acq order pref´Â WCDMA pref(default)·Î º¯°æÇÏµµ·Ï ÇÔ
                    - modified file : qcril_qmi_nas2.c
*/
#ifdef FEATURE_PS_VZW_SPECIFIC
#define FEATURE_PS_GW_MODE_AND_W_PREF_BY_DEFAULT
#endif

/*
2012.7.6. cmjung : modified the 'switch to global mode' popup when G only, W only, GW only and W preferred.
                   - modified file : GsmServiceStateTracker.java, SwitchGlobalModeDialog.java
2012.7.21 hychoi : global popup is blocked when manual search is ongoing. (PLM issue)
                   - modified file : GsmServiceStateTracker.java, SwitchGlobalModeDialog.java,Networksetting.java
*/
#ifdef FEATURE_PS_VZW_SPECIFIC
#define FEATURE_PS_GW_PREF_MODE_SEL_POPUP
#endif

/*
2012.7.18. kdhyun : regard as Roaming when UE registers GW network.
                   - modified file : GsmServiceStateTracker.java
*/
#ifdef FEATURE_PS_VZW_SPECIFIC
#define FEATURE_PS_GW_ROAMING
#endif

/*
2012.07.30. hychoi /msseo/BJK : 
Managed Roamer Box <VZW-REQS-229-30>
When UE receive location update rej#17, UE will display "No service". 
GSM/UMTS System Selection <VZW-REQS-144-358>
The device shall only show it has service when it has been allowed to register on voice
                   - modified file : GsmServiceStateTracker.java, GsmDataconnectionTracker.java, Networkcontroller.java
*/
#ifdef FEATURE_PS_VZW_SPECIFIC
#define FEATURE_PS_DISPLAY_NOSVC_WHEN_PSONLY
#endif

// centralized_eons_supported ±â´É µ¿ÀÛ¿¡ µû¶ó network name ¹× manual search list Ç¥½Ã ¿À·ù°¡ ¹ß»ýÇÔ
#define FEATURE_SKY_CP_CENTRALIZED_EONS_NOT_SUPPORTED

/***************************************************************************
                                 CC
****************************************************************************/
/*ygkim 2012.1.3 AT&T Local CLIR
10776 & Local CLIR for AT&T ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½  get propertyï¿½ï¿½ ï¿½Ì¿ï¿½ï¿½Ø¼ï¿½ CLIR flag on/off ï¿½Ñ´ï¿½.
*/
#define FEATURE_GET_CLIR_LOCAL

/*EsLee 2012.1.3 Error Cause by FDN Enabled
Process USSD FDN Check Failure as Not Unsolicited res But OnRequestComplete
*/
#define FEATURE_PS_USSD_FDN_CHECK_ERROR_RELAY

/*hnlee 2012.1.3 Call end Reason 
 ï¿½ï¿½ï¿½ï¿½ :Add Call End raeson for Reject Indication --> function ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½. 
 ï¿½Ò½ï¿½ï¿½ï¿½ reasonï¿½ï¿½ ï¿½ï¿½ï¿½Ø¼ï¿½ï¿½ï¿½  ï¿½ï¿½ï¿½ï¿½ï¿½Ï¿ï¿½ Presto reason(UI call cause) ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½.
 */
#define FEATURE_ADD_CALL_END_REASON
#define FEATURE_PRESTO_ADD_ECC_FAIL_CAUSE

/* SUNI 20120215 OSCAR PLM 317
  After the call was ended at local, the state 'ACTIVE' is returned in result to get current call */
/* Eunseong, Juhyun 2012. 06.09, modified the side effect about  active state report after hangup  and multicall notend problem */
#define FEATURE_PS_CC_RETURN_NULL_DURING_CALL_DISCONNECTING

/* 20120330, Lee, Eunseong ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ SBA ï¿½Ý¿ï¿½, ï¿½ï¿½ï¿½ï¿½ regiï¿½ï¿½, 2nd MO callï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ rejectï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½, 
   causeï¿½ï¿½ï¿½ï¿½ validï¿½ï¿½ï¿½ï¿½ ï¿½Ê¾ï¿½ UIï¿½ï¿½ conference callï¿½ï¿½ ï¿½ß´ï¿½ ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ ï¿½Ö´Ù°ï¿½ ï¿½ï¿½, ï¿½Ì·ï¿½ ï¿½ï¿½ causeï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½Ô¼ï¿½ È£ï¿½ï¿½.*/
#define FEATURE_SKY_CP_LAST_CALL_FAILURE_FORCED_INIT

/*20120418 Lee, Eunseong
   USSD failure return*/
 #define FEATURE_PS_USSD_FAIL_RETURN_NW_NOT_RESPONSE
   
/* 20120426 Lee, Eunseong
    È¿ï¿½ï¿½: PS only cellï¿½ï¿½ï¿½ï¿½ CS doaminï¿½ï¿½ï¿½Â¸ï¿½ no service ï¿½ï¿½ ï¿½Ã¸ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½ ï¿½ï¿½ï¿½ï¿½
*/
#define  FEATURE_SKY_CP_SUPPPORT_PS_ONLY_MODE

/* 20120518 Lee, Eunseong
   block for DTMF pause, wait qcril flow control
*/
#define  FEATURE_PS_CC_BLCOK_DTMF_FLOW_CONTROL
/***************************************************************************
                                 Data
****************************************************************************/


/*
Info : 
  	<VZW-REQS-144-211>
	The user shall be informed if either HSUPA or HSDPA service is available with an icon. 

Related Feature :
            
Where : 

    config.xml (android\frameworks\base\packages\SystemUI\res\values)
            
When : 2013.03.05
Who  : Yoon.hyoseop


*/
#define FEATURE_P_VZW_DS_HSDPA_HSUPA_INDICATOR_ICON


/*
Info : 
        3GPP Legacy SDM Ã³¸®.

Related Feature :
            
Where : 
	idevConnMo.java (android\pantech\apps\otadmextensions\src\com\innopath\activecare\dev\oem)
	skyDataBroadcastReceiver.java (android\packages\apps\phone\src\com\android\phone)
            
When : 20120418
Who : Seungwan Hong
When : 2013.03.05
Who  : Yoon.hyoseop
*/

#define FEATURE_P_VZW_DATA_3GPPLEGACY_SDM

/* 
Info : 
	qcril_qmi_nas_request_data_registration_state() ÇÔ¼ö³»¿¡ ¾Æ·¡ ÄÚµå°¡ 
	no service ÁøÀÔÇß´Âµ¥µµ ºÒ±¸ÇÏ°í in service ·Î ¹Ù²î´Â ÀÌ½´ ¼öÁ¤
	Disable current data system

where : 
	Qcril_data_netctrl.c (vendor\qcom\proprietary\qcril\common\data)
	ds_qmi_wds.c (modem_proc\modem\datamodem\interface\qmidata\src) ADD EDGE	
When : 2013.03.05
Who  : Yoon.hyoseop
   
*/
#define FEATURE_PS_DISABLE_CURRENT_DATA_SYSTEM




/* 
Info : 
	Bigger packets to trigger Data Stall Alarm
	sent ÀÌÈÄ received°¡ ¾ø´Â °æ¿ì µ¥ÀÌÅÍ¸¦ ²÷´Âµ¥ ÇöÀç´Â sent byte°¡ 10 º¸´Ù Å¬ ¶§ ½ÇÇà
	NUMBER_SENT_PACKETS_OF_HANG = 10; ¿¡¼­ 60 À¸·Î º¯°æ
 
 where : 
	DataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony)
	
When : 2013.03.05
Who  : Yoon.hyoseop

***** 24 hour test was removed by VZW.
*/

// #define FEATURE_PS_SENT_PACKETS_OF_HANG


/***************************************************************************
                                 SIM
****************************************************************************/


/***************************************************************************
                                 SIM-WIFI
****************************************************************************/


#endif //CUST_PS_RIL_H

