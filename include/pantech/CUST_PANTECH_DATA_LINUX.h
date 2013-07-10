#ifndef __CUST_PANTECH_DATA_LINUX_H__
#define __CUST_PANTECH_DATA_LINUX_H__

/* ========================================================================
FILE: CUST_PANTECH_DATA_LINUX.h

Copyright (c) 2010 by PANTECH Incorporated.  All Rights Reserved.

USE the format "FEATURE_P_VZW_DS_XXXX"
=========================================================================== */ 

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---      ----------------------------------------------------------


===========================================================================*/

/*===========================================================================
Feature ó�� MANUAL

-. �� ���� ���� �� �ݵ�� file header�� history�� �����ϵ��� �Ѵ�.

-. system vob�� ���� ó���Ǵ� Feature�� System VOB Features section�� �д�.
   system vob�� ó���Ǵ� Feature�� FL�� �����Ͽ� �����ϵ��� �Ѵ�.

-. feature�̸��� FEATURE_P_VZW_DS_[name] ���� ó���Ѵ�.
   ������ section�� �ε��� �Ѵ�.

-. feature���� author, date, summary�� comment�Ѵ�.
   �� ������ feature specification�� ��ü�ϹǷ�
   summary�� feature�� ������ �ҽ��� ���� �ʴ��� feature�� ������ �밭 �ľ��� �� ���� ������ �ڼ��ϰ� ���´�.

-. JAVA source���� comment�� feature ó���� ������ �����.

-. feature�� ������ �Ʒ� ������ Feature �׷��� ������ ���� �ϵ��� �Ѵ�.

    1. Port Bridge & AT COMMAND Feature
    2. DUN & NDIS & RNDIS Feature 
    3. Hidden Code Feature
    4. Framework Feature
    5. Kernel & init.rc & VPN Feature
    6. SDM Feature

===========================================================================*/

/* 

1. Deprecated

1.1 Partial Retry
#define FEATURE_P_VZW_DS_SKIP_PARTIAL_FAILURE
#define FEATURE_P_VZW_DS_ACQUIRE_ONLY_IPV6_ONE_MORE_CHECK
#define FEATURE_P_VZW_DS_DUALIPPARTIALRETRY_CHANGE
#define FEATURE_P_VZW_DS_RECONNECT_DUALIPFAILURE_ON_IPV4
#define FEATURE_P_VZW_DS_CALLLIST_UPDATE

1.2 checkAndUpdatePartialProtocolFailure() �Լ� ����
#define FEATURE_P_VZW_DS_DATACONNECTION_NULL_POINTER

1.3 PremiaV �� �ƹ��� ������ ����
#define FEATURE_P_VZW_DS_DNS1_CHANGED

1.4 Rat Change ���� ������ JB���� ����
#define FEATURE_P_VZW_DS_QCOM_153613_BUG_FIX
#define FEATURE_P_VZW_DS_RAT_CHANGE_GSMSST

1.5 clearSettings() �Լ��� resetRetryCount() �Լ� ȣ�� �κ��� ���ŵǾ� ����
#define FEATURE_P_VZW_DS_RETRY_TIMER_RESET_BUG_FIX

1.6 onRatChanged() �Լ��� ���⿡ Rat changed ���� ���������� ���� ���� ����.
#define FEATURE_P_VZW_DS_IMS_RECONNECT_IN_EHRPD

2. Verify

2.1 NativeDaemonConnector.java �������� ���Ͽ� Ȯ�� �� ���� ���� ����.
#define FEATURE_P_VZW_DS_NETD_WATCHDOG_FIX

2.2 ���� �ʵ� �׽�Ʈ���� Data connection issue �� �߻����� ��� ���� ���� ����.
#define FEATURE_P_VZW_DS_DATA_RECOVERY_FOR_SETUP_DATA_TIMEOUT_HDLR
#define FEATURE_P_VZW_DS_DATA_RECOVERY_FOR_SETUP_DATA_TIMEOUT_HDLR_MODEM_WAIT_CHECK
#define FEATURE_P_VZW_DS_DATA_RECOVERY_FOR_SETUP_DATA_TIMEOUT_HDLR_ONLY_COUNT
#define FEATURE_P_VZW_DS_DATA_RECOVERY_FOR_BLOCKING_DUE_TO_PENDING
#define FEATURE_P_VZW_DS_DATA_RECOVERY_FOR_BOTH_ADDR_ALREADY_UP

3. Check

3.1 2013.03 VZW requirement �������� undef
#undef FEATURE_P_VZW_DS_LTE_MULTIPLE_APN_EMERGENCYAPN

3.2 Bluebird ���� ����� Requirement�� Premia V JB ���� �������� Ȯ�� �ʿ�
#define FEATURE_P_VZW_DS_GLOBAL_DATA_ROAMING
*/

/*===========================================================================
    Interface Features
===========================================================================*/

//BLUEBIRD
#undef FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED


/*===========================================================================
    Data Service Features
===========================================================================*/
/*
//20120119_yunsik_DATA
CUST_PANTECH_DATA_LINUX.h ������ ������ �ϵ��� CUST_PANTECH.h�� include ��.
*/
#define FEATURE_P_VZW_DS_CUST_INCLUDE

/*
//20111227_yunsik_DATA
- system\core\liblog\Logd_write.c  : CDMA, GSM tag - RADIO to MAIN
//20120207_yunsik_DATA
- LINUX\android\bionic\libc\netbsd\resolv\res_cache.c ���� DNS�� ���� ���� ��� Ǯ����. ���� ��.
*/
#define FEATURE_P_VZW_DS_ANDROID_LOG

/*
//20130401_yunsik_DATA
MUST BE REMOVE at P1 Version.
*/
#define FEATURE_P_VZW_DS_ANDROID_LOG_RIL_TO_GSM


/*
//20130121_yunsik_DATA, please define this feature for IOT/CDG2

Data enabled set to false, Data roaming set to true
- DatabaseHelper.java (frameworks\base\packages\settingsprovider\src\com\android\providers\settings)
- Full_base_telephony.mk (build\target\product)

tethering problem fix
- persist.pantech.dsProv.skip
*/
//#define FEATURE_P_VZW_DS_LTE_IOT

/*==========================================================================
    1. Port Bridge & AT COMMAND Feature
===========================================================================*/

/*
//20120229_yunsik_DATA, ICS User Build ���� DUN�� �Ǵ� ������ Ȯ�� �Ǿ� �Ʒ� ���� ���� ��

//20111107_yunsik_DATA, feature name rename define, comment ����
<Cheetah : Apache���� ����>
 - Android.mk (frameworks\base) : ISkyDun.aidl �߰�
 - AndroidManifest.xml (packages\apps\phone) : SkyDunService �߰�
 - AndroidManifest.xml (vendor\qcom\proprietary\qualcommsettings) : DunService block
 - Dun_Autoboot.java (vendor\qcom\proprietary\qualcommsettings\src\com\android\qualcommsettings) : DunService block
 - SkyDataBroadcastReceiver.java (packages\apps\phone\src\com\android\phone) : ���� (SkyDunService start)
 - SkyDunService.java (packages\apps\phone\src\com\android\phone) : ����
 - SkyTelephonyInterfaceManager.java (frameworks\base\telephony\java\com\android\internal\telephony\cdma) 

  20110121, swchoi
  summary:
    - EF14L�� DS1_DS_DUN_USER_MODE�� APACHE�� ����
 - ������ : QualcommSetting ���丮�� �ִ� ���ϵ��� user mode �� ������� �����Ƿ�, dun, sync manager, data manger, curi explore�� ������� ����.
#define FEATURE_P_VZW_DS_DUN_USER_MODE
*/


/*==========================================================================
    2. DUN & NDIS & RNDIS Feature 
===========================================================================*/

/*
//20111107_yunsik_DATA, feature name rename define
<Cheetah : Apache���� ���� and modified>
 - AndroidManifest.xml (packages\apps\phone)
 - ISkyDun.aidl (frameworks\base\telephony\java\com\android\internal\telephony)
 - DataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony)
 - SkyDataBroadcastReceiver.java (packages\apps\phone\src\com\android\phone)
 - SkyDataIntents.java (frameworks\base\telephony\java\com\android\internal\telephony) : �߰�
 - Strings_cp.xml (packages\apps\phone\res\values)
 - dun_service.png (packages\apps\Phone\res\drawable) : �߰�  

  summary:
  -. DUN ���ӽ� notification �� DUN ��������� ǥ��
  -. DUN init, end �� Toast Msg ���

  SkyDunService.java (E-APACHE_MSM_3035\LINUX\android\packages\apps\Phone\src\com\android\phone)
  strings_cp.xml (E-APACHE_MSM_3035\LINUX\android\packages\apps\Phone\res\values)
  dun_service.png (E-APACHE_MSM_3035\LINUX\android\packages\apps\Phone\res\drawable) : �߰�  

//20130104 : file summary
 - AndroidManifest.xml (linux\android\packages\apps\phone)
 - DataConnectionTracker.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
 - SkyDataBroadcastReceiver.java (linux\android\packages\apps\phone\src\com\android\phone)
 - SkyDataIntents.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
 - Strings_cp.xml (linux\android\packages\apps\phone\res\values)
 - dun_service.png (linux\android\packages\apps\Phone\res\drawable)
*/
#define FEATURE_P_VZW_DS_DUN_UI

/*
Info : 
            Google Fix the bug for the Tethering (USB Tethering, BT Tethering, MHS) FAIL by the Interface Index Excess when Tethering ON/OFF Repeat
              - on Tethering Server Device : shown "Not Tethered" due to NAT's SocketException, even though Tethering Connection Complete
              - on Tethering Client Device : Tethering Data FAIL, even though Tethering Connection Complete
              - https://android.googlesource.com/platform/libcore/+/b0e6dc5464f959b3d42f37b32f4b01767f6fe506
            
Related Feature :
            
Where : 
            CUST_PANTECH_DATA_LINUX.h (linux\android\pantech\include)
            NetworkInterface.java (linux\android\libcore\luni\src\main\java\java\net)
            
When : 20130515
Who : Ju.Hojin
*/
#define FEATURE_P_VZW_DS_GOOGLE_PATCH_SOCKEXCEPTION_BY_IFACE_INDEX_EXCESS_WHEN_TETHERING_ONOFF_REPEAT


/*==========================================================================
    3. Hidden Code Feature
===========================================================================*/

/*
//20111102_yunsik_DATA
Related feature is FEATURE_VZW_CP_COMMON_HIDDEN_CODE
  -> SkyHiddenCode.java

DATA Hidden code
- SkyDataBroadcastReceiver.java create (packages\apps\Phone\src\com\android\phone)
- SkyDataIntents.java create (frameworks\base\telephony\java\com\android\internal\telephony)
- SpecialCharSequenceMgr.java (packages\apps\Phone\src\com\android\phone)
*/
#define FEATURE_P_VZW_DS_HIDDEN_CODE

/*
//20120907_yunsik_DATA
MUST BLOCK for P1 version

- SkyDataBroadcastReceiver.java (packages\apps\phone\src\com\android\phone)
- SkyHiddenCode.java (frameworks\base\telephony\java\com\android\internal\telephony)
*/
#define FEATURE_P_VZW_DS_HIDDEN_CODE_BLOCK

/*
//20120301_yunsik_DATA
- Test enable test code (##*328236*#), only NV access

- AndroidManifest.xml (packages\apps\phone)
- PstDataApnSettings.java (pantech\frameworks\sky_pst\services\com\pantech\service)
- SkyDataBroadcastReceiver.java (packages\apps\phone\src\com\android\phone)
- SkyHiddenCode.java (frameworks\base\telephony\java\com\android\internal\telephony)
*/
#define FEATURE_P_VZW_DS_TEST_ENABLE_TESTCODE

/*
//20120301_yunsik_DATA
- DUN enable/disable code (##*36386*#)
- When P1 submission, MUST default value to false. (SkyDataBroadcastReceiver.java, Dun_service.c)

- AndroidManifest.xml (packages\apps\phone)
- Dun_service.c (vendor\qcom\proprietary\data\port-bridge)
- PstDataApnSettings.java (pantech\frameworks\sky_pst\services\com\pantech\service)
- SkyDataBroadcastReceiver.java (packages\apps\phone\src\com\android\phone)
- SkyHiddenCode.java (frameworks\base\telephony\java\com\android\internal\telephony)
*/
#define FEATURE_P_VZW_DS_ENABLE_DUN_LINUX

#if 0 //def FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED
/*
*  2012.05.22 P11546 Add DMLogging Hidden Menu
*    
*    LINUX/android/package/apps/Contacts/src/com/android/Contacts/SpecialCharSequenceMgr.java
*    LINUX/android/package/apps/DMLogging/AndroidManifest.xml
*    LINUX/android/package/apps/DMLogging/src/com/android/dmlogging/DMLoggingHiddenMenuBroadcastReceiver.java (Added)
*    LINUX/android/package/apps/Phone/src/com/android/Phone/SpecialCharSequenceMgr.java
*    LINUX/android/pantech/apps/Dialer/src/com/android/contacts/PCUSpecialCharSequenceMgr.java
*    LINUX/android/vendor/qcom/proprietary/dm-monitor/etc/Android.mk
*    LINUX/android/vendor/qcom/proprietary/dm-monitor/qdminterface/Android.mk
*
*  2012.06.11 P11546 Remove hidden code for DMLogging due to VZW QC issue
*    - Delete FEATURE_P_VZW_DS_DMLOGGING_HIDDEN_MENU in below files
*
*    LINUX/android/package/apps/Contacts/src/com/android/Contacts/SpecialCharSequenceMgr.java
*    LINUX/android/package/apps/DMLogging/AndroidManifest.xml
*    LINUX/android/package/apps/DMLogging/src/com/android/dmlogging/DMLoggingHiddenMenuBroadcastReceiver.java (Added)
*    LINUX/android/package/apps/Phone/src/com/android/Phone/SpecialCharSequenceMgr.java
*    LINUX/android/pantech/apps/Dialer/src/com/android/contacts/PCUSpecialCharSequenceMgr.java    
*/
#define FEATURE_P_VZW_DS_DMLOGGING_HIDDEN_MENU


//from PremiaV
/*
*  2012.05.22 P11546 Add DMLogging Hidden Menu
*    
*    LINUX/android/package/apps/Contacts/src/com/android/Contacts/SpecialCharSequenceMgr.java
*    LINUX/android/package/apps/Phone/src/com/android/Phone/SpecialCharSequenceMgr.java
*    LINUX/android/pantech/apps/Dialer/src/com/android/contacts/PCUSpecialCharSequenceMgr.java
*    LINUX\android\vendor\qcom\proprietary\dm-monitor\DMLogging\Android.mk
*    LINUX\android\vendor\qcom\proprietary\dm-monitor\DMLogging\AndroidManifest.xml
* 
*  2012.06.29 P11546 Remove Hidden code for DMLogging
*/
//#define FEATURE_DMLOGGING_HIDDEN_MENU


/*
*  2012.06.25 P11546 Disable DM log compression temporarily
* 
*    LINUX\android\vendor\qcom\proprietary\dm-monitor\diag\diag.c
*    LINUX\android\vendor\qcom\proprietary\dm-monitor\dmmond\adblog_core.c
*/
#define FEATURE_P_VZW_DS_DMLOGGING_MENU
#endif /* FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED */

/*
//20120613_yunsik_DATA
- DNS block test code (##*367999*#, ##*DNSXXX*#), dormant maintains for test equipment.
- Yunsik wanted to modify to InetAddress.java, but System.getProperty fuction does not work in InetAddress.java

- AndroidManifest.xml (packages\apps\phone)
- Cust_pantech_linux_data.h (pantech\include)

- SkyDataBroadcastReceiver.java (packages\apps\phone\src\com\android\phone)
- SkyHiddenCode.java (frameworks\base\telephony\java\com\android\internal\telephony)

//20130418_yunsik_DATA, 
code changed from open source(getaddrinfo) to framework(ConnectivityService)
 - code deleted : Getaddrinfo.c (bionic\libc\netbsd\net)
 - code modified :  ConnectivityService.java (frameworks\base\services\java\com\android\server)
*/
#define FEATURE_P_VZW_DS_DNS_BLOCK_TESTCODE

