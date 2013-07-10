#ifndef __CUST_PANTECH_PROTOCOL_LINUX_H__
#define __CUST_PANTECH_PROTOCOL_LINUX_H__

/* ========================================================================
FILE: cust_pantech_protocol_linux.h

Copyright (c) 2010 by PANTECH Incorporated.  All Rights Reserved.

USE the format "FEATURE_VZW_CP_XXXX_XXXX"
=========================================================================== */ 

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---      ----------------------------------------------------------


===========================================================================*/

/*===========================================================================
FEATURE Naming Rule

-. Protocol(PS2 Team) Default : FEATURE_VZW_CP_XXXX_XXXX

-. Common Function or Interface : FEATURE_VZW_CP_COMMON_XXXX_XXXX

-. Each Parts

-. FEATURE_VZW_CP_1X_XXXX_XXXX

-. FEATURE_VZW_CP_EVDO_XXXX_XXXX

-. FEATURE_VZW_CP_UTS_XXXX_XXXX

-. FEATURE_VZW_CP_LBS_XXXX_XXXX

-. FEATURE_VZW_CP_LTE_XXXX_XXXX

-. FEATURE_VZW_CP_UICC_XXXX_XXXX

===========================================================================*/

/*===========================================================================
    Common Function or Interface Features - FEATURE_VZW_CP_COMMON_XXXX_XXXX
===========================================================================*/

/*===========================================================================
    System VOB Features
===========================================================================*/

/*
** When/Who : 20120308, kmk_protocol
** Summary: Set preferredSubscriptionMode (NV or UICC)  and preferredNetworkMode
**                 If this model support Global mode, set to NT_MODE_LTE_CMDA_EVDO_GSM_WCDMA
**                 If this model support CDMA mode, set to NT_MODE_LTE_CDMA_AND_EVDO
*/
#define FEATURE_VZW_CP_COMMON_SETTING_PHONE


/*===========================================================================
    Interface Features
===========================================================================*/

/*
** Summary: Cpmgr Interface.
*/
#define FEATURE_VZW_CP_COMMON_MGR_DAEMON_IF

/*
** Summary: 
*/
#define FEATURE_VZW_CP_COMMON_OEM_QMI_ACCESS

/*
** Summary: It is related with FEATURE_VZW_CP_COMMON_OEM_COMMANDS_WITH_QMI in Modem part.
*/
#define FEATURE_VZW_CP_COMMON_OEM_COMMANDS_WITH_QMI_LINUX

/*
** Summary: It is related with FEATURE_VZW_CP_COMMON_NVIO_WITH_QMI in Modem part.
  */ 
#define FEATURE_VZW_CP_COMMON_NVIO_WITH_QMI_LINUX

/*
** Summary: 
*/
#define FEATURE_VZW_CP_COMMON_TELEPHONY_IF

/*
** When/Who : 20110118, hksong
** Summary: Interface for Hidden Menu
*/
#define FEATURE_VZW_CP_COMMON_HIDDEN_CODE

/*
** When/Who : BKS_20111013
** Summary: Debug Screen FEATURE
*/
#define FEATURE_VZW_CP_COMMON_DEBUG_SCREEN

/*
** When/Who : 20120704, kim.jeongmin2 PS2-AJANTECH
** Summary: Change QMI_RIL_FW_ANDROID_REQUEST_HNDL_MAX_EVT_ID, 127 > RIL_REQUEST_PANTECH_EVENT_MAX
*/
#define FEATURE_VZW_CP_COMMON_RIL_EVENT_MAX

/*
** When/Who : 20120210 BANG KYOUNGSU
** Summary: 
*/
#define FEATURE_VZW_CP_COMMON_RSSI

//PS1 team Feature.
#define FEATURE_P_VZW_CP_GW_RSSI_LEVEL

/*
Info : 8960machine) evdo only network, There is no RSSI bar when EVDO only netwrok is attached by 8960 machine.

Related Feature : 
            
When/Who : 20130419_lhs_PS2
*/
#define FEATURE_VZW_CP_EVDO_ONLY_RSSI


/*
Info : SIM Lock, Emergency call orig, Roaming Indicator issue

Related Feature : 
            
When/Who : 20130420_lhs_PS2
*/
#define FEATURE_VZW_CP_NO_PRL_ROAM_INDICATOR

