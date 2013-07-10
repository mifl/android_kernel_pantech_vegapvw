#ifndef __CUST_PANTECH_MMP_H__
#define __CUST_PANTECH_MMP_H__

/*
  2011/03/02 권오윤
  PANTECH multimedia 개발 관련 공통 최상위 feature.
  세부 feature를 정의하기 어려운 부분 또는 multimedia관련 소스가 아닌 부분을
  수정하는 경우 사용. (make file 등인 경우 #주석 부분에 추가)
*/
#define FEATURE_PANTECH_MMP

/*
  2011/03/02 권오윤
  VisuialOn VOME engine관련 /external/vome/... 외의 부분을 수정하거나
  VisualOn 소스를 일부 수정해서 사용하는 부분에 대해 feature작업
*/
#define FEATURE_PANTECH_MMP_VOME
/*****************************************************************/
/*
   2012/04/03 Heekyoung Seo.
   VisualOn Engine Enable Features..
*/
#define FEATURE_PANTECH_MMP_VOME_AVI
#define FEATURE_PANTECH_MMP_VOME_ASF
#define FEATURE_PANTECH_MMP_VOME_MKV
/*#define FEATURE_PANTECH_MMP_VOME_FLAC // Using Stagefrightplayer */
#define FEATURE_PANTECH_MMP_VOME_QTIME
#define FEATURE_PANTECH_MMP_VOME_MPEG1_PS

/*#define FEATURE_PANTECH_MMP_VOME_RTSP // We don't use this feature, now.
RTSP Engine is hard coded by apk team in MediaPlayerService.cpp*/
#define FEATURE_PANTECH_MMP_VOME_DLNA

/************************************************************************
 -  To Support flv file format with VOME              
 -  2012/10/17 P12276 Heekyoung Seo 
************************************************************************/ 
// #define FEATURE_PANTECH_MMP_VOME_FLV

/************************************************************************
 -  To Support mp2ts & ts file format with VOME  
 -  2012/10/17 P12276 Heekyoung Seo 
************************************************************************/ 
//#define FEATURE_PANTECH_MMP_VOME_M2TS

#if defined(T_STARQ)
#define FEATURE_PANTECH_MMP_VOME_ADPCM_WAV
#endif
/******************************************************************/

/* 
  2011/03/10 최병주
  QualComm Patch 적용한 부분에 대한 frature 작업
*/
#define FEATURE_PANTECH_MMP_QCOM_CR

/* 
  2011/03/11 권오윤
  StageFright 관련 수정하는 부분들을 찾기 쉽게 하기 위해 사용
*/
#define FEATURE_PANTECH_MMP_STAGEFRIGHT
 
/*
  2011/04/29 Heekyoung Seo
  Add WMA S/W Decoder and code to use VC-1 H/W Decoder with Stagefright for 
  SKT HOPPIN Service.
  If Don't need HOPPIN Service, it also need to modify 
  frameworks/base/media/libstagefright/Android.mk. (remove 
  BUILD_WITH_WMA_SW_DECODER:=true)
  */
#if defined(T_EF47S)  
#define FEATURE_PANTECH_MMP_HOPPIN
#endif

#if defined(T_EF46L)
#define FEATURE_PANTECH_MMP_LGT
#endif

/* Visualon AAC Codec for Stagefright player */
//#define FEATURE_PANTECH_MMP_VOMEAAC

#if 0 // TODO: define by carrier
/*
 2011/06/22 최병주
  내수와 해외를 통합하기 위해 FEATURE를 적용하여 구분
*/
#if defined(T_EF45K) || defined(T_EF46L) || defined(T_EF47S)
  #define FEATURE_PANTECH_MMP_DOMESTIC
#elif defined(T_CHEETAH) || defined(T_STARQ) || defined(T_RACERJ) || defined(T_VEGAPVW) || defined(T_OSCAR) || defined(T_VEGAPKDDI)
  #define FEATURE_PANTECH_MMP_ABROAD
#elif defined(T_CSFB) || defined(T_SVLTE) //temp
  #define FEATURE_PANTECH_MMP_ABROAD  
#else
  #error "FEATURE_PANTECH_MMP ? DOMESTIC or ABROAD"