/*
//20120820_yunsik_DATA, test code for recovery

- AndroidManifest.xml (packages\apps\phone)
- ISkyDataConnection.aidl (frameworks\base\telephony\java\com\android\internal\telephony)
- RecoveryModeSettingMenu.java (packages\apps\phone\src\com\android\phone)
- Recovery_mode_setting.xml (packages\apps\phone\res\xml)
- SkyDataBroadcastReceiver.java (packages\apps\phone\src\com\android\phone)
- SkyDataConInterfaceManager.java (frameworks\base\telephony\java\com\android\internal\telephony)
- SkyHiddenCode.java (frameworks\base\telephony\java\com\android\internal\telephony)
- Strings_cp.xml (packages\apps\phone\res\values)

*/
#define FEATURE_P_VZW_DS_RECOVERY_MODE_TESTCODE

/*
//20130412_yunsik_DATA
IRAT Test mode. (##*4728*#) ##*IRAT*#

1. ON : DNS block & NV_SPARE_1_I (107) set to one (UE initiated session nego) & VZWAPP disable
2. OFF : DNS unblock & NV_SPARE_1_I (107) set to zero & VZWAPP enable

NV_SPARE_1_I related to NV_HDRSCP_SESSION_STATUS_I.

Related Feature : FEATURE_VZW_CP_EVDO_SESSION_CLOSE_SETTING in modem side
                            Info : To reduce MPSR of IRAT Test, able to do Session Nego whenever boot up. 
                                      NV_SPARE1_I (107) = 0 (default), 1 (every time session nego)

- AndroidManifest.xml (packages\apps\phone)
- SkyDataBroadcastReceiver.java (packages\apps\phone\src\com\android\phone)
- SkyHiddenCode.java (frameworks\base\skytelephony\common\java\com\android\internal\telephony)
*/
#define FEATURE_P_VZW_DS_IRAT_TESTCODE

/*==========================================================================
    4. Framework Feature
===========================================================================*/

/*
  20110307, swchoi
  summary:
    - ef30 FEATURE_SKY_DS_ADD_DATA_AIDL ����.
    - Phone Interface�� ���� ���ϴ� APP�� ���Ͽ� AIDL�� �߰�    
    - aidl ���� : ISkyDataConnection.aidl
    - �������̽� ���� : MMDataConnectionTracker.java
    - �߰� ���� ���� : CDMAPhone.java
    - aidl ���� �߰� : service_manager�� .aidl �߰�
    - make ���� ���� : android/framework/base/Android.mk ����
	
   Android.mk (LINUX\android\frameworks\base)
   ISkyDataConnection.aidl (LINUX\android\frameworks\base\telephony\java\com\android\internal\telephony) �����߰�
   MMDataConnectionTracker.java (LINUX\android\frameworks\base\telephony\java\com\android\internal\telephony)
   service_manager.c (LINUX\android\frameworks\base\cmds\servicemanager)
   SkyDataConInterfaceManager.java (LINUX\android\frameworks\base\telephony\java\com\android\internal\telephony) �����߰�	

  20121026_kns
  summary:
    - merged from PREMIA_V
    - change to DataConnectionTracker.java from MMDataConnectionTracker.java
*/
#define FEATURE_P_VZW_DS_ADD_DATA_AIDL

/*
  20110307, swchoi
  summary:
    - ef30 DS1_DS_DISCONNECT_ALL_NETWORK_FEATURE ����.
    - startUsingNetwrokFeature�� ����� ��� APP�� ���Ͽ� stopUsingNetworkFeature�� ȣ���� �Ŀ�
      3g Data Disable �ϵ��� �߰���.

   ConnectivityManager.java (E-APACHE_MSM_3035\LINUX\android\frameworks\base\core\java\android\net)
   ConnectivityService.java (E-APACHE_MSM_3035\LINUX\android\frameworks\base\services\java\com\android\server)
   IConnectivityManager.aidl (E-APACHE_MSM_3035\LINUX\android\frameworks\base\core\java\android\net)
   MMDataConnectionTracker.java (LINUX\android\frameworks\base\telephony\java\com\android\internal\telephony)

  20121026_kns
  summary:
    - merged from PREMIA_V
    - change to DataConnectionTracker.java from MMDataConnectionTracker.java
*/
#define FEATURE_P_VZW_DS_DISCONNECT_ALL_NETWORK_FEATURE

/*
//20111115_yunsik_DATA, for Cheetah modified (from apache)
  20110307, swchoi
  summary:
   - ef30 DS1_DS_PS_DISABLE_FOR_SKT , DS1_DS_EASY_SETTING ����.
   - 3g Data Enable/Disable �Լ� �߰�     
   - Easy Setting�� 3G Data�� Mobile network setting���� Data enabled���� sync�۾�
   - Easy Setting���� 3G Data ���� ���� �� Settings.java�� ���氪 ��� �ݿ�

 - ConnectivityService.java (frameworks\base\services\java\com\android\server)
 - EasySetting.java (frameworks\base\packages\systemui\src\com\android\systemui\statusbar)
 - ISkyDataConnection.aidl (frameworks\base\telephony\java\com\android\internal\telephony)
 - MMDataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony)
 - Settings.java (packages\apps\phone\src\com\android\phone)
 - SkyDataConInterfaceManager.java (frameworks\base\telephony\java\com\android\internal\telephony)

//20130104_kns: file summary
 - ConnectivityService.java (linux\android\frameworks\base\services\java\com\android\server)
 - DataConnectionTracker.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
 - ISkyDataConnection.aidl (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
 - SkyDataConInterfaceManager.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
 - SkyDataIntents.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
*/
 #define FEATURE_P_VZW_DS_PS_DISABLE

/*
//Multiple APN

- Apns-conf.xml (development\data\etc)
- Apns-conf.xml (device\generic\goldfish\data\etc)
- Apns.xml (frameworks\base\core\res\res\xml)
- ConnectivityService.java (frameworks\base\services\java\com\android\server)
- MobileDataStateTracker.java (frameworks\base\core\java\android\net)
- Phone.java (frameworks\base\telephony\java\com\android\internal\telephony)
*/
#define FEATURE_P_VZW_DS_LTE_MULTIPLE_APN

/*
//20120127 P11546
- config.xml (frameworks\base\core\res\res\values)
- ConnectivityManager.java
- ConnectivityService.java
- Phone.java
- MobileDataStateTracker.java
- DataConnectionTracker.java
- GsmDataConnectionTracker.java
- RILConstants.java
- ServiceTypeListActivity.java

//20120116_yunsik_DATA
- 800 APN added
- apns.xml 
- apns-conf.xml (\LINUX\android\development\data\etc)

//20120116_yunsik_DATA
- apns-conf.xml (LINUX\android\device\generic\goldfish\data\etc\apns-conf.xml)

//20121026_kns
 - LanguageSelection.java (\pantech\apps\vzwsetupwizard\src\com\pantech\app\vzwsetupwizard)
 - NetworkController.java (\frameworks\base\packages\systemui\src\com\android\systemui\statusbar\policy)
 - VzwConnectivityService.java (\frameworks\base\services\java_pantech\vzw\com\verizon\server)
*/
#define FEATURE_P_VZW_DS_LTE_MULTIPLE_APN_800APN

// TODO:Check
/*
//20121120_kns
- EMERGENCY APN added
    1. emergency PDN�� LTE(No roaming)���� �ش�Ǹ�, No VoLTE device�� disable,
      - No roaming ó�� : MobileDataStateTracker.setIsNetworkAvailable()
      - No VoLTE device ó�� : apns.xml
      - LTE Only ó�� : ??
      
- Apns-conf.xml (msm8930_jb\linux\android\development\data\etc)
- Apns-conf.xml (msm8930_jb\linux\android\device\generic\goldfish\data\etc)
- Apns.xml (msm8930_jb\linux\android\frameworks\base\core\res\res\xml)
- Config.xml (msm8930_jb\linux\android\frameworks\base\core\res\res\values)
- ConnectivityManager.java (msm8930_jb\linux\android\frameworks\base\core\java\android\net)
- ConnectivityService.java (msm8930_jb\linux\android\frameworks\base\services\java\com\android\server)
- CUST_PANTECH_DATA_LINUX.h (msm8930_jb\linux\android\pantech\include)
- DataConnectionTracker.java (msm8930_jb\linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
- GsmDataConnectionTracker.java (msm8930_jb\linux\android\frameworks\base\telephony\java\com\android\internal\telephony\gsm)
- LanguageSelection.java (msm8930_jb\linux\android\pantech\apps\vzwsetupwizard\src\com\pantech\app\vzwsetupwizard)
- MobileDataStateTracker.java (msm8930_jb\linux\android\frameworks\base\core\java\android\net)
- Phone.java (msm8930_jb\linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
- RILConstants.java (msm8930_jb\linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
- VzwConnectivityService.java (msm8930_jb\linux\android\frameworks\base\services\java_pantech\vzw\com\verizon\server)

//20130405_kwak.nosob
- undefined
Info : 
            VZW Reqs-LTE_DataDevices(2013.03)
            - 6.2.12.3	PDN CONNECTIVITY REQUEST MESSAGE FOR THE EMERGENCY PDN
               Per 3GPP TS 24.301: Non-Access-Stratum (NAS) protocol for Evolved Packet 
               System (EPS); Stage 3, when making a PDN connection request for the Emergency PDN, 
               VoLTE-capable devices shall set the following information elements in the PDN 
               CONNECTIVITY REQUEST message as described below:
                    * The device shall set the "Request Type" to Emergency.
                    * The device shall not include the APN. 
*/
#undef FEATURE_P_VZW_DS_LTE_MULTIPLE_APN_EMERGENCYAPN

/*
//20120110_yunsik_DATA, LINUX\android\packages\apps\VerizonPermissonsVZWapnPermission �߰�
//20120110_yunsik_DATA, Config.xml (frameworks\base\core\res\res\values) ���� config_protectedNetworks ����
//201104021_ckhong
- multiple apn requirement sing. & meta data check :: vzw-reqs-117-37
- vzw app apn ���ó���.  connectivity service function ���� net type hipri �� ��û�ϴ� ��� meta-data, sign ��� vzw app apn ����ó�� 
- ConnectivityService.java (E-APACHE_MSM_3035\LINUX\android\frameworks\base\services\java\com\android\server)
*/
#define FEATURE_P_VZW_DS_PS_VZW_APP_APN_REQ
#define FEATURE_P_VZW_DS_PS_VZW_APP_APN_REQ_NEW_IMPLEMENT //20120510_yunsik_DATA

/*
SUPL permission check
- ConnectivityService.java (\LINUX\android\frameworks\base\services\java\com\android\server)
*/
#define FEATURE_P_VZW_DS_ADD_SUPL_FEATURE

/*
//201104021_ckhong
- vzw apn �� ��� inactivity timer 1439 min. ���� ����
- ConnectivityService.java (\LINUX\android\frameworks\base\services\java\com\android\server)
//20120502_yunsik_DATA,//20120525_yunsik_DATA, HIPRI 30mins blocked because of removing of ECID app
- HIPRI added getRestoreHipriNetworkDelay(). 30minutes. 

//20121122_yunsik_DATA, removed IMS, changed to -1 (unlimited), QC issue (Pan_Int_60613)
 - Config.xml (frameworks\base\core\res\res\values)
*/
#define FEATURE_P_VZW_DS_PS_INACTIVITY_TIMER

/*
//20120116_yunsik_DATA
- eHRPD Internet PDN IPv6 ignore.
*/
#define FEATURE_P_VZW_DS_IGNORE_eHRPD_INTERNET_IPV6

/*
//20120131_yunsik_DATA
- eHRPD Internet PDN IPv6 enable/disable setting
- ���� feature : FEATURE_P_VZW_DS_SDM

<Req>------------------
5.3.4.1.4 IPV6 on eHRPD
The device shall support the enabling and disabling of IPV6Enable Mode. See "Reqs-Data Requirements" for full set of functional requirements and behavior of the IPV6 enabled node.  
If a Get command is sent to the ./ManagedObjects/ConnMO/IPV6Enable/Settings/Operations node, the device shall return the value of "enabled" 
if IPV6 Traffic on the eHPRD is enabled, and a value of "Disabled" if the IPV6 Traffic is currently disabled.  

5.3.4.1.4.2 Disable IPV6 Traffic on eHPRD
When an Exec command is sent to the device on this node, the device shall not allow any IPv6 data from either an application on the device or an application on a tethered laptop to pass 
through to the Internet PDN. The default value for IPV6 Enable shall be "disabled" 

5.3.4.1.4.3 Enable IPV6 Traffic on eHPRD
When an Exec command is sent to the device on this node, the device shall support normal operation and allow any IPv6 data from either an application on the device 
or an application on a tethered laptop to pass through to the Internet PDN.  
-----------------------

secure.setting�� ��� write�ϰ� eHRPD Modem�� ������ ����
android/LTE   eHRPD       enable    disable  
    V4V6          V4(V4V6)     V4V6        V4        => android �� ������ V4V6�� �����ؼ� disconnect ó��
      V4                V4          nothing   nothing   => return true
      V6                V6          nothing   nothing   => android �� ������ V6�� �����ؼ� disconnect ó��

Cust_pantech_linux_data.h (linux\android\pantech\include)
DataConnectionTracker.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
GsmDataConnection.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony\gsm)
GsmDataConnectionTracker.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony\gsm)
IdevConnMo.java (linux\android\pantech\apps\otadmextensions\src\com\innopath\activecare\dev\oem)
ISkyDataConnection.aidl (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
SkyDataBroadcastReceiver.java (linux\android\packages\apps\phone\src\com\android\phone)
SkyDataConInterfaceManager.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)

2012.03.27 SJM
Modify eHRPD IPv6
DataConnection.java
GsmDataConnection.java
GsmDataConnectionTracker.java
ApnContext.java

//20130104_kns : file summary
 - ApnContext.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
 - DataConnectionTracker.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
 - GsmDataConnectionTracker.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony\gsm)
 - IdevConnMo.java (linux\android\pantech\apps\otadmextensions\src\com\innopath\activecare\dev\oem)
 - ISkyDataConnection.aidl (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
 - Property_service.c (linux\android\system\core\init)
 - PstDataApnSettings.java (linux\android\pantech\frameworks\sky_pst\services\com\pantech\service)
 - Qmi_psdm_svc.c (linux\android\pantech\frameworks\testmenu_server\psdm)
 - SkyDataBroadcastReceiver.java (linux\android\packages\apps\phone\src\com\android\phone)
 - SkyDataConInterfaceManager.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
*/
#define FEATURE_P_VZW_DS_eHRPD_INTERNET_IPV6

/* P12239_DATA 
-SVDO enable setting

//20121025_yunsik_DATA, modified for JB
- CdmaLteServiceStateTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\cdma)
- CdmaServiceStateTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\cdma)
*/
#define FEATURE_P_VZW_DS_SVDO_ENABLE

/*
//2012.01.09 P11546 ICS porting - ICS ������ ����� ������ Apache �ڵ� ������. 
- DataConnectionTracker.java, GsmDataConnectionTracker.java

//20110526_ckhong_DATA
- IMS REG/DE-REG  ���� üũ , power off/airplane mode �� de-reg �� wait.
- DataConnectionTracker.java, MMDataConnectionTracker.java  (\android\frameworks\base\telephony\java\com\android\internal\telephony)
*/