/*
Info : Network Operator Name error when LTE is attatched.

Related Feature : 
            
When/Who : 20130427_lhs_PS2
*/
#define FEATURE_VZW_CP_VZW_LTE_OPERATOR_NAME

/*===========================================================================
    1x Voice Features   - Start -
   ===========================================================================
     - FEATURE_VZW_CP_1X_XXXX_XXXX
     1. FEATURE_VZW_CP_1X_ERI_XXXX
     2. FEATURE_VZW_CP_1X_OTA_XXXX
     3. FEATURE_VZW_CP_1X_SA_XXXX
     4. FEATURE_VZW_CP_1X_WPS_XXXX
     5. FEATURE_VZW_CP_1X_E911_XXXX
     6. FEATURE_VZW_CP_1X_NBPCD_XXXX
     7. FEATURE_VZW_CP_1X_ETC_XXXX
===========================================================================*/

/*=======================================
   1. ERI - FEATURE_VZW_CP_1X_ERI_XXXX
========================================*/

/*
** Summary: 
*/
#define FEATURE_VZW_CP_1X_ERI_LINUX

/*
** Summary: 
*/
#undef FEATURE_P_VZW_CS_SID_INIT_SETTING_FOR_ERI

/*
** When/Who : 20130315, sungjin yoon
** Summary: Add more ERI HOME 65, 76~83
*/
#define FEATURE_VZW_CP_1X_ERI_ADD_MORE_ERI_HOME


/*=======================================
   2. OTA - FEATURE_VZW_CP_1X_OTA_XXXX
========================================*/


/*=======================================
   3. System Acquisition - FEATURE_VZW_CP_1X_SA_XXXX
========================================*/

/*
** Summary: Verizon System Selection Requirement
*/
#define FEATURE_VZW_CP_1X_SYSTEM_SELECTION


/*=======================================
   4. WPS - FEATURE_VZW_CP_1X_WPS_XXXX
========================================*/

/*
** When/Who : 20120511, kim.jeongmin2 PS2-AJANTECH
** Summary: VZW Wireless Priority Service
**                 It is related with FEATURE_VZW_CP_1X_WPS in Modem part.
*/
#define FEATURE_VZW_CP_1X_WPS_B


/*=======================================
   5. E911 - FEATURE_VZW_CP_1X_E911_XXXX
========================================*/

/*
** Summary: 4.1.4 Emergency Calling on Negative CDMA System in GSM Only Mode
**                 4.3.1 System Loss on GSM System while on Emergency Call in GSM Only Mode
**                 4.4.1 Emergency Calling through CDMA System in 'GSM Only' Mode
*/
//#define FEATURE_VZW_CP_1X_E911_CALL_IN_GSM_ONLY_MODE

/*
** When/Who : 20130315, sungjin yoon
** Summary: Enable E911 call origination in LPM mode after QXDM cmd "mode lpm" input
*/
#define FEATURE_VZW_CP_1X_E911_ENABLE_CALL_IN_LPM_MODE

/*
** Summary: implemented Emergency Call Number List
*/
#define FEATURE_VZW_CP_1X_E911_ECC_LIST


/*=======================================
   6. NBPCD - FEATURE_VZW_CP_1X_NBPCD_XXXX
========================================*/

/*
** Summary: NBPCD, HBPCD MCC,PCD, IDD Display in 1x Debug Screen (test code ##2773#)
**                 It is related with FEATURE_VZW_CP_1X_PLUS_CODE_DIALING in Modem part.
*/
#define FEATURE_VZW_CP_1X_PLUS_CODE_DIALING_LINUX

/*
** Summary: VZW NBPCD.. but this Req is removed from 2012.10 VZW Req
**                 So, In next model, should to be deleted.
**                 It is related with FEATURE_VZW_CP_1X_NBPCD in Modem part.
*/
#define FEATURE_VZW_CP_1X_NBPCD_LINUX


/*=======================================
   7. Test Menu, Debug Screen ,. etc
========================================*/

/*
** Summary: DB for 1x Debug Screen (##2773)
**                 It is related with FEATURE_VZW_CP_1X_LOCAL_DB in Modem part.
*/
#define FEATURE_VZW_CP_1X_LOCAL_DB_LINUX

