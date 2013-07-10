#ifndef __CUST_PANTECH_DRM_H__
#define __CUST_PANTECH_DRM_H__

/*
  2012/01/16 이용연
  PANTECH 해외향 DRM 개발 관련 공통 최상위 feature.
  - 세부 feature를 정의하기 어려운 부분
  - DRM 관련 소스가 아닌 부분을 수정하는 경우 사용 (make file 등인 경우 #주석 부분에 추가)
*/
#define FEATURE_PANTECH_DRM

/*
  2012/01/16 이용연
  FwdLock plugin 지원을 위해 수정한 사항
*/
#define FEATURE_PANTECH_DRM_FWDLOCK

/*
  2012/01/16 이용연
  WIDEVINE plugin 지원을 위해 수정한 사항
  FEATURE_PANTECH_WIDEVINE_DRM 으로 변경
*/
//#define FEATURE_PANTECH_DRM_WVM

/*
  2012/01/16 이용연
  DRM LOG enable
  - DRM 관련 소스가 아닌 부분에 DRM LOG를 추가하는 경우에도 사용
*/
//#define FEATURE_PANTECH_DRM_LOGGING

/*
  2012/03/30 이용연
  This feature should be disable - WideVine log packets to files
*/
//#define FEATURE_PANTECH_DRM_WV_PACKET_LOG

/*
  2012/03/30 이용연
  This feature should be disable - WideVine keybox
*/
//#define FEATURE_PANTECH_DRM_WV_KEYBOX_LOG

/*
  2012/03/30 이용연
  This feature should be disable - rewrite Widevine raw data.
*/
//#define FEATURE_PANTECH_DRM_REWRITE_WV_RAW_DATA

/*
  2012/01/16 이용연
  WideVine security level 1
*/
//#define FEATURE_PANTECH_DRM_WVL1

/*
  2012/mm/dd who
  ...description...
*/
//#define FEATURE_PANTECH_DRM_blahblah

#endif /* __CUST_PANTECH_DRM_H__ */