#define FEATURE_P_VZW_DS_IMS_REG_DEREG

/*
//2012.01.09 P11546 ICS porting - ICS ������ ����� ������ Apache �ڵ� ������. 
- GsmDataConnectionTracker.java
- MobileDataStateTracker.java

//20110527_ckhong_DATA
- Data Mode/Data Roaming Mode �� ������� LTE/EHRPD ���� ����.
- MMDataConnectionTracker.java  (\android\frameworks\base\telephony\java\com\android\internal\telephony)
- ConnectivityService.java (\android\frameworks\base\services\java\com\android\server)
- MobileDataStateTracker.java (LINUX\android\frameworks\base\core\java\android\net)
*/
#define FEATURE_P_VZW_DS_IMS_ALWAYS_ON

/*
//2012.03.13 swchoi
- For hide data icon when ims apn only activated.
- NetworkController.java (android\frameworks\base\packages\SystemUI\src\com\android\systemui\statusbar\policy)
*/
#define FEATURE_P_VZW_DS_IMS_DATA_ICON_VISIBLE

/*
//2012.02.04 swchoi
- �𵩿��� AT command�� ���� IMS TEST MODE ������ ����ϴ� NV_IMS_CLIENT_ENABLED_I �� �о�� Ecrio IMS���� ���
- NV_EHRPD_ENABLED_I �� NV������ ����.
 NVInterface.java (android\frameworks\base\telephony\java\com\android\internal\telephony)
 */
#define FEATURE_P_VZW_DS_AT_CMD_LTE_IMS_TEST_MODE

/*
//20120206_yunsik_DATA
- APN2�� disable���� ��� CdmaDataConnectionTracker.java ���� exception �߻�
- workaround code��. ���� patch ��Ȳ�� ����.

// 20121026_kns
 - This issue is fixed in JB 
*/
//#define FEATURE_P_VZW_DS_CDMA_TRACKER_EXCEPTION

/*
//20120207_yunsik_DATA
- getaddrinfo.c (linux\android\bionic\libc\netbsd\net)
- _have_ipv6(), _have_ipv4() ������ default �������� �Ǿ� �־� ���� vzwinternet�� IPv6 only ��� �ٸ� PDN���� V4�� DNS �Ҽ� ���� ������ �߻���.
- _test_connect() ���� return 1; �� ����
//20120615_yunsik_DATA, modified
- IPv4, IPv6 : AAAA, A query
- IPv4 only : A query (eHRPD ipv6 default was enabled, so changed)
- IPv6 only : AAAA, A query (for other PDN)
//FEATURE_P_VZW_DS_DISABLE_TEST_CONNECT_FOR_DNS_DEBUG
*/
#define FEATURE_P_VZW_DS_DISABLE_TEST_CONNECT_FOR_DNS

/*
//2012.02.02 p11546 
   /proc/net/xt_qtaguid/stats ���Ͽ� interface �� uid �� data traffic ���� ��ϵǹǷ� �̸� �о����

- NetworkStats.java (LINUX\android\frameworks\base\core\java\android\net) : ���� 
- TrafficStats.java (LINUX\android\pantech\frameworks\vzw\java\com\verizon\net) : �߰� (com.verizon.net ��Ű���� ������ ���� VZW���� �䱸��) 
- Android.mk (LINUX\android\frameworks\base) : ����
- system/core/rootdir/init_STARQ.rc
- pantech/frameworks/vzw/Android.mk 
- pantech/frameworks/vzw/com.verizon.net.xml 
- dex_preopt.mk
- NetworkManagementService.java
- INetworkManagementService.aidl
*/
#define FEATURE_P_VZW_DS_TRAFFIC_MEASUREMENT

/*
//20120217_yunsik_DATA, modified
- APN �������� default �� ���̰� ������ vzw apn ����Ʈ ������ �ʵ��� ó�� 
- "##276#7388464#" ���� apn ���� �����ؾ� editing ����

- AndroidManifest.xml (packages\apps\phone)
- ApnEditor.java (packages\apps\settings\src\com\android\settings)
- ApnPreference.java (packages\apps\settings\src\com\android\settings)
- ApnSettings.java (packages\apps\settings\src\com\android\settings)
- Cust_pantech_linux_data.h (pantech\include)
- SkyDataBroadcastReceiver.java (packages\apps\phone\src\com\android\phone)
- SkyHiddenCode.java (frameworks\base\telephony\java\com\android\internal\telephony)

//20121210_yunsik_DATA,  applied TOTAL_APN_NUMBER
-  warning! Do check TOTAL_APN_NUMBER at all files

//20130104 : file summary
 - AndroidManifest.xml (linux\android\packages\apps\phone)
 - ApnEditor.java (linux\android\packages\apps\settings\src\com\android\settings)
 - ApnPreference.java (linux\android\packages\apps\settings\src\com\android\settings)
 - ApnSettings.java (linux\android\packages\apps\settings\src\com\android\settings)
 - ReferenceCountryPreference.java (linux\android\pantech\apps\callsettings\src\com\pantech\psui\callsettings)
 - SkyDataBroadcastReceiver.java (linux\android\packages\apps\phone\src\com\android\phone)
 - SkyHiddenCode.java (linux\android\frameworks\base\skytelephony\common\java\com\android\internal\telephony)
*/
#define FEATURE_P_VZW_DS_PREVENT_EDIT_DEFAULT_APN

/*
//20120222_yunsik_DATA
- test code (toast) of FEATURE_P_VZW_DS_MMS_ERROR_REASON modem implementation, Only for Test, blocked in CdmaDataConnectionTracker.java.
- CdmaDataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\cdma)
*/
#define FEATURE_P_VZW_DS_MMS_ERROR_REASON_TEST_TOAST

/* 2012.02.27 P11546 PS3�� �������� ���� (Data Usage ������ ���� ����)
- datausage �޴����� background data ���� �� for loop �� ���� UID ���� ip table �� set �� �־� 
   app ���� �ð� �� ���� ���� anr �߻� �� background data ���� ���� �� booting �� system ���� 
   anr �߻��ϴ� ���� ����   =>�� UID ���� event �� ó���ǵ��� ��.
- android_filesystem_config.h  �� Define �� UID �� system UID �� ����. 
- system/bin/iptables �� system �������� �Ǿ� ������ ip6tables �� shell �� �Ǿ� �վ� system ���� ����.
   -NetworkPolicyManagerService.java
*/
#define FEATURE_P_VZW_DS_BACKGROUND_RESTRICT_BUG_FIX

/* 2012.02.27 P11546 PS3�� �������� ���� (Data Usage ������ ���� ����)
- system/bin/iptables �� system �������� �Ǿ� ������ ip6tables �� shell �� �Ǿ� �վ� system ���� ����.
   -android_filesystem_config.h
*/

#define FEATURE_P_VZW_DS_IP6TABLE_UID_BUG_FIX

#if 0 //def FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED
/*
//20120229_yunsik_DATA
- Qualcomm patch 1031�� �߰��� partial protocol failure�� ���� ���� ó��
- APN setting�� Dual IP �� ��� �� type�� ����Ǹ� partial protocol failure �� ���� reconnect ��ƾ�� Ÿ�Ե�
- IMS�� Dual IP ������ IPv6�� ����ǰ�, eHRPD internet �� Dual IP ������ IPv4�� �����Ű�� ������ �� �ΰ��� ��쿡 ���� ó�� �ʿ�
- ���� feature : FEATURE_P_VZW_DS_IGNORE_eHRPD_INTERNET_IPV6, FEATURE_P_VZW_DS_eHRPD_INTERNET_IPV6
- dataconnection.java ( android\frameworks\base\telephony\java\com\android\internal\telephony\ )
*/
#define FEATURE_P_VZW_DS_SKIP_PARTIAL_FAILURE
#endif

//20120403 sjm removed this feature
//#define FEATURE_P_VZW_DS_SKIP_PARTIAL_FAILURE_PREVIOUS_LTE_CHECK

/* 2012.03.01 P11546
- LINUX\android\pantech\frameworks\vzw\net\ConnectivityManager.aidl
- LINUX\android\pantech\frameworks\vzw\server\VzwConnectivityService.java
- LINUX\android\frameworks\base\Android.mk
- LINUX\android\frameworks\base\services\java\Android.mk
- SystemServer.java (LINUX\android\frameworks\base\services\java\com\android\server)
- ProcFileReader.java

//20121025_yunsik_DATA
Android.mk (frameworks\base)
Android.mk (frameworks\base\services\java)
ConnectivityManager.java (frameworks\base\pantech_fw\java\com\verizon\net)
Context.java (frameworks\base\core\java\android\content)
ContextImpl.java (frameworks\base\core\java\android\app)
IVzwConnectivityManager.aidl (frameworks\base\pantech_fw\java\com\verizon\net)
ProcFileReader.java (frameworks\base\core\java\com\android\internal\util)
SystemServer.java (frameworks\base\services\java\com\android\server)
VzwConnectivityService.java (frameworks\base\services\java_pantech\vzw\com\verizon\server)
*/
#define FEATURE_P_VZW_DS_DATA_METER_API

/*
//20120314_yunsik_DATA, from apache
- "com.android.phone.action.SHOW_OTA_ACTIVATION" intent �޾Ƽ� OTA start/end �� ���� ó��
- OTA start �� admin �̿��� apn �� disable ��Ű�� not available �� ����� data �õ����� ���ϵ��� ��. 
- MobileDataStateTracker.java (LINUX\android\frameworks\base\core\java\android\net)

//20121010_yunsik_DATA, modified (in case SHOW -> HIDE(USIM) -> SHOW -> HIDE(CSIM) )

//20121025_yunsik_DATA, from PremiaV
*/
#define FEATURE_P_VZW_DS_PS_OTA_REQ

/*
//20120314_yunsik_DATA
- isConcurrentVoiceAndDataAllowed() �Լ����� �Ʒ��� ���� �κ��� �ִµ�, mLteSS.getRadioTechnology() �� �׻� 0���� return �ǰ� ����
  if (mLteSS.getRadioTechnology() != ServiceState.RADIO_TECHNOLOGY_1xRTT)
- workaround�� mLteSS.getRadioTechnology() �� networkType ���� ����

//20121026_yunsik_DATA, patch fixed
- SUSPENDED added
- feature name changed FEATURE_P_VZW_DS_GET_RADIO_TECHNOLOGY_BUG_WORKAROUND to FEATURE_P_VZW_DS_SUSPEND_FOR_MMS

*/
#define FEATURE_P_VZW_DS_SUSPEND_FOR_MMS

/*
//20120316_yunsik_DATA
- ACTIVE or DORMANT check API for PPP session/PDN management 

//20121026_yunsik_DATA, removed
*/
//#define FEATURE_P_VZW_DS_ACTIVE_DORMANT_CHECK_API

/*
* 2012.04.10 P11546 
*   In case IMS registration started before GsmDataConnectionTracker is not ready (e.g. mAllApnList is not created yet) so IMS apn request failed, GsmDataConnectionTracker should retry IMS apn request. 
*   GsmDataConnectionTracker.java
*/
#define FEATURE_P_VZW_DS_IMS_APN_REQ_RETRY

/*
* 2012.04.10 P11546 
*   If IMS APN request failed in eHRPD network, it should be done again after 15 min (VZW Requirement)
*   GsmDataConnectionTracker.java

//20130408_yunsik_DATA
- added generic failure case,
   At booting sequence, If UE received the session close from network, UE recognized to 1xRTT. (eHRPD -> 1X -> eHRPD)
   So, eHRPD VZWIMS call can be duplicated. 
   This causes "Invalid SETUP_DATA_CALL attempt, stopping existing call" in Qcril_data_netctrl.c and causes generic failure (qcril_data_response_generic_failure).
   But VZWIMS's retry timer set already to 15min. So, IMS retries after 15 MIn.
   In this case, we have to modify that IMS call must retry immediately when received the generic failure.
   
- DataConnection.java (frameworks\base\telephony\java\com\android\internal\telephony)
- GsmDataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\gsm)
*/
#define FEATURE_P_VZW_DS_IMS_RECONNECT_IN_EHRPD

/*
//20130412_yunsik_DATA,
this feature introduced for IMS retry timer in eHRPD.
this feature find the VSNCP failure or PDN reject. but can not find the failure of IPv6 not assigned.
So, we need more debugging. 
We will define this feature after completing of debugging.

- DataConnection.java (frameworks\base\telephony\java\com\android\internal\telephony)
- Qcril_data_utils.c (vendor\qcom\proprietary\qcril\common\data)
- Ril.h (hardware\ril\include\telephony)
- Ril.h (hardware\ril\mock-ril\src\cpp)
- Ril.h (hardware\ril\reference-ril)
*/
#ifdef FEATURE_P_VZW_DS_IMS_RECONNECT_IN_EHRPD
//#define FEATURE_P_VZW_DS_EHRPD_DETAILED_ERROR_REASON
#endif

/*
*  2012.04.11 p11546
*    Qcom SBA_1045_CR350813_Pantech_case821603_04102012
*    - Fix traversal of traffic control's flow_list while trying to find the tail of the doubly linked list.
*    netmgr_tc.c (android\vendor\qcom\proprietary\data\netmgr\src)

//20121029_yunsik_DATA, patch applied
*/
//#define FEATURE_P_VZW_DS_SBA_1045_CR350813

/*
 - P12239
 - FOR MMS IOT 1XRTT Buffersize setting
 */

#define FEATURE_P_VZW_DS_FOR_MMS_IOT_1XRTT 

/*
 - P12239 2012.04.14 Add CR347576
 - FIX Maintaining RSSI

//20121026_yunsik_DATA, default value changed to false (snapshot_enabled)
 */
//#define FEATURE_P_VZW_DS_CR347576

/*
* 2012.04.14 p11546
*  - Change TCP buffer size according to the network type in case the network type is changed
*  - MobileDataStateTracker.java
*/
#define FEATURE_P_VZW_DS_SET_TCPBUF_IN_RAT_CHANGE

/*
* 2012.04.16 p11546
*  - Qcom 1045 source bug fix : There's a corner case qdi_cb_hdlr() may return without QDI_MUTEX_UNLOCK
*  - qdi.c (LINUX\android\vendor\qcom\proprietary\data\qdi\src)

//20121026_yunsik_DATA, patch applied
*/
//#define FEATURE_P_VZW_DS_QCOM_1045_MUTEX_UNLOCK_BUG_FIX


/*
* 2012.04.21 p11546
*  - Bug fix for data icon and wifi icon are coexist in case USB tethering -> Data off -> Data on -> Wifi on 
*  - Tethering.java
*/
//20120503_yunsik_DATA, block because connect/disconnect ping pong issue occurred during 1X/HRPD in case of Data off->Data on
//#define FEATURE_P_VZW_DS_TETHERD_ICON_BUG_FIX

/*
* 2012.04.21 p11546
*  - To show 4G Icon using VZW Test SIM
*     This feature depends on FEATURE_P_VZW_DIABLE_SIM_LOCK (IccCard.State.READY_UNKNOWN)
*  - NetworkController.java
*/
#define FEATURE_P_VZW_DS_TEST_SIM_DATA_ICON

/*
//20120424_yunsik_DATA
During GPS test mode, Do not disconnect even though NO Rx PKT
- DataConnectionTracker.java
- CdmaDataConnectionTacker.java
- GsmDataConnectionTracker.java
*/
#define FEATURE_P_VZW_DS_DO_NOT_RESTART_RADIO_WHEN_NO_RX_PKT_FOR_TESTING