/*
** Summary: DB Interface for 1x Debug Screen
**                 It is related with FEATURE_VZW_CP_1X_LOCAL_DB_WITH_QMI in Modem part.
*/
#define FEATURE_VZW_CP_1X_LOCAL_DB_WITH_QMI_LINUX

/*
** Summary: 1x Debug Screen (##2773)
*/
#define FEATURE_VZW_CP_1X_DEBUG_SCREEN

/*
** Summary: Engineer NAM Setting Menu (test code ##46632874#)
*/
#define FEATURE_VZW_CP_1X_ENG_NAM

/*
** Summary: P_Rev Setting Menu (test code ##8378#)
*/
#define FEATURE_VZW_CP_1X_P_REV_CHANGE_MENU

/*
** Summary: SW Test Setting Menu (test code ##2240#)
*/
#define FEATURE_VZW_CP_1X_SW_TEST_MENU

/*
** Summary: Test Call Service Option Setting Menu (test code ##2240#)
*/
#define FEATURE_VZW_CP_1X_TEST_CALL_MENU

/*
** Summary: A Key Setting Menu (test code ##46632874#)
**                (Eng Menu -> NAM Setting -> A-Key Input Mode)
*/
#define FEATURE_VZW_CP_1X_AKEY_MENU

/*
** When/Who : 20110330, drgnyp
** Summary: Support Network Mode Pref Setting in PST Tool
*/
#define FEATURE_VZW_CP_1X_PST_NET_PREF

/*
** When/Who : 20110127, drgnyp
** Summary: Preferred network mode TEST MENU UI
*/
#define FEATURE_VZW_CP_1X_NET_PREF_MENU

/*
** Summary: Lock Order Info display when received Lock Order from network.
**                 It is related with FEATURE_VZW_CP_1X_AUTH_REJ_DISPLAY in Modem Part.
*/
#define FEATURE_VZW_CP_1X_AUTH_REJ_DISPLAY_B

/*
** Summary: CDG2 Mode Setting Menu ( test code ##2342#)
**                 Open when CDG2 Test,  Blocked when release to VZW (SkyHiddenCode.java)
**                 It is related with FEATURE_VZW_CP_1X_SETTING_CDG2_MENU in Modem Part.
*/
#define FEATURE_VZW_CP_1X_CDG2_MENU_B

/*
** Summary: 6.2	AUTO CUSTOMER WARRANTY DATE CODE 
**                 It is related with FEATURE_VZW_CP_1X_WARRANTY_DATE_CODE in Modem Part.
*/
#define FEATURE_VZW_CP_1X_WARRANTY_DATE_CODE_B

/*
** Summary: MRU Clear Menu (test code ##678#)
**                 It is related with FEATURE_VZW_CP_1X_MRU_CLR_MENU in Modem Part.
*/
#define FEATURE_VZW_CP_1X_MRU_CLR_MENU_B

/*
** Summary: For NAM Selection
**                 It is related with FEATURE_VZW_CP_1X_NAM_SEL in Modem Part.
** P10597.. In smart phone, nam selection is not needed.
*/
#define FEATURE_VZW_CP_1X_NAM_SEL_B

/*
** When/Who : 20120628 BANG KYOUNG SU
** Summary: Hybrid Pref Mode Setting Menu In ##8378#
**                 It is related with FEATURE_VZW_CP_1X_SETTING_HYBRID_MODE_MENU in Modem Part.
*/
#define FEATURE_VZW_CP_1X_HYBRID_MODE_MENU_B

/*
** When/Who : 20130408 Lee Hyunsook
** Summary: Add System Time Info in 1x Debug Screen (##2773#)
**                 It is related with FEATURE_VZW_CP_1X_SYSTEM_TIME in Modem Part.
*/
#define FEATURE_VZW_CP_1X_SYSTEM_TIME_LINUX


/*=======================================
   8. Others
========================================*/

/* 
** Summary: added missing code in qcril_request_name[]
*/
#define FEATURE_VZW_CP_1X_ADD_QCRIL_LOG

/*
** Summary: 1x Keypad PST
*/
#define FEATURE_VZW_CP_1X_KEYPAD_PST