#endif
#endif

/*
 2011/06/26 최병주

 TestSBA_M8260AAABQNLZA3040_Pantech_EF33S-EF34K_05252011_Case00518255

 Qcom H/W Dec을 사용하여 XivD 파일 재생시 화면이 일그러지는 문제를 해결하기 위한 Test SBA
 
 ( simple profile B-frame )
 *  Remove it -> 2012/10/11 : Included QCOM Main Branch
 
#define FEATURE_PANTECH_MMP_XVID_QCOM_HWDEC_SBA
*/

/*
 2011/07/12 최병주

 SBA_M8660AAABQNLYA109020_Pantech_EF33S_07122011_Case00522374_00522374
*  Remove it -> 2012/10/11 : Included QCOM Main Branch
 #define FEATURE_PANTECH_MMP_QCOM_SBA_TIMESTAMP
*/

/*
  2011/mm/dd who
  ...description...
*/
#define FEATURE_PANTECH_MMP_xxx

/*
  2011/mm/dd who
  ...description...
*/
#define FEATURE_PANTECH_MMP_zzz

/*****************************************************************/
/*
   2012/04/06 Hyeran.lee
   Add WORKAROUND FEATURE for fixing OGG sound cutoff.   
   Incease number of outbuffer.
*/
#define FEATURE_PANTECH_MMP_OGGCUTOFF_WORKAROUND
/*****************************************************************/


/*****************************************************************/
/*
   2012/12/18 Hyeran.lee
   Add WORKAROUND FEATURE for fixing initual mute on voice reocrding 
*/
#define FEATURE_PANTECH_MMP_VOICERECORDING_WORKAROUND
/*****************************************************************/


/******************************************
* 2012/06/25 Heekyoung Seo P12276
*  Remove it -> 2012/10/11 : Included QCOM Main Branch
*  FEATURE_PANTECH_MMP_MEM_LEAK 
*  Media Server's memory leak fix.
#define FEATURE_PANTECH_MMP_MEM_LEAK
*******************************************/
#if defined(T_OSCAR)
#define FEATURE_PANTECH_MMP_ATT
#endif

/******************************************
* 2012/10/11 Heekyoung Seo P12276
*  
*  FEATURE_PANTECH_MMP_DISABL_MP2 
*  To don't support MP2 H/W Video Decoder because of Royalty.
*******************************************/
#define FEATURE_PANTECH_MMP_DISABLE_MP2


/******************************************
* 2012/10/11 Heekyoung Seo P12276
*  
*  FEATURE_PANTECH_MMP_VOME_DivXDRM 
*  Seperate from FEATURE_PANTECH_MMP_VOME within frameworks
*******************************************/
#define FEATURE_PANTECH_MMP_VOME_DivXDRM

/******************************************
* 2012/06/20 Heekyoung Seo P12276
*  
*  FEATURE_PANTECH_MMP_WFD_PIC_ORDER configurs QCT Decoder to decode
*  decode-ordered avc TS streams with NuPlayer
*  
*/
#define FEATURE_PANTECH_MMP_WFD_PIC_ORDER

/************************************************************************
 -  To Support New API For JellyBean
 -  2012/11/12 P12276 Heekyoung Seo 
************************************************************************/ 
/* #define FEATURE_PANTECH_MMP_VOME_JB */

/************************************************************************
 -  Remove Noise when call seekTo after play. Workaround Code.
 -  2013/01/07 P12276 Heekyoung Seo 
************************************************************************/ 
#define FEATURE_PANTECH_MMP_AMR_NOISE_WORKAROUND

/************************************************************************
 -  Add NativeOutputBuffer Size Setting Code in ACodec same as OMXCodec.
 -  2013/01/17 P12276 Heekyoung Seo 
************************************************************************/ 
#define FEATURE_PANTECH_MMP_ACODEC_BUFFER_SET

//+US1-CF1
//Feature : FW_VENDOR_FOR_AOT_VIDEO_APP
//API support for AOT
#define FEATURE_PANTECH_MMP_SUPPORT_AOT
//-US1-CF1

#endif/* __CUST_PANTECH_MMP_H__ */