/*
//20120424_yunsik_DATA, SBA porting
searching point 
    //SBA CRs-Fixed: 353696
    //SBA CRs-Fixed: 353897
- DataConnection.java (frameworks\base\telephony\java\com\android\internal\telephony)   : CR 353696
- Ril.h (hardware\ril\include\telephony)   : CR 353696
    ==> Ril.h (hardware\ril\mock-ril\src\cpp)
    ==> Ril.h (hardware\ril\reference-ril)
- Qcril_data_netctrl.c (vendor\qcom\proprietary\qcril\common\data)  : CR 353897

//20121029_yunsik_DATA, dual-IP partial
*/
//#define FEATURE_P_VZW_DS_QUALCOMM_SBA_CR353696_CR353897

/*
* 2012.04.28 p11546
*  1) Dsi_netctrl: Added param to indicate a partial retry (CRs-Fixed: 355724)
*     - Added a param to dsi_netctrl for clients to indicate a Dual-IP partial  retry attempt. 
*        In QDI, fail the call attempt if a partial retry is being attempted  but the corresponding call object had already been freed
*     - dsi_netctrl/inc/dsi_netctrl.h     
*     - dsi_netctrl/src/dsi_netctrl.c     
*     - dsi_netctrl/src/dsi_netctrl_mni.c 
*     - dsi_netctrl/src/dsi_netctrli.h
*     - qdi/inc/qdi.h
*     - qdi/src/qdi.c 
*
*  2) QCRIL Data: Updates to handle Telephony, QCRIL Data race conditions (CRs-Fixed: 353897, 355724, 353696)
*     - Use PDP_FAIL_PARTIAL_RETRY_FAIL error code when reporting partial retry failure instead of the generic failure code
*     - Block subsequent call attempts with the same RIL parameters if a pending call attempt (in CALL_INACTIVE state) is found
*     - Use the DSI_CALL_INFO_PARTIAL_RETRY parameter to indicate a Dual-IP  partial retry attempt to dsi_netctrl library. 
*        Also, clean-up stale  QMI WDS handle if we receive an unexpected DSI_EVT_NET_IS_CONN event.
*     - common/data/qcril_data_netctrl.c
* 
*  3) Telephony: Add code to recover from special out of sync state (CRs-Fixed: 353696)
*     - If somehow it happened that modem things there is partially succesfulcall, but apps think this is a new call request, 
*       consider partialretry failure as success to get back in sync with modem
*     - /android/internal/telephony/DataConnection.java
*     - this is aleady applied with FEATURE_P_VZW_DS_QUALCOMM_SBA_CR353696_CR353897 
* 
*  4) Telephony: Add partial retry failure code (CRs-Fixed: 353696)
*     - this is aleady applied with FEATURE_P_VZW_DS_QUALCOMM_SBA_CR353696_CR353897 
* 
*  5) Telephony: support dual ip retry error code handling (CRs-Fixed: 353696,353897)
*     - /android/internal/telephony/DataCallState.java 
*
*  6) Telephony: Reset retry count in inactive state (CRs-Fixed: 355482)
*     - /android/internal/telephony/DataConnection.java
*  
*  7) Telephony: Fix setup_data in a loop (CRs-Fixed: 351626)
*     - Pass partial retry flag as an argument to bringUp function callinstead of passing it inside apn (apn can be touched from differentthreads)
*     - When sending EVENT_TRY_SETUP_DATA in onRatChanged method due to having partial failure - set the flag accordingly
*     -  .../android/internal/telephony/DataConnection.java 
*     - .../internal/telephony/DataConnectionTracker.java
*     - .../android/internal/telephony/DataProfile.java
*     - .../java/com/android/internal/telephony/Phone.java
*     - .../telephony/cdma/CdmaDataConnectionTracker.java
*     - .../cdma/MSimCdmaDataConnectionTracker.java
*     - .../telephony/gsm/GsmDataConnectionTracker.java
*     - .../gsm/MSimGsmDataConnectionTracker.java
* 

//20121029_yunsik_DATA, dual-IP partial
*/
//#define FEATURE_P_VZW_QCOM_SBA_CN836489_042712


/*
//20120503_yunsik_DATA
- Out Of Service is considered as data call disconnect => false setting
- PhoneBase.java (frameworks\base\telephony\java\com\android\internal\telephony)
*/
#define FEATURE_P_VZW_DS_DATA_CALL_NOT_DISCONNECT_WHEN_OOS

/*
* 2012.05.09 p11546
*  In case of single PDN network e.g. 1x or EvDO, mobile upstream is not disconnected even though wifi is connected. 
*  (Problem Reproduce Step : Data on -> USB tethering on -> wifi on -> wifi off -> wifi on) 
*  To prevent this symptom, StarQ doesn't use ConnectivityManager.TYPE_MOBILE_DUN as a tethering upstream and dun APN is not available in CDMA. 
* 
*   - Tethering.java (android\frameworks\base\services\java\com\android\server\connectivity)
*   - CdmaDataProfileTracker.java (android\frameworks\base\telephony\java\com\android\internal\telephony\cdma)
*/
#define FEATURE_P_VZW_DS_TETHERING_1X_EVDO_BUG_FIX


/*
* 2012.05.15 P11546
*  Exception handling in case DataConnectionAc is null 
*  - GsmDataConnectionTracker.java

//20121029_yunsik_DATA, dual-IP partial
*/
//#define FEATURE_P_VZW_DS_DCAC_NULL_EXCEPTION

/*
//20120515_yunsik_DATA
- do not show 3G/4G icon (HIPRI, dormant) during wifi connection <- hipri restore timer is 30minutes (ECID porting guide)
- NetworkController.java
- DataConnectionTracker.java (onEnableApn)
- CdmaDataConnectionTracker.java
- GsmDataConnectionTracker.java

//20120525_yunsik_DATA
- this feature removed. becasue ECID app was removed
-  ADR910L - Pan_Int_46045, 
    Description - 
    Application Preload changes:
    1) Add VZ Tones
    2) Remove Caller Name ID

    Here are few important instructions for removing Caller Name ID:

    1) Please make sure APN setting is set properly:
        a. Do not set android.telephony.apn-restore to 30 minutes as instructed by CaNID application integration guide
        b. Following requirement need to be meet:

    [VZW Reqs-Android.docx] INTERNET PDN - 1 min.
*/
//#define FEATURE_P_VZW_DS_DO_NOT_SHOW_HIPRI_ICON_DURING_WIFI

/*
//20120430_yunsik_DATA
//2.9.2 Support to PPP Session/PDN Attachment Management Req was implemented 
//BUT during Data restriction, FOTA could not be connect to vzwadmin PDN.
//So, We should consider that On LTE/eHRPD networks this requirement does not apply to VZWADMIN PDN.
//and we should consider on LTE/eHRPD/HRPD/1X.

-CdmaDataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\cdma)
-ConnectivityService.java (frameworks\base\services\java\com\android\server)
-DataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony)
-GsmDataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\gsm)
-MobileDataStateTracker.java (frameworks\base\core\java\android\net)

// 20121026_kns
 - "FEATURE_P_VZW_DS_IMS_ALWAYS_ON" feature �� ���õ� �κ��� ����(IMS).
*/
#define FEATURE_P_VZW_DS_VZWADMIN_NOT_BLOCK_EVEN_THOUGH_DATA_DISABLED

/*
//20120516_yunsik_DATA
- android\vendor\qcom\proprietary\telephony-apps\MultiplePdnTest -> only added vzw800 apn, font size modified

- FEATURE is not labeling.
- Pantech Multiple PDN Test application with vzw permission
- android\vendor\qcom\proprietary\telephony-apps\PantechMultiplePdnTest
*/
#define FEATURE_P_VZW_DS_PANTECH_MPDP_APK

/*
*  2012.06.11 P11546 VZW Request regarding Data Usage (Reqs-Smartphone_UI.docx)
*    warningBytes of default NetworkPolicy should be initialized as WARING_DISABLE to satisfy below VZW req.
*
*    <DEVICE REQUIREMENTS SMARTPHONE UI REQUIREMENTS VERSION 1.12>
*       "6.6 Data Usage 
*    	    Under Data usage settings screen, two options shall be provided to the user
*	    "Limit Mobile data usage"  - OFF by default. When is ON, set it at 5GB
*	    "Alert me about data usage" - OFF by default. Set at 2GB "
*   
*   - LINUX\android\frameworks\base\services\java\com\android\server\net\NetworkPolicyManagerService.java
*   - LINUX\android\frameworks\base\core\res\res\values\strings.xml
*   - LINUX\android\frameworks\base\core\res\res\values-es-rUS\strings.xml
*   - LINUX\android\frameworks\base\core\res\res\drawable-hdpi\data_usage_warning.png
*   - LINUX\android\frameworks\base\core\res\res\drawable-hdpi\data_usage_disabled.png
*
*  2012.07.09 P11546 Icon changed
*   - LINUX\android\frameworks\base\core\res\res\drawable-hdpi\data_usage_warning.png (changed)
*   - LINUX\android\frameworks\base\core\res\res\drawable-hdpi\data_usage_disabled.png (changed)
*   - LINUX\android\frameworks\base\core\res\res\drawable-hdpi\data_usage.png (added)
*
*  2013.03.07 P11516 xml changed
*   - LINUX\android\frameworks\base\core\res\res\drawable-xhdpi\data_usage_warning.png (moved)
*   - LINUX\android\frameworks\base\core\res\res\drawable-xhdpi\data_usage_disabled.png (moved)
*   - LINUX\android\frameworks\base\core\res\res\drawable-xhdpi\data_usage.png (moved)
*   - LINUX\android\frameworks\base\core\res\res\values\public.xml
*/
#define FEATURE_P_VZW_DS_DATA_USAGE

#if 0 //def FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED
/*
*  2012.06.27 p11546 
*   - Workaround code for the watchdog reset due to netd 
*   - LINUX\android\system\netd\logwrapper.c
*  2012.07.03 p11546
*   - Workaround code for the case stopTethering() is blocked due to wating for child process exit 
*   - LINUX\android\system\netd\TetherController.cpp
*/
#define FEATURE_P_VZW_DS_NETD_WATCHDOG_FIX
#endif

/*
*  2012.07.09 p11546 Klockwork problem fix
*   - line 625: Null pointer dereference of 'mDataConnections.get(...)' where null is returned from a map or a collection
*      LINUX/android/frameworks/base/telephony/java/com/android/internal/telephony/cdma/CdmaDataConnectionTracker.java
*   - line 91: Null pointer dereference of 'cp.apn' where null comes from condition
*      LINUX/android/frameworks/base/telephony/java/com/android/internal/telephony/cdma/CdmaDataConnection.java
*/
#define FEATURE_P_VZW_DS_KLOCKWORK_PROBLEM_FIX

/*
//20120705_yunsik_DATA
- GFIT Defect#125
   PreGFIT IN ADR930L: Device UI indicates "123456" as network operator and "1x" symbol 
   when device switch to GSM/UMTS mode when previously registered on CDMA in LTE/CDMA mode

   Steps to reproduce:
    1) Place the device in LTE/CDMA mode device should latch to CDMA network.
    2) Change the network mode to LTE/GSM/UMTS mode and check the UI screen

- This issue occurred when prefer mode changed during 1X dormant.
- Because GsmDataConnectionTracker does not handle 1XRTT, Setup Data call with 1XRTT must be blocked at GsmDataConnectionTracker when pref mode changed to mode without CDMA. (GWL or WCDMA only etc.)
- GsmDataConnectionTracker.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony\gsm)
*/
#define FEATURE_P_VZW_DS_1XRTT_ORIGIN_BLOCK_WHEN_CHANGED_GWL_PREF_MODE

/*
//20120816_yunsik_DATA
when netmgr_kif_powerup_init, rmnet1 & rmnet2 & rmnet3 more check by modem_wait is TRUE

- netmgr_main.c
- netmgr_kif.c
*/
#define FEATURE_P_VZW_DS_DATA_RECOVERY_FOR_SETUP_DATA_TIMEOUT_HDLR_MODEM_WAIT_CHECK

#if 0 //def FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED
/*
//20120713_yunsik_DATA

Recovery action and silent reset for issue of android state maintained to CONNECTING

- BaseCommands.java (frameworks\base\telephony\java\com\android\internal\telephony)
- CommandsInterface.java (frameworks\base\telephony\java\com\android\internal\telephony)
- Cust_pantech_linux_data.h (pantech\include)
- DataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony)
- GsmDataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\gsm)
- NetworkController.java (frameworks\base\packages\systemui\src\com\android\systemui\statusbar\policy)
- Qcril_data_netctrl.c (vendor\qcom\proprietary\qcril\common\data)
- Qcril_log.c (vendor\qcom\proprietary\qcril\qcril_qmi)
- Ril.cpp (hardware\ril\libril)
- Ril.h (hardware\ril\include\telephony)
- Ril.h (hardware\ril\mock-ril\src\cpp)
- Ril.h (hardware\ril\reference-ril)
- RIL.java (frameworks\base\telephony\java\com\android\internal\telephony)
- RILConstants.java (frameworks\base\telephony\java\com\android\internal\telephony)
- Ril_unsol_commands.h (hardware\ril\libril)

for TEST, must be blocked
FEATURE_P_VZW_DS_DATA_RECOVERY_FOR_SETUP_DATA_TIMEOUT_HDLR_ONLY_TEST
- Netmgr_exec.c (vendor\qcom\proprietary\data\netmgr\src)

//20120828_yunsik_DATA, changed from vzwinternet to vzwims
*/
#define FEATURE_P_VZW_DS_DATA_RECOVERY_FOR_SETUP_DATA_TIMEOUT_HDLR

/*
//20120725_yunsik_DATA
only for information, set property for qcril timeout Count

- BaseCommands.java (frameworks\base\telephony\java\com\android\internal\telephony)
- CommandsInterface.java (frameworks\base\telephony\java\com\android\internal\telephony)
- Cust_pantech_linux_data.h (pantech\include)
- DataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony)
- GsmDataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\gsm)
- Qcril_data_netctrl.c (vendor\qcom\proprietary\qcril\common\data)
- Qcril_log.c (vendor\qcom\proprietary\qcril\qcril_qmi)
- Ril.cpp (hardware\ril\libril)
- Ril.h (hardware\ril\include\telephony)
- Ril.h (hardware\ril\mock-ril\src\cpp)
- Ril.h (hardware\ril\reference-ril)
- RIL.java (frameworks\base\telephony\java\com\android\internal\telephony)
- RILConstants.java (frameworks\base\telephony\java\com\android\internal\telephony)
- Ril_unsol_commands.h (hardware\ril\libril)
*/
#define FEATURE_P_VZW_DS_DATA_RECOVERY_FOR_SETUP_DATA_TIMEOUT_HDLR_ONLY_COUNT

/*
//20120725_yunsik_DATA
CONNECTING ISSUE
when this issue occurred, QXDM log show "Blocking SETUP_DATA_CALL due to pending call attempt"

- BaseCommands.java (frameworks\base\telephony\java\com\android\internal\telephony)
- CommandsInterface.java (frameworks\base\telephony\java\com\android\internal\telephony)
- Cust_pantech_linux_data.h (pantech\include)
- DataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony)
- GsmDataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\gsm)
- Qcril_data_netctrl.c (vendor\qcom\proprietary\qcril\common\data)
- Qcril_log.c (vendor\qcom\proprietary\qcril\qcril_qmi)
- Ril.cpp (hardware\ril\libril)
- Ril.h (hardware\ril\include\telephony)
- Ril.h (hardware\ril\mock-ril\src\cpp)
- Ril.h (hardware\ril\reference-ril)
- RIL.java (frameworks\base\telephony\java\com\android\internal\telephony)
- RILConstants.java (frameworks\base\telephony\java\com\android\internal\telephony)
- Ril_unsol_commands.h (hardware\ril\libril)
*/
#define FEATURE_P_VZW_DS_DATA_RECOVERY_FOR_BLOCKING_DUE_TO_PENDING