/*
** Summary: In Korea, time info diplayed in Wrong Country. so fixed.
*/
#define FEATURE_VZW_CP_1X_TIMEZONE_FIX

/*
** Summary: 
*/
#define FEATURE_VZW_CP_1X_SDM_CDMA

/*
** When/Who : 20110324, drgnyp
** Summary: Support Voice/Data RSSI, Current Network System, Roaming Status of SDM
*/
#define FEATURE_VZW_CP_1X_SDM_DIAG_MON

/*
** Summary: If Display info is Resctric or Unavailable, Copy and process Information.
**                 It is related with FEATURE_VZW_CP_1X_PROCESS_DISPLAY_INFO in Modem Part.
*/
#define FEATURE_VZW_CP_1X_PROCESS_DISPLAY_INFO_LINUX

/*
** When/Who : 20120330 kim.jeongmin2 PS2-AJANTECH
** Summary: 
*/
#define FEATURE_VZW_CP_1X_GET_NETWORK_TIME

/*
** Summary: Get PRL Version
*/
#define FEATURE_VZW_CP_1X_PRL_VERSION
#define FEATURE_VZW_CP_1X_PRL_VERSION_BUG_FIX

/*
** Summary: Phone Type GSM >> CDMA, Handle NITZ Event
*/
//P10597..Plz check.. Maybe this bug is fixed in Google (JB). So not porting
//CdmaLteServiceStateTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\cdma):
//CdmaServiceStateTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\cdma):
//GsmServiceStateTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\gsm):
//ServiceStateTracker.java (frameworks\base\telephony\java\com\android\internal\telephony): 
#define FEATURE_VZW_CP_1X_TIMEOFFSET_LINUX

/*
** When/Who : BKS_20120615
** Summary: pref mode selection pop-up in CDMA only or GSM only
*/
#define FEATURE_VZW_CP_1X_PREF_MODE_SEL_POPUP

/*
** When/Who : 20111121, kim.jeongmin2 PS2-AJANTECH
** Summary: WakeUp when LCD is On in power save mode
**                 It is related with FEATURE_VZW_CP_1X_WAKEUP_IN_PWR_SAVE in Modem part.
*/
#define FEATURE_VZW_CP_1X_WAKEUP_IN_PWR_SAVE_LINUX

/*
** When/Who : PS2-P13811
** Summary: After being EFS deleted, it doesn't make EqPRL well.
*/
#define FEATURE_P_VZW_CP_BLOCK_CDMA_SUB_SRC

/*
** 20130405, P10344 porting from Premia V ICS(FEATURE_P_VZW_ANDROID_USES_MEID_ME by kim.youngchai)
** Summary: 1. Add MEID_ME value to DeviceID
**                 2. In order to get currect ERUIMID from UICC, Framework should read it after RUIM recoreds are loaded.
**                 It is renamed from FEATURE_P_VZW_ANDROID_USES_MEID_ME to FEATURE_VZW_CP_1X_GET_MEID_ME
**                 It is related with FEATURE_VZW_CP_1X_GET_MEID_ME in Modem part.
*/
#define FEATURE_VZW_CP_1X_GET_MEID_ME_LINUX


/*===========================================================================
    EVDO Features - FEATURE_VZW_CP_EVDO_XXXX_XXXX    
===========================================================================*/

/*
** Summary: DB Interface for EVDO Debug Screen
**                 It is related with FEATURE_VZW_CP_EVDO_DB_QMI in Modem part.
*/
#define FEATURE_VZW_CP_EVDO_DB_QMI_LINUX

/*
** Summary: EVDO Debug Screen (##2773)
*/
#define FEATURE_VZW_CP_EVDO_DEBUG_SCREEN


/*===========================================================================
    UTS Features  - FEATURE_VZW_CP_UTS_XXXX_XXXX
===========================================================================*/
/*
20110216, GEE 
summary:
  -. UTS Feature �۾� 
*/
#define FEATURE_VZW_CP_UTS
#ifdef FEATURE_VZW_CP_UTS
/*
20110216, GEE 
summary:
  -. UTS verizon stage 2�� ���� FEATURE (verizon stage2 by C2k)
*/
#define FEATURE_VZW_CP_UTS_UDM_C2K

