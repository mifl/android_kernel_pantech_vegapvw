#ifndef __CUST_PANTECH_DRM_H__
#define __CUST_PANTECH_DRM_H__

/*
  2012/01/16 �̿뿬
  PANTECH �ؿ��� DRM ���� ���� ���� �ֻ��� feature.
  - ���� feature�� �����ϱ� ����� �κ�
  - DRM ���� �ҽ��� �ƴ� �κ��� �����ϴ� ��� ��� (make file ���� ��� #�ּ� �κп� �߰�)
*/
#define FEATURE_PANTECH_DRM

/*
  2012/01/16 �̿뿬
  FwdLock plugin ������ ���� ������ ����
*/
#define FEATURE_PANTECH_DRM_FWDLOCK

/*
  2012/01/16 �̿뿬
  WIDEVINE plugin ������ ���� ������ ����
  FEATURE_PANTECH_WIDEVINE_DRM ���� ����
*/
//#define FEATURE_PANTECH_DRM_WVM

/*
  2012/01/16 �̿뿬
  DRM LOG enable
  - DRM ���� �ҽ��� �ƴ� �κп� DRM LOG�� �߰��ϴ� ��쿡�� ���
*/
//#define FEATURE_PANTECH_DRM_LOGGING

/*
  2012/03/30 �̿뿬
  This feature should be disable - WideVine log packets to files
*/
//#define FEATURE_PANTECH_DRM_WV_PACKET_LOG

/*
  2012/03/30 �̿뿬
  This feature should be disable - WideVine keybox
*/
//#define FEATURE_PANTECH_DRM_WV_KEYBOX_LOG

/*
  2012/03/30 �̿뿬
  This feature should be disable - rewrite Widevine raw data.
*/
//#define FEATURE_PANTECH_DRM_REWRITE_WV_RAW_DATA

/*
  2012/01/16 �̿뿬
  WideVine security level 1
*/
//#define FEATURE_PANTECH_DRM_WVL1

/*
  2012/mm/dd who
  ...description...
*/
//#define FEATURE_PANTECH_DRM_blahblah

#endif /* __CUST_PANTECH_DRM_H__ */