/*
//20120725_yunsik_DATA

CONNECTING issue.
broken between rmnet0 and rmnet1, vzwims(rmnet0) was assigned to vzwinternet ip address.
so, vzwims has dual-IP address and ipv6 prefix of vzwinternet.

- Cust_pantech_linux_data.h (pantech\include)
- DataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony)
- GsmDataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\gsm)

*/
#define FEATURE_P_VZW_DS_DATA_RECOVERY_FOR_BOTH_ADDR_ALREADY_UP
#endif

/*
//20120716_yunsik_DATA
What application will try establish through vzwapp APN?

-ConnectivityService.java (frameworks\base\services\java\com\android\server)
*/
#define FEATURE_P_VZW_DS_GET_APP_NAME

#if 0 //def FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED
/*
*  2012.07.16 P11546 Qualcomm 153613 version bug fix
*     Fix the bug can cause null pointer exception 
*     (dcac variable can be refered without any assignment so recover the previous source code)
*     - linux\android\frameworks\base\telephony\java\com\android\internal\telephony\gsm\GsmDataConnectionTracker.java
*/
#define FEATURE_P_VZW_DS_QCOM_153613_BUG_FIX
#endif

/*
//20120719_yunsik_DATA
porting from EF44S

- Tethering.java (android\frameworks\base\services\java\com\android\server\connectivity)
*/
#define FEATURE_P_VZW_DS_TETHERING_SETDNSFORWARD_BUG_FIX

/*
*   2012.07.13 P11546
*
*   - frameworks\base\telephony\java\com\android\internal\telephony\cdma\CdmaDataConnectionTracker.java
*   - frameworks\base\telephony\java\com\android\internal\telephony\DataConnectionTracker.java
*   - frameworks\base\telephony\java\com\android\internal\telephony\gsm\GsmDataConnectionTracker.java
*   - frameworks\base\services\java\com\android\server\ConnectivityService.java
*   - frameworks\base\packages\systemui\src\com\android\systemui\statusbar\policy\NetworkController.java
*   - system\core\rootdir\init_VEGAPVW.rc
*   - system\core\init\Property_service.c

//20130412_kwak.nosob
ICS�϶��� startUsingNetworkFeature�� ȣ���ϴ� ��ü�� com.fusionone.android.sync.service ������, 
JB�϶��� startUsingNetworkFeature�� ȣ���ϴ� ��ü�� com.vzw.apnservice �� �����
"com.vzw.apnservice" package�� �׻� ���� �����ϵ��� ����

   - frameworks\base\services\java\com\android\server\ConnectivityService.java

//20130416_yunsik_DATA
Data disabled �Ǿ� �ִ� ���¿��� startUsingNetworkFeature�� APN Service�� �θ���, DNS query�� BuA�� �θ��µ�, net.dns1.[pid] �� APN service�� ��ϵǾ� �־� 
getaddrinfo �� _using_alt_dns() �� false return �ǰ�, default �� ���� ������ _have_ipv4(), _have_ipv6() �� false ���� DNS query�� ������ ���ϴ� ����
Apn service�� VZWAPP ����� net.dns1.[pid] �� BuA pid �� �����ϵ��� ����

   - frameworks\base\services\java\com\android\server\ConnectivityService.java
   - frameworks\base\telephony\java\com\android\internal\telephony\cdma\CdmaDataConnectionTracker.java
   - frameworks\base\telephony\java\com\android\internal\telephony\gsm\GsmDataConnectionTracker.java
*/
#define FEATURE_P_VZW_DS_VZWAPP_FOR_BUA_ALWAYS_ON

/*
*   2012.07.27 p11546
*    StarQ QC Issue fix 
*    - frameworks\base\core\res\res\config.xml
*/
#define FEATURE_P_VZW_DS_DATA_USAGE_NETWORK_TYPES

/*
//20120816_yunsik_DATA
 iface_id  FIX
- Dsi_netctrli.c (vendor\qcom\proprietary\data\dsi_netctrl\src)
- Dsi_netctrli.h (vendor\qcom\proprietary\data\dsi_netctrl\src)
- Dsi_netctrl_multimodem.c (vendor\qcom\proprietary\data\dsi_netctrl\src)

//20130118_yunsik_DATA, undefined, ���� �ʿ��ϸ� define
*/
#undef FEATURE_P_VZW_DS_INTERNET_RMNET0

#if 0 //def FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED
/*
//20120810_yunsik_DATA
BLOCK resetRetryCount() in clearSettings().
//https://www.codeaurora.org/gitweb/quic/la/?p=platform/frameworks/base.git;a=commit;h=e1c8c842127daf71871f9b59fb8dde596e2f4134
- DataConnection.java (frameworks\base\telephony\java\com\android\internal\telephony)
*/
#define FEATURE_P_VZW_DS_RETRY_TIMER_RESET_BUG_FIX
#endif

/*
//20120817_yunsik_DATA, from 50L
when screen on, reset retrycount and trysetup immediately

//20130107_yunsik_DATA, modified for JB
-DataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony)
*/
#define FEATURE_P_VZW_DS_RETRY_TIMER_RESET_WHEN_SCREEN_ON

/*
//20120813_yunsik_DATA
block log related to IP,DNS,GW
NFC ISIS issue
- RIL.java (frameworks\base\telephony\java\com\android\internal\telephony)
*/
#define FEATURE_P_VZW_DS_BLOCK_IP_LOG_FOR_ISIS

#if 0 //def FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED
/*
//20120920_yunsik_DATA
IPv6 only issue

I don't know why assigned Dual IP in RIL but assigned IPv6 only in Android.
And did not entered IPv6 recovery code. (FEATURE_P_VZW_DS_RECONNECT_DUALIPFAILURE_ON_IPV4)

So, I decided that will check Dual IP or not after call connected.

- DataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony)
- GsmDataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\gsm)
- Phone.java (frameworks\base\telephony\java\com\android\internal\telephony)
*/
#define FEATURE_P_VZW_DS_ACQUIRE_ONLY_IPV6_ONE_MORE_CHECK
#endif

/*
//20120917_yunsik_DATA
 MMS sending fail issue (1X/HRPD)

MMS want to receive APN_ALREADY_ACTIVE when called 2nd startUsingNetworkFeature, but ConnectivityService returned APN_REQUEST_STARTED sometimes.
So, I modifed the reconnect method in MobileDataStateTracker.
modification is that will not to set DetailedState.IDLE during MMS transaction.

- BluetoothTetheringDataTracker.java (frameworks\base\core\java\android\bluetooth)
- ConnectivityService.java (frameworks\base\services\java\com\android\server)
- DummyDataStateTracker.java (frameworks\base\core\java\android\net)
- EthernetDataTracker.java (frameworks\base\core\java\android\net)
- MobileDataStateTracker.java (frameworks\base\core\java\android\net)
- NetworkStateTracker.java (frameworks\base\core\java\android\net)
- WifiStateTracker.java (frameworks\base\wifi\java\android\net\wifi)

//20130221_yunsik_DATA,
this bug fix applied for all app. above files was restored.
- MobileDataStateTracker.java (frameworks\base\core\java\android\net)
*/
#define FEATURE_P_VZW_DS_MMS_SENDING_FAIL_BUG_FIX

// TODO:Check
#if 0 //def FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED
/*
//20121204_yunsik_DATA
implemented Reqs-DataRoaming.doc (global data roaming)

- AndroidManifest.xml (pantech\apps\callsettings)
- CdmaDataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\cdma)
- DataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony)
- GsmDataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\gsm)
- Strings.xml (pantech\apps\callsettings\res\values)
- VZWDataRoamingDlgReceiver.java (pantech\apps\callsettings\src\com\pantech\psui\callsettings)
- VZWDataRoamingNotification.java (pantech\apps\callsettings\src\com\pantech\psui\callsettings)

*/
#define FEATURE_P_VZW_DS_GLOBAL_DATA_ROAMING
#endif /* FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED */

/*
// 20121120_kns
  unsol QMI service message for notifing the information to framework

- Qcril_data_netctrl.c (linux\android\vendor\qcom\proprietary\qcril\common\data)
- Qmi_wds_srvc.c (linux\android\vendor\qcom\proprietary\qmi\src)
- Qmi_wds_srvc.h (linux\android\vendor\qcom\proprietary\qmi\inc)
*/
#define FEATURE_P_VZW_DS_UNSOL_QMI_SERVICE_ENABLE

#ifdef FEATURE_P_VZW_DS_UNSOL_QMI_SERVICE_ENABLE
/*
// 20121120_kns
  unsol QMI service message for notifing the information to framework

- BaseCommands.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
- CommandsInterface.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
- DataConnectionTracker.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
- Ds_qmi_defs.h (modem_proc\modem\datamodem\interface\qmidata\inc)
- Ds_qmi_task.c (modem_proc\modem\datamodem\interface\qmicore\src)
- Ds_qmi_wds.c (modem_proc\modem\datamodem\interface\qmidata\src)
- Ds_qmi_wds.h (modem_proc\modem\datamodem\interface\qmidata\inc)
- Qcril_log.c (linux\android\vendor\qcom\proprietary\qcril\qcril_qmi)
- Qmi_wds_srvc.c (linux\android\vendor\qcom\proprietary\qmi\src)
- Qmi_wds_srvc.h (linux\android\vendor\qcom\proprietary\qmi\inc)
- Ril.h (linux\android\hardware\ril\include\telephony)
- Ril.cpp (linux\android\hardware\ril\libril)
- RIL.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
- RILConstants.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
- Ril_unsol_commands.h (linux\android\hardware\ril\libril)
- SkyDataConInterfaceManager.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
*/
#define FEATURE_P_VZW_DS_UNSOL_QMI_SERVICE

#ifdef FEATURE_P_VZW_DS_UNSOL_QMI_SERVICE
/*
// 20121120_kns
  If the device has attached to LTE, modem notify pcscf addr to framework. (only Test code) 
- DataConnectionTracker.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
*/
#define FEATURE_P_VZW_DS_UNSOL_QMI_SERVICE_PCSCF_ADDR
#endif
#endif  /* FEATURE_P_VZW_DS_UNSOL_QMI_SERVICE_ENABLE */

/*
// 20121212_kns
  Data enable/disable �� �ݺ��� ��� ���� �ð�(�� 60000ms) ���� handlerMessage�� 
  EVENT_SET_MOBILE_DATA �� ���� �ʴ� ������ �־� starQ JB�� �ڵ带 ���� (���ֱ⼺)

- NativeDaemonConnector.java (msm8930_jb\linux\android\frameworks\base\service\java\com\android\server)

// 20121218_kns
  Qualcomm 1232 patch ���� ������.
*/
//#define FEATURE_P_VZW_DS_NDC_COMMAND_TOOK_TOO_LONG

/*
//20121212_yunsik_DATA, StarQ JB PLM 73, 144 in JB version
   *** FATAL EXCEPTION IN SYSTEM PROCESS: ConnectivityServiceThread
   java.util.ConcurrentModificationException

Occurred System_Server_Crash when removeRoute executed
 : applied try&catch at ConnectivityService.java
 : applied synchronized / try&catch at AbstractCollection.java

- AbstractCollection.java (libcore\luni\src\main\java\java\util)
- ConnectivityService.java (frameworks\base\services\java\com\android\server)
*/
#define FEATURE_P_VZW_DS_CONCURRENT_MODIFICATION_EXCEPTION

/*
//20130104_yunsik_DATA
 data roaming default value set false.
 
- android\build\target\product\full_base_telephony.mk
*/
#define FEATURE_P_VZW_DS_DATA_ROAMING_SET_FALSE

/*
//20130109_yunsik_DATA
tethering error fix with equipment
Block NTP force Refreshing in equipment with NO DNS

- NetworkStatsService.java (frameworks\base\services\java\com\android\server\net)
- SkyEngMenuBroadcastReceiver.java (packages\apps\settings\src\com\android\settings\skyengmenu)
*/
#define FEATURE_P_VZW_DS_TETHERING_ERROR_FIX_WITH_EQUIPMENT

/*
//20130215_yunsik_DATA, from EF51K
advisePersistThreshold ó���� RemoteException ���� catch �Ͽ� Null Pointer Exception/illegal State Exception �߻��� silent reboot�߻�
changed to Exception
   -NetworkPolicyManagerService.java
*/
#define FEATURE_P_VZW_DS_NETWORK_POLICY_EXCEPTION_BUG_FIX

/*
//20130218_yunsik_DATA, OMH disabled for CDMA

- System.prop (device\qcom\msm8960)
*/
#define FEATURE_P_VZW_DS_OMH_DISABLED

/*
//20130218_yunsik_DATA, There is NO qcril response, for stop data call

- qcril_data_netctrl.c (android\vendor\qcom\proprietary\qcril\common\data)
*/
#define FEATURE_P_VZW_DS_QCRIL_RESPONSE_BUG_FIX

/*
//20130315_yunsik_DATA
When ecrio lib receives CONNECTED event, ecrio lib tries REGISTRATION.
if framework receives V4 address without V6 address at first because of partial retry, IMS REGISTRATION will be fail.
So, We MUST BE consider that protocol set V6 only for IMS in equipment case.

IF EQUIPMENT DOES NOT SUPPORT IPV6 ONLY MODE FOR IMS, DEFINE THIS FEATURE.
OTHERWISE, DO UNDEFINE.

GsmDataConnection.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony\gsm)
*/
#undef FEATURE_P_VZW_IMS_REPLACE_IPV6_ONLY_FOR_EQUIPMENT

/*
//20130329_yunsik_DATA
In Multiple APN case,
When SETUP_DATA_CALL was failed consecutively by PDN reject etc , if max retry reaches three times after reconnectAfterFail(), framework does not change from FAILED state to IDLE state.
So, although application calls StartUsingNetworkFeature, setupData function can not be called.

- GsmDataConnection.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony\gsm)
*/
#define FEATURE_P_VZW_FAILED_STATE_BUG_FIX_AFTER_RECONNECT_AFTER_FAIL_MAX_RETRY 

/*
//20130408_yunsik_DATA
   At booting sequence, If UE received the session close from network, UE recognized to 1xRTT. (eHRPD -> 1X -> eHRPD)
   So, eHRPD VZWIMS / VZWINTERMET calls can be duplicated. 
   This causes "Invalid SETUP_DATA_CALL attempt, stopping existing call" in Qcril_data_netctrl.c and causes generic failure (qcril_data_response_generic_failure).
   In this case, the 1st call of duplicating can be terminated(VSNCP).
   So, I modified that delayed "GsmDataConnectionTracker dispose" in Boot sequence and test UICC for a while.

- CdmaDataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\cdma)
- CdmaLteServiceStateTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\cdma)
- GsmDataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\gsm)
- ServiceStateTracker.java (frameworks\base\telephony\java\com\android\internal\telephony)
- SkyDataBroadcastReceiver.java (packages\apps\phone\src\com\android\phone)
*/
#define FEATURE_P_VZW_DS_BLOCK_GSM_DISPOSE_IN_BOOT_SEQUENCE_FOR_IRAT_TEST

/*
//20130410_yunsik_DATA
If ecrio lib did not connect before internet connected, IRAT 5.7 will be failed.
So, Internet apn will try to establish after completing of IMS connection.
this feature uses only with TEST UICC

- DataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony)
- GsmDataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\gsm)
*/
#define FEATURE_P_VZW_DS_WAIT_INTERNET_BY_IMS_CONNECTED_IN_LTE_FOR_IRAT_TEST