/*
20110321, GEE
summary:
  -. ANDROID MODEL�� W2Bi TEST������ FEATURE 
*/
#define FEATURE_VZW_CP_UTS_W2Bi_ANDROID
/*
20110324, GEE
summary:
  -. ANDROID MODEL��UTS EXTENDED COMMAND�� ���� FEATURE  
*/
#define FEATURE_VZW_CP_UTS_XCMD_ANDROID
#endif


/*===========================================================================
    LTE Features
===========================================================================*/


#ifdef FEATURE_VZW_CP_COMMON_DEBUG_SCREEN
#define FEATURE_P_VZW_CP_LTE_DEBUG_SCREEN_LINUX
#define FEATURE_P_VZW_CP_LTE_DB_QMI_LINUX
#endif /* FEATURE_VZW_CP_COMMON_DEBUG_SCREEN */

#if (0)

/*
info : t_mpsr, tbsr_cdma Timer R/W �����ϱ� ���Ͽ�
       �ش� sd cfg nv ���� android app���� R/W �����ϵ��� ����
when : 20110317
who : drgnyp
*/

/*
info : Ecrio �ַ��ǿ��� IMS PDN�� ���� ��� ����
       T3402�� get �� �� �ֵ��� ������
when : 20110524
who : drgnyp
*/
#define FEATURE_P_VZW_CP_GET_T3402_FOR_IMS

/*
info : Ecrio �ַ��ǿ��� IMS PDN�� ���� ��� ����
       PS attach/detach�� �� �� �ֵ��� ������
when : 20110525
who : drgnyp
*/

#define FEATURE_P_VZW_CP_PS_ATTACH_DETACH_FOR_IMS
#endif //#if (0)

#define FEATURE_P_VZW_CP_MM_OTADM_SYSTEM_SCAN_TIMER_LINUX

/*
** 20130313, ycjung
** Summary: origin code change from GLOBAL to Preferred mode (G/W/C/E/L)
*/
#define FEATURE_VZW_CP_LTE_PREFERRED_MODE_SET

/*
** 20130319, 
** Summary: Service State bug fix for LTE only
*/
#define FEATURE_VZW_CP_LTE_SRV_STATE_BUG_FIX


/*===========================================================================
    LBS Features
===========================================================================*/

/*
** When / Who  :
**    20110113, hksong
** Description :
**    To support GLL NMEA structure
** Files :
**    aries_gpsdiag.c
**    dmss7x30modem.mak - add GPSDIAG_LIB_OBJS
**	  modem_objects.min - modify  MODEM_OBJECTS to  MODEM_OBJECTS += $(CGPS_SM_OBJS)  $(CGPS_EXT_OBJS) $(CGPS_NV_OBJS) $(GPSDIAG_LIB_OBJS)
**	                    - add include $(CGPS)/diag/src/lib_gpsdiag.min in USE_CGPS field
*/
//#define FEATURE_VZW_CP_LBS_SUPPORT_NMEA_GPGLL

/*
** When / Who  :
**    20110118, hksong
** Description :
**    For GPS test menu. 
*/
#define FEATURE_VZW_CP_LBS_GPS_TEST_MENU
#ifdef FEATURE_VZW_CP_LBS_GPS_TEST_MENU
#define FEATURE_VZW_CP_LBS_GPS_TEST
#define FEATURE_VZW_CP_LBS_GPS_CNO_TEST
#endif

/*
** When / Who  :
**    20110119, hksong
** Description :
**    This feature is added to sync NV_GPS1_LOCK_I and Location Setting. 
**    Refer to LocationManagerService.java
**    Observe the change of Settings.Secure.LOCATION_PROVIDERS_ALLOWED and change NV value
*/
#define FEATURE_VZW_CP_LBS_GPS_LOCK

/*
** When / Who  :
**   - 20110125, hksong
** Description :
**   - Privacy setting value change to VZW model, that is related to GPS privacy setting. 
**     LOCATION_PROVIDERS_ALLOWED in framework is used to setting.
**   - TBD: ASSISTED_GPS_ENABLED if protected by setting.
**          GpsLocationProvider will be work standalone only.
**   - TBD: Decide the name of VzwGpsLocationProvider is "vzw_lbs" followed by VZW's spec. It can be changed.
**          CDMAPhone.java, SecuritySettings.java
*/
#define FEATURE_VZW_CP_LBS_PRIVACY_SETTING

