//
//  cgeFaceCommon.h
//  cgeFaceTracker
//
//  Created by wysaid on 16/1/17.
//  Copyright © 2016年 wysaid. All rights reserved.
//

#ifndef cgeFaceCommon_h
#define cgeFaceCommon_h

#ifdef __cplusplus
extern "C"
{
#endif
    
#define CGE_FACE_EFFECT_VERSION 20160126

typedef enum CGEFaceFeature {
    CGE_FACE_LEFT_EYE_TOP,
    CGE_FACE_RIGHT_EYE_TOP,
    CGE_FACE_LEFT_EYEBROW,
    CGE_FACE_RIGHT_EYEBROW,
    CGE_FACE_LEFT_EYE,
    CGE_FACE_RIGHT_EYE,
    CGE_FACE_LEFT_JAW,
    CGE_FACE_RIGHT_JAW,
    CGE_FACE_JAW,
    CGE_FACE_OUTER_MOUTH,
    CGE_FACE_INNER_MOUTH,
    CGE_FACE_NOSE_BRIDGE,
    CGE_FACE_NOSE_BASE,
    CGE_FACE_OUTLINE,
    CGE_FACE_ALL_FEATURES,
    CGE_FACE_TOTAL_FEATURE_NUM
}CGEFaceFeature;

#ifdef __cplusplus
}
#endif

#endif /* cgeFaceCommon_h */