/*
//20130514_yunsik_DATA, interface broken issue (PLM 01089)
MSG        [05004/03] DS IS 707/Error            03:43:06.820     ds707_pkt_mgr_util.c  00555  DS3G system is LTE, Can't originate 1x/HDR call

[0402]> SETUP_DATA_CALL 16 2 VZWIMS * * 0 IPV4V6
[0402]< SETUP_DATA_CALL DataCallState: {version=7 status=0 retry=-1 cid=0 active=2 type=IPV4V6' ifname='rmnet0' addresses=[2600:1001:802c:d032:a9dc:63d7:f621:c5bf/64] dnses=[2001:4888:0010:ff00:0132:000d:0000:0000,2001:4888:0011:ff00:0137:000d:0000:0000] gateways=[2600:1001:802c:d032:684d:a59c:3068:4b4a]}
[0525]> SETUP_DATA_CALL 16 0 VZWINTERNET * * 0 IPV4V6
[0525]< SETUP_DATA_CALL DataCallState: {version=7 status=0 retry=-1 cid=1 active=2 type=IPV4V6' ifname='rmnet0' addresses=[10.163.178.145/30,2600:1001:802c:d032:a9dc:63d7:f621:c5bf/64] dnses=[198.224.184.135,198.224.185.135,2001:4888:0010:ff00:0132:000d:0000:0000,2001:4888:0011:ff00:0137:000d:0000:0000] gateways=[10.163.178.146,2600:1001:802c:d032:684d:a59c:3068:4b4a]}

VZWINTERNET assigned to cid 1, but interface name is rmnet0. VZWIMS's interface name is also rmnet0.
and VZWINTERNET has a IPv6 of VZWIMS (2600:1001:802C ..)

How to recovery : cleanUpAllConnections

- DataConnection.java (frameworks\base\telephony\java\com\android\internal\telephony)
- DataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony)
- GsmDataConnectionTracker.java (frameworks\base\telephony\java\com\android\internal\telephony\gsm)
*/
#define FEATURE_P_VZW_DS_DATA_RECOVERY_FOR_IFACE_BROKEN

/*
//20130524_yunsik_DATA, Watchdog PLM  01231
system_server_watchdog

at com.android.server.net.NetworkStatsService.setUidForeground(NetworkStatsService.java:~638)
- waiting to lock <0x4208a280> (a java.lang.Object) held by tid=42 (NetworkStats)

"NetworkStats" prio=5 tid=42 NATIVE
  at libcore.io.Posix.getaddrinfo(Native Method)
  at libcore.io.ForwardingOs.getaddrinfo(ForwardingOs.java:55)
  at java.net.InetAddress.lookupHostByName(InetAddress.java:405)
  at java.net.InetAddress.getAllByNameImpl(InetAddress.java:236)
  at java.net.InetAddress.getByName(InetAddress.java:289)
  at android.net.SntpClient.requestTime(SntpClient.java:78)
  at android.util.NtpTrustedTime.forceRefresh(NtpTrustedTime.java:83)
  at com.android.server.net.NetworkStatsService.performPoll(NetworkStatsService.java:930)

Move-NTP-updates-outside-locks
- NetworkStatsService.java (frameworks\base\services\java\com\android\server\net)
*/
#define FEATURE_P_VZW_DS_NETWORKSTATSSERIVCE_WATCHDOG


/*==========================================================================
    5. Kernel & init.rc & VPN Feature
===========================================================================*/
/* Kernel Config */
/*
Kernel Config Ȯ�� ��� //20110621_yunsik_DATA

Apache�� ��� LINUX\android\device\qcom\msm7630_fusion\AndroidBoard.mk ����
Cheetah�� ��� LINUX\android\device\qcom\msm8960\AndroidBoard.mk ����
KERNEL_DEFCONFIG �� Ȯ�� �ϸ� �Ʒ��� ���� kernel config ��ġ�� Ȯ���� �� �ִ� 

= APACHE =======================================
ifeq ($(KERNEL_DEFCONFIG),)
    KERNEL_DEFCONFIG := msm7630-perf_defconfig
endif
==============================================
     APACHE������  LINUX\android\kernel\arch\arm\configs\msm7630-perf_defconfig �� kernel config�� ��ġ �� �ִ�. 

= CHEETAH ======================================
ifeq ($(KERNEL_DEFCONFIG),)
    KERNEL_DEFCONFIG := msm8960_defconfig
endif
==============================================
     CHEETAH���� LINUX\android\kernel\arch\arm\configs\msm8960_defconfig �� kernel config�� ��ġ �� ������,
                          �����δ� msm8960_cheetah_ws10_defconfig �� msm8960_cheetah_ws12_defconfig �� ���ľ� �Ѵ�. board version �ö󰡸� Ȯ�� �ʿ�.

- J-MASAI ������ �Ʒ��� ���� �����Ǿ�����, Apache�� �ణ Ʋ����.

CONFIG_NET_KEY=y, CONFIG_INET_ESP=y, CONFIG_INET_IPCOMP=y, 
CONFIG_XFRM=y, CONFIG_XFRM_USER=y, CONFIG_INET_XFRM_MODE_TRANSPORT=y, CONFIG_INET_XFRM_MODE_TUNNEL=y, 
CONFIG_CRYPTO=y, CONFIG_CRYPTO_HMAC=y, CONFIG_CRYPTO_XCBC=y, CONFIG_CRYPTO_NULL=y, CONFIG_CRYPTO_MD5=y,
CONFIG_CRYPTO_SHA1=y, CONFIG_CRYPTO_DES=y, CONFIG_CRYPTO_AES=y, CONFIG_CRYPTO_DEFLATE=y, CONFIG_PPP=y, 
CONFIG_PPP_ASYNC=y, CONFIG_PPP_SYNC_TTY=y, CONFIG_PPP_DEFLATE=y, CONFIG_PPP_BSDCOMP=y, CONFIG_PPP_MPPE=y,
CONFIG_PPPOE=y, CONFIG_PPPOL2TP=y
CONFIG_L2TP=y, CONFIG_L2TP_V3=y, CONFIG_L2TP_IP=y, CONFIG_L2TP_ETH=y, CONFIG_PPPOLAC=y, CONFIG_PPPOPNS=y

//20121026_kns
JB���� Kernel Config �� PPP ���� ������ �����ϸ� pppolac.c ���� compile error �� �߻��Ͽ� PPP_MTU �� ppp_def.h �� �߰�
- ppp_def.h (kernel\include\linux)
*/
#define FEATURE_P_VZW_DS_VPN_FIX

//VpnProfileEditor.java
//#define FEATURE_P_VZW_DS_VPN_SPACE_CHECK

/*  20110718, swchoi : IPSec �� ���� �Ʒ��� config �߰�
     CONFIG_INET_AH=y, CONFIG_INET_XFRM_TUNNEL=y
*/
#define FEATURE_P_VZW_DS_IPSEC_CONFIG

/*
//20111101_yunsik_DATA, TUN Driver enable
*/
#define FEATURE_P_VZW_DS_TUN_DRIVER_ENABLE

/*
   2012.02.29 P11546 CTS Fail �׸� ���� 
   TrafficStats �� UID �� ������ ������ ���� method�� �����ϱ� ���ؼ��� CONFIG_UID_STAT=y �� �����Ǿ�� ��
- LINUX\android\kernel\arch\arm\configs\msm8960_<model name>_<board ver>_defconfig : ���� 
*/
#define FEATURE_P_VZW_DS_UID_TRAFFIC_STATS_ENABLE

/*
Info : 
            Since default DHCP Lease Time is 1 hour, VPN, RMNET and android Tethering are disconnected frequently.
            So, Increase the DHCP Lease Time to 7 days by VZW request.
Related Feature :
            
Where : 
            Cust_pantech_modem_data.h (modem_proc\build\cust)
            Dhcp_am_ps_iface.c (modem_proc\modem\datamodem\protocols\inet\src)

            Cust_pantech_linux_data.h (linux\android\pantech\include)
            Showlease.c (linux\android\external\dhcpcd)
            TetherController.cpp (linux\android\system\netd)
            
When : 20110531
Who : Ju.Hojin
*/
#define FEATURE_P_VZW_DS_INCREASE_DHCP_LEASETIME

/*
//20110727_yunsik_DATA

1. Apache : ipv6���� MTU�� 2000���� �Ǿ��־� ���� ������ 
                  init.rc : write /sys/class/net/rmnet_sdio0~7/mtu 1428
2. Cheetah : init.rc : write /sys/class/net/rmnet0~7/mtu 1428 
              
*/
//#define FEATURE_P_VZW_DS_1428_MTU_SETTING

/*
//20110907_yunsik_DATA
1. Apache
   - rmem_max�� wmem_max �� ���� 262144 ���� 1730560 ���� �ø�
   - 1730560 �� LTE window max size�̰�, �̰� �ø����� ���� streaming service(YouTube)���� ����� ������ ������ �� ���� ������ ������
2. Cheetah
   - http://www.ipbalance.com/traffic-analysis/%0bthroughput/104-tcp-throughput-calculation-formula.html

   - 80-N7573-1_A_Android_Embedded_Tput_Tuning.pdf �� ���� TCP buffer sizes ����
      setprop net.tcp.buffersize.lte 4096,221184,3461120,4096,221184,3461120

   - Also assign TCP buffer thresholds to be the ceiling value of technology maximums   
     write /proc/sys/net/core/rmem_max 3461120 
     write /proc/sys/net/core/wmem_max 3461120
    
// 20130205_kns
  1. JB Kernel Version Up�� ���� LTE Buffer size ����
    - kernet ���� ���� �� tcp rwin size �� ���õ� ������ ������ ���� parameter�� �Ʒ��� ����
        (http://www.spinics.net/lists/netdev/msg197190.html)
      A.	tcp_adv_win_scale factor �� 2 -> 1 �� ���� �Ǿ����ϴ�.
      B.	tcp_rmem MAX ���� 4MB -> 6MB �� ���� �Ǿ����ϴ�.

    -�� �ΰ��� parameter �������� ���Ͽ� tcp_rmem_max �� ����Ǿ�� ��.(��� ������ ��տ� ������ ���̳� �ڷ� ����.)
      VZW, LGU+ : 5191630
      KDDI : 1048800
      
    - init_STARQ.rc (android\system\core\rootdir)
      ICS : setprop net.tcp.buffersize.lte     4096,221184,3461120,4096,221184,3461120
      JB  : setprop net.tcp.buffersize.lte     4096,221184,5191630,4096,221184,5191630 
*/
#define FEATURE_P_VZW_DS_TCP_BUFFER_SIZES_INCREASE

/*
//20120308_yunsik_DATA
busybox install for root, version : 1.19.0
- init_STARQ.rc (android\system\core\rootdir)
- busybox folder all (android\pantech\development\busybox)
*/
#define FEATURE_P_VZW_DS_BUSYBOX_INSTALL

/*
//20120308_yunsik_DATA
iperf install for root, version : 2.0.5 (08 Jul 2010)
- init_STARQ.rc (android\system\core\rootdir)
- iperf folder all (android\pantech\development\iperf)
*/
#define FEATURE_P_VZW_DS_IPERF_INSTALL

/*
 - 20120422 PS4 BTKim : FTP Disconnect
 - FTP disconnection issue. A 
 - init.qcom.rc  (write /proc/sys/net/netfilter/nf_conntrack_tcp_be_liberal 1)
                         ==> /proc/sys/net/ipv4/netfilter/ip_conntrack_tcp_be_liberal copied
 - 2012.05.14 p11546
    Qcom 8960 1.5.30 version includes this patch so Pantech feature is removed.
*/
//#define FEATURE_P_VZW_DS_TCP_BE_LIBERAL_ENABLE

#if 0 //def FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED
/*
*  2012.06.07 P11546 Android Security Patch
*   [CVE-2012-2811] IPv6 remote denial of service
*       - A remote attacker capable of spoofing large portions of the IPv6 address space can fill the Linux kernel's neighbors table and eventually cause the device to reboot.
*       - LINUX\android\kernel\net\ipv6\route.c  
*
*   [CVE-2012-2808] Incorrect randomization during DNS resolution
*       - During DNS resolution, a random port and a random TXID are generated. 
*          In Android 4.0.4 and prior these values are initialized with insufficient entropy, potentially allowing an attacker to spoof results for DNS queries.
*       - LINUX\android\bionic\libc\netbsd\resolv\res_init.c
*
* 20121026_kns
* - There is not CVE-2012-2811 patch in PREMIA V
* - CVE-2012-2808 is already included in JB 
*/
#define FEATURE_P_DS_GOOGLE_PATCH_20120607
#endif

/*
//20120913_yunsik_DATA

SR00906752 workaround code applied, and panic()
- Bam_dmux.c (kernel\arch\arm\mach-msm)

//20130401_yunsik_DATA, only applied 1HZ to 3HZ
*/
//#define FEATURE_P_VZW_DS_KERNEL_PANIC_FOR_BAM_ISSUE_TRACKING renamed
#define FEATURE_P_VZW_DS_BAM_ISSUE_TRACKING_3HZ

/*
//20121128_yunsik_DATA
-android\kernel\arch\arm\configs\msm8960_bluebird_ws10_defconfig
-android\kernel\arch\arm\configs\msm8960_bluebird_ws20_defconfig
-NatController.cpp (system\netd)

CONFIG_NETFILTER_XT_TARGET_TCPMSS=y
CONFIG_NETFILTER_XT_MATCH_TCPMSS=y

//20130318_yunsik_DATA, modified from MAROKO, Mangle table�� parameter add�� ���� add or delete�ϵ��� ����
*/
#define FEATURE_P_VZW_DS_USB_KERNEL_MTU_SETTING

/*
//20121126_yunsik_DATA, Native Daemon exception (raw), this exception causes NO display DATA USAGE setting menu.
- android\kernel\arch\arm\configs\msm8960_bluebird_ws10_defconfig
- android\kernel\arch\arm\configs\msm8960_bluebird_ws20_defconfig

CONFIG_IP_NF_RAW=y
CONFIG_IP6_NF_RAW=y

// 20130104_kns
- There are configs in msm8960_bluebird_XX_defconfig (LA1.7 version)
*/
#define FEATURE_P_VZW_DS_ENABLE_CONFIG_IP_NF_RAW_FOR_DATA_USAGE

/*
//20130122_yunsik_DATA
ICS BUILD : tcp_adv_win_scale set one in Init.qcom.rc, but sysctl_tcp_adv_win_scale set two in Tcp_input.c
JB BUILD : sysctl_tcp_adv_win_scale set one. 
This change will cause low throughput, this value must be changed to 2.

searching point
- Init.qcom.rc (device\qcom\common\rootdir\etc):    write /proc/sys/net/ipv4/tcp_adv_win_scale  1
- Tcp_input.c (kernel\net\ipv4):int sysctl_tcp_adv_win_scale __read_mostly = 1;

changed file
- Tcp_input.c (kernel\net\ipv4)

//20130130_yunsik_DATA, removed, needed more tuning.

#ifdef FEATURE_P_VZW_DS_TCP_ADV_WIN_SCALE_SET_2
int sysctl_tcp_adv_win_scale __read_mostly = 2;
#else
int sysctl_tcp_adv_win_scale __read_mostly = 1;
#endif
*/
//#define FEATURE_P_VZW_DS_TCP_ADV_WIN_SCALE_SET_2

/*
3. Linux kernel: sock_diag: Fix out-of-bounds access to sock_diag_handlers[] 
ID: CVE-2013-1763 
Severity: High 
Affected versions: described below 
Userland can send a netlink message requesting SOCK_DIAG_BY_FAMILY with a family greater or equal then AF_MAX -- the array size of sock_diag_handlers[]. The current code does not test for this condition therefore is vulnerable to an out-of-bound access opening doors for a privilege escalation. 
This vulnerability is publicly known. 

Introduced in: 
http://git.kernel.org/?p=linux/kernel/git/torvalds/linux.git;a=commit;h=d366477a52f1df29fa066ffb18e4e6101ee2ad04 

Fixed in: 
http://thread.gmane.org/gmane.linux.network/260061 


- kernel/net/core/sock_diag.c
*/
#define FEATURE_P_DS_SECURITY_PATCH_CVE_2013_1763