/*
** When / Who  :
**   - 20110131, hksong
** Description :
**   - Give 200ms delay at update  Privacy Settings in Location Manager Service.
**     It's come from SKT model for system stable.
*/
#define FEATURE_VZW_CP_LBS_DELAY_UPDATING_PROVIDERS

/*
** When / Who  :
**   - 20110210, hksong
** Description :
**   - FOR APACHE : System folder property is changed to ro. So file path change to "data/gpsone_d".
**
*/
//#define FEATURE_VZW_CP_LBS_GPSONE_DMN_PATH_FIX

/*
** When / Who  :
**   - 20110308, hksong
** Description :
**   - For FOTA SDM : Make GPS Methods
**     Refer to 2010 september requirement
** File : 
**     \pantech\apps\OtadmExtensions\src\com\innopath\activecare\dev\oem\IdevDcMo.java
*/
#define FEATURE_VZW_CP_LBS_SDM

/*
** When / Who  :
**   - 20110125, hksong
** Description :
**   - Add NetworkLocationProvider and GeocodeProvider to Config.xml
**     If not the provider is not loaded at LocationManagerService.
*/
#define FEATURE_VZW_CP_LBS_LOADING_PROVIDER_BUG_FIX


/*
** When / Who  :
**   - 20110318, hksong
** Description :
**   - Added for LBS MMS Debug Screen
**     It depend on local db RPC
** Files :
**   - Cpmgrjniutil.cpp, Cpmgrnative.cpp, db.h, DBInterface.java, db_xdr.c, LbsScreen.java
*/
#define FEATURE_VZW_CP_LBS_DEBUG_SCREEN

/*
** When / Who  :
**   - 20110125, hksong
** Description :
**   - Added for LBS MMS Debug Screen
*/
#define FEATURE_VZW_CP_LBS_MMS_DEBUG_SCREEN

/*
** When / Who  :
**   - 20110125, hksong
** Description :
**   - WITS solution integration
**     Before make system.img connect the VZW GPS Provider and VZW Location Manager Service at build\core\Makefile.
**     Add vzwandroid in LOCAL_STATIC_JAVA_LIBRARIES framework\base\Android.mk
**     Add all source files to external\wits folder.
**     Chande the other files followed by WITS Location Framework Integration.
*/
#define FEATURE_VZW_CP_LBS_WITS_SOLUTION

/*
** When / Who  :
**   - 20110425, hksong
** Description :
**   - Solve the problem that parameter value is not transfer to VZW's Stack.
*/
#define FEATURE_VZW_CP_LBS_VZW_STACK_SET_PARAM

/*
** When / Who  :
**   - 20110425, hksong
** Description :
**   - Modify script file for start GPSONE_DAEMON.
**     It should be added to AGPS operation
**     Originaly init.qcom.sh, init.qcom.rc files are exist in system/core/rootdir/etc/ folder. But it doesn't work.
**     In init.qcom.sh
**       case "$target" in
**         "apache" | "ADR8995" | "msm7630_fusion")
**         start gpsone_daemon
**       esac
**     It should be added looks like "apache" | "ADR8995".
*/
//#define FEATURE_VZW_CP_LBS_GPSONE_DAEMON_START

/*
** When / Who  :
**   - 20110425, hksong
** Description :
**   - For the field test to display location fix information at call end time.
*/
#define FEATURE_VZW_CP_LBS_GPS_FIELD_POP_UP

/*
** When / Who  :
**   - 20110503, hksong
** Description :
**   - Change VzwGpsProvider State Machine
*/
#define FEATURE_VZW_CP_LBS_STATE_MACHINE

/*
** When / Who  :
**   - 20110503, hksong
** Description :
**   - GPSONE DAEMON message block
**     It should be added to user mode binary.
*/
//#define FEATURE_VZW_CP_LBS_BLOCK_GPSONE_DAEMON_MSG

/*
** When / Who  :
**   - 20110624, hksong
** Description :
**   - This feature is added to read PDE IP/PORT information in ##Program menu.
 */
#define FEATURE_VZW_CP_LBS_PST_PDE_READ

/*
** When / Who  :
**   - 20120412, AMJ
** Description :
**   - AFLT �׸� �׽�Ʈ �� MSA, Pref_time=0 ���� set ��Ŵ Cell id�� 3GPP�԰�
*/
#define FEATURE_VZW_CP_LBS_AFLT

/*
** When / Who  :
**   - 20120419, kim.jeongmin2 PS2-AJANTECH
** Description :
**   - PTLS �׽�Ʈ�� 1x ������ �� �� ���� �Ǵ� ������ ����, �ڵ�ȭ �׽�Ʈ�� ���� ����, 
**   - �߰����� �������� �׽�Ʈ ������ NTP, XTRA ���� ���� 
*/
#define FEATURE_VZW_CP_LBS_UTS_AUTO

/*
** When / Who  :
**   - 20120430, AMJ
** Description :
**   - LBS���� QMI load�� ���̱� ���� ���� 
 */
#define FEATURE_VZW_CP_LBS_REDUCE_QMI_LOAD


/*===========================================================================
    UIM Features
===========================================================================*/
#define FEATURE_P_VZW_UIM_CARD_DEFINITION
#ifdef FEATURE_P_VZW_UIM_CARD_DEFINITION
#define FEATURE_P_UICC_CARD_CUSTOM_INFO
#endif

/*
info :
when : 20111013
who  : hyko
*/
#define FEATURE_P_VZW_SUPPORT_FOR_ISIM_APPLICATION


#define FEATURE_QCRIL_UIM_QMI_APDU_ACCESS 

//#define FEATURE_P_VZW_ONCHIP_AVOID_PWR_DN
//#define FEATURE_P_VZW_APPLY_UICC_CARD_DEFINITION
//#ifdef FEATURE_P_VZW_APPLY_UICC_CARD_DEFINITION
//#define FEATURE_P_VZW_CPMGR_GET_CARD_TYPE_MODI
//#define FEATURE_P_VZW_SEND_PROVISION_STATUS_WITH_CARD_TYPE
//#endif


/*
info : Change error value from RIL_E_GENERIC_FAILURE 
       to RIL_E_MISSING_RESOURCE, 
          RIL_E_NO_SUCH_ELEMENT, 
          RIL_E_INVALID_PARAMETER
       or RIL_E_GENERIC_FAILURE 
when : 20120503
who  : JKA
*/
#define FEATURE_P_UICC_APDU_ACCESS_RESP_DETAIL


/*
info : Android resetes device when a card is removed.  (When Android reveived a card status message with RIL_CARDSTATE_ABSENT)
         To support this feature, send RIL_CARDSTATE_ABSENT only card is really removed.
when : 20120521
who  : kim.youngchai@pantech.com
*/
#define FEATURE_P_VZW_SUPPORT_RECYCLING_DEVICE_WHEN_CARD_IS_REMOVED


/*
info :     
when : 20120713
who  : kim.youngchai@pantech.com
*/
#define FEATURE_P_UICC_SUPPORT_RIL_REQUEST_STK_SEND_ENVELOPE_WITH_STATUS

/*
info : FEATURE_P_UICC_ACTIVATION_EVENT_RUIM_READY, not to skip getSubscriptionInfoAndStartPollingThreads(); during CSIM activation.
when : 20130226
who  : KWJ P16612
*/
#define FEATURE_P_UICC_ACTIVATION_EVENT_RUIM_READY
/*
info :     
when : 20121203
who  : 
*/
#define FEATURE_P_UICC_CARD_STATUS_PINPUK_RETRY_CNT

#define FEATURE_P_VZW_CP_UICC_BROADCAST_MDN_LOADED_EVENT
/* PS2 code for FEATURE_P_VZW_SUPPORT_FOR_ACTIVATION is remarked as FEATURE_P_VZW_CP_UICC_SUPPORT_FOR_ACTIVATION */
#define FEATURE_P_VZW_CP_UICC_SUPPORT_FOR_ACTIVATION
/*===========================================================================
    UIM END
===========================================================================*/

#endif/* __CUST_PANTECH_PROTOCOL_LINUX_H__ */