/*
//20130429_yunsik_DATA
ISSUE : During tethering, private IP address was sent to network. so, device receives Regular deactivation from network.

Subject: [PATCH] system/netd: RFC1918 Source Address Tether Packet Leak
This patch modifisd NatController.cpp to add an iptables rule toprevent Non-NATted tethered IPv4 traffic from being forwarded.
Change-Id: If95f4dec0602f915f58f1c436935f3e61e0d5183
CRs-Fixed: 000000

- NatController.cpp

*/
#define FEATURE_P_VZW_DS_PATCH_FOR_PRIVATE_IP_WAS_SENT_TO_NETWORK_DURING_TETHERING

/////////////////////below feature MUST BE BLOCKED at release version //////////////////////////

/*
//for SR
Smd.c (kernel\arch\arm\mach-msm)
*/
//20120913_yunsik_DATA, log undef
//#define BAM_ISSUE_TRACKING_SMD_LOG

/*
//20121004_yunsik_DATA, scheduler debug code for SR
//DEBUG_ALRAN_ULIS_ICSk3.0v3.2_maruko.patch applied

Board-8960.c (kernel\arch\arm\mach-msm)
Irqdesc.c (kernel\kernel\irq)
Kallsyms.c (kernel\kernel)
Pm-8x60.c (kernel\arch\arm\mach-msm)
Printk.c (kernel\kernel)
Sched.c (kernel\kernel)
Scm.c (kernel\arch\arm\mach-msm)
Timer.c (kernel\arch\arm\mach-msm)
Workqueue.c (kernel\kernel)

*/
//#define BAM_ISSUE_TRACKING_SCHEDULER_LOG

/*
//20121003_yunsik_DATA, for SR
*/
//#define FEATURE_P_VZW_DS_KERNEL_PANIC_FOR_BAM_ISSUE_TRACKING_TEST

////////////////////////////////////////////////////////////////////////////////////////
 
/*==========================================================================
    6. SDM Feature
===========================================================================*/

// TODO:Merge
//4 FEATURE_P_VZW_CP_PS_ATTACH_DETACH_FOR_IMS�� ����Ǹ�compile error block ����(SkyDataBroadcastReceiver.java)
/*
Info : 
            VZW_Reqs-LTE_3GPP_Band13_NetworkAcess
            VZW_Reqs-LTE_OTADM
            VZW_Reqs-LTE_Data_Retry
            VZW_Reqs-Data_Requirement(eHRPD)

            1. LTE > �ϳ��� PDN Connected ���¿��� �ϳ��� PDN Disconnect ��, Detach Request
            2. LTE > �� �̻� PDN Connected ���¿��� �ϳ��� PDN Disconnect ��, �ش� PDN Disconnect Request & LTE Attached ����
                - �� ���, �ش� PDN�� IMS PDN�� ���, IMS PDN Disconnect ����, ��ٷ� IMS PDN Re-Connectivity Request
            3. LTE & eHRPD > PDN Enabled/Disabled ������ �Ϲ����� parameter ���� ��, �ش� PDN Disconnect Request ����, PDN Re-Connectivity Request

            4. LTE > IMS PDN �Ǵ� Admin PDN�� Enabled/Disabled Setting�� Disabled�� �ٲ��, Detach & NOT Re-Attach Request / 3G Fall Back(?)
            5. LTE > ��Ÿ PDN�� Enabled/Disabled Setting�� Disabled�� �ٲ��, �ش� PDN Disconnect Request
            6. eHRPD > IMS PDN �Ǵ� Admin PDN�� Enabled/Disabled Setting�� Disabled�� �ٲ��, All PDN Disconnect(Connection Close + Session ����) & NOT Re-Connection Request
            7. eHRPD > ��Ÿ PDN�� Enabled/Disabled Setting�� Disabled�� �ٲ��, �ش� PDN Disconnect

Related Feature :
            FEATURE_P_VZW_DATA_EHRPD_SDM
            FEATURE_P_VZW_DATA_LTE_SDM
Where : 
            cust_pantech_protocol_linux.h (android\pantech\include)
            IdevConnMo.java (android\pantech\apps\OtadmExtensions\src\com\innopath\activecare\dev\oem)
            ISkyDataConnection.aidl (android\frameworks\base\telephony\java\com\android\internal\telephony)
            SkyDataConInterfaceManager.java (android\frameworks\base\telephony\java\com\android\internal\telephony)
            
When : 20111026
Who : Ju.Hojin

//20111104_yunsik_DATA
==========================================
From: Youtz, Andrew [mailto:Andrew.Youtz@VerizonWireless.com] 
Sent: Tuesday, November 01, 2011 2:28 AM
To: 'Giseop Won'; Chen, Jennifer; 'EuiSeok Han'; 'Jin Yang'
Cc: 'Kostas Kastamonitis'; '�ֿ�ȣ_Young H. Choi'; ''������''; Guarino, Bernard
Subject: RE: [UML290] Question regarding OTADM SDM

The device should attach to eHRPD if the eHRPD class 1 APN is disabled. The device should not attach to eHRPD if the eHRPD class 2 APN is disabled.

The device should not attach to LTE if the LTE class 1 APN is disabled. The device should not attach to LTE if the LTE class 2 APN is disabled.

Thanks,
Andy
==========================================

ICS �������� ���� //20120203_yunsik_DATA

IMS Deregi�� ������� ����

<LTE>
VZWIMS 
 - parameter change : onCleanUpConnection (3sec)
 - APN TYPE DISABLE : IMS de-regi (4sec), cleanUpAllConnections (6sec), Modem Detach(8sec)
VZWADMIN : 
 - parameter change : onCleanUpConnection (3sec)
 - APN TYPE DISABLE : IMS de-regi (4sec), cleanUpAllConnections (6sec), Modem Detach(8sec)
VZWINTERNET : 
 - parameter change : onCleanUpConnection (3sec)
 - APN TYPE ENABLE : enableApnType (3sec)
 - APN TYPE DISABLE : disableApnType (3sec)
VZWAPP: 
 - parameter change : onCleanUpConnection (3sec)
 - APN TYPE ENABLE : nothing (3sec)
 - APN TYPE DISABLE : disableApnType (3sec)
VZW800: 
 - parameter change : onCleanUpConnection (3sec)
 - APN TYPE ENABLE : nothing (3sec)
 - APN TYPE DISABLE : disableApnType (3sec)

<eHRPD>
VZWIMS 
 - parameter change : onCleanUpConnection (3sec)
 - APN TYPE DISABLE : IMS de-regi (1sec), disableApnType (3sec)
VZWADMIN : 
 - parameter change : onCleanUpConnection (3sec)
 - APN TYPE DISABLE : IMS de-regi (4sec), cleanUpAllConnections (6sec), Modem Detach(8sec)
VZWINTERNET : 
 - parameter change : onCleanUpConnection (3sec)
 - APN TYPE ENABLE : enableApnType (3sec)
 - APN TYPE DISABLE : disableApnType (3sec)
VZWAPP: 
 - parameter change : onCleanUpConnection (3sec)
 - APN TYPE ENABLE : nothing (3sec)
 - APN TYPE DISABLE : disableApnType (3sec)
VZW800: 
 - parameter change : onCleanUpConnection (3sec)
 - APN TYPE ENABLE : nothing (3sec)
 - APN TYPE DISABLE : disableApnType (3sec)

*/

/*
//20120116_yunsik_DATA, 2011�� 12�� Req.
- SDMInterface.java (LINUX\android\frameworks\base\telephony\java\com\android\internal\telephony) �߰�

//20120203_yunsik_DATA, ����
Android.mk (linux\android\vendor\qcom\proprietary\cpmanager\cpmgrif)
AndroidManifest.xml (linux\android\packages\apps\phone)
Cpmgrif.c (linux\android\vendor\qcom\proprietary\cpmanager\cpmgrif)
Cpmgrif.h (linux\android\vendor\qcom\proprietary\cpmanager\cpmgrif)
Cpmgrnative.cpp (linux\android\vendor\qcom\proprietary\cpmanager\cpmgrjni)
Cust_pantech_linux_data.h (linux\android\pantech\include)
DataConnectionTracker.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
GsmDataConnectionTracker.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony\gsm)
IdevConnMo.java (linux\android\pantech\apps\otadmextensions\src\com\innopath\activecare\dev\oem)
ISkyDataConnection.aidl (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
ISkyTelephony.aidl (linux\android\frameworks\base\telephony\java\com\android\internal\telephony\cdma)
Phone.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
SDMInterface.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
SkyDataBroadcastReceiver.java (linux\android\packages\apps\phone\src\com\android\phone)
SkyDataConInterfaceManager.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
SkyDataIntents.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony)
SkyTelephonyInterfaceManager.java (linux\android\frameworks\base\telephony\java\com\android\internal\telephony\cdma)

*/
#define FEATURE_P_VZW_DS_SDM

/*
// 20121212_kns
- Reqs-LTE_MultimodeDevice_OTADM.doc (12.07)

  5.3.4.1 Connectivity Managed Object Tree
  The following nodes shall also be implemented:

  Note: VZW has 'collapsed' the APN table, This means the eHRPD and 3GPP Legacy APNs have
  the same parameters and respond to the same commands that are sent to the LTE APNs. When
  the OTADM functionality is applied to the LTE APN, the same commands will be applied to
  eHRPD, 3GPPLegacy as well
  
SkyDataBroadcastReceiver.java (linux\android\packages\apps\phone\src\com\android\phone)
*/
#define FEATURE_P_VZW_DS_SDM_SYNC_APN_REGARDLESS_MODE

/*
//20130114_yunsik_DATA, added for MTU

LTE_3GPP_Band13_NetworkAccess_October2012.pdf
3.2.4.8 MTU Size
The device shall set the LTE MTU size to 1428 bytes for all PDN connections. 
This MTU size shall be configurable via OTADM. Refer to the Verizon Wireless LTE OTADM Requirements for additional details. 
The device vendor shall not allow the user to modify the MTU size setting through the device user interface or the remote access user interface for tethered devices.

This implemantation includes LTE, 3GPP legacy and eHRPD system.
If this req is NOT applicable to eHRPD, please undef FEATURE_P_VZW_DS_SDM_MTU_eHRPD of MODEM side

[Android side]
- ConnMoInfo.java (pantech\apps\otadmextensions\src\com\innopath\activecare\dev\oem)
- IdevConnMo.java (pantech\apps\otadmextensions\src\com\innopath\activecare\dev\oem)
- Phone.java (frameworks\base\telephony\java\com\android\internal\telephony)
- PSDMInterface.java (pantech\frameworks\psdmif\java\com\pantech\psdmif)
- Psdm_jni.cpp (vendor\qcom\proprietary\psdmif)
- Sdm_node.xml (pantech\apps\otadmextensions\assets)

[IDL]
- qmi_psdm_api_v01.c (core\pantech\qmi\common)
- qmi_psdm_api_v01.h (core\pantech\qmi\common)
- qmi_psdm_api_v01.idl (core\pantech\qmi\common)

<NOTICE>
If you want to change MTU size in all system, You can change MTU size through property setting.
MTU property can be set as below..
persist.data_netmgrd_mtu=1428 (android\device\qcom\msm8960\system.prop)
refer to netmgr_kif_set_mtu at netmgr_qmi.c

You can also change MTU size using busybox ifconfig (./busybox ifconfig rmnet0 mtu 1428)

*/
#define FEATURE_P_VZW_DS_SDM_MTU

/*
//20120502_yunsik_DATA
- android\vendor\qcom\proprietary\qmi-framework\qcci\src\Android.mk
- LOCAL_CFLAGS += -DQCCI_OVER_QMUX block
*/
#define FEATURE_P_VZW_DS_QMI_BUILD_OPTION

//------------------------------------------------------------------------------------------------
// 2012.02.14 cho.hyunsoo :: PSDM QMI Service Enable
// Location : /home/p12360/sq/MSM8960ICS/LINUX/android/pantech/frameworks/testmenu_server/psdm *
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_PSDM_QMI_LINUX


//FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED, need more porting
//------------------------------------------------------------------------------------------------
// 2012.02.14 cho.hyunsoo :: PSDM QMI ?? ????? JAVA  JNI Interface (Java)
//  						- cpmgrif?? ??u??? ???? interface 
// Location : /home/p12360/sq/MSM8960ICS/LINUX/android/pantech/frameworks/psdmif *
//
// 2012.10.26 kns
//  - Android.mk (msm8960ics_wtr\linux\android\frameworks\base)
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_USING_PSDM_INTERFACE

//------------------------------------------------------------------------------------------------
// 2012.02.14 cho.hyunsoo :: PSDM QMI ?? ????? JAVA  JNI Native Method
// Location : /home/p12360/sq/MSM8960ICS/LINUX/android/vendor/qcom/proprietary/psdmif *
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_PSDM_JNI_NATIVE

//------------------------------------------------------------------------------------------------
// 2012.02.14 cho.hyunsoo :: PSDM QMI Service Debug
// Location : /home/p12360/sq/MSM8960ICS/LINUX/android/pantech/frameworks/testmenu_server/psdm *
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_PSDM_QMI_CLINENT_DEBUG

//------------------------------------------------------------------------------------------------
// 2012.03.19 SJM Tethering Null Pointer Exception
// Tethering.java
// ConnectivityService.java
//
// 2012.05.11 p11546
//  Qcom 8960 1.5.30 version includes this fix so Pantech feature is removed

// 2012.10.26 kns
// Reopen...This issue is not fixed in JB.
// Tethering.java
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_TETHERING_EXCEPTION

//------------------------------------------------------------------------------------------------
// 2012.03.28 SJM Tethering Null Pointer Exception when Add/Remove v6 interface
// Tethering.java
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_TETHERING_EXCEPTION_V6_INTERFACE

#if 0 //def FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED
//------------------------------------------------------------------------------------------------
// 2012.03.28 SJM DataConnection Null Pointer Exception when checkAndUpdatePartialProtocolFailure
// IMS Iniating -> Airplane Mode on -> IMS Connected -> Null Pointer Exception in checkAndUpdatePartialProtocolFailure due to mApn is null.
// DataConnection.java
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_DATACONNECTION_NULL_POINTER
#endif

/*
Info : 
            VZW Reqs-LTE_Multimode_PST
              - 2.1.2.1	APN settings
                  * APN Class(ID), APN NI(Name), APN IP Type, APN Bearer, APN Disable, APN Inactivity Timer
Related Feature :
            FEATURE_P_VZW_DS_SDM
            FEATURE_P_VZW_DS_eHRPD_INTERNET_IPV6

            FEATURE_P_VZW_DS_PST_LTE_APN_SETTINGS  // Feature of Modem side
            FEATURE_P_VZW_DS_PST_EHRPD_APN_SETTINGS  // Feature of Modem side
Where : 
            Cust_pantech_linux_data.h (linux\android\pantech\include)
            PstDataApnSettings.java (linux\android\pantech\frameworks\sky_pst\java\com\pantech\service)

When : 20110320
Who : Ju.Hojin
*/
#define FEATURE_P_VZW_DS_PST_ANDROID_APN_SETTINGS

//------------------------------------------------------------------------------------------------
// 2012.03.20 SJM for Data Call State Logging
// cdmaDataConnectionTracker.java
// gsmDataConnectionTracker.java
// DataConnection.java
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_DATA_CALL_STATE_LOG

//------------------------------------------------------------------------------------------------
// 2012.03.23 SJM for Do not add IMS DNS address to routing table
// ConnectivityService.java
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_DONOT_ADD_IMS_DNS_ROUTING_TABLE

#if 0 //def FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED
//------------------------------------------------------------------------------------------------
// 2012.03.23 SJM for Modify DualIpPartialRetry Algorithm 
// GsmDataConnectionTracker.java
// MobileDataStateTracker.java
// NetMgr_kif.c
// NetMgr_Kif.h
// NetMgr_qmi.c
//
// 2012.05.11 p11546 Qcom 1530 S/W Migration
//  Qcom 1530 includes same fix so remove Pantech feature in below files
//  NetMgr_kif.c, NetMgr_kif.h, NetMgr_qmi.c 
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_DUALIPPARTIALRETRY_CHANGE
#endif

/*
//20120324_yunsik_DATA, //20120524_yunsik_DATA, BLOCK
- res_cache.c
- MMS IOT fail, do not query IPv6, when accuired IPv4 address
- but i will handle cache, even though no answer
*/
//#define FEATURE_P_VZW_DS_DNS_CACHE_EVEN_THOUGH_NO_ANSWER

/*
//20120524_yunsik_DATA, with SKPark_Msg
- res_cache.c => remove
- getaddrinfo.c
- res_cache.c
- res_send.c
- MMS IOT fail, do not query IPv6, when accuired IPv4 address
*/
#define FEATURE_P_VZW_DS_DNS_TTL_MMS_ONLY

/*
//20120525_yunsik_DATA, maximum cache time
- Reqs-DataRequirement.docx  1.10.1	CACHING <VZW-REQS-128- 41>
   	- In the event that the source software offers an option for setting the maximum cache time 
   	   (e.g. via a compile-time static variable), the time shall be set to a value of 24 hours. 
- Res_cache.c (bionic\libc\netbsd\resolv)
*/
#define FEATURE_P_VZW_DS_MAXIMUM_CACHE_TIME

/*
//20120328_yunsik_DATA
- UsbManager.java
- Tethering.java 
- add diag port during tethering

//20121127_kns
- Because Qualcomm add property to connect the DIAG port, Don't not add the diag when tethering.
*/
//#define FEATURE_P_VZW_DS_ADD_DIAG_PORT_WHEN_TETHERING

//------------------------------------------------------------------------------------------------
// 2012.04.05 SJM Data Disconnection after factory reset due to apnchanged event
// GsmDataConnectionTracker.java
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_APNCHANGED_FACTORY_RESET

#if 0 //def FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED
//------------------------------------------------------------------------------------------------
// 2012.04.06 SJM DNS Property Changed after application connection. from OSCAR
// ConnectivityService.java
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_DNS1_CHANGED
#endif

#if 0 //def FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED
//------------------------------------------------------------------------------------------------
// 2012.04.07 chhong block partial retyr call during connecting by partial retry
// ApnContext.java
// GsmDataConnectionTracker.java
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_MULTIPLE_PARTIAL_RETRY_BLOCKING

//------------------------------------------------------------------------------------------------
// 2012.04.07 SJM reconnect dual ip fail on IPv4
// DataConnection.java
// DataConnectionAC.java
// Phone.java
// GsmDataConnectionTracker.java
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_RECONNECT_DUALIPFAILURE_ON_IPV4
#endif

#if 0 //def FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED
//------------------------------------------------------------------------------------------------
// 2012.04.10 SJM Ipv6 connection fix in HDR_LTE_ONLY_MODE
// GsmServiceStateTrakcer.java
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_RAT_CHANGE_GSMSST
#endif

#if 0 //def FEATURE_P_VZW_DS_PREMIA_V_JB_NOT_MERGED
//------------------------------------------------------------------------------------------------
// 2012.04.13 SJM Data Call List Update when modem data disconnection
// qcril_data_netctrl.c
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_CALLLIST_UPDATE
#endif

//------------------------------------------------------------------------------------------------
// 2012.04.14 SJM QOS Disable due to netmgrd crash
// init_starq.rc
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_QOS_DISALBE

//------------------------------------------------------------------------------------------------
// 2012.04.14 SJM NetworkInfo isAvailable is false when APN State is failed
// GsmDataConnectionTracker.java
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_APN_FAILED_STATE_BUG

//------------------------------------------------------------------------------------------------
// 2012.04.17 SJM Reconnect when routing add failed.
// ConnectivityService.java
// NetworkStateTracker.java
// MobileDataStateTracker.java
// WifiStateTracker.java
// BluetoothTetheringStateTrakcer.java
// DummyDataStateTracker.java
// EthernetDataTracker.java
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_ROUTE_ADD_FAIL

//------------------------------------------------------------------------------------------------
// 2012.04.17 SJM enableApnType delay 500ms when state is disconnecting.
// gsmDataConnectionTracker.java
// DataConnectionTracker.java
//------------------------------------------------------------------------------------------------
#define FEATURE_P_VZW_DS_SUPL_ENABLE_BLOCKING

/*===========================================================================
    IMS Features
===========================================================================*/

/*
20110322, swchoi
summary:
  - IMS���� ���ÿ��� �������� ����� ������ �����.
  - Ÿ���ÿ��� ����ϱ����� frameworks�� ���Խ�Ŵ
  - IMS�� ���ð� ���ÿ� ������ �����Ͽ����ϱ⶧���� BootUpListener�� ���Խ�Ŵ.

Android.mk (LINUX\android\frameworks\base) : ����
app_icon.png (LINUX\android\pantech\providers\SkyIMSProvider\res\drawable) : ����
strings.xml (LINUX\android\pantech\providers\SkyIMSProvider\res\values) : ����
strings.xml (LINUX\android\pantech\providers\SkyIMSProvider\res\values-ko) : ����
Android.mk (LINUX\android\pantech\providers\SkyIMSProvider) : ����
AndroidManifest.xml (LINUX\android\pantech\providers\SkyIMSProvider) : ����
BootUpListener.java (LINUX\android\pantech\providers\SkyIMSProvider\src\com\pantech\provider\skyims) : ����
SkyIMS.java (LINUX\android\pantech\frameworks\skyims\java\com\pantech\provider\skyims) : ����
SkyIMSProvider.java (LINUX\android\pantech\providers\SkyIMSProvider\src\com\pantech\provider\skyims) : ����
*/
#define FEATURE_P_VZW_DS_IMS_PROVIDER

/*
20120128, swchoi
summary:
  - Ecrio IMS on/off �߰�
  - IMS AKA Version �߰�

SkyIMS.java (android\pantech\frameworks\skyims\java\com\pantech\provider\skyims)
SkyIMSProvider.java (android\pantech\providers\SkyIMSProvider\src\com\pantech\provider\skyims)
SkyIMSService.java (android\packages\apps\Phone\src\com\android\phone)
*/
#ifdef FEATURE_P_VZW_DS_IMS_PROVIDER  
#define FEATURE_P_VZW_DS_IMS_PROVIDER_EXPANSION
#endif

/*
  20110404, swchoi
  summary:
    - ���� ���� IMS���� ����� Contents Provider�� ������  system properties�� ����
    - IMS Contents Provider ���� �� system properties update

AndroidManifest.xml (LINUX\android\packages\apps\Phone) : ����
SkyIMSService.java (LINUX\android\packages\apps\Phone\src\com\android\phone) : ����

  2010.04.24 add
  - vzwims�� IP ���� �߰�
  - property_perms[] �� ims���� profix�߰� "ims."-> AID_RADIO, AID_SYSTEM �Ӽ��߰�  
property_service.c (LINUX\android\system\core\init) : ����

  20111006 : cheetah�� imsservice�� start�� SkyPhoneBroadcastReceiver.java �� �ƴ� �Ʒ����� ����.
                  ���� SkyPhoneBroadcastReceiver ���� �߰��Ǹ� IMS���� ������ �ߺ����� ������� �ʴ��� üũ�ʿ���.
IMSStartupReceiver.java (LINUX\android\packages\apps\Phone\src\com\android\phone) : ����

  20120101, swchoi ICS����

  2012.01.11 add
  - LinkProperties ������� �ڵ� ����
  
MobileDataStateTracker.java (LINUX\android\frameworks\base\core\java\android\net)
*/
#ifdef FEATURE_P_VZW_DS_IMS_PROVIDER  
#define FEATURE_P_VZW_DS_IMS_SERVICE

/*
//20130402_yunsik_DATA
For Dual IP Equipment,
When IMS partial retry happened, DO SET property one more. (ims.ipv6.addr, ims.ipv6.iface)

- GsmDataConnectionTracker.java (msm8930_jb\linux\android\frameworks\base\telephony\java\com\android\internal\telephony\gsm)
*/
#define FEATURE_P_VZW_DS_IMS_SERVICE_IPV6_UPDATE_FOR_PARTIAL_RETRY
#endif

  /*
  20111012, swchoi
  summary:
    - LTE/eHRPD IMS Connected���� PCO Data�� Linux������ ����
    - P-CSCF Addr �� DNS Addr �� �����ϰ� IMS APP�� ��û �� �Ѱ���
    - IPv6 ������ ó���� ������.    
  
  20120101, swchoi ICS����
  IP������ ó�� �����ϱ� ������ APN_TYPE_VZWIMS�� ��� IPV6�� ����...IPV4�� ���� �̺�Ʈ�� �� ��� �����
  (DCT�� notifyDataConnection() �� IPV�� ���� ó����ƾ�� �������� TelephonyRegistry���� �߰��� �� ����.)
  
cust_pantech_protocol_linux.h (LINUX\android\pantech\include)
SIPInterface.java (LINUX\android\frameworks\base\telephony\java\com\android\internal\telephony) : �߰�
MobileDataStateTracker.java (LINUX\android\frameworks\base\core\java\android\net) : ����
  */
#define FEATURE_P_VZW_DS_IMS_SERVER_ADDR

/*
20110403, swchoi
FEATURE_P_VZW_DS_IMS_UICC_EF_LOAD:
  - IMS���� ����� UICC������ system properties�� set
  - 20110908 : ISIM EF ������ �ּұ��� üũ �߰�.
20120103, swchoi
  - FEATURE_P_VZW_DS_IMS_PST_UICC �� ���� �� ����.
  - ISIMRecords.java��� ICS���� ���� �⺻���� �߰��� IsimUiccRecords.java ���

SIMRecords.java (LINUX\android\frameworks\base\telephony\java\com\android\internal\telephony\gsm)  : ����
IsimUiccRecords.java (android\frameworks\base\telephony\java\com\android\internal\telephony\ims)  : ����

20120129, swchoi
FEATURE_P_VZW_DS_IMS_UICC_SET_PUID_DOMAIN:  
  - Domain of Public User ID ���� ISIM���� �о������ ����(Read Only)
  - MSISDN Based PUID�� ���ų� Non-provisioned �� ���� default���� vzims�� ���
  - MSISDN Based PUID�� ��ȿ�� ��� '@'���� string�� ���

IsimUiccRecords.java (android\frameworks\base\telephony\java\com\android\internal\telephony\ims)

2012.04.10, swchoi
[For Limited Service]
*/
#ifdef FEATURE_P_VZW_DS_IMS_PROVIDER  
#define FEATURE_P_VZW_DS_IMS_UICC_EF_LOAD
#define FEATURE_P_VZW_DS_IMS_UICC_SET_PUID_DOMAIN
#endif


/*
20110403, swchoi
FEATURE_P_VZW_DS_IMS_INTENT:

20120103, swchoi
  - USIM �� ISIM EF Loading Ȯ�� ���� APP�� ��� �����ϵ��� intent broadcast
  - IMS Test Mode �� eHRPD Mode �� SMS Over IP Net Ind ���� ���¸� intent broadcast
  - IMS���� DB Loaded�Ǿ������� ����  intent broadcast
  - ISIMRecords.java��� ICS���� ���� �⺻���� �߰��� IsimUiccRecords.java ���
  
  SIMRecords.java (android\frameworks\base\telephony\java\com\android\internal\telephony\gsm)
  IsimUiccRecords.java (android\frameworks\base\telephony\java\com\android\internal\telephony\ims)
  SkyIMSService.java (LINUX\android\packages\apps\Phone\src\com\android\phone)
*/
#ifdef FEATURE_P_VZW_DS_IMS_PROVIDER  
#define FEATURE_P_VZW_DS_IMS_INTENT
#endif

  /*
  20120104, swchoi
  summary:
    - qmi ����� AKA API��� ����: PS2������ ���ķ� ���� ���ּ��� 
    - init.rc���� data/radio �׷캯�� �� cmpgrif service�� ���� �߰�
    - isim�� usim aka���� ���� �ٸ� API�� ȣ���ϹǷ� sky_qmi_uim_ims_aka �Լ����� sim type pram�� �����ϰ� ISIM���θ� �����ϵ��� ����

init_STARQ.rc (android\system\core\rootdir)

qmi_aka ���
android\vendor\qcom\proprietary\qmi\inc\sky_qmi_uim_ims_aka.h : �߰�
android\vendor\qcom\proprietary\qmi\src\sky_qmi_uim_ims_aka.c : �߰�
android\vendor\qcom\proprietary\qmi\src\Android.mk
android\vendor\qcom\proprietary\qmi\src\Makefile.am

qmi_aka test app
android\vendor\qcom\proprietary\qmi\sky_isim_aka\*.* :�߰�
  */

  /*
  20110329, swchoi
  summary:
    - IMS SDM
      otadmGetIMSDomain()
      otadmSetIMSDomain() 
      :read only -> ���� ������ ISIM�� ����Ǿ��־� ����Ұ���. 
      otadmGetIMSSIPT1()
      otadmSetIMSSIPT1()
      otadmGetIMSSIPTf()
      otadmSetIMSSIPTf()
      otadmGetIMSSIPT2()
      otadmSetIMSSIPT2()

  20120108, swchoi
    - ICS���� cursor�� update���� �޼��尡 ��� �����Ǿ� ����
    - update�� cursor.updateXXX �� ������� �ʰ� ContentResolver.update�� �����.
    - sms ���� method�鵵 �Բ� ���ĸ�
      
IdevConnMo.java (LINUX\android\pantech\apps\OtadmExtensions\src\com\innopath\activecare\dev\oem)      
  */
#ifdef FEATURE_P_VZW_DS_IMS_PROVIDER  
#define FEATURE_P_VZW_DS_IMS_SDM
#endif


/*
//2012.02.04 swchoi
 - IMS Test Mode

 SkyIMS.java (android\pantech\frameworks\skyims\java\com\pantech\provider\skyims)
 SkyIMSService.java (android\packages\apps\Phone\src\com\android\phone)
 */
#define FEATURE_P_VZW_DS_IMS_TEST_MODE_LINUX

/*  20120302 swchoi : AKAv1 With IPSec

  - FEATURE_P_VZW_DS_IPSEC_CONFIG ���� �Ʒ� config y �� �����ʿ�
    LINUX\android\kernel\arch\arm\configs\msm8960_starq_ws20_defconfig
    CONFIG_XFRM_USER , CONFIG_INET_XFRM_TUNNEL , CONFIG_INET_XFRM_MODE_TRANSPORT
    CONFIG_INET_XFRM_MODE_TUNNEL , CONFIG_INET_AH , CONFIG_INET_ESP
    
  - ahope�� sipsad deamon�� ���ؼ� net_admin �׷�Ӽ� �ο� �� sipsa socket�� ���� system���Ѻο�.
   - init.qcom.rc   starQ    init.qcom_starq.rc      .->
  LINUX\android\system\core\rootdir\etc\init.qcom.rc
  
  - ahope�� sipsa ���̺귯���ҽ� �� test code
    LINUX\android\pantech\frameworks\pantech_ipsec\*.*
*/
#define FEATURE_P_VZW_IMS_ECRIO_AKAV1_WITH_IPSEC


/*===========================================================================
    eHRPD Features
===========================================================================*/
#if 0
/*
20110321, SJM
  -eHRPD SDM
*/
#define FEATURE_P_VZW_DATA_EHRPD_SDM
#endif

/*===========================================================================
    LTE Features
===========================================================================*/


/*===========================================================================
    Wi-Fi Features
===========================================================================*/

#endif/* __CUST_PANTECH_LINUX_DATA_H__ */

